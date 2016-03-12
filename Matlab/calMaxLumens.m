function [ maxLumens, trueCoeffs ] = calMaxLumens( LERs, powers, coeffs )
%CALMAXLUMENS Calculates maximum lumens
%Syntax
%   [maxLumens, trueCoeffs] = calMaxLumens(LERs, powers, coeffs); 
%Input
%   LERs   := Column vector of luminous efficacies of radiations for LEDs
%   powers := Column vector of LED maximum powers
%   coeffs := Column vector of mixing coefficients for LEDS
%Output
%   maxLumen   := Maximum lumens achievable with given leds and coeffs
%   trueCoeffs := True coeffients needed for mixing given LEDs

K = 1 / max(coeffs);
normalizedCoeffs = K*coeffs;
scaledCoeffs = normalizedCoeffs ./ powers;
trueCoeffs = scaledCoeffs * (1/max(scaledCoeffs));
truePowers = trueCoeffs .* powers;
maxLumens = sum(truePowers .* LERs);

end

