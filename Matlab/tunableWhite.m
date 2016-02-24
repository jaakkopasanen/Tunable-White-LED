clear; t = cputime;

load('cie.mat');
load('led_data.mat');

L = 380:5:780; % Wavelengths
resolution = 0.01; % LED mixing coefficient resolution
cctBinResolution = 50;
minCCT = 1000;
maxCCT = 6800;
redWavelength = 630;
mixN = 2; % Basically no need to mix 3 leds

% Gaussian distribution from 380nm to 779nm with center in 670nm
red = gaussmf(380:5:780, [20 redWavelength]).*100;
%red = Yuji_Red;

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

% Yuji BC2835L series
% 1800 lm/m @ 6500K
cold = Yuji_BC2835L_5600K;
% Yuji BC5730L series
% 1000 lm/m @ 5600K
%cold = Yuji_BC5730L_5600K;
%cold = Yuji_BC5730L_6500K;
% Yuji VTC5730L series
% 1000 lm/m @ 5600K
%cold = Yuji_VTC5730_5600K;

% Mix 2 leds: red and warm to warm led CCT, warm and cold to cold led CCT
if mixN == 2
    i = 1;
    data = zeros(2/resolution + 1, 5);
    for r = 0:resolution:1
        spd = mixSpd([red; warm], [r; 1-r]);
        [cri, cct] = spdToCri(spd);
        data(i,:) = [cct;cri;r;1-r;0];
        i = i + 1;
    end
    for w = 0:resolution:1
        spd = mixSpd([warm; cold], [w; 1-w]);
        [cri, cct] = spdToCri(spd);
        data(i,:) = [cct; cri; 0; w; 1-w];
        i = i + 1;
    end
end

% Mix all 3 led for all color temperatures
if mixN == 3
    i = 1;
    data = zeros(sum(1:(1/resolution+1)), 5);
    for r = 0:resolution:1 % Iterate coefficients for red LED
        for w = 0:resolution:(1-r) % Iterate coefficients for warm LED
            % Generate mixed spectrum
            spd = mixSpd([red; warm; cold],[r; w; 1-r-w]);
            % Calculate color rendering index and correlated color temperature
            [cri, cct] = spdToCri(spd);
            % Add row to data matrix where 1st element is cct, 2nd cri and
            % 3rd to 5th are mixing coefficients for red, warm and cold LEDs
            % respectively
            data(i,:) = [cct;cri;r;w;1-r-w];
            i = i + 1;
        end
    end
end

% Sort data by CCT (first column)
[~,I] = sort(data(:,1)); 
data = data(I,:);

% Select mixing coeffs with best CRI rsults
% select highest CRI value for each hundred Kelvins

% 71 bins of hundred Kelvins, row 1 is index, row 2 is max CRI
cctBins = zeros(round((maxCCT - minCCT)/cctBinResolution) + 1, 2);

for i = 1:length(data)
    T = data(i,1);
    CRI = data(i,2);
    if T < minCCT % Exclude everything below minimum temperature
        continue;
    end
    if T > maxCCT
        continue;
    end
    bin = (round((T - minCCT)/cctBinResolution) + 1); % hundred Kelvin bin
    maxCRI = cctBins(bin, 2); % Current highest CRI in hunderd Kelvin bin
    if CRI > maxCRI % New max CRI found
        cctBins(bin, 1) = i; % Save index to original data
        cctBins(bin, 2) = CRI; % Save new highest CRI
    end
end
cctBins(cctBins(:,1) == 0, :) = []; % Remove hundred bins without data
selectedCoeffs = data(cctBins(:, 1), :); % Select data with winning indexes

% Create fit curves for mixing coefficients
ccts = minCCT:100:maxCCT;
coeffs = zeros(length(ccts), 3);

% Create SPDs for each hundred Kelvins
spds = zeros(length(selectedCoeffs), 81);
i = 1;
refs = zeros(length(selectedCoeffs), 81);
for i = 1:length(selectedCoeffs(:,1))
    T = selectedCoeffs(i,1);
    spds(i,:) = mixSpd([red; warm; cold], selectedCoeffs(i,3:5)');
    refs(i,:) = refSpd(T);
    c = spdToLumens(spds(i,:)) / spdToLumens(refs(i,:));
    refs(i,:) = refs(i,:).*c;
    i = i + 1;
end

figure;

% Plot mesh
subplot(2,2,1);
mesh(L, selectedCoeffs(:,1), spds);
xlabel('Wavelength (nm)');
ylabel('CCT (K)');
zlabel('Relative LED Power');
title('SPD mesh');
axis([380 780 minCCT maxCCT 0 100])
grid on;

% Plot CRI vs CCT
subplot(2,2,2);
plot(data(:,1), data(:,2), selectedCoeffs(:,1), selectedCoeffs(:,2), '-o');
axis([minCCT maxCCT 60 100]);
title('CRI vs CCT');
xlabel('CCT (K)');
ylabel('CRI');
grid on;

% Plot mixing coefficients
subplot(2,2,[3,4]);
plot(...
    selectedCoeffs(:,1), selectedCoeffs(:,3),'ro-',... % Red raw data
    selectedCoeffs(:,1), selectedCoeffs(:,4),'go-',... % Warm raw data
    selectedCoeffs(:,1), selectedCoeffs(:,5),'bo-'... % Cold raw data
    );
title('LED power coefficients');
legend('Red', 'Warm', 'Cold');
xlabel('CCT (K)');
ylabel('Relative LED Power');
axis([minCCT maxCCT 0 1]);
grid on;

duration = cputime - t