function [ spd ] = imageToSpd( imgPath )
%imageToSpd Read image and covert it to Spectral power distribution
%   Detailed explanation goes here
spd = imread(imgPath);
spd = spd(:,:,1);
spd = spd < 1;
spd = sum(spd);
spd = spd./2;
end

