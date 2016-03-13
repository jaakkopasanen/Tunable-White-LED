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
mode = 3; % LED mixing mode: 2 for 2 LED mixing, 3 for 3 LED mixing
inspectSpds = true; % Inspect SPDs for various color temperatures?

load('cie.mat'); % Load lookup tables for colorimetry calculations
load('led_data.mat'); % Load spectrums for various LEDs

%% Spectrum for red LED
% Gaussian distribution from 380nm to 780nm with center in 630nm
red = gaussmf(380:5:780, [20 625]); redL = 320;
%red = Yuji_Red;

%% RGB model
%red = gaussmf(380:5:780, [20 625]); redL = 130;
%warm = gaussmf(380:5:780, [20 520]); warmL = 200;
%cold = gaussmf(380:5:780, [20 465]); coldL = 80;

%% Spectrum for warm white LED
warm = Yuji_BC2835L_2700K; warmL = 1400; warmL = 700;
%warm = Yuji_BC2835L_3200K; warmL = 1400;
%warm = Yuji_BC5730L_2700K; warmL = 900;
%warm = Yuji_BC5730L_3200K; warmL = 900;
%warm = Yuji_VTC5730_2700K; warmL = 800;
%warm = Yuji_VTC5730_3200K; warmL = 800;
%warm = Cree_A19_2700K; warmL = 350;
%warm = Generic_3000K; warmL = 350;

%% Spectrum for cold white LED
cold = Yuji_BC2835L_5600K; coldL = 1700;% coldL = 900;
%cold = Yuji_BC5730L_5600K; coldL = 1000;
%cold = Yuji_BC5730L_6500K; coldL = 1000;
%cold = Yuji_VTC5730_5600K; coldL = 1000;
%cold = Generic_6500K; coldL = 1000;
%cold = Generic_10000K; coldL = 350;

%%
supertitle = 'Red 625nm + Generic 3000K + Generic 6500K';

%% Radiation powers for LEDs
redLER = spdToLER(red);
warmLER = spdToLER(warm);
coldLER = spdToLER(cold);
redP = redL / redLER;
warmP = warmL / warmLER;
coldP = coldL / coldLER;

%% Correlated color temperatures for each LED
redT = spdToCct(red);
warmT = spdToCct(warm);
coldT = spdToCct(cold);

i = 1;
% Data container for raw mixing results
% Each row is result of one coefficient mixing pair
% 1st column: CRI, 2nd and 3rd columns: Rf and Rg, columns 4 to 6: 
% coefficients for red LED, warm white LED and cold white LED, respectively
mixingData = zeros(2/resolution + 1, 4);

%% Mix 2 leds: red and warm up to warm led CCT, warm and cold up to cold led CCT
if mode == 2
    % Mix red and warm white LEDs
    % Iterate coefficient for warm LED from 0 to 1 with specified resolution
    for c = 0:resolution:1
        % Normalize factors
        a = c; b = 1 - a; k = 1 / max(a, b); %a = k*a; b = k*b;
        % Create mix spectrum with mix factors
        % Coefficients for warm white LED and red LED must sum to 1
        spd = mixSpd([red; warm], [b; a]);
        cct = spdToCct(spd);
        % Add row to data matrix
        % Mixing only red and warm white, coefficient for cold white is zero
        mixingData(i,:) = [cct; b; a; 0];
        i = i + 1;
    end
    % Mix warm white LED and cold white LED
    for c = resolution:resolution:1
        a = c; b = 1 - a; k = 1 / max(a, b); %a = k*a; b = k*b;
        spd = mixSpd([warm; cold], [a; b]);
        cct = spdToCct(spd);
        % Mixing only warm white and cold white, coefficient for red is zero
        mixingData(i,:) = [cct; 0; a; b];
        i = i + 1;
    end
    
    % Fit polynomial functions for warm white LED
    % p matrix contains coefficients for two polynomial functions, one per row
    % First function is for estimating warm white LED coefficients for CCTs
    % below warm white LED CCT
    % Second function is for estimating warm white LED coefficients for CCTs
    % above warm white LED CCT
    p = zeros(2, polynomialOrder + 1);
    % Find the index of mixing data where warm white LED coefficient is 1
    [~, I] = max(mixingData(:, 3));
    % Last index of mixing data
    N = length(mixingData);
    % Find coefficients for polynomial fit function for warm LED
    % Please note that Matlab may give a warning for polynomial being badly
    % conditioned. However this is most likely of no concern, since fit will
    % probably still be good enough to produce almost perfect results.
    % Check the results from plots generated in the end of this script. If the
    % fit is good, the CCT vs CRI and CCT vs coefficients plots are identical
    warmDataRedCct = mixingData(1:I,1);
    warmDataRed = mixingData(1:I,3);
    warmDataColdCct = mixingData(I+1:N,1);
    warmDataCold = mixingData(I+1:N,3);
    p(1, :) = polyfit(mixingData(1:I,1), mixingData(1:I,3), polynomialOrder);
    p(2, :) = polyfit(mixingData(I+1:N,1), mixingData(I+1:N,3), polynomialOrder);

%% Mix all 3 LEDs
elseif mode == 3
    rawMixingData = mixingData;
    
    for r = 1:-resolution:0
        for w = 1-r:-resolution:0
            spd = mixSpd([red; warm; cold], [r, w, 1-r-w]);
            cct = spdToCct(spd);
            rawMixingData(i, :) = [cct r w 1-r-w];
            i = i + 1;
        end
    end
    
    binSize = 100;
    cctBins = zeros(maxCCT / binSize - minCCT / binSize + 1, 3);
    
    for i = 1:length(rawMixingData)
        % Skip results outside of CCT range
        if rawMixingData(i, 1) < minCCT
            continue;
        elseif rawMixingData(i, 1) > maxCCT
            continue;
        end
        
        cctBin = floor(rawMixingData(i, 1) / binSize) - minCCT / binSize + 1;
        
        spd = mixSpd([red; warm; cold], rawMixingData(i, 2:4));
        [Rf, Rg, Rp] = spdToRfRg(spd);
        
        if Rp > cctBins(cctBin, 2)
            cctBins(cctBin, 2) = Rp;
            cctBins(cctBin, 3) = i;
        end
        
    end
    
    mixingData = zeros(length(cctBins), 4);
    
    for i = 1:length(cctBins)
        if cctBins(i, 3) == 0
            continue;
        end
        mixingData(i, :) = rawMixingData(cctBins(i, 3), :);
    end
    
    % Delete rows without samples
    mixingData(mixingData(:, 1) == 0, :) = [];
    
else
    error('Mode must be 2 or 3');
end

%% Generate spectrums for each 10 Kelvins based on polynomial fit functions
% Array of CCTs
ccts = minCCT:10:maxCCT;
% Spectrums generated with mixing coefficients
% Each row contains one spectrum
spds = zeros(length(ccts), length(L));
% Reference spectrums to which the mixed spectrums are compared
% Uses black body radiator below 5000K and IlluminantD above 5000K
refs = spds;
% Color rendering indexes for each generated spectrum
%cris = zeros(length(ccts), 1);
% Rf and Rg for each spectrum
Rfs = zeros(length(ccts), 1);
Rgs = zeros(length(ccts), 1);
Rps = zeros(length(ccts), 1);
% Luminous Efficacy Radiation functions
LERs = zeros(length(ccts), 1);
% Maximum lumens
maxLumens = zeros(length(ccts), 1);
% Mixing coefficients based on polynomial fit functions
% Each row contains mixing coefficients for respective spectrum
% Columns red, warm white and cold white coefficients repectively
fitCoeffs = zeros(length(ccts), 3);
% True coefficients needed for LED mixing with taking powers into account
trueCoeffs = zeros(length(ccts), 3);
for i = 1:length(ccts)
    % Save mixing coefficients
    fitCoeffs(i, :) = estimateCoeffs(ccts(i), mixingData);
    
    % Generate mixed spectrum with the generated mixing coefficients
    spds(i, :) = mixSpd([red; warm; cold], fitCoeffs(i, :)');
    
    % Save CRI for the generated spectrum
    %cris(i) = spdToCri(spds(i, :));
    % Save Rf and Rg for the generated spectrum
    [Rfs(i), Rgs(i), Rps(i)] = spdToRfRg(spds(i, :));
    
    % Save luminous efficacy of spectrum normalized to Y=100
    LERs(i) = spdToLER(spds(i, :));
    
    % Save max lumens and true coefficients
    [maxLumens(i), trueCoeffs(i, :)] = calMaxLumens([redLER; warmLER; coldLER], [redP; warmP; coldP], fitCoeffs(i, :)');
    
    % Generate reference spectrum with current CCT
    refs(i, :) = refSpd(ccts(i));
    % Scale reference spectrum so that luminosity outputs for mixed spectrum
    % and reference spectrum are equal
    [~, ~, ~, ~, Y_spd] = spdToXyz(spds(i,:));
    [~, ~, ~, ~, Y_ref] = spdToXyz(refs(i,:));
    refs(i, :) = refs(i, :) * (Y_spd / Y_ref);
end


%% Plot results
figure;

%% Plot mixing coefficients
% CCTs on the X-axis, mixing coefficients on the Y-axis.
% Plot CCTs and mixing coefficients for all LEDs from raw mixing data, and
% CCTs and mixing coefficients from spectrums generated with estimated
% mixing coefficients.
% Both should be almost indetical, if they are not then the polynomial fit
% failed somehow.
subplot(2,2,1);
plot(...
    mixingData(:,1), mixingData(:,2), 'ro', ccts, fitCoeffs(:,1), 'r',... % Red
    mixingData(:,1), mixingData(:,3), 'go', ccts, fitCoeffs(:,2), 'g',... % Warm
    mixingData(:,1), mixingData(:,4), 'bo', ccts, fitCoeffs(:,3), 'b');   % Cold
title('Relative power coefficients');
legend('Red', 'Red fitted', 'Warm', 'Warm fitted', 'Cold', 'Cold fitted');
xlabel('CCT (K)');
ylabel('Relative LED Power');
axis([minCCT maxCCT -0.2 1.2]);
grid on;

%% Plot true coefficients
subplot(2,2,2);
plot(...
    ccts, trueCoeffs(:,1), 'r',... % Red
    ccts, trueCoeffs(:,2), 'g',... % Warm
    ccts, trueCoeffs(:,3), 'b',... % Cold
    'linewidth', 1.5);   
title('True power coefficients');
legend('Red', 'Warm', 'Cold');
xlabel('CCT (K)');
ylabel('Relative LED Power');
axis([minCCT maxCCT -0.2 1.2]);
grid on;

%% Plot Rf, Rg and Rp
subplot(2,2,3);
plot(ccts, Rfs, ccts, Rgs, ccts, Rps, 'linewidth', 1.5);
axis([minCCT maxCCT 75 125]);
title('Fidelity (Rf), Saturation (Rg) and Preference (Rp)');
xlabel('CCT (K)');
legend('Rf', 'Rg', 'Rp');
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
if ~exist('supertitle', 'var')
    suptitle(supertitle);
end

%% Inspect spectrums at 2000K, 2700K, 4000K and 5600K
if inspectSpds
    plotCcts = [2800, 4000, 5600];
    for i = 1:length(plotCcts)
       inspectSpd(mixSpd([red;warm;cold], estimateCoeffs(plotCcts(i), mixingData)));
    end
end

duration = cputime - t