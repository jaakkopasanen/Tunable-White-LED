clear; clc;
load('led_data.mat'); % Load spectrums for various LEDs
L = 380:5:780;
red = gaussmf(380:5:780, [20 625]); redL = 320;
warm = Yuji_BC2835L_2700K; warmL = 1400;
cold = Generic_6500K; coldL = 350;

LERs = zeros(1, 3);
LERs(1) = spdToLER(red);
LERs(2) = spdToLER(warm);
LERs(3) = spdToLER(cold);

% Radiation powers
redP = redL / LERs(1);
warmP = warmL / LERs(2);
coldP = coldL / LERs(3);
powers = [redP warmP coldP];

coeffs = [0.3 0.1 0.6];

spd = mixSpd([red;warm;cold], coeffs');

coeffs
K = 1 / max(coeffs);
normalizedCoeffs = K*coeffs
powers
scaledCoeffs = normalizedCoeffs ./ powers
normalizedScaledCoeffs = scaledCoeffs * (1/max(scaledCoeffs))
truePowers = normalizedScaledCoeffs .* powers
LERs
maxLumens = sum(truePowers .* LERs)

SPD = mixSpd(bsxfun(@times, [red;warm;cold], powers'), normalizedScaledCoeffs');
plot(L, spd, L, SPD, '--', 'linewidth', 1.5);
legend('W/o powers', 'W/ powers');