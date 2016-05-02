minT = 1000;
maxT = 10000;
res = 10;
rgb = [];
for t = minT:res:maxT
    u = cie1976PlanckianLocusUv(t, 1);
    v = cie1976PlanckianLocusUv(t, 2);
    X = -(9*u)/(3*u + 20*v - 12);
    Y = -(4*v)/(3*u + 20*v - 12);
    Z = 1;
    x = X / (X+Y+Z);
    y = Y / (X+Y+Z);
    z = Z / (X+Y+Z);
    xyz = [x y z];
    ind = (t-1000)/10+1;
    rgb(1, ind, :) = xyz2rgb(xyz);
    rgb(1, ind, :) = rgb(1, ind, :) * (1 / max(rgb(1, ind, :)));
end
imagesc(rgb);