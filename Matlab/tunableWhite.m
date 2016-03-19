clear; t = cputime;

%% Glossary
% SPD := Spectral power distribution, also called spectrum in this script
% CCT := Correlated color temperature, nearest black body radiator
% CRI := Color rendering index, calculated with all 14 test color samples

%% Declare variables
L = 380:5:780; % Wavelengths: from 380nm to 780nm sampled at 5nm
resolution = 0.01; % LED mixing coefficient resolution
polynomialOrder = 5; % Order of polynomial fit function
minCCT = 1000; % Minimum correlated color temperature
maxCCT = 6500; % Maximum correlated color temperature
targetRg = 110; % Target color saturation for optimization
multiLedMixing = true; % Mix multiple LEDs or only two?
inspectSpds = false; % Inspect SPDs for various color temperatures?

load('cie.mat'); % Load lookup tables for colorimetry calculations
load('led_data.mat'); % Load spectrums for various LEDs

%% Spectrums for red, green and blue LEDs
red = gaussmf(L, [20 670]); redL = 120;
green = gaussmf(L, [20 530]); greenL = 240;
blue = gaussmf(L, [20 455]); blueL = 150;

%% Spectrum for warm white LED
warm = Yuji_BC2835L_2700K; warmL = 1400; warmL = 700;
%warm = Yuji_BC2835L_3200K; warmL = 1400;
%warm = Yuji_BC5730L_2700K; warmL = 900;
%warm = Yuji_BC5730L_3200K; warmL = 900;
%warm = Yuji_VTC5730_2700K; warmL = 800;
%warm = Yuji_VTC5730_3200K; warmL = 800;
%warm = Cree_A19_2700K; warmL = 350;
%warm = Generic_3000K; warmL = 300;
%warm = mixSpd([Yuji_BC2835L_2700K;green],[0.9;0.1]); warmL = 700;
%warm = mixSpd([Yuji_BC2835L_2700K;green],[0.8;0.2]); warmL = 700;

%% Spectrum for cold white LED
cold = Yuji_BC2835L_5600K; coldL = 1700; coldL = 2700;
%cold = Yuji_BC5730L_5600K; coldL = 1000;
%cold = Yuji_BC5730L_6500K; coldL = 1000;
%cold = Yuji_VTC5730_5600K; coldL = 1000;
%cold = Generic_6500K; coldL = 200;
%cold = Generic_10000K; coldL = 350;

%% LEDs used for simulations
leds = [
    Led('red', red, redL, 1)
    Led('warm', warm, warmL, 1)
    Led('cold', cold, coldL, 1)
    %Led('green', green, greenL, 0.2)
    %Led('blue', blue, blueL, 0.5)
];
ledSpds = zeros(length(leds), length(L));
for i = 1:length(leds)
    ledSpds(i, :) = leds(i).spd;
end

%%
supertitle = 'Red = 625nm, Warm = BC2835L 2700K + Green 20%, Cold = BC2835L 5600K';
initializedIn = cputime - t

%% Never use multi led mixing for 2 LEDs
if length(leds) < 3
    multiLedMixing = false;
end

%% Mix 2 consecutive leds
if ~multiLedMixing
    
    rawMixingData = zeros((length(leds)-1)/resolution + 1, length(leds) + 1);
    i = 1;
    for l = 1:length(leds)-1
        for c = 1:-resolution:0
            % Create mix spectrum with mix factors
            spd = mixSpd([leds(l).spd; leds(l+1).spd], [c; 1-c]);
            cct = spdToCct(spd);
            % Add row to data matrix
            rawMixingData(i, 1) = cct;
            rawMixingData(i, l+1) = c;
            rawMixingData(i, l+2) = 1 - c;
            i = i + 1;
        end
    end

%% Mix all LEDs
else
    
    rawMixingData = mixLeds(leds, resolution);
    
end

%%
rawMixingIn = cputime - t

%% Select best results
binSize = 100;
% goodness, index
cctBins = zeros(maxCCT / binSize - minCCT / binSize + 1, 2);

% Iterate all bins and find largest Rp for each bin
for i = 1:length(rawMixingData)
    % Skip results outside of CCT range
    if rawMixingData(i, 1) < minCCT
        continue;
    elseif rawMixingData(i, 1) > maxCCT
        continue;
    end

    % CCT bin index
    cctBin = floor(rawMixingData(i, 1) / binSize) - minCCT / binSize + 1;

    spd = mixSpd(ledSpds, rawMixingData(i, 2:end));
    [~, ~, ~, X, Y ,Z] = spdToXyz(spd);
    [~, ~, ~, Xw, Yw ,Zw] = spdToXyz(refSpd(rawMixingData(i, 1)));
    [Rf, Rg] = spdToRfRg(spd, rawMixingData(i, 1));
    goodness = lightGoodness(Rf, Rg, [X Y Z], [Xw Yw Zw], targetRg);
    
    % Greates Rp so far -> update
    if goodness > cctBins(cctBin, 1)
        cctBins(cctBin, 1) = goodness;
        cctBins(cctBin, 2) = i;
    end

end

% Copy to mixing data
mixingData = zeros(length(cctBins), length(leds) + 1);
for i = 1:length(cctBins)
    if cctBins(i, 2) == 0 % index is 0 -> missed bin
        continue;
    end
    mixingData(i, :) = rawMixingData(cctBins(i, 2), :);
end

% Remove empty bins
mixingData(mixingData(:, 1) == 0, :) = [];

% Add singular LED ccts
if ~multiLedMixing

    for i = 1:length(leds)
        % Does not exist yet
        if ~find(mixingData(:, 1) == leds(i).cct)
            row = zeros(1, size(mixingData, 2));
            % 1st is the cct
            row(1) = leds(i).cct;
            % LEDs coeff is 1, others are left 0
            row(i + 1) = 1;
            % Add to mixing data
            mixingData = [mixingData row];
        end
    end
end

%%
binSelectionIn = cputime - t

%% Generate spectrums for each 10 Kelvins based on polynomial fit functions
% Array of CCTs
ccts = minCCT:10:maxCCT;
% Spectrums generated with mixing coefficients
% Each row contains one spectrum
spds = zeros(length(ccts), length(L));
% Reference spectrums to which the mixed spectrums are compared
% Uses black body radiator below 5000K and IlluminantD above 5000K
%refs = spds;
% Color rendering indexes for each generated spectrum
%cris = zeros(length(ccts), 1);
% Rf and Rg for each spectrum
Rfs = zeros(length(ccts), 1);
Rgs = zeros(length(ccts), 1);
goodnesses = zeros(length(ccts), 1);
duvs = zeros(length(ccts), 1);
% Luminous Efficacy Radiation functions
LERs = zeros(length(ccts), 1);
% Maximum lumens
maxLumens = zeros(length(ccts), 1);
% Mixing coefficients based on polynomial fit functions
% Each row contains mixing coefficients for respective spectrum
% Columns red, warm white and cold white coefficients repectively
fitCoeffs = zeros(length(ccts), length(leds));
% True coefficients needed for LED mixing with taking powers into account
trueCoeffs = zeros(length(ccts), length(leds));
for i = 1:length(ccts)
    % Save mixing coefficients
    %fc = estimateCoeffs(ccts(i), mixingData);
    %size(fc)
    fitCoeffs(i, :) = estimateCoeffs(ccts(i), mixingData);
    
    % Generate mixed spectrum with the generated mixing coefficients
    spds(i, :) = mixSpd(ledSpds, fitCoeffs(i, :)');
    
    % Save CRI for the generated spectrum
    % Save Rf and Rg for the generated spectrum
    [Rfs(i), Rgs(i)] = spdToRfRg(spds(i, :));
    [~, ~, ~, X, Y ,Z] = spdToXyz(spd);
    [~, ~, ~, Xw, Yw ,Zw] = spdToXyz(refSpd(ccts(i)));
    [goodnesses(i), duvs(i)] = lightGoodness(Rfs(i), Rgs(i), [X Y Z], [Xw Yw Zw], targetRg);

    % Save luminous efficacy of spectrum normalized to Y=100
    %LERs(i) = spdToLER(spds(i, :));
    
    % Save max lumens and true coefficients
    [maxLumens(i), trueCoeffs(i, :)] = calMaxLumens(leds, fitCoeffs(i, :)');
    
    % Generate reference spectrum with current CCT
    % Scale reference spectrum so that luminosity outputs for mixed spectrum
    % and reference spectrum are equal
    %refs(i, :) = refs(i, :) * (Y / Yw);
end

%%
interpolationIn = cputime - t

%% Plot results
figure;
colors = [
    0         0.4470    0.7410
    0.8500    0.3250    0.0980
    0.9290    0.6940    0.1250
    0.4940    0.1840    0.5560
    0.4660    0.6740    0.1880
    0.3010    0.7450    0.9330
    0.6350    0.0780    0.1840
];


%% Plot mixing coefficients
% CCTs on the X-axis, mixing coefficients on the Y-axis.
% Plot CCTs and mixing coefficients for all LEDs from raw mixing data, and
% CCTs and mixing coefficients from spectrums generated with estimated
% mixing coefficients.
% Both should be almost indetical, if they are not then the polynomial fit
% failed somehow.
subplot(2,2,1);
hold on;
legendMatrix = cell(length(leds)*2, 1);
for i = 1:length(leds)
    plot(mixingData(:,1), mixingData(:,i+1), 'o', 'Color', colors(i,:));
    legendMatrix{(i-1)*2+1} = strcat([leds(i).name, ' raw']);
    plot(ccts, fitCoeffs(:,i), 'Color', colors(i,:));
    legendMatrix{(i-1)*2+2} = strcat([leds(i).name, ' fitted']);
end
title('Relative power coefficients');
legend(legendMatrix);
xlabel('CCT (K)');
ylabel('Relative LED Power');
axis([minCCT maxCCT -0.2 1.2]);
grid on;

%% Plot true coefficients
subplot(2,2,2);
hold on;
legendMatrix = cell(length(leds), 1);
for i = 1:length(leds)
    plot(ccts, trueCoeffs(:,i), 'LineWidth', 1.5, 'Color', colors(i,:));
    legendMatrix{i} = leds(i).name;
end
title('True power coefficients');
legend(legendMatrix);
xlabel('CCT (K)');
ylabel('Relative LED Power');
axis([minCCT maxCCT -0.2 1.2]);
grid on;

%% Plot Rf, Rg and Rp
subplot(2,2,3);
plot(ccts, Rfs, ccts, Rgs, ccts, goodnesses, 'linewidth', 1.5);
axis([minCCT maxCCT 50 125]);
title('Fidelity (Rf), Saturation (Rg) and Goodness');
xlabel('CCT (K)');
legend('Rf', 'Rg', 'Goodness');
grid on;

%% Plot max lumens
subplot(2,2,4);
plot(ccts, maxLumens, 'linewidth', 1.5);
axis([minCCT maxCCT 0 max(maxLumens)*1.2]);
title('Max Lumens per Meter');
xlabel('CCT (K)');
ylabel('Luminocity (lm/m)');
grid on;

%% Plot Luminous efficacy radiation function
%{
subplot(2,2,4);
plot(ccts, LERs);
title('Luminous Efficacy of Radiation');
xlabel('CCT (K)');
ylabel('LER (lm/w)');
axis([minCCT maxCCT 150 300]);
grid on;
%}

%% Set supertitle
if ~isempty('supertitle')
    suptitle(supertitle);
end

%%
allButInspectionsIn = cputime - t

%% Inspect spectrums at 2000K, 2700K, 4000K and 5600K
if inspectSpds
    plotCcts = [1500, 2000, 2800, 4000, 5600];
    for i = 1:length(plotCcts)
       inspectSpd(mixSpd(ledSpds, estimateCoeffs(plotCcts(i), mixingData)), targetRg, supertitle);
    end
end

duration = cputime - t