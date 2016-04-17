clear all;
L = 380:5:780;
red = gaussmf(L, [20/2.355 630]); redL = 160;
green = gaussmf(L, [20/2.355 525]); greenL = 320;
blue = gaussmf(L, [20/2.355 465]); blueL = 240;
yellow = gaussmf(L, [5/2.355 571]);
cyan = gaussmf(L, [5/2.355 492]);

leds = [
    Led('red', red, redL, 1)
    Led('green', green, greenL, 1)
    Led('blue', blue, blueL, 1)
];

% Color by wavelength
subplot(2,2,2)
rgbs = zeros(1, 81, 3);
l = 440:630;
for i = 1:length(l)
    spd = gaussmf(L, [5/2.355 l(i)]);
    [~, xyz] = spdToXyz(spd);
    rgbs(1, i, :) = xyz2rgb(xyz);
    rgbs(1, i, :) = rgbs(1, i, :) ./ max(rgbs(1, i, :));
end
imagesc([l(1) l(end)], [0 1], rgbs);
axis([l(1) l(end) 0 1])
xlabel('Wavelength (nm)');

% Gamma
subplot(2,2,3)
uv_r = xyzToCie1976UcsUv(spdToXyz(red));
uv_g = xyzToCie1976UcsUv(spdToXyz(green));
uv_b = xyzToCie1976UcsUv(spdToXyz(blue));
uv_y = xyzToCie1976UcsUv(spdToXyz(yellow));
uv_c = xyzToCie1976UcsUv(spdToXyz(cyan));
uv_p = [0.338 0.297];
d_rg = sqrt(sum((uv_g-uv_r).^2));
d_gb = sqrt(sum((uv_g-uv_b).^2));
d_rb = sqrt(sum((uv_b-uv_r).^2));
%{
d_y = sqrt(sum((uv_r-uv_y).^2)) / d_rg;
d_c = sqrt(sum((uv_g-uv_c).^2)) / d_gb;
d_p = sqrt(sum((uv_r-uv_p).^2)) / d_rb;
e_min_y = Inf; e_min_c = Inf; e_min_p = Inf;
rg50 = 0; gb50 = 0; rb50 = 0;
for c = 0:0.01:1
    uv_t_rg = xyzToCie1976UcsUv(spdToXyz(mixSpd([red;green],[c;1-c])));
    e_y = sqrt(sum((uv_y-uv_t_rg).^2));
    if e_y < e_min_y
        e_min_y = e_y;
        rg50 = c;
    end
    
    uv_t_gb = xyzToCie1976UcsUv(spdToXyz(mixSpd([green;blue],[c;1-c])));
    e_c = sqrt(sum((uv_c-uv_t_gb).^2));
    if e_c < e_min_c
        e_min_c = e_c;
        gb50 = c;
    end
    
    uv_t_rb = xyzToCie1976UcsUv(spdToXyz(mixSpd([red;blue],[c;1-c])));
    e_p = sqrt(sum((uv_p-uv_t_rb).^2));
    if e_p < e_min_p
        e_min_p = e_p;
        rb50 = c;
    end
end

d = 0:0.01:1;
gamma_rg = log(rg50)/log(1-d_y);
gamma_gr = log(1-rg50)/log(d_y);
rg_gamma = (1-d).^gamma_rg;

gamma_gb = log(gb50)/log(1-d_c);
gamma_bg = log(1-gb50)/log(d_c);
gb_gamma = (1-d).^gamma_gb;

gamma_rb = log(rb50)/log(1-d_p);
gamma_br = log(1-rb50)/log(d_p);
rb_gamma = (1-d).^gamma_rb;
%}
d_true_rg = zeros(1,101);
d_true_gb = zeros(1,101);
d_true_rb = zeros(1,101);

c = 0:0.01:1;
for i = 1:101
    d_true_rg(i) = sqrt(sum((uv_r-xyzToCie1976UcsUv(spdToXyz(mixSpd([red;green], [c(i),1-c(i)])))).^2))/d_rg;
    d_true_gb(i) = sqrt(sum((uv_g-xyzToCie1976UcsUv(spdToXyz(mixSpd([green;blue], [c(i),1-c(i)])))).^2))/d_gb;
    d_true_rb(i) = sqrt(sum((uv_r-xyzToCie1976UcsUv(spdToXyz(mixSpd([red;blue], [c(i),1-c(i)])))).^2))/d_rb;
end

% TODO: Rational fit: Numerator degree = 1, denominator degree = 2

plot(...
    d_true_rg,c,...
    d_true_gb,c,...
    d_true_rb,c...
);
axis([0 1 0 1]);
xlabel('Relative distance'); ylabel('Power');
legend('True RG', 'True GB', 'True RB');
grid on;

% Gamut
subplot(2,2,1)
XYZ = [
    spdToXyz(red)
    spdToXyz(green)
    spdToXyz(blue)
    spdToXyz(red)
    %spdToXyz(mixSpd([red;green],[rg50;1-rg50]))
    %spdToXyz(mixSpd([green;blue],[gb50;1-gb50]))
    %spdToXyz(mixSpd([red;blue],[rb50;1-rb50]))
    %spdToXyz(mixSpd([red;green],[rg50;1-rg50]))
];
plotCieLuv(XYZ, false)

% 3 source gamma
subplot(2,2,4)
plotCieLuv(XYZ(1:4,:), false);
uv_ests = zeros(5,2);
uvs = zeros(5,2);
for i = 1:10
    c = rand(1, 3);
    c = c .* (1/max(c));
    spd = mixSpd([red;green;blue], c');
    XYZ = spdToXyz(spd);
    uvs(i,:) = xyzToCie1976UcsUv(XYZ);
    %uv = [0.195 0.467];
    % Intersection points
    p_r = intersection([uv_g;uv_b], [uv_r;uvs(i,:)]);
    p_g = intersection([uv_r;uv_b], [uv_g;uvs(i,:)]);
    p_b = intersection([uv_r;uv_g], [uv_b;uvs(i,:)]);
    % Relative distances
    d_r = sqrt(sum((uv_r-uvs(i,:)).^2)) / sqrt(sum((uv_r-p_r).^2));
    d_g = sqrt(sum((uv_g-uvs(i,:)).^2)) / sqrt(sum((uv_g-p_g).^2));
    d_b = sqrt(sum((uv_b-uvs(i,:)).^2)) / sqrt(sum((uv_b-p_b).^2));
    % Relative distances on the gamut borders for gamma interpolation
    d_pr = sqrt(sum((uv_g-p_r).^2)) / d_gb;
    d_pg = sqrt(sum((uv_r-p_g).^2)) / d_rb;
    d_pb = sqrt(sum((uv_r-p_b).^2)) / d_rg;
    % Interpolated gammas
    %{
    gamma_r = d_pr * (gamma_rb - gamma_rg) + gamma_rg;
    gamma_g = d_pg * (gamma_gb - gamma_gr) + gamma_gr;
    gamma_b = d_pb * (gamma_bg - gamma_br) + gamma_br;
    m = [
        (1-d_r)^gamma_r
        (1-d_g)^gamma_g
        (1-d_b)^gamma_b
    ];
    %}
    
    m = m.*(1/sum(m));
    uv_ests(i,:) = xyzToCie1976UcsUv(spdToXyz(mixSpd([red;green;blue], m)));
end
% Plot
hold on;
%{
plot([uv_r(1) uv(1)], [uv_r(2) uv(2)], 'k');
plot([uv_g(1) uv(1)], [uv_g(2) uv(2)], 'k');
plot([uv_b(1) uv(1)], [uv_b(2) uv(2)], 'k');
plot([p_r(1) uv(1)], [p_r(2) uv(2)], '--k');
plot([p_g(1) uv(1)], [p_g(2) uv(2)], '--k');
plot([p_b(1) uv(1)], [p_b(2) uv(2)], '--k');
%}
for i = 1:size(uvs,1)
    plot(uvs(i,1), uvs(i,2), 'ok');
    plot(uv_ests(i,1), uv_ests(i,2), '+k');
    plot([uvs(i,1) uv_ests(i,1)], [uvs(i,2) uv_ests(i,2)], 'k');
end
hold off;
c = 0:0.01:1;

