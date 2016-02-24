function [ ] = inspectSpd( spd )
%INSPECTSPD Plot spd with reference spd scaled correctly
%   inspectSpd(spd, 1) plots spd with reference spd with same color
%   temperature scaled so that both have same luminous output.
%
%   Reference spd is also returned

if ~exist('shouldPlot', 'var')
    shouldPlot = 0;
end

[CRI, CCT, ~] = spdToCri(spd);
CRI = round(CRI);
ref = refSpd(CCT);
ref = ref.*spdToLumens(spd)/spdToLumens(ref);
L = 380:5:780;
plot(L, spd, L, ref, '--');
axis([380 780 0 200]);
xlabel('Wavelength (nm)');
legend('Test SPD', 'Reference SPD');
title(strcat(['CCT = ', num2str(CCT), 'K, CRI = ', num2str(CRI)]));

end

