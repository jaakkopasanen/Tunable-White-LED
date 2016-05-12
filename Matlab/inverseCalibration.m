%clear; clc;

syms Rp1 Rp2 Rq1 Rq2 Lp1 Lp2 Lq1 Lq2 dR dL
syms PRu PRv PLu PLv
syms P0u P0v P1u P1v P2u P2v
syms PTu PTv


%dL = (Lp1*dR + Lp2) / (dR + Lq1);
dL = (Lp1*(1-dR) + Lp2) / (1 - dR + Lq1);

PRu = P0u + (P1u - P0u) * dR;
PRv = P0v + (P1v - P0v) * dR;

PLu = P0u + (P2u - P0u) * dL;
PLv = P0v + (P2v - P0v) * dL;

EQ_PRPL = (PTu - PRu)*(PLv - PRv) == (PTv - PRv) * ( PLu - PRu);
sol = solve(EQ_PRPL, dR)
disp('Holy fucking shit!')