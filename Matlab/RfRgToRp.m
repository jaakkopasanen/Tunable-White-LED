function [ Rp ] = RfRgToRp( Rf, Rg, targetRg )
%RFRGTORP Calculates color preference Rp from fidelity and gamut

if ~exist('targetRg', 'var')
    targetRg = 110;
end

%Rg, Rf

% Punish for deviating from upper limit
% Real upper limit Q2 would be 90, 120 but we wish to put emphasis on the
% fidelity score so that preference score drops steeper with fidelity
Q1 = [100 100]; Q2 = [90 120]; P = [Rf Rg];
d_line = abs(det([Q2-Q1;P-Q1]))/norm(Q2-Q1);
% Gaussian scaling for distance from upper limit
g_line = gaussmf(0:0.1:200, [30 0]);
Rp = g_line(round(d_line*10)+1);

% Punish for deviating from targetRg with gaussian scaling
g_gamut = gaussmf(0:0.1:200, [20 targetRg]);
Rp = Rp * g_gamut(round(Rg*10)+1);

% Scale to 100
Rp = Rp * 100;

end

