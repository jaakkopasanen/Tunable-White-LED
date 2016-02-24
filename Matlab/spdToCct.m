function [ CCT ] = spdToCct( spd )
%SPDTOCCT Calculates correlated color temperature for spectrum
%   CCT = spdToCct(spd) for spectral power distribution spd from
%   380nm to 780nm sampled at 5nm returns correlated color temperature in
%   Kelvins

% Calculate CIE 1931 color coordinates x, y
[x, y] = spdToXyz(spd);

% Transform into CIE1960 color coordinates u, v
[u, v] = xyToUv(x, y);

% Calculate correlated color temperature
CCT = uvToCct(u, v);

end

