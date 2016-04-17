clear;
tic
% Wavelengths
L = 380:5:780;

% LEDs
red = gaussmf(L, [20/2.355 630]); redL = 160;
green = gaussmf(L, [20/2.355 525]); greenL = 320;
blue = gaussmf(L, [20/2.355 465]); blueL = 240;
leds = [
    Led('red', red, redL, 1)
    Led('green', green, greenL, 1)
    Led('blue', blue, blueL, 1)
];

% Parameters
resolution = 0.01;
HMsize = [300 300];
interpolationMethod = 'linear';
extrapolationMethod = 'nearest';

% Led u'v' coordinates
sourceUvs = [
    xyzToCie1976UcsUv(spdToXyz(red));
    xyzToCie1976UcsUv(spdToXyz(green));
    xyzToCie1976UcsUv(spdToXyz(blue));
    xyzToCie1976UcsUv(spdToXyz(red));
];

% Mixing data
data = [];
i = 1;
for r = 0:resolution:1
    for g = 0:resolution:1-r
        b = 1-r-g;
        uv = xyzToCie1976UcsUv(spdToXyz(mixSpd([red;green;blue], [r;g;b])));
        data(i, :) = [uv r g b];
        i = i + 1;
    end
end

% Heatmap data
HM = ones(HMsize(1), HMsize(2), 3);
[U,V] = meshgrid(linspace(0,0.63,HMsize(1)),linspace(0,0.6,HMsize(2)));
Fr = scatteredInterpolant(data(:,1),data(:,2),data(:,3),interpolationMethod,extrapolationMethod);
HM(:,:,1) = Fr(U,V);
Fg = scatteredInterpolant(data(:,1),data(:,2),data(:,4),interpolationMethod,extrapolationMethod);
HM(:,:,2) = Fg(U,V);
Fb = scatteredInterpolant(data(:,1),data(:,2),data(:,5),interpolationMethod,extrapolationMethod);
HM(:,:,3) = Fb(U,V);

% RGB heatmap
subplot(2,2,1);
imagesc([0 0.63], [0 0.6], HM);
hold on; plot(sourceUvs(:,1), sourceUvs(:,2), 'k'); hold off;
axis([0 0.63 0 0.6]);
set(gca, 'ydir', 'normal');
title('RGB heatmap');
xlabel('u'''); ylabel('v''');

% Heatmap for red
subplot(2,2,2);
contourf(U,V,HM(:,:,1),100,'LineColor','none');
hold on; plot(sourceUvs(:,1), sourceUvs(:,2), 'k'); hold off;
axis([0 0.63 0 0.6]);
colorbar;
title('Red LED heatmap');
xlabel('u'''); ylabel('v''');
% Heatmap for green
subplot(2,2,3);
contourf(U,V,HM(:,:,2),100,'LineColor','none');
hold on; plot(sourceUvs(:,1), sourceUvs(:,2), 'k'); hold off;
axis([0 0.63 0 0.6]);
colorbar;
title('Green LED heatmap');
xlabel('u'''); ylabel('v''');
% Heatmap for blue
subplot(2,2,4);
contourf(U,V,HM(:,:,3),100,'LineColor','none');
hold on; plot(sourceUvs(:,1), sourceUvs(:,2), 'k'); hold off;
axis([0 0.63 0 0.6]);
colorbar;
title('Blue LED heatmap');
xlabel('u'''); ylabel('v''');

toc