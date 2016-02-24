function [ C ] = minRmse( test, ref )
%minRmse Calculates coefficient for reference vector so that root mean
%square minimizes
%   Detailed explanation goes here
min = Inf;
C = 0;
for c = 0:0.01:5
    RMSE = sqrt(mean((test-(ref.*c)).^2));
    if RMSE < min
       min = RMSE;
       C = c;
    end
end

end

