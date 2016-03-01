function [ x, y, z, X, Y, Z, K ] = spdToXyz( spd )
%SPDTOXYZ Calculates CIE1931 color coordinates x, y, z for spectrum
%   [x, y, z] = spdToXyz(spd) for spectral power distribution spd from
%   380nm to 780nm sampled at 5nm returns CIE1931 color coordinates x, y,
%   z, X, Y, Z

persistent cie2DegObserver;
if isempty(cie2DegObserver)
    % Load color matching functions if not already loaded
    % Columns are red, green and blue color match functions of CIE 2 degree
    % standard observer
    load('cie.mat', 'cie2DegObserver');
end

X = sum(spd .* cie2DegObserver(1,:));
Y = sum(spd .* cie2DegObserver(2,:));
Z = sum(spd .* cie2DegObserver(3,:));
K = 100 / Y;

XYZ = X + Y + Z;

x = X / XYZ;
y = Y / XYZ;
z = Z / XYZ;

end

