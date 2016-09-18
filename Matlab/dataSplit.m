clear;
spd = readSpd('HR4C13841_10-33-11-877.txt');
L = 380:5:780;

rgbFit = fitGaussianToRgb(L, spd);
coeffs = coeffvalues(rgbFit);
a1 = coeffs(1); b1 = coeffs(2); c1 = coeffs(3);
a2 = coeffs(4); b2 = coeffs(5); c2 = coeffs(6);
a3 = coeffs(7); b3 = coeffs(8); c3 = coeffs(9);
a4 = coeffs(10); b4 = coeffs(11); c4 = coeffs(12);
a5 = coeffs(13); b5 = coeffs(14); c5 = coeffs(15);
a6 = coeffs(16); b6 = coeffs(17); c6 = coeffs(18);

x = L;
blue = a1.*exp(-((x-b1)./c1).^2) + a2.*exp(-((x-b2)./c2).^2);
green = a3.*exp(-((x-b3)./c3).^2) + a4.*exp(-((x-b4)./c4).^2);
red = a5.*exp(-((x-b5)./c5).^2) + a6.*exp(-((x-b6)./c6).^2);
white = red+green+blue;

plot(L,spd,'k', L,red,'r', L,green,'g', L,blue,'b', L,white,'ok')
legend('Full', 'Red', 'Green', 'Blue', 'White');
axis([380 780 0 1])
xlabel('Wavelength (nm)');
grid on;

%{
uvSpd = xyzToCie1976UcsUv(spdToXyz(spd, 2));
uvWhite = xyzToCie1976UcsUv(spdToXyz(white, 2));
plotCieLuv([], false);
hold on;
plot(uvSpd(1), uvSpd(2), 'ok')
plot(uvWhite(1), uvWhite(2), '+k')
hold off;
%}