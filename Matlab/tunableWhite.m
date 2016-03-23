clear; t = cputime; % Save current time for duration calculations
load('cie.mat'); % Load lookup tables for colorimetry calculations
load('led_data.mat'); % Load spectrums for various LEDs
L = 380:5:780; % Wavelengths: from 380nm to 780nm sampled at 5nm
time = clock; disp(['Started at ' num2str(time(4)) ':' num2str(time(5))]); % Display start time

%% Parameters for simulations

% LED mixing coefficient resolution. Higher the resolution, better the
% results but time requirement is higher (in high order)
resolution = 0.01;

% Minimum correlated color temperature. All combinations producing
% temperatures below this value are ignored. Also used as lower limit for
% plotting various visualization aids.
minCCT = 1200; 

% Maximum correlated color temperature. All combinations producing
% temperatures over this value are ignored. Also used as upper limit for
% plotting the various visualization aids.
maxCCT = 7000;

% Target IES TM-30-15 Rg value for light quality optimization. Higher the
% value more the light will oversaturate the colors. Some oversaturation is
% generally preferred since human visual system desaturates color in dimmer
% than daylight lighting. Setting too high target may cause hue distortions
% and also too high color saturation is not preferred.
targetRg = 105; 

% Maximum deviation from planckian locus in CIE 1976 CIELUV UCS u', v'
% units. All LED mixing combinations producing ligth deviating more from
% the Planckian locus are ignore. This parameter is only for simulation
% speed optimization. Too low values may lead into ignoring good results,
% too high value may lead to longer calculation times.
maxDuv = 0.02;

% Color temperature samples for inspection. inspectSpd function is called
% with spds resulting in all of these correlated color temperatures.
inspectSpds = [1500, 2000, 2700, 4000, 5600, 6500];

% Spectrums for red, green and blue LEDs. First value in the array (second
% parameter for gaussmf) is the peak width in nanometers, and the second
% value is the dominating wavelength of the led in nanometers.
red = gaussmf(L, [20 670]); redL = 240;
green = gaussmf(L, [20 530]); greenL = 480;
blue = gaussmf(L, [20 455]); blueL = 300;

% Spectrum for warm white LED. Yuji BC5730L strips are not recommended
% anymore by the manufacturer since all new and improved strips use BC2835L
% chips. Remove comment from one of the lines or add your own spectrum.
%warm = Yuji_BC2835L_2700K; warmL = 700; % Yuji BC2835L
%warm = Yuji_VTC5730_2700K; warmL = 800; % Yuji Violet chip 
%warm = Cree_A19_2700K; warmL = 700; % Represents decent warm white LED
warm = Generic_3000K; warmL = 700; % Represents cheap Chinese warm white

% Spectrum for cold white LED
%cold = Yuji_BC2835L_5600K; coldL = 1700; coldL = 2700;
cold = Yuji_BC2835L_6500K; coldL = 900;
%cold = Yuji_VTC5730_5600K; coldL = 1000;
%cold = Generic_6500K; coldL = 200;

% LEDs used for simulations
% Parameters for Led contructor are:
%   name:string Name of the LED, used in the plots
%   spd:[double] Row vector of doubles in range [0,1]
%   lumens:double Luminous intensity of the LED, used to calculate power
%   maxCoeff:double Maximum coefficient for the LED
leds = [
    Led('red', red, redL, 1)
    Led('green', green, greenL, 0.3)
    Led('blue', blue, blueL, 0.2)
    Led('warm', warm, warmL, 1)
    Led('cold', cold, coldL, 1)
];

% LEDs are combined in groups. LEDs in a group are combined only with other
% LEDs in the same group. This way combining too many LEDs can be avoided
% to ensure faster simulation. Each row contains one group and the values
% in a group are indexes to leds array created above.
%
% Examples:
% [1 2 3 4 5] := Combining 5 LEDs with each other, will result in very long
%                simulation time
% [1 2; 2 3] := Combines only consecutive LEDs e.g. red with warm white and
%               warm white with cold white
ledGroups = [
    %1 2 4 5 % From red to cold white
    %2 3 5 0  % From cold white onwards
    1 2 3 4 5
    %1 2
    %2 3
];

%% Generate mixing data for simulations
generateMixingData

%% Simulate results
simulateResults

%% Plots for visual estimations
visualize

%% Display total duration in minutes
disp(['Total duration is ' num2str(round((cputime - t) / 60)) 'min']);