function [ spd ] = mixSpd( spds, coeffs )
%SUMSPD Sum spectral power distributions and scale peak to 100
%   [spd] = mixSpd(spds, coeffs); returns mixed and scaled spectral power
%   distibution
%
%   Assumes spds to be matrix of row vectors where each row is spectral
%   power distribution from 380nm to 779nm sampled at 1nm.
%
%   Assumes coeffs to be column vector of coefficients.

spd = sum(bsxfun(@times,spds,coeffs));
spd = spd.*(100/max(spd));

end

