function [ ] = inspectSpd( spd, targetRg, RfPenalty, RgPenalty, duvPenalty, supertitle )
%INSPECTSPD Plot all kinds of spectra inspections
%Input:
%   spd        := Spectral power distribution from 380nm to 780nm at 5nm
%   targetRg   := Optional target gamut score for goodness calculation
%   supertitle := Optional supertitle for figure

% Load test color samples rgb data
persistent cieRaTestColorsRgb;
if isempty(cieRaTestColorsRgb)
    load('cie.mat', 'cieRaTestColorsRgb');
end

% Load TM-30-15 bin hues
persistent TM3015BinsRgb;
persistent TM3015RfRgBg
if or(isempty(TM3015BinsRgb), isempty(TM3015RfRgBg))
    load('TM3015.mat', 'TM3015BinsRgb', 'TM3015RfRgBg');
end

% Background image for Rf Rg figure
persistent RgIconBG;
if isempty(RgIconBG)
    RgIconBG = imread('img/RgIconBG.PNG');
end

% Wavelengths
L = 380:5:780;

% CIE CRI
%[~, Ri] = spdToCri(spd, 14);
%CRI = mean(Ri(1:8));
%CRI_1_14 = mean(Ri(1:14));

% IES TM-30-15 Rf and Rg
[Rf, Rg, bins] = spdToRfRg(spd);

% Reference spectrum
CCT = spdToCct(spd);
ref = refSpd(CCT);
[x, y, z, X, Y, Z] = spdToXyz(spd);
[~, ~, ~, X_ref, Y_ref, Z_ref] = spdToXyz(ref);
[goodness, duv] = lightGoodness(Rf, Rg, [X Y Z], [X_ref Y_ref Z_ref], targetRg, RfPenalty, RgPenalty, duvPenalty);
ref = ref.*(Y/Y_ref);

% Light color
rgb = xyz2rgb([x y z]);
rgb = rgb.*(1/max(rgb));
rgb(rgb < 0) = 0;
figure('Color', rgb);
%figure;

% Plot spectrum with reference spectrum
subplot(2,3,1);
plot(L, spd, L, ref, '--');
axis([380 780 0 1.2]);
xlabel('Wavelength (nm)');
legend('Test SPD', 'Reference SPD');
title(strcat(['CCT = ', num2str(CCT), 'K']));
grid on;

% Plot CRI bar graph
%{
subplot(2,3,2);
hold on;
for i = 1:length(Ri)
   h = bar(i, Ri(i), 0.5);
   set(h, 'FaceColor', cieRaTestColorsRgb(i, :));
   set(h, 'EdgeColor', cieRaTestColorsRgb(i, :));
end
axis([0.5 14.5 0 100]);
title(strcat(['CRI = ', num2str(round(CRI)), ', CRI_1_-_1_4 = ', num2str(round(CRI_1_14))]));
grid on;
hold off;
%}

% Plot CIE 1976 chromacity diagram
ax = subplot(2,3,2);
plotCieLuv([X Y Z], true, ax);
title(strcat(['\Deltau''v'' = ', num2str(duv)]));

% Plot Rf by hue
subplot(2,3,4);
hold on;
hold on;
for i = 1:16
   h = bar(i, bins(i, 9), 0.5);
   set(h, 'FaceColor', TM3015BinsRgb(i, :));
   set(h, 'EdgeColor', TM3015BinsRgb(i, :));
end
axis([0.5 16.5 0 100]);
title(strcat(['Rf = ', num2str(round(Rf))]));
grid on;
hold off;

% Plot Rf / Rg figure
subplot(2,3,6);
imagesc([50 100], [60 140], TM3015RfRgBg);
hold on;
plot(Rf, Rg, 'ro', 'linewidth', 2);
set(gca, 'ydir', 'normal');
xlabel('Rf');
ylabel('Rg');
title(strcat(['Goodness = ', num2str(round(goodness))]));
grid on;
hold off;

% Plot color icon
subplot(2,3,5);
% Background
imagesc([-1.5 1.5], [-1.5 1.5], flip(RgIconBG, 1));
hold on;
% Reference, Test
plot(bins(:,5), bins(:,6), 'k--', bins(:,7), bins(:,8), 'r');
set(gca, 'ydir', 'normal');
grid on;
legend('Reference SPD', 'Test SPD');
title(strcat(['Rg = ', num2str(round(Rg))]));
hold off;

% Plot hue distortion
subplot(2,3,3);
imagesc([-1.5 1.5], [-1.5 1.5], flip(RgIconBG, 1));
hold on;
dHue_sum = 0;
for i = 1:16
    % Reference hue line
    plot([0 bins(i,5)*2], [0 bins(i,6)*2], 'k--');
    % Test coordinates
    plot(bins(i,7), bins(i,8), 'ro');
    % Reference hue angle
    theta_r = atan2d(bins(i,6), bins(i,5));
    if theta_r < 0
        theta_r = theta_r + 180;
    end
    % Test hue angle
    theta_t = atan2d(bins(i,8), bins(i,7));
    if theta_t < 0
        theta_t = theta_t + 180;
    end
    % Cumulate error
    dHue_sum = dHue_sum + abs(theta_r - theta_t);
end
% Average hue angle error
dHue_avg = dHue_sum / 16;

set(gca, 'ydir', 'normal');
grid on;
legend('Reference hue', 'Test sample');
title(strcat(['\Deltahue_a_v_g = ', num2str(dHue_avg, '%.1f'), char(176)]));
hold off;

% Supertitle
if exist('supertitle', 'var')
    suptitle(supertitle);
end

end

