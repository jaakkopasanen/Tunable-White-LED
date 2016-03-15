function [ Rf, Rg, Rp, bins ] = spdToRfRg( spd )
%SPDTORFRG Calculates TM-30-15 Rf and Rg metrics from SPD
%Input:
%   spd := Spectral power distribution from 380nm to 780nm sampled at 5nm
%Output:
%   Rf     := Fidelity score [0,100]
%   Rg     := Gamut score
%   Rp     := Preference score
%   bins   := Data for all 16 bins. Row per bin, columns are:
%             a_r := a coordinate under reference illuminant
%             b_r := b coordinate under reference illuminant
%             a_t := a coordinate under test illuminant
%             b_t := b coordinate under test illuminant
%             x_r := Color icon path x coordinate under reference illuminant
%             y_r := Color icon path y coordinate under reference illuminant
%             x_t := Color icon path x coordinate under test illuminant
%             y_t := Color icon path y coordinate under test illuminant
%             Rfb := Average fidelity score

% Load test colors if not yet loaded
persistent TM3015TestColors;
if isempty(TM3015TestColors)
    % Creates variable TM3015TestColors
    % each row is spectral radiance factors for TM-30-15 test color sample
    % Wavelengths span 380nm to 780nm with 5nm sampling
    load('TM3015.mat', 'TM3015TestColors');
end

% Error/score factor
cfactor = 7.54;

% Calculate colorimetry for test illuminant
[~, ~, ~, X_t, Y_t, Z_t, K_t] = spdToXyz(spd, 10);
XYZ_t = K_t * [X_t;Y_t;Z_t];
%X_t = K_t * X_t;
%Y_t = K_t * Y_t;
%Z_t = K_t * Z_t;

% Calculate correlated color temperature CCT
cct = spdToCct(spd);

% Calculate reference spd
ref = refSpd(cct, true);

% Calculate colorimetry for reference illuminant
[~, ~, ~, X_r, Y_r, Z_r, K_r] = spdToXyz(ref, 10);
XYZ_r = K_r * [X_r;Y_r;Z_r];
%X_r = K_r * X_r;
%Y_r = K_r * Y_r;
%Z_r = K_r * Z_r;

% Parameters for CIECAM02 color appearance model
LA = 100; % Absolute luminance
Yb = 20; % Relative background luminance
Did = 1; % Adaptation mode: <1 := Manual, 1 := Full adaptation, 2 := Partial adaptation
F = 1; % Adaptation factor, 1 for bright environment
c = 0.69; % Impact of surrounding
Nc = 1; % Chromatic induction factor
% Degree of adaptation (discounting)
if Did == 1
    D = 1;
elseif Did < 1
    D = Did;
else
    D = F * (1 - 1/3.6*exp((-LA-42) / 92));
end

% Chromatic adaptation transformation matrix
persistent MCAT02;
if isempty(MCAT02)
    MCAT02 = [
        0.7328	0.4296	-0.1624
        -0.7036	1.6975	0.0061
        0.0030	0.0136	0.9834
    ];
end

% Hunt-Point-Estévez transformation matrix
persistent MHEP
if isempty(MHEP)
    MHEP = [
        0.38971 0.68898 -0.07868
        -0.22981 1.1834 0.04641
        0 0 1
    ];
end

% Errors
dEi = zeros(1, 99);

    % Calculates CAM02-UCS color coordinates for spd
    function [ JcaMcbMc, C ] = spdToJcaMcbMc(spd, XYZw, debug)
        if ~exist('debug', 'var')
            debug = false;
        end
        
        % CIE 1931
        [~, ~, ~, X, Y, Z] = spdToXyz(spd, 10);
        XYZ = [X; Y; Z];
        
        k = 1 / (5 * LA + 1);
        FL = 1/5*k^4 * 5*LA + 1/10*(1 - k^4)^2 * (5*LA)^(1/3);
        n = Yb / XYZw(2);
        Nbb = 0.725 * (1/n)^0.2;
        Ncb = Nbb;
        z = 1.48 + sqrt(n);
        RGB = MCAT02 * XYZ; % Column vector!
        RGBw = MCAT02 * XYZw; % Column vector!
        RGBc = [
            (D * XYZw(2) / RGBw(1) + 1 - D) * RGB(1)
            (D * XYZw(2) / RGBw(2) + 1 - D) * RGB(2)
            (D * XYZw(2) / RGBw(3) + 1 - D) * RGB(3)];
        RGBcw = [
            (D * XYZw(2) / RGBw(1) + 1 - D) * RGBw(1)
            (D * XYZw(2) / RGBw(2) + 1 - D) * RGBw(2)
            (D * XYZw(2) / RGBw(3) + 1 - D) * RGBw(3)];
        XYZc = MCAT02 \ RGBc; % inv(MCAT02) * RGBc
        XYZcw = MCAT02 \ RGBcw; % inv(MCAT02) * RGBcw
        RGBp = MHEP * XYZc;
        RGBpw = MHEP * XYZcw;
        RGBpa = [
            ((400 * (FL*RGBp(1)/100)^0.42) / (((FL*RGBp(1)/100)^0.42) + 27.13)) + 0.1
            ((400 * (FL*RGBp(2)/100)^0.42) / (((FL*RGBp(2)/100)^0.42) + 27.13)) + 0.1
            ((400 * (FL*RGBp(3)/100)^0.42) / (((FL*RGBp(3)/100)^0.42) + 27.13)) + 0.1];
        RGBpaw = [
            ((400 * (FL*RGBpw(1)/100)^0.42) / (((FL*RGBpw(1)/100)^0.42) + 27.13)) + 0.1
            ((400 * (FL*RGBpw(2)/100)^0.42) / (((FL*RGBpw(2)/100)^0.42) + 27.13)) + 0.1
            ((400 * (FL*RGBpw(3)/100)^0.42) / (((FL*RGBpw(3)/100)^0.42) + 27.13)) + 0.1];
        a = RGBpa(1) - 12 * RGBpa(2) / 11 + RGBpa(3) / 11;
        b = 1/9 * (RGBpa(1) + RGBpa(2) - 2*RGBpa(3));
        
        % h
        % Matlab: atan2(Y,X) instead of atan2(X,Y), help atan2
        if a == 0
            if b == 0
                h = 0;
            else
                if b >= 0
                    h = (360/(2*pi))*atan2(b,a);
                else
                    h = 360+(360/(2*pi))*atan2(b,a);
                end
            end
        else
            if b >= 0
                h = (360/(2*pi))*atan2(b,a);
            else
                h = 360+(360/(2*pi))*atan2(b,a);
            end
        end
        
        % H
        %{
        if h < 20.14
            H = 385.9+(14.1*(h)/0.856)/((h)/0.856+(20.14-h)/0.8);
        elseif h < 90
            H = (100*(h-20.14)/0.8)/((h-20.14)/0.8+(90-h)/0.7);
        elseif h < 164.25
            H = 100+(100*(h-90)/0.7)/((h-90)/0.7+(164.25-h)/1);
        elseif h < 237.53
            H = 200+(100*(h-164.25)/1)/((h-164.25)/1+(237.53-h)/1.2);
        else
            H = 300+(85.9*(h-237.53)/1.2)/((h-237.53)/1.2+(360-h)/0.856);
        end
        
        %
        Hc = zeros(4,1);
        % Hc (red)
        if H > 300
            Hc(1) = H - 300;
        elseif H < 100
            Hc(1) = 100 - H;
        else
            Hc(1) = 0;
        end
        % Hc (yellow)
        if H <= 100
            Hc(2) = H;
        elseif H < 200
            Hc(2) = 200 - H;
        else
            Hc(2) = 0;
        end
        % Hc (green)
        if H > 100
            if H <= 200
                Hc(3) = H - 100;
            else
                if H < 300
                    Hc(3) = 300 - H;
                else
                    Hc(3) = 0;
                end
            end
        else
            Hc(3) = 0;
        end
        % Hc ( blue)
        if H > 300
            Hc(4) = 400 - H;
        elseif H > 200
            Hc(4) = H - 200;
        else
            Hc(4) = 0;
        end
        %}
        
        e = ((12500/13) * Nc * Ncb) * (cos((h*pi/180) + 2) + 3.8);
        A = (2*RGBpa(1) + RGBpa(2) + 1/20*RGBpa(3) - 0.305) * Nbb;
        Aw = (2*RGBpaw(1) + RGBpaw(2) + 1/20*RGBpaw(3) - 0.305) * Nbb;
        J = 100*(A / Aw)^(c * z);
        %Q = (4 / c) * sqrt(J / 100) * (Aw + 4) * FL^0.25;
        t = (e * sqrt(a^2 + b^2)) / (RGBpa(1) + RGBpa(2) + 21/20*RGBpa(3));
        C = (t^0.9) * sqrt(J / 100) * ((1.64 - 0.29^n)^0.73);
        M = C * FL^0.25;
        %s = 100 * sqrt(M / Q);
        %ac = C * cos(pi * h / 180);
        %bc = C * sin(pi * h / 180);
        %aM = M * cos(pi * h / 180);
        %bM = M * sin(pi * h / 180);
        %as = s * cos(pi * h / 180);
        %bs = s * sin(pi * h / 180);
        % CAM02UCS
        Jc = (1 + 100*0.007) * J / (1 + 0.007 * J);
        Mc = (1 / 0.0228) * log(1 + 0.0228 * M);
        aMc = Mc * cos(h * pi / 180);
        bMc = Mc * sin(h * pi / 180);
        JcaMcbMc = [Jc aMc bMc];
    end

    function [ t ] = calculateTheta( a, b )
    % Calculates angle for color
        t = atan(b / a);
        if and(a < 0, b > 0)
            t = t + pi;
        end
        if and(a < 0, b < 0)
            t = t + pi;
        end
        if and(a > 0, b < 0)
            t = t + 2*pi;
        end
    end

% Temporary bin data
binData = zeros(99, 9);

% Calculate Jc, aMc, bMc and error for all test color samples
for i = 1:99
    % Reference light
    JcaMcbMc_r = spdToJcaMcbMc(TM3015TestColors(i,:).*ref.*K_r, XYZ_r);
    
    % Test light
    JcaMcbMc_t = spdToJcaMcbMc(TM3015TestColors(i,:).*spd.*K_t, XYZ_t);
    
    % Error
    dEi(i) = sqrt(sum((JcaMcbMc_r - JcaMcbMc_t).^2));
    
    % Bin data
    theta = calculateTheta(JcaMcbMc_r(2), JcaMcbMc_r(3));
    binN = floor(((theta/2)/pi)*16) + 1;
    % Jc_r, aMc_r, bMc_r, Jc_t, aMc_t, bMc_t, theta, binNumber dE
    binData(i, :) = [JcaMcbMc_r JcaMcbMc_t theta binN dEi(i)];
end

% Source
source = zeros(1, 4);
[source(1:3), source(4)] = spdToJcaMcbMc(spd.*K_t, XYZ_r);

% Calculate average a, b coordinates for bins
binCoords = zeros(17, 6);
for i = 1:16
    % Select all test color samples from bin data where bin number is <i>
    tcs = binData(binData(:, 8) == i, :);

    dE = mean(tcs(:, 9));
    Rfb = 10 * log(exp((100 - cfactor * dE) / 10) + 1);
    % Bin averages for: aMc_r, bMc_r, aMc_t, bMc_t, theta, Rf
    binCoords(i, :) = [mean(tcs(:, 2)) mean(tcs(:, 3)) mean(tcs(:, 5)) mean(tcs(:, 6)) mean(tcs(:, 7)) Rfb];
end

% Copy first bin coordinates to 17th bin for stats calculations
binCoords(17, :) = binCoords(1, :);

% Calculate bin, bin+1 stats from bin coordinates
binStats = zeros(16, 9);
for i = 1:16
    % aMc_r difference between next sample and this one
    dar = binCoords(i + 1, 1) - binCoords(i, 1);
    % bMc_r average from next sample and this one
    mbr = (binCoords(i + 1, 2) + binCoords(i, 2)) / 2;
    
    % aMc_r difference between next sample and this one
    dat = binCoords(i + 1, 3) - binCoords(i, 3);
    % bMc_r average from next sample and this one
    mbt = (binCoords(i + 1, 4) + binCoords(i, 4)) / 2;
    
    % Path for icon plot
    theta = binCoords(i, 5);
    x_r = cos(theta);
    y_r = sin(theta);
    %[x_r;y_r]
    % da_rel = (aMc_t - aMc_r) / sqrt(aMc_r^2 + bMc_r^2)
    da_rel = (binCoords(i, 3) - binCoords(i, 1)) / sqrt(binCoords(i, 1)^2 + binCoords(i, 2)^2);
    % db_rel = (bMc_t - bMc_r) / sqrt(aMc_r^2 + bMc_r^2)
    db_rel = (binCoords(i, 4) - binCoords(i, 2)) / sqrt(binCoords(i, 1)^2 + binCoords(i, 2)^2);
    x_t = x_r + da_rel;
    y_t = y_r + db_rel;
    
    % Average fidelity score
    Rfb = binCoords(i, 6);
    
    % Save stats for current bin
    binStats(i, :) = [dar, mbr, dat, mbt, x_r, y_r, x_t, y_t, Rfb];
end

bins = binStats;
bins(17, :) = bins(1, :);

A0 = sum(bsxfun(@times, binStats(:, 1), binStats(:, 2)));
A1 = sum(bsxfun(@times, binStats(:, 3), binStats(:, 4)));
Rg = A1 / A0 * 100;
if isnan(Rg)
    Rg = 150;
end
% Gamut score
Rg = max(min(150, Rg), 50);

% Average error
dEavg = mean(dEi);

% Special fidelity scores
%Rfi = 10*log(exp((100 - cfactor .* dEi') / 10) + 1);

% General fidelity score
Rf = 10*log(exp((100 - cfactor * dEavg) / 10) + 1);

% Preference score
targetRg = 110;
maxRf = 100 - abs(100 - Rg);
% Distance from (maxRf,targetRg) point with weighting for Rg
d = sqrt((1*(targetRg - Rg))^2 + (1*(maxRf - Rf))^2);
% Force to range [0,100]. No negative values!
Rp = 10*log(exp((100 - 2 * d) / 10) + 1);
Rp = Rp * (1 - source(4) / 100);

end