function [ x, y, z, X, Y, Z, K ] = spdToXyz( spd, observerAngle )
%SPDTOXYZ Calculates CIE1931 color coordinates x, y, z for spectrum
%Syntax
%   [x, y, z, X, Y, Z, K] = spdToXyz(spd, observerAngle)
%Input
%   spd           := Spectral power distribution sampled from 380nm to
%                    780nm at 5nm intervals
%   observerAngle := 2 or 10. Use CIE 2 degree observer or 10 degree
%                    observer?
%Output
%   x := Normalized X
%   y := Normalized Y
%   z := Normalized Z
%   X := X tristimulus value
%   Y := Y tristimulus value
%   Z := Z tristimulus value
%   K := K = 100/Y

persistent cie2DegObserver;
persistent cie10DegObserver;
if isempty(cie2DegObserver)
    % Load color matching functions if not already loaded
    % Columns are red, green and blue color match functions of CIE 2 degree
    % standard observer
    load('cie.mat', 'cie2DegObserver', 'cie10DegObserver');
end

if ~exist('observerAngle', 'var')
    observerAngle = 2;
end

if observerAngle == 2
    observer = cie2DegObserver;
elseif observerAngle == 10
    observer = cie10DegObserver;
end

X = sum(spd .* observer(1,:));
Y = sum(spd .* observer(2,:));
Z = sum(spd .* observer(3,:));
K = 100 / Y;

XYZ = X + Y + Z;

x = X / XYZ;
y = Y / XYZ;
z = Z / XYZ;

end

