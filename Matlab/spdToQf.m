function [ Qa, Qi ] = spdToQf( spd )
%SPDTOQF Calculates Color quality scale fidelity score

% Load test colors if not yet loaded
persistent cqsTestColors;
if isempty(cqsTestColors)
    load('cqs.mat', 'cqsTestColors');
end

% MCMCCAT chromatic adaptation matrix
MCMCCAT = [
    0.7982    0.3389   -0.1371
   -0.5918    1.5512    0.0406
    0.0008    0.0239    0.9753
];

% Calculate color temperature
CCT = spdToCct(spd);

% Test illuminant colorimetry
[~, ~, ~, X_t, Y_t, Z_t, K_t] = spdToXyz(spd, 2);
X_t = K_t*X_t;
Y_t = K_t*Y_t;
Z_t = K_t*Z_t;
RGB_t = (MCMCCAT * [X_t/Y_t; 1; Z_t/Y_t])';
RGBc_t = zeros(1, 3);
L_t = 1000;

% Reference illuminant colorimetry
ref = refSpd(CCT);
[~, ~, ~, X_r, Y_r, Z_r, K_r] = spdToXyz(ref, 2);
X_r = K_r*X_r;
Y_r = K_r*Y_r;
Z_r = K_r*Z_r;
RGB_r = (MCMCCAT * [X_r/Y_r; 1; Z_r/Y_r])';
RGBc_r = zeros(1, 3);
L_r = 1000;

% Mythical D variable
D = 0.08*log(L_t + L_r) + 0.76 - 0.45 * (L_t - L_r) / (L_t + L_r);
D = min(D, 1);

% Chromatically adapted RGB coordinates
for i = 1:3
    RGBc_t(i) = (D * (RGB_r(i) / RGB_t(i)) + 1 - D) * RGB_t(i) * Y_t;
    RGBc_r(i) = (D * (RGB_r(i) / RGB_t(i)) + 1 - D) * RGB_r(i) * Y_r;
end

% Chromatically adapted XYZ coordinates
XYZc_t = (inv(MCMCCAT) * RGBc_t')';


% For each test color sample
% 1. Calculate X_t, Y_t, Z_t from 2degObserver .* spd .* sampleSpd

    function [L, a, b] = spdToLab( spd, K, doChromaticAdaptation )
        % Calculate colorimetry for test color sample under test illuminant
        [~, ~, ~, X, Y, Z] = spdToXyz(spd, 2);
        % Scale luminance with emitted luminance since this is reflectance
        X = X*K;
        Y = Y*K;
        Z = Z*K;
        
        % Do CMCCAT200 chromatic adaptation for test illuminant
        if doChromaticAdaptation
            RGB = (MCMCCAT * [X/Y; 1; Z/Y])';
            RGBc = zeros(1, 3);
            for j = 1:3
                RGBc(j) = (D * (RGB_r(j) / RGB_t(j)) + 1 - D) * RGB(j) * Y;
            end
            XYZ = (inv(MCMCCAT) * RGBc')';
            X = XYZ(1); Y = XYZ(2); Z = XYZ(3);
        end
        
        % Convert to CIELAB
        [L, a, b] = xyzToLab(X, Y, Z, X_r, Y_r, Z_r);
    end

dEp = zeros(1, 15);
Qi = dEp;
for i = 1:15
   % Calculate colorimetry for test color sample under test illuminant
   [Lab_t(1), Lab_t(2), Lab_t(3)] = spdToLab(spd .* cqsTestColors(i, :), K_t, true);
   % ab distance
   Cab_t = sqrt(Lab_t(2)^2 + Lab_t(3)^2);

   % Calculate colorimetry for test color sample under reference illuminant
   [Lab_r(1), Lab_r(2), Lab_r(3)] = spdToLab(ref .* cqsTestColors(i, :), K_r, false);
   % ab distance
   Cab_r = sqrt(Lab_r(2)^2 + Lab_r(3)^2);
   
   % Calculate error
   % Lab coordinate distance
   dE = sqrt(sum((Lab_r - Lab_t).^2));
   % ab distance difference
   dCap = Cab_t - Cab_r;
   % Limit between [0,10]
   dCap_lim = min(max(dCap, 0), 10);
   % Coordinate distance with saturation factor
   dEp(i) = sqrt(dE^2 - dCap_lim^2);
   % Individual CQS
   Qi(i) = 10*log(exp((100 - 3.2*dEp(i)) / 10) + 1);
end
% General error
dE = sqrt(sum(dEp.^2)/15);
% General CQS
Qa = 10*log(exp((100 - 3.2*dE) / 10) + 1);

end

