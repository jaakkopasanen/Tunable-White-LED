function [ Rp ] = RfRgToRp( Rf, Rg, targetRg )
%RFRGTORP Calculates color preference Rp from fidelity and gamut

if ~exist('targetRg', 'var')
    targetRg = 110;
end

cfactor = 1;

% Maximum Rf for given Rg
maxRf = 100 - abs(100 - Rg);
% Distance from (maxRf,targetRg) point with weighting for Rg
d = sqrt((2*(targetRg - Rg))^2 + (1*(maxRf - Rf))^2);
% Force to range [0,100]. No negative values!
Rp = 10*log(exp((100 - cfactor * d) / 10) + 1);

end

