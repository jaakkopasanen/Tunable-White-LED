clear; t = cputime;

load('cie.mat');
load('led_data.mat');

L = 380:5:780; % Wavelengths
resolution = 0.05; % LED mixing coefficient resolution
polynomialPower = 3;
minCCT = 1000;
maxCCT = 6800;
redWavelength = 630; % Red LED wavelength peak

% Gaussian distribution from 380nm to 780nm with center in redWavelength
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
i = 1;
% Matrix of coefficients, 1st column is the cct last ones are the factors
coeffs = zeros(2/resolution + 1, 5);
% Mix red and warm white LEDs
for w = 0:resolution:1
    % Create mix spectrum with mix factors
    spd = mixSpd([red; warm], [1 - w; w]);
    % Calculate correlated color temperature
    [cri, cct] = spdToCri(spd);
    % Add row to data matrix
    coeffs(i,:) = [cct; cri; 1 - w; w; 0];
    i = i + 1;
end
% Mix warm white and cold white LED
for w = 0:resolution:1
    spd = mixSpd([warm; cold], [w; 1-w]);
    [cri, cct] = spdToCri(spd);
    coeffs(i,:) = [cct; cri; 0; w; 1-w];
    i = i + 1;
end

% Fit second order polynomials for each led mixing factors
p = zeros(2, polynomialPower + 1);
[~, I] = max(coeffs(:, 4));
N = length(coeffs);
p(1, :) = polyfit(coeffs(1:I,1), coeffs(1:I,4), polynomialPower); % Warm below break
p(2, :) = polyfit(coeffs(I+1:N,1), coeffs(I+1:N,4), polynomialPower); % Warm above break

redT = spdToCct(red);
warmT = spdToCct(warm);
coldT = spdToCct(cold);

% TODO: generate fitted coefficients based on fit factors
% below and above break off temperature
ccts = minCCT:10:maxCCT;
spds = zeros(length(ccts), length(L));
refs = spds;
cris = zeros(length(ccts), 1);
fitCoeffs = zeros(length(ccts), 3);
for i = 1:length(ccts)
    T = ccts(i);
    if T < warmT
        warmC = min(max(polyval(p(1,:), T), 0), 1);
        redC = 1 - warmC;
        coldC = 0;
    else
        redC = 0;
        warmC = min(max(polyval(p(2,:), T), 0), 1);
        coldC = 1 - warmC;
    end
    fitCoeffs(i, :) = [redC, warmC, coldC];
    spds(i, :) = mixSpd([red; warm; cold], fitCoeffs(i, :)');
    cris(i) = spdToCri(spds(i, :));
    refs(i, :) = refSpd(i);
end

figure;

% Plot mesh
subplot(2,2,1);
mesh(L, ccts, spds);
xlabel('Wavelength (nm)');
ylabel('CCT (K)');
zlabel('Relative LED Power');
title('SPD mesh');
axis([380 780 minCCT maxCCT 0 100])
grid on;

% Plot CRI vs CCT
subplot(2,2,2);
plot(coeffs(:,1), coeffs(:,2), 'o', ccts, cris);
axis([minCCT maxCCT 50 100]);
title('CRI vs CCT');
xlabel('CCT (K)');
ylabel('CRI');
legend('By raw coefficients', 'By fitted coefficients');
grid on;

% Plot mixing coefficients
subplot(2,2,[3,4]);
plot(...
    coeffs(:,1), coeffs(:,3), 'ro', ccts, fitCoeffs(:,1), 'r',... % Red
    coeffs(:,1), coeffs(:,4), 'go', ccts, fitCoeffs(:,2), 'g',... % Warm
    coeffs(:,1), coeffs(:,5), 'bo', ccts, fitCoeffs(:,3), 'b');   % Cold
title('LED power coefficients');
legend('Red', 'Red fitted', 'Warm', 'Warm fitted', 'Cold', 'Cold fitted');
xlabel('CCT (K)');
ylabel('Relative LED Power');
axis([minCCT maxCCT -0.3 1.3]);
grid on;

% Print coefficients to command window
format shorteng;

%{
disp(strcat(['Red, T <= ', num2str(redT), 'K : ', num2str(1)]));
disp(strcat(['Red, T < ', num2str(warmT), 'K : ', num2str(1 - p(1,:))]));
disp(strcat(['Red, T >= ', num2str(warmT), 'K : ', num2str(0)]));
disp('-----------------------------');
disp(strcat(['Warm, T < ', num2str(redT), 'K : ', num2str(0)]));
disp(strcat(['Warm, T <= ', num2str(warmT), 'K : ', num2str(p(1,:))]));
disp(strcat(['Warm, T > ', num2str(warmT), 'K : ', num2str(p(2,:))]));
disp(strcat(['Warm, T >= ', num2str(coldT), 'K : ', num2str(0)]));
disp('-----------------------------');
disp(strcat(['Cold, T <= ', num2str(warmT), 'K : ', num2str(0)]));
disp(strcat(['Cold, T > ', num2str(warmT), 'K : ', num2str(1 - p(2,:))]));
disp(strcat(['Cold, T >= ', num2str(coldT), 'K : ', num2str(1)]));
%}

disp('T<=830K'); format short;
disp(1);
disp(0);
disp(0);

disp(strcat(['830K<T<', num2str(warmT), 'K'])); format shorteng;
disp(1 - p(1,:));
disp(p(1,:));
disp(zeros(1, polynomialPower+1));

disp(strcat(['T==', num2str(warmT), 'K'])); format short;
disp(0);
disp(1);
disp(0);

disp(strcat([num2str(warmT), 'K<T<=', num2str(coldT), 'K'])); format shorteng;
disp(zeros(1, polynomialPower+1));
disp(p(2,:));
disp(1 - p(2,:));

disp(strcat(['T>=', num2str(coldT), 'K'])); format short;
disp(0);
disp(0);
disp(1);


%duration = cputime - t