clear; t = cputime;

%% Glossary
% SPD := Spectral power distribution, also called spectrum in this script
% CCT := Correlated color temperature, nearest black body radiator
% CRI := Color rendering index, calculated with all 14 test color samples

%% Declare variables
L = 380:5:780; % Wavelengths: from 380nm to 780nm sampled at 5nm
resolution = 0.05; % LED mixing coefficient resolution
polynomialOrder = 5; % Order of polynomial fit function
minCCT = 800; % Minimum correlated color temperature
maxCCT = 6800; % Maximum correlated color temperature

load('cie.mat'); % Load lookup tables for colorimetry calculations
load('led_data.mat'); % Load spectrums for various LEDs

%% Spectrum for red LED
% Gaussian distribution from 380nm to 780nm with center in 630nm
red = gaussmf(380:5:780, [20 630]);
%red = Yuji_Red;

%% Spectrum for warm white LED
% Yuji BC2835L series
% 1400 lm/m @ 2700K
warm = Yuji_BC2835L_2700K;
%warm = Yuji_BC2835L_3200K;
% Yuji BC5730L series
% 900 lm/m @ 3200K
%warm = Yuji_BC5730L_2700K;
%warm = Yuji_BC5730L_3200K;
% Yuji VTC5730L series
% 800 lm/m @ 3200K
%warm = Yuji_VTC5730_2700K;
%warm = Yuji_VTC5730_3200K;
% Gaussian distribution from 380nm to 780nm with center in 630nm

%% Spectrum for cold white LED
% Yuji BC2835L series
% 1800 lm/m @ 6500K
%cold = Yuji_BC2835L_5600K;
% Yuji BC5730L series
% 1000 lm/m @ 5600K
%cold = Yuji_BC5730L_5600K;
cold = Yuji_BC5730L_6500K;
% Yuji VTC5730L series
% 1000 lm/m @ 5600K
%cold = Yuji_VTC5730_5600K;
% Gaussian distribution from 380nm to 780nm with center in 630nm

%% Mix 2 leds: red and warm up to warm led CCT, warm and cold up to cold led CCT
i = 1;
% Data container for raw mixing results
% Each row is result of one coefficient mixing pair
% 1st column: CRI, 2nd and 3rd columns: Rf and Rg, columns 4 to 6: 
% coefficients for red LED, warm white LED and cold white LED, respectively
mixingData = zeros(2/resolution + 1, 6);

% Mix red and warm white LEDs
% Iterate coefficient for warm LED from 0 to 1 with specified resolution
for c = 0:resolution:1
    % Create mix spectrum with mix factors
    % Coefficients for warm white LED and red LED must sum to 1
    spd = mixSpd([red; warm], [1 - c; c]);
    % Calculate color rendering properties
    %cri = spdToCri(spd);
    [Rf, Rg] = spdToRfRg(spd);
    cct = spdToCct(spd);
    % Add row to data matrix
    % Mixing only red and warm white, coefficient for cold white is zero
    mixingData(i,:) = [cct; Rf; Rg; 1 - c; c; 0];
    i = i + 1;
end
% Mix warm white LED and cold white LED
for c = resolution:resolution:1
    spd = mixSpd([warm; cold], [c; 1-c]);
    %cri = spdToCri(spd);
    [Rf, Rg] = spdToRfRg(spd);
    cct = spdToCct(spd);
    % Mixing only warm white and cold white, coefficient for red is zero
    mixingData(i,:) = [cct; Rf; Rg; 0; c; 1-c];
    i = i + 1;
end

%% Fit polynomial functions for warm white LED
% p matrix contains coefficients for two polynomial functions, one per row
% First function is for estimating warm white LED coefficients for CCTs
% below warm white LED CCT
% Second function is for estimating warm white LED coefficients for CCTs
% above warm white LED CCT
p = zeros(2, polynomialOrder + 1);
% Find the index of mixing data where warm white LED coefficient is 1
[~, I] = max(mixingData(:, 5));
% Last index of mixing data
N = length(mixingData);
% Find coefficients for polynomial fit function for warm LED
% Please note that Matlab may give a warning for polynomial being badly
% conditioned. However this is most likely of no concern, since fit will
% probably still be good enough to produce almost perfect results.
% Check the results from plots generated in the end of this script. If the
% fit is good, the CCT vs CRI and CCT vs coefficients plots are identical
warmDataRedCct = mixingData(1:I,1);
warmDataRed = mixingData(1:I,5);
warmDataColdCct = mixingData(I+1:N,1);
warmDataCold = mixingData(I+1:N,5);
p(1, :) = polyfit(mixingData(1:I,1), mixingData(1:I,5), polynomialOrder);
p(2, :) = polyfit(mixingData(I+1:N,1), mixingData(I+1:N,5), polynomialOrder);

% Correlated color temperatures for each LED
redT = spdToCct(red);
warmT = spdToCct(warm);
coldT = spdToCct(cold);

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
% Luminous Efficacy Radiation functions
LERs = zeros(length(ccts), 1);
% Mixing coefficients based on polynomial fit functions
% Each row contains mixing coefficients for respective spectrum
% Columns red, warm white and cold white coefficients repectively
fitCoeffs = zeros(length(ccts), 3);
for i = 1:length(ccts)
    CCT = ccts(i); % Save current CCT for convenience
    
    % CCT is below warm white CCT -> use first polynomial
    if CCT < warmT
        % Mixing coefficient for warm white LED limited between [0, 1]
        warmC = min(max(polyval(p(1,:), CCT), 0), 1);
        % Mixing coefficient for red LED is what is left from warm white
        redC = 1 - warmC;
        % Cold white LED is not powered on below warm white LED CCT
        coldC = 0;
        
    % CCT is above warm whtie CCT -> use second polynomial
    else 
        % Red LED is not powered on above warm white LED CCT
        redC = 0;
        % Mixing coefficient for warm white LED limited between [0, 1]
        warmC = min(max(polyval(p(2,:), CCT), 0), 1);
        % Mixing coefficient for cold white LED is what is left from warm white
        coldC = 1 - warmC;
    end
    
    % Save mixing coefficients
    fitCoeffs(i, :) = [redC, warmC, coldC];
    % Generate mixed spectrum with the generated mixing coefficients
    spds(i, :) = mixSpd([red; warm; cold], fitCoeffs(i, :)');
    % Save CRI for the generated spectrum
    %cris(i) = spdToCri(spds(i, :));
    % Save Rf and Rg for the generated spectrum
    [Rfs(i), Rgs(i)] = spdToRfRg(spds(i, :));
    % Save luminous efficacy of spectrum normalized to Y=100
    LERs(i) = spdToLER(spds(i, :)); 
    % Generate reference spectrum with current CCT
    refs(i, :) = refSpd(CCT);
    % Scale reference spectrum so that luminosity outputs for mixed spectrum
    % and reference spectrum are equal
    [~, ~, ~, ~, Y_spd] = spdToXyz(spds(i,:));
    [~, ~, ~, ~, Y_ref] = spdToXyz(refs(i,:));
    refs(i, :) = refs(i, :) * (Y_spd / Y_ref);
end


%% Plot results
figure;

%% Plot 3D mesh
% Wavelengths on X-axis, CCTs on Y-axis, SPDs are the spectral power values
subplot(2,2,1);
mesh(L, ccts, spds);
xlabel('Wavelength (nm)');
ylabel('CCT (K)');
zlabel('Relative LED Power');
title('SPD mesh');
axis([380 780 minCCT maxCCT 0 1])
grid on;

%% Plot Rf and Rg vs CCT
% CCT on the X-axis, Rf and Rg on the Y-axis.
% Plot CCTs and CRIs from raw mixing data, and CCTs and CRIs from spectrums
% generated with estimated mixing coefficients.
% Both should be almost indetical, if they are not then the polynomial fit
% failed somehow.
subplot(2,2,2);
plot(mixingData(:,1), mixingData(:,2), 'o', ccts, Rfs, mixingData(:,1), mixingData(:,3), 'o', ccts, Rgs);
axis([minCCT maxCCT 75 125]);
title('Rf and Rg vs CCT');
xlabel('CCT (K)');
legend('Raw Rf', 'Fitted Rf', 'Raw Rg', 'Fitted Rg');
grid on;

%% Plot mixing coefficients
% CCTs on the X-axis, mixing coefficients on the Y-axis.
% Plot CCTs and mixing coefficients for all LEDs from raw mixing data, and
% CCTs and mixing coefficients from spectrums generated with estimated
% mixing coefficients.
% Both should be almost indetical, if they are not then the polynomial fit
% failed somehow.
subplot(2,2,3);
plot(...
    mixingData(:,1), mixingData(:,4), 'ro', ccts, fitCoeffs(:,1), 'r',... % Red
    mixingData(:,1), mixingData(:,5), 'go', ccts, fitCoeffs(:,2), 'g',... % Warm
    mixingData(:,1), mixingData(:,6), 'bo', ccts, fitCoeffs(:,3), 'b');   % Cold
title('LED power coefficients');
legend('Red', 'Red fitted', 'Warm', 'Warm fitted', 'Cold', 'Cold fitted');
xlabel('CCT (K)');
ylabel('Relative LED Power');
axis([minCCT maxCCT -0.3 1.3]);
grid on;

%% Plot Luminous efficacy radiation function
% CCT on the X-axis LER on the Y-axis
subplot(2,2,4);
plot(ccts, LERs);
title('Luminous Efficacy');
xlabel('CCT (K)');
ylabel('LER (lm/w)');
axis([minCCT maxCCT 150 300]);
grid on;

%% Inspect spectrums at 2000K, 2700K, 4000K and 5600K
%
ccts = [2000, 2700, 4000, 5600];
for i = 1:4
   inspectSpd(cctToSpd(ccts(i), [red; warm; cold], p), strcat([num2str(ccts(i)), 'K']));
end
%}

%% Print coefficients for polynomial fit functions to command window
format shorteng;

disp('------------------------------------------------------------------');
disp('Coefficients A, B and C for estimating warm white LED mixing coefficients with given CCT');

% From 0K to red LED CCT
disp('------------------------------------------------------------------');
disp(strcat(['T <= ', num2str(redT), 'K'])); format shorteng;
disp('red = 1');
disp('warm = 0');
disp('cold = 0');

% From red LED CCT to warm white LED CCT
disp('------------------------------------------------------------------');
disp(strcat(['830K < T < ', num2str(warmT), 'K'])); format shorteng;
disp('red = 1 - warm');
disp(strcat(['warm = (', num2str(p(1,1)), '*T^2) + (', num2str(p(1,2)), '*T) + (', num2str(p(1,3)), '*T) + (', num2str(p(1,4)), ')']));
disp('cold = 0');

% Warm white CCT
disp('------------------------------------------------------------------');
disp(strcat(['T == ', num2str(warmT), 'K'])); format shorteng;
disp('red = 0');
disp('warm = 1');
disp('cold = 0');

% From warm white LED CCT to cold white LED CCT
disp('------------------------------------------------------------------');
disp(strcat([num2str(warmT), 'K < T < ', num2str(coldT), 'K'])); format shorteng;
disp('red = 0');
disp(strcat(['warm = (', num2str(p(2,1)), '*T^3) + (', num2str(p(2,2)), '*T^2) + (', num2str(p(2,3)), '*T) + (', num2str(p(2,4)), ')']));
disp('cold = 1 - warm');

% Cold white CCT and above
disp('------------------------------------------------------------------');
disp(strcat(['T >= ', num2str(coldT), 'K'])); format shorteng;
disp('red = 0');
disp('warm = 0');
disp('cold = 1');
disp('------------------------------------------------------------------');

%duration = cputime - t