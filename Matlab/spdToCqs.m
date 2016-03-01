function [ Qa, Qi ] = spdToCqs( spd )
%SPDTOCQS Calculates Color quality scale score from spd

% Load test colors if not yet loaded
persistent cqsTestColors;
if isempty(cqsTestColors)
    load('cie.mat', 'cqsTestColors');
end

% Calculate color temperature
CCT = spdToCct(testSpd);

% Test illuminant colorimetry
[x_t, y_t, z_t, X_t, Y_t, Z_t, K_t] = spdToXyz(spd);

% Reference illuminant colorimetry
ref = refSpd(CCT);
[x_r, y_r, z_r, X_r, Y_r, Z_r, K_r] = spdToXyz(ref);

% For each test color sample
% 1. Calculate X_t, Y_t, Z_t from 2degObserver .* spd .* sampleSpd

    function [L, a, b] = spdToLab( spd, K, doChromaticAdaptation )
        % Calculate colorimetry for test color sample under test illuminant
        [~, ~, ~, X, Y, Z] = spdToXyz(spd);
        % Scale luminance with emitted luminance since this is reflectance
        X = X*K;
        Y = Y*K;
        Z = Z*K;
        
        % Do CMCCAT200 chromatic adaptation for test illuminant
        if doChromaticAdaptation
            
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
   dCap = Cab_r - Cab_t;
   % Limit between [0,10]
   dCap_lim = min(max(dCap, 0), 10);
   % Coordinate distance with saturation factor
   dEp(i) = sqrt(dE^2 - dCap_lim^2);
   % Individual CQS
   Qi(i) = 10*log(exp((100 - 3.2*dEp(i)) / 10) + 1);
end
% General error
dE = sqrt(sum(dEp)/15);
% General CQS
Qa = 10*log(exp((100 - 3.2*dE) / 10) + 1);

end

