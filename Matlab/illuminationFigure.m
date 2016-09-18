% Simulation for even light distribution on the ceiling reflection
%
% - Two illumination sources (S) hanging from the ceiling at height y_0.
% - Sources are apart from each other and coordinate origin (O) resides in
%   between of the two. Distance from origin is x_0
% - Sources are rotated outwards from the origin by angle a_0.
% - Sources have horizontal reflectors at their base to prevent light from
%   getting below horizon.
%
%              Ceiling
% ---------------------------------------
%                       | a_0 /        A
%                       |----/         |
%                       |   /          | y_0
%                       |  /           |
%                       | /            | 
%                       |/             v
% ======S-------O-------S===========-----
%               |       | reflector
%               |<----->|
%                  x_0


a_0 = 0.25*pi; % Source angle (outwards) in radians
x_0 = 25; % Source x-coordinate e.g. in centimeters
y_0 = 50; % Source height (distance from ceiling) in same unit as x

res_a = 0.01; % Angle resolution
a = -pi/2:res_a:pi/2; % Angles
x_a = tan(a) * y_0; % X-coordinate
x_max = 150; % Maximum x-position
a_s = -pi/2:res_a:pi/2; % Angles relative to source
s_a = gaussmf(a_s, [2/3*pi/2.355, 0]); % Source

% Shift source by source angle
n = round(a_0 / res_a); % Sample count
if n >= 0
    s_a = [zeros(1,n) s_a]; % Add zeros to left side
    % Mirror part of source which extends pi/2 by pi/2 (horizontal reflector)
    s_tail = fliplr(s_a(end-n+1:end));
    s_a = s_a(1:end-n);
    s_a(end-n+1:end) = s_a(end-n+1:end) + s_tail;
else
    n = -n;
    s_a = [s_a zeros(1,n)]; % Add zeros to right side
    % Mirror part of source which extends pi/2 by pi/2 (horizontal reflector)
    s_head = fliplr(s_a(1:n));
    s_a = s_a(n+1:end);
    s_a(1:n) = s_a(1:n) + s_head;
end

% Linearize X-coordinates and interpolate source values for linear x
x = -x_max:x_max;
s = interp1(x_a, s_a, x);
r = interp1(x_a, s_a.*cos(a), x); % Reflection on the ceiling

% Shift source by source x-coordinate x_0
s = [zeros(1, x_0) s];
s = s(1:end-x_0);
r = [zeros(1, x_0) r];
r = r(1:end-x_0);

% Create mirrored left sides and sums
s = s + fliplr(s); % Mirrored source
r = r + fliplr(r); % Mirrored source
p = r.^(1/2.2); % Perceived brightness

% Plot
%plot(x, s(1,:), x, s(2,:), x, s(3,:), x, r(3,:));
%legend('Right source', 'Left source', 'Sum source', 'Reflected sum')
plot(x, r, x, p);
legend('Reflected', 'Perceived')
axis([-x_max x_max 0 1.2])
xlabel('X-coordinate (cm)')
ylabel('Iintensity')
grid on;
title([
    strcat(['Mean of Perception: ', num2str(mean(p), '%1.2f')]);
    strcat(['STD of Perception:  ', num2str(std(p), '%1.2f')])
]);