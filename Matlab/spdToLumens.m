function [ lumens ] = spdToLumens( spd )
%SPDTOLUMENS Calculates lumens for spectral power distribution
%   lumens = spdToLumens(spd) for spectral power distribution spd from
%   380nm to 780nm sampled at 5nm returns lumen output
%
%   This is basically just spectral luminous efficiency scaling!

persistent cieSpectralLuminousEfficiency;
if isempty(cieSpectralLuminousEfficiency)
    % Creates variable cieRa95TestColors
    % each column is spectral radiance factors for CIE CRI Ra95 test color
    % samples 1 - 14 respectively
    % Wavelengths span 380nm to 780nm with 5nm sampling
    load('cie.mat', 'cieSpectralLuminousEfficiency');
end
lumens = sum(cieSpectralLuminousEfficiency.*spd)/400;

end

