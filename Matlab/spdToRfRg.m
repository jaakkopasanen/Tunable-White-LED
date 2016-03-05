function [ Rf, Rg, Rfi, dEi, tcsi, bins ] = spdToRfRg( spd )
%SPDTORFRG Calculates TM-30-15 Rf and Rg metrics from SPD
%Input:
%   spd := Spectral power distribution from 380nm to 780nm sampled at 5nm
%Output:
%   Rf    := Fidelity score [0,100]
%   Rg    := Gamut score
%   Rif   := Special fidelity scores 1 to 99
%   dEi   := Errors for each test color sample
%   binst := CAM02-UCS color coordinates (J, a, b) for 16 color icon bins
%            under test illuminant. Each row contains one bin, 16 rows.
%   binsr := CAM02-UCS color coordinates (J, a, b) for 16 color icon bins
%            under reference illuminant. Each row contains one bin, 16 rows.

% Error/score factor
cfactor = 7.54;

% Calculate colorimetry for test illuminant
[x_t, y_t, ~, X_t, Y_t, Z_t, K_t] = spdToXyz(spd, 10);
X_t = K_t * X_t;
Y_t = K_t * Y_t;
Z_t = K_t * Z_t;

% Calculate correlated color temperature CCT
cct = spdToCct(spd);

% Calculate reference spd
ref = refSpd(cct, true);

% Calculate colorimetry for reference illuminant
[x_r, y_r, ~, X_r, Y_r, Z_r, K_r] = spdToXyz(ref, 10);
X_r = K_r * X_r;
Y_r = K_r * Y_r;
Z_r = K_r * Z_r;

% Errors
dEi = zeros(1, 99);

for i = 1:99
    
end

% Average error
dEavg = mean(dEi);

% Special fidelity scores
Rfi = 10*log(exp((100 - cfactor .* dEi) / 10) + 1);

% General fidelity score
Rf = 10*log(exp((100 - cfactor * dEavg) / 10) + 1);

end

