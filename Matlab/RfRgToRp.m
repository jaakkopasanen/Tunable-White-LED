function [ Rp ] = RfRgToRp( Rf, Rg, targetRg )
%RFRGTORP Calculates color preference Rp from fidelity and gamut

if ~exist('targetRg', 'var')
    targetRg = 110;
end

% Punish for deviating from upper limit
Rp = atan((Rg - 100) / (100 - Rf)) / atan(1) / 2 + 0.5;

% Punish for deviating from <targetRg>
g = gaussmf(0:0.1:200, [20 targetRg]);
Rp = Rp * g(round(Rg*10));

% Scale to 100
Rp = Rp * 100;

end

