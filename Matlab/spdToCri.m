function [ CRI, CCT, Ri ] = spdToCri( testSpd, nSamples )
%SPDTOCRI
%   See http://onlinelibrary.wiley.com/doi/10.1002/9781119975595.app7/pdf
%   CRI is combined color rendering index (mean)
%   CCT is correlated color temperature
%   Ri  is array of color rendering indexes for each test color

% Load test colors if not yet loaded
persistent cieRa95TestColors;
if isempty(cieRa95TestColors)
    % Creates variable cieRa95TestColors
    % each row is spectral radiance factors for CIE CRI Ra95 test color
    % samples 1 - 14 respectively
    % Wavelengths span 380nm to 780nm with 5nm sampling
    load('cie.mat', 'cieRa95TestColors');
end

if ~exist('nSamples', 'var')
    nSamples = 14;
end

% Calculate color temperature
CCT = spdToCct(testSpd);

% Calculate colorimetry
[x_k, y_k, ~, ~, Y_k] = spdToXyz(testSpd); % CI1931 color coordinates
[u_k, v_k] = xyToUv(x_k, y_k); % CIE1960
c_k = 1/v_k*(4-u_k-10*v_k); % c function for von Kries transformation
d_k = (1.708*v_k + 0.404 - 1.481*u_k)/v_k; % d functoin for von Kries transformation
K_k = 100/Y_k; % Reflected - emmited luminance factor

% Generate reference illuminant and calculate colorimetry
ref = refSpd(CCT);
[x_r, y_r, ~, ~, Y_r] = spdToXyz(ref);
[u_r, v_r] = xyToUv(x_r, y_r);
c_r = (4-u_r-10*v_r)/v_r;
d_r = (1.708*v_r + 0.404 - 1.481*u_r)/v_r;
K_r = 100/Y_r;

    function [Ua, Va, Wa] = spdToUaVaWa( spd, K, isRef )
        % CIE1931
        [x, y, ~, ~, Y] = spdToXyz(spd);
        % Scale reflected luminance with with emitted luminanace
        % http://www.konicaminolta.com/instruments/knowledge/color/part4/02.html
        %         100
        % K = -----------, Y_reflected = K * Y
        %      Y_emitted
        Y = K*Y; 
        
        % Move to CIE1960 color space
        [u, v] = xyToUv(x, y);
       
        % CIE 1964 U*V*W* color coordinates
        % https://en.wikipedia.org/wiki/CIE_1964_color_space
        Wa = 25*Y^(1/3) - 17;
        if isRef
            % Color coordinates of test color sample under the reference
            % illuminant is not transformed with von Kries chromatic
            % adaptation function
            
            % Move u, v color coordinates to CIE1964 color space
            Va = 13*Wa*(v - v_r);
            Ua = 13*Wa*(u - u_r);
        else
            % Transform color coordinates of test color sample under the
            % test illuminant with von Kries chromatic adaptation function
            
            % c and d are functions needed for up and vp calculations
            c = (4-u-10*v) / v;
            d =(1.708*v - 1.481*u + 0.404 ) / v;
            
            % Transformed coordinates
            up = (10.872 + 0.404*c_r/c_k*c - 4*d_r/d_k*d) / (16.518 + 1.481*c_r/c_k*c - d_r/d_k*d);
            vp = 5.52 / (16.518 + 1.481*c_r/c_k*c - d_r/d_k*d);
            
            % Move chromatically adapted u,v color coordinates to
            % CIE1964 color space
            Va = 13*Wa*(vp - v_r);
            Ua = 13*Wa*(up - u_r);
        end
    end

% Calculate colorimetries for test colors
Ri = zeros(1, nSamples);
for i = 1:nSamples
    
    % Reference light
    [UaVaWa_r(1), UaVaWa_r(2), UaVaWa_r(3)] = spdToUaVaWa(cieRa95TestColors(i,:).*ref, K_r, true);
    
    % Test light
    [UaVaWa_k(1), UaVaWa_k(2), UaVaWa_k(3)]  = spdToUaVaWa(cieRa95TestColors(i,:).*testSpd, K_k, false);
    
    % Color rendering index for currect test color sample
    dE = sqrt(sum((UaVaWa_r - UaVaWa_k).^2));
    Ri(i) = 100 - 4.6 * dE;
end
CRI = mean(Ri);

end