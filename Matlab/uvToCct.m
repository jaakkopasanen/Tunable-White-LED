function [ CCT, d_min ] = uvToCct( u, v )
%UVTOCCT Finds correlated color temperature from CIE1960 color coordinates
%   CCT = uvToCct(u, v) for CIE1960 color coordinates u, v and start
%   temperature (in Kelvins) return correlated color temperature in Kelvins
%
%   [CCT, d_min] = uvToCct(u, v) also return minimum distance

persistent cie1960PlanckianLocusUv;
if isempty(cie1960PlanckianLocusUv)
    load('cie.mat', 'cie1960PlanckianLocusUv');
end

[d_min, CCT] = min(sqrt((cie1960PlanckianLocusUv(:,1) - u).^2 + (cie1960PlanckianLocusUv(:,2) - v).^2));

end

