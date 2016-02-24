function [ u, v ] = xyToUv( x, y )
%XYTOUV Converts CIE1931 xy color coordinates to CIE1960 uv coordinates
%   [u, v] = xyToUv(x, y);
denom = (-2*x + 12*y + 3);
u = 4*x / denom;
v = 6*y / denom;
end

