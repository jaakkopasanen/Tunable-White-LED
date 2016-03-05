function [ spd ] = refSpd( T, linearTransition )
%REFSPD Generates reference spd
%   spd = refSpd(T) for temperature T in Kelvins returns spectral power
%   distribution of reference illumination source
%
%   Uses Planckian radiator for temperatures less than 5000K and CIE
%   illuminant D (daylight) for temperatures over 5000K

% Load planck spd data
persistent planckSpd;
if isempty(planckSpd)
    % Creates variable planckSpd
    % each row is spectral power distribution for planck radiator at
    % temperature
    % Temperatures range from 1K to 25000K with 1K sampling rate
    load('cie.mat', 'planckSpd');
end

% Load CIE illuminant D data
persistent cieIlluminantDSpd;
if isempty(cieIlluminantDSpd)
    % Creates variable cieIlluminantDSpd
    % each row is spectral power distribution for illuminant D at
    % temperature
    % Temperatures range from 1K to 25000K with 1K sampling rate
    load('cie.mat', 'cieIlluminantDSpd');
end

if ~exist('linearTransition', 'var')
    linearTransition = false;
end

% Use linear transition from Planckian radiator to IlluminantD in the CCT
% range of 4500K to 5500K
if linearTransition
    if T < 4500 % Planckian radiator
        spd = planckSpd(T, :);
    elseif T < 5500 % Linear combination
        c = (5500 - T) / 1000;
        spd = sum(bsxfun(@times,[planckSpd(T, :), cieIlluminantDSpd(T, :)],[c; 1-c]));
    else % Illuminant D
        spd = cieIlluminantDSpd(T, :);
    end
    
% Normal hard breakoff point in the 5000K
else
    if T < 5000
        spd = planckSpd(T, :);
    else
        spd = cieIlluminantDSpd(T, :);
    end
end

end

