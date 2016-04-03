function [ ] = plotCieLuv( XYZ, planckianLocus, ax )
%PLOTCIELUV Plots XYZ color on CIELuv 1976 chromatic diagram
%Syntax
%   plotCieLuv(XYZ, planckianLocus, ax)
%Input
%   XYZ            := Matrix of CIE 1931 tristimulus values [X Y Z]. Each
%                     row contains one tristimulus vector.
%   planckianLocus := Plot planckian locus?
%   ax             := Axes handle to plot on to (Optional)

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
u = zeros(1, size(XYZ, 1));
v = zeros(1, size(XYZ, 1));
for i = 1:size(XYZ, 1)
    denom = XYZ(i,1) + 15*XYZ(i,2) + 3*XYZ(i,3);
    u(i) = 4*XYZ(i,1)  / denom;
    v(i) = 9*XYZ(i,2) / denom;
end

% Plots
if size(XYZ, 1) > 1
    style = '-k';
else
    style = 'ok';
end

hold on;

if exist('ax', 'var') % Axes given, use it
    % Plot chromacity diagram background
    imagesc([0 0.63], [0 0.6], flip(cie1976UcsFull, 1), 'Parent', ax);
    % Plot planckian locus
    if planckianLocus
        plot(ax, cie1976PlanckianLocusUv(:, 1), cie1976PlanckianLocusUv(:, 2), '--k');
    end
    % Plot border
    plot(ax, wavelengths(:, 1), wavelengths(:, 2), 'k', 'LineWidth', 1.5);
    % Plot with coordinates
    plot(ax, u, v, style);
else % No axes given
    imagesc([0 0.63], [0 0.6], flip(cie1976UcsFull, 1));
    if planckianLocus
        plot(cie1976PlanckianLocusUv(:, 1), cie1976PlanckianLocusUv(:, 2), '--k');
    end
    plot(wavelengths(:, 1), wavelengths(:, 2), 'k', 'LineWidth', 1.5);
    plot(u, v, style);
end

set(gca, 'ydir', 'normal');
axis([0 0.63 0 0.6]);
xlabel('u''');
ylabel('v''');

end

