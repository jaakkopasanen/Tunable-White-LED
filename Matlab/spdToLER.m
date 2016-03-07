function [ LER ] = spdToLER( spd )
%SPDTOLER Calculates luminous efficacy of radiation for spd
%Input:
%   spd := Spectral power distribution
%Output:
%   LER := Luminous efficacy radiation

persistent cieSpectralLuminousEfficiency;
if isempty(cieSpectralLuminousEfficiency)
    load('cie.mat', 'cieSpectralLuminousEfficiency');
end
LER = 683 * sum(cieSpectralLuminousEfficiency.*spd) / sum(spd);

end

