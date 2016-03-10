function [ coeffs ] = estimateCoeffs( cct, coeffData )
%ESTIMATECOEFFS Estimate coeffs for single color temperature
%Input:
%   cct       := Correlated color temperature in Kelvins
%   coeffData := Matrix of coeffs, each row contains cct and coeffs for all
%                three LEDS
%Output:
%   coeffs := Estimated coeffs needed to produce required cct

coeffs = zeros(1, 3);

% Below lowest reachable cct, return first
if coeffData(1, 1) >= cct
    coeffs = coeffData(1, 2:4);
    return;
end

N = length(coeffData);
for i = 1:N
    % Exact match found
   if coeffData(i, 1) == cct
       coeffs = coeffData(i, 2:4);
       return;
       
   % Next sample is greater, interpolate current and next
   elseif i < N
       if coeffData(i+1, 1) >= cct
           range = coeffData(i+1, 1) - coeffData(i, 1); 
           p = (cct - coeffData(i, 1)) / range;
           for j = 1:3
               coeffs(j) = coeffData(i, j+1) + p * (coeffData(i+1, j+1) - coeffData(i, j+1));
           end
           return;
       end
       
   % Last sample reached
   elseif i == N
       coeffs = coeffData(i, 2:4);
       return;
   end
end

end

