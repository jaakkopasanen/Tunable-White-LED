clear; clc
load('cie.mat', 'cieSpectralLuminousEfficiency');
tic
% Wavelengths
L = 380:5:780;
%figure;

% LEDs
% Parameters for these shold come from rgbCalibration.m
red = gaussmf(L, [10/2.355 630]); redL = 160;
green = gaussmf(L, [10/2.355 525]); greenL = 320;
blue = gaussmf(L, [10/2.355 465]); blueL = 240;

% Led u'v' coordinates
sourceUvs = [
    xyzToCie1976UcsUv(spdToXyz(red));
    xyzToCie1976UcsUv(spdToXyz(green));
    xyzToCie1976UcsUv(spdToXyz(blue));
    xyzToCie1976UcsUv(spdToXyz(red));
];


c = 0:0.01:1;

% Red to green
D_rg = sqrt(sum((sourceUvs(1,:)-sourceUvs(2,:)).^2));
d_rg = []; i = 1;
for r = c
    g = 1 - r;
    uv = xyzToCie1976UcsUv(spdToXyz(mixSpd([red;green], [r;g]), 2));
    d_rg(i) = sqrt(sum((sourceUvs(1, :) - uv).^2)) / D_rg;
    i = i + 1;
end
d_gr = flip(1 - d_rg, 2);

% Green to blue
D_gb = sqrt(sum((sourceUvs(2,:)-sourceUvs(3,:)).^2));
d_gb = []; i = 1;
for g = c
    b = 1 - g;
    uv = xyzToCie1976UcsUv(spdToXyz(mixSpd([green;blue], [g;b]), 2));
    d_gb(i) = sqrt(sum((sourceUvs(2, :) - uv).^2)) / D_gb;
    i = i + 1;
end
d_bg = flip(1 - d_gb, 2);

% Blue to red
D_br = sqrt(sum((sourceUvs(3,:)-sourceUvs(1,:)).^2));
d_br = []; i = 1;
for b = c
    r = 1 - b;
    uv = xyzToCie1976UcsUv(spdToXyz(mixSpd([blue;red], [b;r]), 2));
    d_br(i) = sqrt(sum((sourceUvs(3, :) - uv).^2)) / D_br;
    i = i + 1;
end
d_rb = flip(1 - d_br, 2);

% Create fits
[rgFit, rgInv] = fitRat11(d_rg, c);
[gbFit, gbInv] = fitRat11(d_gb, c);
[brFit, brInv] = fitRat11(d_br, c);

% Test fits
x = 0:0.01:1;
c_rg = rat11(x, rgFit);
c_gr = 1 - rat11(1 - x, rgFit);
c_gb = rat11(x, gbFit);
c_bg = 1 - rat11(1 - x, gbFit);
c_br = rat11(x, brFit);
c_rb = 1 - rat11(1 - x, brFit);
plot(d_rg, c, x, c_rg, 'o', d_gr, c, x, c_gr, 'o');

% Red - Green
subplot(2,2,1);
plot(d_rg, c, 'r', d_gr, c, 'g',  x, c_rg, 'r.', x, c_gr, 'g.');
title('Red to green, Green to red');
xlabel('Distance from source');
ylabel('Source level');
legend('Red', 'Green');
grid on;

% Green - Blue
subplot(2,2,2);
plot(d_gb, c, 'g', d_bg, c, 'b',  x, c_gb, 'g.', x, c_bg, 'b.');
title('Green to blue, Blue to green');
xlabel('Distance from source');
ylabel('Source level');
legend('Green', 'Blue');
grid on;

% Blue - Red
subplot(2,2,3);
plot(d_br, c, 'b', d_rb, c, 'r',  x, c_br, 'b.', x, c_rb, 'r.');
title('Blue to red, Red to blue');
xlabel('Distance from source');
ylabel('Source level');
legend('Blue', 'Red');
grid on;

% TODO: fit left hand side function by right hand side distances
% e.g. d_rb vs d_rg

%{
% Test points
plotCieLuv([], false);
hold on;
% Plot gamut
plot(sourceUvs(:,1), sourceUvs(:,2), '-k');
err = [];

testPoints = [0.1934 0.4985];

for i = 1:10
    % Test point
    target = [rand*0.55 rand*0.6];
    %target = testPoints(i,:);
    
    % Not inside gamut, omit point
    if ~inpolygon(target(1), target(2), sourceUvs(1:3,1), sourceUvs(1:3,2))
        continue;
    end
    
    % Find mixing coefficients
    r = findLevel2(target, sourceUvs(1,:), sourceUvs(2,:), sourceUvs(3,:), rgFit, blueToRedFit);
    
    rgb = [r g b];
    rgb = rgb .* (1/max(rgb));
    %uvRgb =  [target rgb]
    
    % Simulate color
    uv = xyzToCie1976UcsUv(spdToXyz(mixSpd([red;green;blue], [r;g;b])));
    
    % Error
    err(i) = sqrt( sum( (uv - target).^2 ) );
    
    % Plot target and result
    plot(target(1), target(2), 'ok');
    plot(uv(1), uv(2), '+k');
    plot([target(1) uv(1)], [target(2) uv(2)], 'k');
end

title(['Mean error = ' num2str(mean(err))]);
hold off;

% Luminous efficiencies
luminousFluxes = [
    sum(bsxfun(@times, red, cieSpectralLuminousEfficiency))
    sum(bsxfun(@times, green, cieSpectralLuminousEfficiency))
    sum(bsxfun(@times, blue, cieSpectralLuminousEfficiency))
];
luminousFluxes = luminousFluxes * (1 / max(luminousFluxes));

%
disp([
    '----------------------------'
    'Relative luminous fluxes    '
    '----------------------------'
    'Rows                        '
    '    red                     '
    '    green                   '
    '    blue                    '
    '----------------------------'
]);
disp(luminousFluxes);

disp([
    '----------------------------'
    'u'' v'' coordinates           '
    '----------------------------'
    'Rows                        '
    '    red                     '
    '    green                   '
    '    blue                    '
    'Columns                     '
    '    u''  v''                  '
    '----------------------------'
]);
disp(sourceUvs(1:3,:));

disp([
    '----------------------------'
    'LED fit data                '
    '----------------------------'
    '       p1*x + p2            '
    'y = ---------------         '
    '    x^2 + q1*x + q2         '
    'Rows                        '
    '    red to green            '
    '    green to blue           '
    '    blue to red             '
    'Columns                     '
    '    p1  p2  q1  q2          '
    '----------------------------'
]);
disp([
    coeffvalues(redToGreenFit)
    coeffvalues(greenToBlueFit)
    coeffvalues(blueToRedFit)
]);
%}
%toc