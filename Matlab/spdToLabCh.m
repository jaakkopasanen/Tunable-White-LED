function [ L, a, b, C, h ] = spdToLabCh( spd )
%SPDTOLABCH Calculates CIE 1976 coordinates for SPD

cct = spdToCct(spd);
ref = refSpd(cct);
[x,y,z] = spdToXyz(spd);
[x_r,y_r,z_r] = spdToXyz(ref);
[L, a, b, C, h] = xyzToLab(x, y, z, x_r, y_r, z_r);

end

