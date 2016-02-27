function [ ] = inspectSpd( spd )
%INSPECTSPD Plot spd with reference spd scaled correctly
%   inspectSpd(spd) plots spd with reference spd with same color
%   temperature scaled so that both have same luminous output.
%
%   Reference spd is also returned

% Load test color samples rgb data
persistent cieRa95TestColorsRgb;
if isempty(cieRa95TestColorsRgb)
    % Creates variable planckSpd
    % each row is spectral power distribution for planck radiator at
    % temperature
    % Temperatures range from 1K to 25000K with 1K sampling rate
    load('cie.mat', 'cieRa95TestColorsRgb');
end

[CRI, CCT, R] = spdToCri(spd);
CRI = round(CRI);
ref = refSpd(CCT);
ref = ref.*spdToLumens(spd)/spdToLumens(ref);
L = 380:5:780;

figure;
subplot(1,2,1);
plot(L, spd, L, ref, '--');
axis([380 780 0 120]);
xlabel('Wavelength (nm)');
legend('Test SPD', 'Reference SPD');
title(strcat(['CCT = ', num2str(CCT), 'K']));
grid on;

subplot(1,2,2);
hold on;
for i = 1:length(R)
   h = bar(i, R(i), 0.5);
   set(h, 'FaceColor', cieRa95TestColorsRgb(i, :));
   set(h, 'EdgeColor', cieRa95TestColorsRgb(i, :));
end
title(strcat(['CRI = ', num2str(CRI)]));
grid on;
hold off;

end

