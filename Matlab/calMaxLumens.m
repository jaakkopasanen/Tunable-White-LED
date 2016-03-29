function [ maxLumens, trueCoeffs ] = calMaxLumens( leds, coeffs )
%CALMAXLUMENS Calculates maximum lumens
%Syntax
%   [maxLumens, trueCoeffs] = calMaxLumens(leds, coeffs); 
%Input
%   leds   := Leds
%   coeffs := Column vector of mixing coefficients for LEDS
%Output
%   maxLumen   := Maximum lumens achievable with given leds and coeffs
%   trueCoeffs := True coeffients needed for mixing given LEDs

powers = zeros(length(leds), 1);
LERs = powers;
powerKs = LERs;
for i = 1:length(leds)
    powers(i) = leds(i).power;
    % Power coefficient: LED true power divided by power of LED's
    % normalized spectral power distribution
    powerKs(i) = powers(i) / (sum(leds(i).spd) / 400);
    LERs(i) = leds(i).ler;
end

K = 1 / max(coeffs);
normalizedCoeffs = K*coeffs;
scaledCoeffs = normalizedCoeffs ./ powerKs;
trueCoeffs = scaledCoeffs * (1/max(scaledCoeffs));
truePowers = trueCoeffs .* powers;
maxLumens = sum(truePowers .* LERs);

end

