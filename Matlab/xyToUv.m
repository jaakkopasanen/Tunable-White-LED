function [ u, v ] = xyToUv( x, y )
%XYTOUV Converts CIE1931 xy color coordinates to CIE1960 uv coordinates
%Syntax
%   [u, v] = xyToUv(x, y)
%Input
%   x := Normalized X (CIE1931)
%   y := Normalized Y (CIE1931)
%Output
%   u := u coordinate
%   v := v coordinate

u = 4*x / (-2*x + 12*y + 3);
v = 6*y / (-2*x + 12*y + 3);

end

