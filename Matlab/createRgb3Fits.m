function [fitresult, gof] = createRgb3Fits(c, d_rg, d_gb, d_br, doPlots)
%CREATERGB3FITS(C,D_RG,D_GB,D_BR)
%  Create fits.
%
%  Data for 'd_rg' fit:
%      X Input : d_rg
%      Y Output: c
%  Data for 'd_gb' fit:
%      X Input : d_gb
%      Y Output: c
%  Data for 'd_br' fit:
%      X Input : d_br
%      Y Output: c
%  Output:
%      fitresult : a cell-array of fit objects representing the fits.
%      gof : structure array with goodness-of fit info.
%
%  See also FIT, CFIT, SFIT.

%  Auto-generated by MATLAB on 11-May-2016 18:26:24

%% Initialization.

% Initialize arrays to store fits and goodness-of-fit.
fitresult = cell( 3, 1 );
gof = struct( 'sse', cell( 3, 1 ), ...
    'rsquare', [], 'dfe', [], 'adjrsquare', [], 'rmse', [] );

%% Fit: 'd_rg'.
[xData, yData] = prepareCurveData( d_rg, c );

% Set up fittype and options.
ft = fittype( 'rat12' );
opts = fitoptions( 'Method', 'NonlinearLeastSquares' );
opts.Display = 'Off';
opts.StartPoint = [0.552291341538775 0.629883385064421 0.0319910157625669 0.614713419117141];

% Fit model to data.
[fitresult{1}, gof(1)] = fit( xData, yData, ft, opts );

% Plot fit with data.
if doPlots
    figure( 'Name', 'd_rg' );
    h = plot( fitresult{1}, xData, yData );
    legend( h, 'c vs. d_rg', 'd_rg', 'Location', 'NorthEast' );
    % Label axes
    xlabel d_rg
    ylabel c
    grid on
end

%% Fit: 'd_gb'.
[xData, yData] = prepareCurveData( d_gb, c );

% Set up fittype and options.
ft = fittype( 'rat12' );
opts = fitoptions( 'Method', 'NonlinearLeastSquares' );
opts.Display = 'Off';
opts.StartPoint = [0.0426524109111434 0.635197916859882 0.28186685588043 0.53859667804534];

% Fit model to data.
[fitresult{2}, gof(2)] = fit( xData, yData, ft, opts );

% Plot fit with data.
if doPlots
    figure( 'Name', 'd_gb' );
    h = plot( fitresult{2}, xData, yData );
    legend( h, 'c vs. d_gb', 'd_gb', 'Location', 'NorthEast' );
    % Label axes
    xlabel d_gb
    ylabel c
    grid on
end

%% Fit: 'd_br'.
[xData, yData] = prepareCurveData( d_br, c );

% Set up fittype and options.
ft = fittype( 'rat12' );
opts = fitoptions( 'Method', 'NonlinearLeastSquares' );
opts.Display = 'Off';
opts.StartPoint = [0.270294332292698 0.208461358751314 0.564979570738201 0.640311825162758];

% Fit model to data.
[fitresult{3}, gof(3)] = fit( xData, yData, ft, opts );

% Plot fit with data.
if doPlots
    figure( 'Name', 'd_br' );
    h = plot( fitresult{3}, xData, yData );
    legend( h, 'c vs. d_br', 'd_br', 'Location', 'NorthEast' );
    % Label axes
    xlabel d_br
    ylabel c
    grid on
end


