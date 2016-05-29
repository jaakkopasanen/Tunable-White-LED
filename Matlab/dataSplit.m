clear;
spd = readSpd('HR4C13841_10-17-37-728.txt');

L = 380:5:780;
blue = gaussmf(L, [10 454])*0.915 + gaussmf(L, [25 470])*0.15;
green = gaussmf(L, [10 511])*0.45 + gaussmf(L, [20 527])*0.23;
red = gaussmf(L, [10 630])*0.20 + gaussmf(L, [22 620])*0.03;
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