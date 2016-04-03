function [ L, a, b, C, h ] = xyzToLab( X, Y, Z, Xn, Yn, Zn )
%CIEXYZ_TO_CIELAB Converts CIEXYZ coordinates to CIELAB (+ Ch)
%Syntax
%   [L, a, b, C, H] = xyzToLab(X, Y, Z, Xn, Yn, Zn)
%Input
%   X: CIE1931 tristimulus X-coordinate
%   Y: CIE1931 tristimulus Y-coordinate
%   Z: CIE1931 tristimulus Y-coordinate
%   Xn: Reference white point X coordinate
%   Yn: Reference white point Z coordinate
%   Zn: Reference white point Y coordinate
%Output
%   L: CIELAB lightness coordinate
%   a: CIELAB red-green chromacity coordinate
%   b: CIELAB blue-yellow chromacity coordinate
%   C: CIELAB cylindrical representation chroma coordinate
%   h: CIELAB cylindrical representation hue angle
%Syntax:
%   [ L, a, b, C, h ] = xyzToLab( X, Y, Z, Xn, Yn, Zn )

    function [ x ] = f( t )
        if t > (6/29) ^3
           x = t^(1/3); 
        else
           x = 1/3 * (29/6)^2 * t + 4/29;
        end
    end

L = 116 * f(Y/Yn) - 16;
a = 500 * (f(X/Xn) - f(Y/Yn));
b = 200 * (f(Y/Yn) - f(Z/Zn));
C = sqrt(a^2 + b^2);
h = atan(b/a);

end

