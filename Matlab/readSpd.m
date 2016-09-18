function [ spd, uv ] = readSpd( fileName )
%READSPD Read SPD from CSV file
%Syntax
%   [spd, uv] = readSpd(fileName, wavelengths);
%Input
%   fileName    := Filepath of the CSV file
%Output
%   spd := Spectral power distribution
%   uv  := CIE 1976 UCS coordinates [u', v']

% Read data from file
fid = fopen(fileName);
data = textscan(fid, '%s\t%s');
strX = data{1}(48:end); % Start from 200nm
strSpd = data{2}(48:end,1); % Start from 200nm
x = zeros(1, length(strX));
y = zeros(1, length(strX));

% Convert to numbers
for i = 1:length(x)
    x(i) = str2double(strrep(strX(i), ',', '.'));
    y(i) = str2double(strrep(strSpd(i), ',', '.'));
end

% Remove white noise component
y = y - y(1);
y(y < 0) = 0;

y = y .* (1/max(y));

% Downsample
L = 380:5:780;
spd = zeros(1, length(L));
j = 1;
for i = 1:length(L)
    while x(j) < L(i)
        j = j + 1;
    end
    spd(i) = y(j);
end

% Normalize
%spd = spd .* (1 / max(spd));


plot(L,spd,'o-');
grid on;
axis([380 780 0 1])
xlabel('Wavelength (nm)');

uv = xyzToCie1976UcsUv(spdToXyz(spd, 2));

end

