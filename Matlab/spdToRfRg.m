function [ Rf, Rg, Rfi, dEi, tcsi, bins ] = spdToRfRg( spd )
%SPDTORFRG Calculates TM-30-15 Rf and Rg metrics from SPD
%Input:
%   spd := Spectral power distribution from 380nm to 780nm sampled at 5nm
%Output:
%   Rf    := Fidelity score [0,100]
%   Rg    := Gamut score
%   Rif   := Special fidelity scores 1 to 99
%   dEi   := Errors for each test color sample
%   binst := CAM02-UCS color coordinates (J, a, b) for 16 color icon bins
%            under test illuminant. Each row contains one bin, 16 rows.
%   binsr := CAM02-UCS color coordinates (J, a, b) for 16 color icon bins
%            under reference illuminant. Each row contains one bin, 16 rows.

% Load test colors if not yet loaded
persistent TM3015TestColors;
if isempty(TM3015TestColors)
    % Creates variable cieRa95TestColors
    % each row is spectral radiance factors for TM-30-15 test color sample
    % Wavelengths span 380nm to 780nm with 5nm sampling
    load('TM3015.mat', 'TM3015TestColors');
end

% Error/score factor
cfactor = 7.54;

% Calculate colorimetry for test illuminant
[x_t, y_t, ~, X_t, Y_t, Z_t, K_t] = spdToXyz(spd, 10);
X_t = K_t * X_t;
Y_t = K_t * Y_t;
Z_t = K_t * Z_t;

% Calculate correlated color temperature CCT
cct = spdToCct(spd)

% Calculate reference spd
ref = refSpd(cct, true);

% Calculate colorimetry for reference illuminant
[x_r, y_r, ~, X_r, Y_r, Z_r, K_r] = spdToXyz(ref, 10);
X_r = K_r * X_r;
Y_r = K_r * Y_r;
Z_r = K_r * Z_r;

% Parameters for CIECAM02 color appearance model
LA = 100; % Absolute luminance
Yb = 20; % Relative background luminance
Did = 1;
F = 1; % Adaptation factor
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
MCAT02 = [
    0.7328	0.4296	-0.1624
    -0.7036	1.6975	0.0061
    0.0030	0.0136	0.9834
];

% Hunt-Point-Est�vez transformation matrix
MHEP = [
    0.3897	0.6890	-0.0787
    -0.2298	1.1834	0.0464
    0.0000	0.0000	1.0000
];

% Errors
dEi = zeros(1, 99);

    % Calculates CAM02-UCS color coordinates for spd
    function [ Jc, aMc, bMc ] = spdToJcaMcbMc(spd, K, XYZw)
        % CIE 1931
        [x, y, ~, X, Y, Z] = spdToXyz(spd, 10);
        XYZ = [X; Y; Z].*K
        
        k = 1 / (5 * LA + 1)
        FL = 1/5*k^4 * 5*LA + 1/10*(1 - k^4)^2 * (5*LA)^(1/3)
        n = Yb / XYZw(2)
        Nbb = 0.725 * (1/n)^0.2
        Ncb = Nbb
        z = 1.48 + sqrt(n)
        RGB = MCAT02 * XYZ % Column vector!
        RGBw = MCAT02 * XYZw' % Column vector!
        RGBc = [
            (D * XYZw(2) / RGBw(1) + 1 - D) * RGB(1)
            (D * XYZw(2) / RGBw(2) + 1 - D) * RGB(2)
            (D * XYZw(2) / RGBw(3) + 1 - D) * RGB(3)]
        RGBcw = [
            (D * XYZw(2) / RGBw(1) + 1 - D) * RGBw(1)
            (D * XYZw(2) / RGBw(2) + 1 - D) * RGBw(2)
            (D * XYZw(2) / RGBw(3) + 1 - D) * RGBw(3)]
        XYZc = MCAT02 \ RGBc % inv(MCAT02) * RGBc
        XYZcw = MCAT02 \ RGBcw % inv(MCAT02) * RGBcw
        RGBp = MHEP * XYZc
        RGBpw = MHEP * XYZcw
        RGBpa = [
            ((400 * (FL*RGBp(1)/100)^0.42) / (((FL*RGBp(1)/100)^0.42) + 27.13)) + 0.1
            ((400 * (FL*RGBp(2)/100)^0.42) / (((FL*RGBp(2)/100)^0.42) + 27.13)) + 0.1
            ((400 * (FL*RGBp(3)/100)^0.42) / (((FL*RGBp(3)/100)^0.42) + 27.13)) + 0.1]
        RGBpaw = [
            ((400 * (FL*RGBpw(1)/100)^0.42) / (((FL*RGBpw(1)/100)^0.42) + 27.13)) + 0.1
            ((400 * (FL*RGBpw(2)/100)^0.42) / (((FL*RGBpw(2)/100)^0.42) + 27.13)) + 0.1
            ((400 * (FL*RGBpw(3)/100)^0.42) / (((FL*RGBpw(3)/100)^0.42) + 27.13)) + 0.1]
        a = RGBpa(1) - 12 * RGBpa(2) / 11 + RGBpa(3) / 11
        b = 1/9 * (RGBpa(1) + RGBpa(2) - 2*RGBpa(3))
        
        % h
        % INCORRECT INCORRECT INCORRECT INCORRECT INCORRECT INCORRECT
        if a == 0
            if b == 0
                h = 0
            else
                if b >= 0
                    h = (360/(2*pi))*atan2(a,b)
                else
                    h = 360+(360/(2*pi))*atan2(a,b)
                end
            end
        else
            if b >= 0
                h = (360/(2*pi))*atan2(a,b)
            else
                h = 360+(360/(2*pi))*atan2(a,b)
            end
        end
        
        % H
        if h < 20.14
            H = 385.9+(14.1*(h)/0.856)/((h)/0.856+(20.14-h)/0.8)
        elseif h < 90
            H = (100*(h-20.14)/0.8)/((h-20.14)/0.8+(90-h)/0.7)
        elseif h < 164.25
            H = 100+(100*(h-90)/0.7)/((h-90)/0.7+(164.25-h)/1)
        elseif h < 237.53
            H = 200+(100*(h-164.25)/1)/((h-164.25)/1+(237.53-h)/1.2)
        else
            H = 300+(85.9*(h-237.53)/1.2)/((h-237.53)/1.2+(360-h)/0.856)
        end
        
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
            Hc(4) = 400 - H
        elseif H > 200
            Hc(4) = H - 200
        else
            Hc(4) = 0
        end
        
        e = ((12500/13) * Nc * Ncb) * (cos((H*pi/180) + 2) + 3.8)
        A = (2*RGBpa(1) + RGBpa(2) + 1/20*RGBpa(3) - 0.305) * Nbb
        Aw = (2*RGBpaw(1) + RGBpaw(2) + 1/20*RGBpaw(3) - 0.305) * Nbb
        J = 100*(A / Aw)^(c / z)
        Q = (4 / c) * sqrt(J / 100) * (Aw + 4) * FL^0.25
        t = (e * sqrt(a^2 + b^2)) / (RGBpa(1) + RGBpa(2) + 21/20*RGBpa(3))
        C = (t^0.9) * sqrt(J / 100) * ((1.64 - 0.29^n)^0.73)
        M = C * FL^0.25
        s = 100 * sqrt(M / Q)
        ac = C * cos(pi * h / 180)
        bc = C * sin(pi * h / 180)
        aM = M * cos(pi * h / 180)
        bM = M * sin(pi * h / 180)
        as = s * cos(pi * h / 180)
        bs = s * cos(pi * h / 180)
        % CAM02UCS
        Jc = (1 + 100*0.007) * J / (1 + 0.007 * J)
        Mc = (1 / 0.0228) * log(1 + 0.0228 * M)
        aMc = Mc * cos(h * pi / 180)
        bMc = Mc * sin(h * pi / 180)
    end

% Calculate Jp, aM, bM for all test color samples
for i = 1:1
    % Reference light
    [JcaMbM_r(1), JcaMbM_r(2), JcaMbM_r(3)] = spdToJcaMcbMc(TM3015TestColors(i,:).*ref, K_r, [X_r, Y_r, Z_r]);
end

% Average error
dEavg = mean(dEi);

% Special fidelity scores
Rfi = 10*log(exp((100 - cfactor .* dEi) / 10) + 1);

% General fidelity score
Rf = 10*log(exp((100 - cfactor * dEavg) / 10) + 1);

end

