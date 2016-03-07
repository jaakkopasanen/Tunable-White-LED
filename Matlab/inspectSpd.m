function [ ] = inspectSpd( spd, supertitle )
%INSPECTSPD Plot spd with reference spd scaled correctly
%   inspectSpd(spd) plots spd with reference spd with same color
%   temperature scaled so that both have same luminous output.
%
%   Reference spd is also returned

% Load test color samples rgb data
persistent cieRaTestColorsRgb;
if isempty(cieRaTestColorsRgb)
    load('cie.mat', 'cieRaTestColorsRgb');
end

% Background image for Rf Rg figure
persistent RfRgBG;
if isempty(RfRgBG)
    RfRgBG = imread('img/RfRg.PNG');
end

% Background image for Rf Rg figure
persistent RgIconBG;
if isempty(RgIconBG)
    RgIconBG = imread('img/RgIconBG.PNG');
end

% Wavelengths
L = 380:5:780;

% CIE CRI
[~, Ri] = spdToCri(spd, 14);
CRI = mean(Ri(1:8));
CRI_1_14 = mean(Ri(1:14));

% IES TM-30-15 Rf and Rg
[Rf, Rg, ~, bins] = spdToRfRg(spd);

% Reference spectrum
CCT = spdToCct(spd);
ref = refSpd(CCT);
[~, ~, ~, ~, Y_spd] = spdToXyz(spd);
[~, ~, ~, ~, Y_ref] = spdToXyz(ref);
ref = ref.*(Y_spd/Y_ref);

figure;

% Plot spectrum with reference spectrum
subplot(2,2,1);
plot(L, spd, L, ref, '--');
axis([380 780 0 1.2]);
xlabel('Wavelength (nm)');
legend('Test SPD', 'Reference SPD');
title(strcat(['CCT = ', num2str(CCT), 'K']));
grid on;

% Plot CRI bar graph
subplot(2,2,2);
hold on;
for i = 1:length(Ri)
   h = bar(i, Ri(i), 0.5);
   set(h, 'FaceColor', cieRaTestColorsRgb(i, :));
   set(h, 'EdgeColor', cieRaTestColorsRgb(i, :));
end
title(strcat(['CRI = ', num2str(round(CRI)), ', CRI_1_-_1_4 = ', num2str(round(CRI_1_14))]));
grid on;
hold off;

% Plot Rf / Rg figure
subplot(2,2,3);
imagesc([50 100], [60 140], flip(RfRgBG, 1));
hold on;
plot(Rf, Rg, 'r*');
set(gca, 'ydir', 'normal');
xlabel('Rf');
ylabel('Rg');
title(strcat(['Rf = ', num2str(round(Rf)), ', Rg = ', num2str(round(Rg))]));
grid on;
hold off;

% Plot color icon
subplot(2,2,4);
imagesc([-1.5 1.5], [-1.5 1.5], flip(RgIconBG, 1));
hold on;
plot(bins(:,7), bins(:,8), 'r', bins(:,5), bins(:,6), 'k--', 'LineWidth', 2);
set(gca, 'ydir', 'normal');
legend('Test SPD', 'Reference SPD');
title('Color Icon');
hold off;

% Supertitle
if exist('supertitle', 'var')
    suptitle(supertitle);
end

end

