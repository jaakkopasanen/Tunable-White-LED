function [ CCT, d_min ] = uvToCct( u, v )
%UVTOCCT Finds correlated color temperature from CIE1960 color coordinates
%   CCT = uvToCct(u, v) for CIE1960 color coordinates u, v and start
%   temperature (in Kelvins) return correlated color temperature in Kelvins
%
%   [CCT, d_min] = uvToCct(u, v) also return minimum distance

persistent planckLocusUv;
if isempty(planckLocusUv)
    % Creates variable planckLocusUv
    % each row is u,v coordinate pair for planck locus temperature
    % temperatures range from 1K to 25000K with 1K sampling rate
    load('cie.mat', 'planckLocusUv');
end

[d_min, CCT] = min(sqrt((planckLocusUv(:,1) - u).^2 + (planckLocusUv(:,2) - v).^2));

end

