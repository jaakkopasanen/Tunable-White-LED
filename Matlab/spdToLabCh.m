function [ L, a, b, C, h ] = spdToLabCh( spd )
%SPDTOLABCH Calculates CIE 1976 coordinates for SPD
%Syntax
%   [L, a, b, C, h] = spdToLabCh(spd)
%Input
%   spd := Spectral power distribution sampled from 380nm to 780nm at 5nm
%          intervals
%Output
%   L := CIELAB L component
%   a := CIELAB a component
%   b := CIELAB b component
%   C := CIELCh C component (Chromacity)
%   h := CIELCh h component (Hue)

cct = spdToCct(spd);
ref = refSpd(cct);
[x,y,z] = spdToXyz(spd);
[x_r,y_r,z_r] = spdToXyz(ref);
[L, a, b, C, h] = xyzToLab(x, y, z, x_r, y_r, z_r);

end

