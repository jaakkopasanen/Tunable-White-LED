function [ spd ] = mixSpd( spds, coeffs )
%MIXSPD Sum spectral power distributions and scale peak to 100
%   [spd] = mixSpd(spds, coeffs); returns mixed and scaled spectral power
%   distibution
%
%   Assumes spds to be matrix of row vectors where each row is spectral
%   power distribution from 380nm to 779nm sampled at 1nm.
%
%   Assumes coeffs to be column vector of coefficients.

% Coeffs should be column vector, transpose if it's row vector
if size(coeffs, 1) < size(coeffs, 2)
    coeffs = coeffs';
end

spd = sum(bsxfun(@times,spds,coeffs));
%spd = spd.*(1/max(spd));

end

