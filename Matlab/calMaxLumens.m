function [ maxLumens, trueCoeffs ] = calMaxLumens( leds, coeffs )
%CALMAXLUMENS Calculates maximum lumens
%Syntax
%   [maxLumens, trueCoeffs] = calMaxLumens(LERs, powers, coeffs); 
%Input
%   coeffs := Column vector of mixing coefficients for LEDS
%Output
%   maxLumen   := Maximum lumens achievable with given leds and coeffs
%   trueCoeffs := True coeffients needed for mixing given LEDs

powers = zeros(length(leds), 1);
LERs = powers;
for i = 1:length(leds)
    powers(i) = leds(i).power;
    LERs(i) = leds(i).ler;
end

K = 1 / max(coeffs);
normalizedCoeffs = K*coeffs;
scaledCoeffs = normalizedCoeffs ./ powers;
trueCoeffs = scaledCoeffs * (1/max(scaledCoeffs));
truePowers = trueCoeffs .* powers;
maxLumens = sum(truePowers .* LERs);

end

