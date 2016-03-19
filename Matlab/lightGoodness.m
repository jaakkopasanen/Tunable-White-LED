function [ goodness, duv ] = lightGoodness( Rf, Rg, XYZ, XYZw, targetRg )
%lIGHTGOODNESS Calculates goodness points for light
%Syntax:
%   goodness = lightGoodness(Rf, Rg, XYZ)
%Input:
%   Rf       := TM-30-15 fidelity score
%   Rg       := TM-30-15 gamut score
%   XYZ      := CIE 1931 tristimulus values [X Y Z] for test illuminant
%   XYZw     := CIE 1931 tristimulus values [X Y Z] for reference illuminant
%   targetRg := Optional target for TM-30-15 gamut score. Defaults to 100.
%Output:
%   goodness := Arbitrary goodness scores
%   duv      := Difference in CIE 1976 UCS color coordinates

if ~exist('targetRg', 'var')
    targetRg = 100;
end

% To CIE 1976 UCS
u = 4*XYZ(1)  / (XYZ(1) + 15*XYZ(2) + 3*XYZ(3));
v = 9*XYZ(2) / (XYZ(1) + 15*XYZ(2) + 3*XYZ(3));
uw = 4*XYZw(1)  / (XYZw(1) + 15*XYZw(2) + 3*XYZw(3));
vw = 9*XYZw(2) / (XYZw(1) + 15*XYZw(2) + 3*XYZw(3));
duv = sqrt((uw - u)^2 + (vw - v)^2);

% Preference score
maxRf = 100 - abs(100 - Rg);
% Distance from (maxRf, targetRg, uw, vw) point
%d = sqrt((maxRf - Rf)^2 + (targetRg - Rg)^2 + (uw - u)^2 + (vw - v)^2);
d = sqrt((maxRf - Rf)^2 + (targetRg - Rg)^2) + duv*300;
% Force to range [0,100]. No negative values!
goodness = 10*log(exp((100 - d) / 10) + 1);

end

