function [ spd ] = daylight( T )
%DAYLIGHT Summary of this function goes here
%   Detailed explanation goes here

persistent cieIlluminantDSFunctions;
if isempty(cieIlluminantDSFunctions)
    load('cie.mat', 'cieIlluminantDSFunctions');
end

if and(T > 4000, T <= 7000)
    xD = -4.607 * (10 ^ 9 / T ^ 3) + 2.9678 * (10 ^ 6 / T ^ 2) + 0.09911 * (1000 / T) + 0.244063;
elseif T > 7000
    xD = -2.0064 * (10 ^ 9 / T ^ 3) + 1.9018 * (10 ^ 6 / T ^ 2) + 0.24748 * (1000 / T) + 0.23704;
end

yD = -3 * xD ^ 2 + 2.87 * xD - 0.275;
M1 = (-1.3515 - 1.7703 * xD + 5.9114 * yD) / (0.0241 + 0.2562 * xD - 0.7341 * yD);
M2 = (0.03 - 31.4424 * xD + 30.0717 * yD) / (0.0241 + 0.2562 * xD - 0.7341 * yD);
for i = 1:81
    spd(i) = cieIlluminantDSFunctions(1, i) + M1 * cieIlluminantDSFunctions(2, i) + M2 * cieIlluminantDSFunctions(3, 1);
end

end

