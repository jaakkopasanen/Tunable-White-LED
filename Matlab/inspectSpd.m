function [ ] = inspectSpd( spd )
%INSPECTSPD Plot spd with reference spd scaled correctly
%   inspectSpd(spd) plots spd with reference spd with same color
%   temperature scaled so that both have same luminous output.
%
%   Reference spd is also returned

% Load test color samples rgb data
persistent cieRaTestColorsRgb;
if isempty(cieRaTestColorsRgb)
    % Creates variable planckSpd
    % each row is spectral power distribution for planck radiator at
    % temperature
    % Temperatures range from 1K to 25000K with 1K sampling rate
    load('cie.mat', 'cieRaTestColorsRgb');
end

[CRI, R] = spdToCri(spd, 14);
CCT = spdToCct(spd);
CRI = mean(R(1:8));
CRI = round(CRI);
CRI_1_14 = mean(R(1:14));
CRI_1_14 = round(CRI_1_14);
ref = refSpd(CCT);
ref = ref.*spdToLumens(spd)/spdToLumens(ref);
L = 380:5:780;

figure;
subplot(1,2,1);
plot(L, spd, L, ref, '--');
axis([380 780 0 1.2]);
xlabel('Wavelength (nm)');
legend('Test SPD', 'Reference SPD');
title(strcat(['CCT = ', num2str(CCT), 'K']));
grid on;

subplot(1,2,2);
hold on;
for i = 1:length(R)
   h = bar(i, R(i), 0.5);
   set(h, 'FaceColor', cieRaTestColorsRgb(i, :));
   set(h, 'EdgeColor', cieRaTestColorsRgb(i, :));
end
title(strcat(['CRI = ', num2str(CRI), ', CRI_1_-_1_4 = ', num2str(CRI_1_14)]));
grid on;
hold off;

end

