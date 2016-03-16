function [ ] = plotCieLuv( XYZ, planckianLocus, ax )
%PLOTCIELUV Plots XYZ color on CIELuv 1976 chromatic diagram
%Input:
%   XYZ            := Row vectors of CIE 1931 tristimulus values [X Y Z]
%   planckianLocus := Plot planckian locus?
%   ax             := Axes to plot on (Optional)

persistent cie1976PlanckianLocusUv
if isempty(cie1976PlanckianLocusUv)
    load('cie.mat', 'cie1976PlanckianLocusUv');
end

persistent cie1976UcsFull
if isempty(cie1976UcsFull)
    load('cie.mat', 'cie1976UcsFull');
end

% Generate border by iterating all visible light wavelengths
wavelengths = zeros(60, 2);
for l = 1:59
    spd = zeros(1, 81);
    spd(l+6) = 1;
    [~, ~, ~, X, Y, Z] = spdToXyz(spd);
    denom = X + 15*Y + 3*Z;
    up = 4*X  / denom;
    vp = 9*Y / denom;
    wavelengths(l, :) = [up vp];
end
wavelengths(60, :) = wavelengths(1, :);

% Point
denom = XYZ(1) + 15*XYZ(2) + 3*XYZ(3);
u = 4*XYZ(1)  / denom;
v = 9*XYZ(2) / denom;

% Plots
hold on;

if exist('ax', 'var')
    imagesc([0 0.63], [0 0.6], flip(cie1976UcsFull, 1), 'Parent', ax);
    if planckianLocus
        plot(ax, cie1976PlanckianLocusUv(:, 1), cie1976PlanckianLocusUv(:, 2), 'k');
    end
    plot(ax, wavelengths(:, 1), wavelengths(:, 2), 'k', 'LineWidth', 1.5);
    plot(ax, u, v, 'ok');
else
    imagesc([0 0.63], [0 0.6], flip(cie1976UcsFull, 1));
    if planckianLocus
        plot(cie1976PlanckianLocusUv(:, 1), cie1976PlanckianLocusUv(:, 2), 'k');
    end
    plot(wavelengths(:, 1), wavelengths(:, 2), 'k', 'LineWidth', 1.5);
    plot(u, v, 'ok');
end

set(gca, 'ydir', 'normal');
axis([0 0.63 0 0.6]);
xlabel('u');
ylabel('v');

end

