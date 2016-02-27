function [ spd ] = cctToSpd( cct, spds, p )
%CCTTOSPD Generate mixed SPD from spds with polynomial coefficients for 2nd
%spd at given CCT
%   Input:
%       cct  := Correlated color temperature in Kelvins
%       spds := Each row is one spectrum. Red, warm and cold repectively
%       p    := Polynomial coefficient for estimating warm mixing coeffs
%   Output:
%       spd  := Mixed spectral power distribution

ccts = [spdToCct(spds(1,:)) spdToCct(spds(2,:)) spdToCct(spds(3,:))];

if cct <= ccts(1)
    red = 1;
    warm = 0;
    cold = 0;
elseif cct < ccts(2)
    warm = polyval(p(1, :), cct);
    red = 1 - warm;
    cold = 0;
elseif cct == ccts(2)
    red = 0;
    warm = 1;
    cold = 0;
elseif cct < ccts(3)
    red = 0;
    warm = polyval(p(2, :), cct);
    cold = 1 - warm;
else
   warm = polyval(p(2,:));
   red = 0;
   cold = 1 - warm;
end

spd = mixSpd(spds, [red;warm;cold]);

end

