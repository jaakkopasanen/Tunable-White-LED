function [ rgb ] = spdToRgb( spd )
%SPDTORGB Calculates sRGB values for spectral power distribution

[x, y, z] = spdToXyz(spd);
cct = spdToCct(spd);
ref = refSpd(cct, true);
[x_r, y_r, z_r] = spdToXyz(ref);
xyz_r = [x_r y_r z_r] * (1/y_r);
rgb = xyz2rgb([x y z], 'WhitePoint', xyz_r);
%rgb = round(rgb*255);

end

