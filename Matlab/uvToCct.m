function [ cct, d_min ] = uvToCct( uv )
%UVTOCCT Finds correlated color temperature from CIE1960 color coordinates
%Syntax
%   [cct, d_min] = uvToCct(u, v)
%Input
%   u := u coordinate
%   v := v coordinate
%Outpu
%   cct   := Correlated color temperature in Kelvins
%   d_min := Minimum euclidean distance to Planckian locus

persistent cie1960PlanckianLocusUv;
if isempty(cie1960PlanckianLocusUv)
    load('cie.mat', 'cie1960PlanckianLocusUv');
end

u = uv(1); v = uv(2);
[d_min, cct] = min(sqrt((cie1960PlanckianLocusUv(:,1) - u).^2 + (cie1960PlanckianLocusUv(:,2) - v).^2));

end

