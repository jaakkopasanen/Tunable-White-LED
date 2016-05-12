function [ level ] = findLevel2( P0, P1, P2, rightFit, rightInversed, leftFit, leftInversed )
%FINDLEVEL2 Find level for led

P0u = P0(1); P0v = P0(2);
P1u = P1(1); P1v = P1(2);
P2u = P2(1); P2v = P2(2);
Lp1 = leftFit(1); Lp2 = leftFit(2); Lq1 = leftFit(3);

if leftInversed
    dR = ((Lp1^2*P0u^2*P1v^2 - 2*Lp1^2*P0u^2*P1v*PTv + Lp1^2*P0u^2*PTv^2 - 2*Lp1^2*P0u*P1u*P0v*P1v + 2*Lp1^2*P0u*P1u*P0v*PTv + 2*Lp1^2*P0u*P1u*P1v*P2v - 2*Lp1^2*P0u*P1u*P2v*PTv - 2*Lp1^2*P0u*P2u*P1v^2 + 4*Lp1^2*P0u*P2u*P1v*PTv - 2*Lp1^2*P0u*P2u*PTv^2 + 2*Lp1^2*P0u*P0v*P1v*PTu - 2*Lp1^2*P0u*P0v*PTu*PTv - 2*Lp1^2*P0u*P1v*P2v*PTu + 2*Lp1^2*P0u*P2v*PTu*PTv + Lp1^2*P1u^2*P0v^2 - 2*Lp1^2*P1u^2*P0v*P2v + Lp1^2*P1u^2*P2v^2 + 2*Lp1^2*P1u*P2u*P0v*P1v - 2*Lp1^2*P1u*P2u*P0v*PTv - 2*Lp1^2*P1u*P2u*P1v*P2v + 2*Lp1^2*P1u*P2u*P2v*PTv - 2*Lp1^2*P1u*P0v^2*PTu + 4*Lp1^2*P1u*P0v*P2v*PTu - 2*Lp1^2*P1u*P2v^2*PTu + Lp1^2*P2u^2*P1v^2 - 2*Lp1^2*P2u^2*P1v*PTv + Lp1^2*P2u^2*PTv^2 - 2*Lp1^2*P2u*P0v*P1v*PTu + 2*Lp1^2*P2u*P0v*PTu*PTv + 2*Lp1^2*P2u*P1v*P2v*PTu - 2*Lp1^2*P2u*P2v*PTu*PTv + Lp1^2*P0v^2*PTu^2 - 2*Lp1^2*P0v*P2v*PTu^2 + Lp1^2*P2v^2*PTu^2 + 2*Lp1*Lp2*P0u^2*P1v^2 - 2*Lp1*Lp2*P0u^2*P1v*P2v - 2*Lp1*Lp2*P0u^2*P1v*PTv + 2*Lp1*Lp2*P0u^2*P2v*PTv - 4*Lp1*Lp2*P0u*P1u*P0v*P1v + 2*Lp1*Lp2*P0u*P1u*P0v*P2v + 2*Lp1*Lp2*P0u*P1u*P0v*PTv + 4*Lp1*Lp2*P0u*P1u*P1v*P2v - 2*Lp1*Lp2*P0u*P1u*P2v^2 - 2*Lp1*Lp2*P0u*P1u*P2v*PTv + 2*Lp1*Lp2*P0u*P2u*P0v*P1v - 2*Lp1*Lp2*P0u*P2u*P0v*PTv - 4*Lp1*Lp2*P0u*P2u*P1v^2 + 2*Lp1*Lp2*P0u*P2u*P1v*P2v + 4*Lp1*Lp2*P0u*P2u*P1v*PTv - 2*Lp1*Lp2*P0u*P2u*P2v*PTv + 2*Lp1*Lp2*P0u*P0v*P1v*PTu - 2*Lp1*Lp2*P0u*P0v*P2v*PTu - 2*Lp1*Lp2*P0u*P1v*P2v*PTu + 2*Lp1*Lp2*P0u*P2v^2*PTu + 2*Lp1*Lp2*P1u^2*P0v^2 - 4*Lp1*Lp2*P1u^2*P0v*P2v + 2*Lp1*Lp2*P1u^2*P2v^2 - 2*Lp1*Lp2*P1u*P2u*P0v^2 + 4*Lp1*Lp2*P1u*P2u*P0v*P1v + 2*Lp1*Lp2*P1u*P2u*P0v*P2v - 2*Lp1*Lp2*P1u*P2u*P0v*PTv - 4*Lp1*Lp2*P1u*P2u*P1v*P2v + 2*Lp1*Lp2*P1u*P2u*P2v*PTv - 2*Lp1*Lp2*P1u*P0v^2*PTu + 4*Lp1*Lp2*P1u*P0v*P2v*PTu - 2*Lp1*Lp2*P1u*P2v^2*PTu - 2*Lp1*Lp2*P2u^2*P0v*P1v + 2*Lp1*Lp2*P2u^2*P0v*PTv + 2*Lp1*Lp2*P2u^2*P1v^2 - 2*Lp1*Lp2*P2u^2*P1v*PTv + 2*Lp1*Lp2*P2u*P0v^2*PTu - 2*Lp1*Lp2*P2u*P0v*P1v*PTu - 2*Lp1*Lp2*P2u*P0v*P2v*PTu + 2*Lp1*Lp2*P2u*P1v*P2v*PTu - 2*Lp1*Lq1*P0u^2*P1v^2 + 4*Lp1*Lq1*P0u^2*P1v*P2v - 4*Lp1*Lq1*P0u^2*P2v*PTv + 2*Lp1*Lq1*P0u^2*PTv^2 + 4*Lp1*Lq1*P0u*P1u*P0v*P1v - 4*Lp1*Lq1*P0u*P1u*P0v*P2v - 2*Lp1*Lq1*P0u*P1u*P1v*P2v - 2*Lp1*Lq1*P0u*P1u*P1v*PTv + 6*Lp1*Lq1*P0u*P1u*P2v*PTv - 2*Lp1*Lq1*P0u*P1u*PTv^2 - 4*Lp1*Lq1*P0u*P2u*P0v*P1v + 4*Lp1*Lq1*P0u*P2u*P0v*PTv + 2*Lp1*Lq1*P0u*P2u*P1v^2 - 2*Lp1*Lq1*P0u*P2u*PTv^2 + 4*Lp1*Lq1*P0u*P0v*P2v*PTu - 4*Lp1*Lq1*P0u*P0v*PTu*PTv + 2*Lp1*Lq1*P0u*P1v^2*PTu - 6*Lp1*Lq1*P0u*P1v*P2v*PTu + 2*Lp1*Lq1*P0u*P1v*PTu*PTv + 2*Lp1*Lq1*P0u*P2v*PTu*PTv - 2*Lp1*Lq1*P1u^2*P0v^2 + 2*Lp1*Lq1*P1u^2*P0v*P2v + 2*Lp1*Lq1*P1u^2*P0v*PTv - 2*Lp1*Lq1*P1u^2*P2v*PTv + 4*Lp1*Lq1*P1u*P2u*P0v^2 - 2*Lp1*Lq1*P1u*P2u*P0v*P1v - 6*Lp1*Lq1*P1u*P2u*P0v*PTv + 2*Lp1*Lq1*P1u*P2u*P1v*PTv + 2*Lp1*Lq1*P1u*P2u*PTv^2 - 2*Lp1*Lq1*P1u*P0v*P1v*PTu + 2*Lp1*Lq1*P1u*P0v*PTu*PTv + 2*Lp1*Lq1*P1u*P1v*P2v*PTu - 2*Lp1*Lq1*P1u*P2v*PTu*PTv - 4*Lp1*Lq1*P2u*P0v^2*PTu + 6*Lp1*Lq1*P2u*P0v*P1v*PTu + 2*Lp1*Lq1*P2u*P0v*PTu*PTv - 2*Lp1*Lq1*P2u*P1v^2*PTu - 2*Lp1*Lq1*P2u*P1v*PTu*PTv + 2*Lp1*Lq1*P0v^2*PTu^2 - 2*Lp1*Lq1*P0v*P1v*PTu^2 - 2*Lp1*Lq1*P0v*P2v*PTu^2 + 2*Lp1*Lq1*P1v*P2v*PTu^2 - 2*Lp1*P0u^2*P1v^2 + 4*Lp1*P0u^2*P1v*PTv - 2*Lp1*P0u^2*PTv^2 + 4*Lp1*P0u*P1u*P0v*P1v - 4*Lp1*P0u*P1u*P0v*PTv - 2*Lp1*P0u*P1u*P1v*P2v - 2*Lp1*P0u*P1u*P1v*PTv + 2*Lp1*P0u*P1u*P2v*PTv + 2*Lp1*P0u*P1u*PTv^2 + 2*Lp1*P0u*P2u*P1v^2 - 4*Lp1*P0u*P2u*P1v*PTv + 2*Lp1*P0u*P2u*PTv^2 - 4*Lp1*P0u*P0v*P1v*PTu + 4*Lp1*P0u*P0v*PTu*PTv + 2*Lp1*P0u*P1v^2*PTu + 2*Lp1*P0u*P1v*P2v*PTu - 2*Lp1*P0u*P1v*PTu*PTv - 2*Lp1*P0u*P2v*PTu*PTv - 2*Lp1*P1u^2*P0v^2 + 2*Lp1*P1u^2*P0v*P2v + 2*Lp1*P1u^2*P0v*PTv - 2*Lp1*P1u^2*P2v*PTv - 2*Lp1*P1u*P2u*P0v*P1v + 2*Lp1*P1u*P2u*P0v*PTv + 2*Lp1*P1u*P2u*P1v*PTv - 2*Lp1*P1u*P2u*PTv^2 + 4*Lp1*P1u*P0v^2*PTu - 2*Lp1*P1u*P0v*P1v*PTu - 4*Lp1*P1u*P0v*P2v*PTu - 2*Lp1*P1u*P0v*PTu*PTv + 2*Lp1*P1u*P1v*P2v*PTu + 2*Lp1*P1u*P2v*PTu*PTv + 2*Lp1*P2u*P0v*P1v*PTu - 2*Lp1*P2u*P0v*PTu*PTv - 2*Lp1*P2u*P1v^2*PTu + 2*Lp1*P2u*P1v*PTu*PTv - 2*Lp1*P0v^2*PTu^2 + 2*Lp1*P0v*P1v*PTu^2 + 2*Lp1*P0v*P2v*PTu^2 - 2*Lp1*P1v*P2v*PTu^2 + Lp2^2*P0u^2*P1v^2 - 2*Lp2^2*P0u^2*P1v*P2v + Lp2^2*P0u^2*P2v^2 - 2*Lp2^2*P0u*P1u*P0v*P1v + 2*Lp2^2*P0u*P1u*P0v*P2v + 2*Lp2^2*P0u*P1u*P1v*P2v - 2*Lp2^2*P0u*P1u*P2v^2 + 2*Lp2^2*P0u*P2u*P0v*P1v - 2*Lp2^2*P0u*P2u*P0v*P2v - 2*Lp2^2*P0u*P2u*P1v^2 + 2*Lp2^2*P0u*P2u*P1v*P2v + Lp2^2*P1u^2*P0v^2 - 2*Lp2^2*P1u^2*P0v*P2v + Lp2^2*P1u^2*P2v^2 - 2*Lp2^2*P1u*P2u*P0v^2 + 2*Lp2^2*P1u*P2u*P0v*P1v + 2*Lp2^2*P1u*P2u*P0v*P2v - 2*Lp2^2*P1u*P2u*P1v*P2v + Lp2^2*P2u^2*P0v^2 - 2*Lp2^2*P2u^2*P0v*P1v + Lp2^2*P2u^2*P1v^2 - 2*Lp2*Lq1*P0u^2*P1v^2 + 2*Lp2*Lq1*P0u^2*P1v*P2v + 2*Lp2*Lq1*P0u^2*P1v*PTv - 2*Lp2*Lq1*P0u^2*P2v*PTv + 4*Lp2*Lq1*P0u*P1u*P0v*P1v - 2*Lp2*Lq1*P0u*P1u*P0v*P2v - 2*Lp2*Lq1*P0u*P1u*P0v*PTv - 2*Lp2*Lq1*P0u*P1u*P1v*P2v - 2*Lp2*Lq1*P0u*P1u*P1v*PTv + 4*Lp2*Lq1*P0u*P1u*P2v*PTv - 2*Lp2*Lq1*P0u*P2u*P0v*P1v + 2*Lp2*Lq1*P0u*P2u*P0v*PTv + 2*Lp2*Lq1*P0u*P2u*P1v^2 - 2*Lp2*Lq1*P0u*P2u*P1v*PTv - 2*Lp2*Lq1*P0u*P0v*P1v*PTu + 2*Lp2*Lq1*P0u*P0v*P2v*PTu + 2*Lp2*Lq1*P0u*P1v^2*PTu - 2*Lp2*Lq1*P0u*P1v*P2v*PTu - 2*Lp2*Lq1*P1u^2*P0v^2 + 2*Lp2*Lq1*P1u^2*P0v*P2v + 2*Lp2*Lq1*P1u^2*P0v*PTv - 2*Lp2*Lq1*P1u^2*P2v*PTv + 2*Lp2*Lq1*P1u*P2u*P0v^2 - 2*Lp2*Lq1*P1u*P2u*P0v*P1v - 2*Lp2*Lq1*P1u*P2u*P0v*PTv + 2*Lp2*Lq1*P1u*P2u*P1v*PTv + 2*Lp2*Lq1*P1u*P0v^2*PTu - 2*Lp2*Lq1*P1u*P0v*P1v*PTu - 2*Lp2*Lq1*P1u*P0v*P2v*PTu + 2*Lp2*Lq1*P1u*P1v*P2v*PTu - 2*Lp2*Lq1*P2u*P0v^2*PTu + 4*Lp2*Lq1*P2u*P0v*P1v*PTu - 2*Lp2*Lq1*P2u*P1v^2*PTu - 2*Lp2*P0u^2*P1v^2 - 2*Lp2*P0u^2*P1v*P2v + 6*Lp2*P0u^2*P1v*PTv + 2*Lp2*P0u^2*P2v*PTv - 4*Lp2*P0u^2*PTv^2 + 4*Lp2*P0u*P1u*P0v*P1v + 2*Lp2*P0u*P1u*P0v*P2v - 6*Lp2*P0u*P1u*P0v*PTv - 2*Lp2*P0u*P1u*P1v*P2v - 2*Lp2*P0u*P1u*P1v*PTv + 4*Lp2*P0u*P1u*PTv^2 + 2*Lp2*P0u*P2u*P0v*P1v - 2*Lp2*P0u*P2u*P0v*PTv + 2*Lp2*P0u*P2u*P1v^2 - 6*Lp2*P0u*P2u*P1v*PTv + 4*Lp2*P0u*P2u*PTv^2 - 6*Lp2*P0u*P0v*P1v*PTu - 2*Lp2*P0u*P0v*P2v*PTu + 8*Lp2*P0u*P0v*PTu*PTv + 2*Lp2*P0u*P1v^2*PTu + 6*Lp2*P0u*P1v*P2v*PTu - 4*Lp2*P0u*P1v*PTu*PTv - 4*Lp2*P0u*P2v*PTu*PTv - 2*Lp2*P1u^2*P0v^2 + 2*Lp2*P1u^2*P0v*P2v + 2*Lp2*P1u^2*P0v*PTv - 2*Lp2*P1u^2*P2v*PTv - 2*Lp2*P1u*P2u*P0v^2 - 2*Lp2*P1u*P2u*P0v*P1v + 6*Lp2*P1u*P2u*P0v*PTv + 2*Lp2*P1u*P2u*P1v*PTv - 4*Lp2*P1u*P2u*PTv^2 + 6*Lp2*P1u*P0v^2*PTu - 2*Lp2*P1u*P0v*P1v*PTu - 6*Lp2*P1u*P0v*P2v*PTu - 4*Lp2*P1u*P0v*PTu*PTv + 2*Lp2*P1u*P1v*P2v*PTu + 4*Lp2*P1u*P2v*PTu*PTv + 2*Lp2*P2u*P0v^2*PTu - 4*Lp2*P2u*P0v*PTu*PTv - 2*Lp2*P2u*P1v^2*PTu + 4*Lp2*P2u*P1v*PTu*PTv - 4*Lp2*P0v^2*PTu^2 + 4*Lp2*P0v*P1v*PTu^2 + 4*Lp2*P0v*P2v*PTu^2 - 4*Lp2*P1v*P2v*PTu^2 + Lq1^2*P0u^2*P1v^2 - 2*Lq1^2*P0u^2*P1v*PTv + Lq1^2*P0u^2*PTv^2 - 2*Lq1^2*P0u*P1u*P0v*P1v + 2*Lq1^2*P0u*P1u*P0v*PTv + 2*Lq1^2*P0u*P1u*P1v*PTv - 2*Lq1^2*P0u*P1u*PTv^2 + 2*Lq1^2*P0u*P0v*P1v*PTu - 2*Lq1^2*P0u*P0v*PTu*PTv - 2*Lq1^2*P0u*P1v^2*PTu + 2*Lq1^2*P0u*P1v*PTu*PTv + Lq1^2*P1u^2*P0v^2 - 2*Lq1^2*P1u^2*P0v*PTv + Lq1^2*P1u^2*PTv^2 - 2*Lq1^2*P1u*P0v^2*PTu + 2*Lq1^2*P1u*P0v*P1v*PTu + 2*Lq1^2*P1u*P0v*PTu*PTv - 2*Lq1^2*P1u*P1v*PTu*PTv + Lq1^2*P0v^2*PTu^2 - 2*Lq1^2*P0v*P1v*PTu^2 + Lq1^2*P1v^2*PTu^2 + 2*Lq1*P0u^2*P1v^2 - 4*Lq1*P0u^2*P1v*PTv + 2*Lq1*P0u^2*PTv^2 - 4*Lq1*P0u*P1u*P0v*P1v + 4*Lq1*P0u*P1u*P0v*PTv + 4*Lq1*P0u*P1u*P1v*PTv - 4*Lq1*P0u*P1u*PTv^2 + 4*Lq1*P0u*P0v*P1v*PTu - 4*Lq1*P0u*P0v*PTu*PTv - 4*Lq1*P0u*P1v^2*PTu + 4*Lq1*P0u*P1v*PTu*PTv + 2*Lq1*P1u^2*P0v^2 - 4*Lq1*P1u^2*P0v*PTv + 2*Lq1*P1u^2*PTv^2 - 4*Lq1*P1u*P0v^2*PTu + 4*Lq1*P1u*P0v*P1v*PTu + 4*Lq1*P1u*P0v*PTu*PTv - 4*Lq1*P1u*P1v*PTu*PTv + 2*Lq1*P0v^2*PTu^2 - 4*Lq1*P0v*P1v*PTu^2 + 2*Lq1*P1v^2*PTu^2 + P0u^2*P1v^2 - 2*P0u^2*P1v*PTv + P0u^2*PTv^2 - 2*P0u*P1u*P0v*P1v + 2*P0u*P1u*P0v*PTv + 2*P0u*P1u*P1v*PTv - 2*P0u*P1u*PTv^2 + 2*P0u*P0v*P1v*PTu - 2*P0u*P0v*PTu*PTv - 2*P0u*P1v^2*PTu + 2*P0u*P1v*PTu*PTv + P1u^2*P0v^2 - 2*P1u^2*P0v*PTv + P1u^2*PTv^2 - 2*P1u*P0v^2*PTu + 2*P1u*P0v*P1v*PTu + 2*P1u*P0v*PTu*PTv - 2*P1u*P1v*PTu*PTv + P0v^2*PTu^2 - 2*P0v*P1v*PTu^2 + P1v^2*PTu^2)^(1/2) + P0u*P1v - P1u*P0v - P0u*PTv + P0v*PTu + P1u*PTv - P1v*PTu - Lp1*P0u*P1v + Lp1*P1u*P0v + 2*Lp1*P0u*P2v - 2*Lp1*P2u*P0v - Lp2*P0u*P1v + Lp2*P1u*P0v - Lp1*P1u*P2v + Lp1*P2u*P1v + Lp2*P0u*P2v - Lp2*P2u*P0v - Lp2*P1u*P2v + Lp2*P2u*P1v + Lq1*P0u*P1v - Lq1*P1u*P0v - Lp1*P0u*PTv + Lp1*P0v*PTu + Lp1*P2u*PTv - Lp1*P2v*PTu - Lq1*P0u*PTv + Lq1*P0v*PTu + Lq1*P1u*PTv - Lq1*P1v*PTu)/(2*(P0u*P1v - P1u*P0v - P0u*PTv + P0v*PTu + P1u*PTv - P1v*PTu - Lp1*P0u*P1v + Lp1*P1u*P0v + Lp1*P0u*P2v - Lp1*P2u*P0v - Lp1*P1u*P2v + Lp1*P2u*P1v));
else
    dR = ((Lp1^2*P0u^2*P2v^2 - 2*Lp1^2*P0u^2*P2v*PTv + Lp1^2*P0u^2*PTv^2 - 2*Lp1^2*P0u*P2u*P0v*P2v + 2*Lp1^2*P0u*P2u*P0v*PTv + 2*Lp1^2*P0u*P2u*P2v*PTv - 2*Lp1^2*P0u*P2u*PTv^2 + 2*Lp1^2*P0u*P0v*P2v*PTu - 2*Lp1^2*P0u*P0v*PTu*PTv - 2*Lp1^2*P0u*P2v^2*PTu + 2*Lp1^2*P0u*P2v*PTu*PTv + Lp1^2*P2u^2*P0v^2 - 2*Lp1^2*P2u^2*P0v*PTv + Lp1^2*P2u^2*PTv^2 - 2*Lp1^2*P2u*P0v^2*PTu + 2*Lp1^2*P2u*P0v*P2v*PTu + 2*Lp1^2*P2u*P0v*PTu*PTv - 2*Lp1^2*P2u*P2v*PTu*PTv + Lp1^2*P0v^2*PTu^2 - 2*Lp1^2*P0v*P2v*PTu^2 + Lp1^2*P2v^2*PTu^2 - 2*Lp1*Lp2*P0u^2*P1v*P2v + 2*Lp1*Lp2*P0u^2*P1v*PTv + 2*Lp1*Lp2*P0u^2*P2v^2 - 2*Lp1*Lp2*P0u^2*P2v*PTv + 2*Lp1*Lp2*P0u*P1u*P0v*P2v - 2*Lp1*Lp2*P0u*P1u*P0v*PTv - 2*Lp1*Lp2*P0u*P1u*P2v^2 + 2*Lp1*Lp2*P0u*P1u*P2v*PTv + 2*Lp1*Lp2*P0u*P2u*P0v*P1v - 4*Lp1*Lp2*P0u*P2u*P0v*P2v + 2*Lp1*Lp2*P0u*P2u*P0v*PTv + 2*Lp1*Lp2*P0u*P2u*P1v*P2v - 4*Lp1*Lp2*P0u*P2u*P1v*PTv + 2*Lp1*Lp2*P0u*P2u*P2v*PTv - 2*Lp1*Lp2*P0u*P0v*P1v*PTu + 2*Lp1*Lp2*P0u*P0v*P2v*PTu + 2*Lp1*Lp2*P0u*P1v*P2v*PTu - 2*Lp1*Lp2*P0u*P2v^2*PTu - 2*Lp1*Lp2*P1u*P2u*P0v^2 + 2*Lp1*Lp2*P1u*P2u*P0v*P2v + 2*Lp1*Lp2*P1u*P2u*P0v*PTv - 2*Lp1*Lp2*P1u*P2u*P2v*PTv + 2*Lp1*Lp2*P1u*P0v^2*PTu - 4*Lp1*Lp2*P1u*P0v*P2v*PTu + 2*Lp1*Lp2*P1u*P2v^2*PTu + 2*Lp1*Lp2*P2u^2*P0v^2 - 2*Lp1*Lp2*P2u^2*P0v*P1v - 2*Lp1*Lp2*P2u^2*P0v*PTv + 2*Lp1*Lp2*P2u^2*P1v*PTv - 2*Lp1*Lp2*P2u*P0v^2*PTu + 2*Lp1*Lp2*P2u*P0v*P1v*PTu + 2*Lp1*Lp2*P2u*P0v*P2v*PTu - 2*Lp1*Lp2*P2u*P1v*P2v*PTu - 2*Lp1*Lq1*P0u^2*P1v*P2v + 2*Lp1*Lq1*P0u^2*P1v*PTv + 2*Lp1*Lq1*P0u^2*P2v*PTv - 2*Lp1*Lq1*P0u^2*PTv^2 + 2*Lp1*Lq1*P0u*P1u*P0v*P2v - 2*Lp1*Lq1*P0u*P1u*P0v*PTv - 2*Lp1*Lq1*P0u*P1u*P2v*PTv + 2*Lp1*Lq1*P0u*P1u*PTv^2 + 2*Lp1*Lq1*P0u*P2u*P0v*P1v - 2*Lp1*Lq1*P0u*P2u*P0v*PTv - 2*Lp1*Lq1*P0u*P2u*P1v*PTv + 2*Lp1*Lq1*P0u*P2u*PTv^2 - 2*Lp1*Lq1*P0u*P0v*P1v*PTu - 2*Lp1*Lq1*P0u*P0v*P2v*PTu + 4*Lp1*Lq1*P0u*P0v*PTu*PTv + 4*Lp1*Lq1*P0u*P1v*P2v*PTu - 2*Lp1*Lq1*P0u*P1v*PTu*PTv - 2*Lp1*Lq1*P0u*P2v*PTu*PTv - 2*Lp1*Lq1*P1u*P2u*P0v^2 + 4*Lp1*Lq1*P1u*P2u*P0v*PTv - 2*Lp1*Lq1*P1u*P2u*PTv^2 + 2*Lp1*Lq1*P1u*P0v^2*PTu - 2*Lp1*Lq1*P1u*P0v*P2v*PTu - 2*Lp1*Lq1*P1u*P0v*PTu*PTv + 2*Lp1*Lq1*P1u*P2v*PTu*PTv + 2*Lp1*Lq1*P2u*P0v^2*PTu - 2*Lp1*Lq1*P2u*P0v*P1v*PTu - 2*Lp1*Lq1*P2u*P0v*PTu*PTv + 2*Lp1*Lq1*P2u*P1v*PTu*PTv - 2*Lp1*Lq1*P0v^2*PTu^2 + 2*Lp1*Lq1*P0v*P1v*PTu^2 + 2*Lp1*Lq1*P0v*P2v*PTu^2 - 2*Lp1*Lq1*P1v*P2v*PTu^2 + Lp2^2*P0u^2*P1v^2 - 2*Lp2^2*P0u^2*P1v*P2v + Lp2^2*P0u^2*P2v^2 - 2*Lp2^2*P0u*P1u*P0v*P1v + 2*Lp2^2*P0u*P1u*P0v*P2v + 2*Lp2^2*P0u*P1u*P1v*P2v - 2*Lp2^2*P0u*P1u*P2v^2 + 2*Lp2^2*P0u*P2u*P0v*P1v - 2*Lp2^2*P0u*P2u*P0v*P2v - 2*Lp2^2*P0u*P2u*P1v^2 + 2*Lp2^2*P0u*P2u*P1v*P2v + Lp2^2*P1u^2*P0v^2 - 2*Lp2^2*P1u^2*P0v*P2v + Lp2^2*P1u^2*P2v^2 - 2*Lp2^2*P1u*P2u*P0v^2 + 2*Lp2^2*P1u*P2u*P0v*P1v + 2*Lp2^2*P1u*P2u*P0v*P2v - 2*Lp2^2*P1u*P2u*P1v*P2v + Lp2^2*P2u^2*P0v^2 - 2*Lp2^2*P2u^2*P0v*P1v + Lp2^2*P2u^2*P1v^2 - 2*Lp2*Lq1*P0u^2*P1v^2 + 2*Lp2*Lq1*P0u^2*P1v*P2v + 2*Lp2*Lq1*P0u^2*P1v*PTv - 2*Lp2*Lq1*P0u^2*P2v*PTv + 4*Lp2*Lq1*P0u*P1u*P0v*P1v - 2*Lp2*Lq1*P0u*P1u*P0v*P2v - 2*Lp2*Lq1*P0u*P1u*P0v*PTv - 2*Lp2*Lq1*P0u*P1u*P1v*P2v - 2*Lp2*Lq1*P0u*P1u*P1v*PTv + 4*Lp2*Lq1*P0u*P1u*P2v*PTv - 2*Lp2*Lq1*P0u*P2u*P0v*P1v + 2*Lp2*Lq1*P0u*P2u*P0v*PTv + 2*Lp2*Lq1*P0u*P2u*P1v^2 - 2*Lp2*Lq1*P0u*P2u*P1v*PTv - 2*Lp2*Lq1*P0u*P0v*P1v*PTu + 2*Lp2*Lq1*P0u*P0v*P2v*PTu + 2*Lp2*Lq1*P0u*P1v^2*PTu - 2*Lp2*Lq1*P0u*P1v*P2v*PTu - 2*Lp2*Lq1*P1u^2*P0v^2 + 2*Lp2*Lq1*P1u^2*P0v*P2v + 2*Lp2*Lq1*P1u^2*P0v*PTv - 2*Lp2*Lq1*P1u^2*P2v*PTv + 2*Lp2*Lq1*P1u*P2u*P0v^2 - 2*Lp2*Lq1*P1u*P2u*P0v*P1v - 2*Lp2*Lq1*P1u*P2u*P0v*PTv + 2*Lp2*Lq1*P1u*P2u*P1v*PTv + 2*Lp2*Lq1*P1u*P0v^2*PTu - 2*Lp2*Lq1*P1u*P0v*P1v*PTu - 2*Lp2*Lq1*P1u*P0v*P2v*PTu + 2*Lp2*Lq1*P1u*P1v*P2v*PTu - 2*Lp2*Lq1*P2u*P0v^2*PTu + 4*Lp2*Lq1*P2u*P0v*P1v*PTu - 2*Lp2*Lq1*P2u*P1v^2*PTu + 4*Lp2*P0u^2*P1v*P2v - 4*Lp2*P0u^2*P1v*PTv - 4*Lp2*P0u^2*P2v*PTv + 4*Lp2*P0u^2*PTv^2 - 4*Lp2*P0u*P1u*P0v*P2v + 4*Lp2*P0u*P1u*P0v*PTv + 4*Lp2*P0u*P1u*P2v*PTv - 4*Lp2*P0u*P1u*PTv^2 - 4*Lp2*P0u*P2u*P0v*P1v + 4*Lp2*P0u*P2u*P0v*PTv + 4*Lp2*P0u*P2u*P1v*PTv - 4*Lp2*P0u*P2u*PTv^2 + 4*Lp2*P0u*P0v*P1v*PTu + 4*Lp2*P0u*P0v*P2v*PTu - 8*Lp2*P0u*P0v*PTu*PTv - 8*Lp2*P0u*P1v*P2v*PTu + 4*Lp2*P0u*P1v*PTu*PTv + 4*Lp2*P0u*P2v*PTu*PTv + 4*Lp2*P1u*P2u*P0v^2 - 8*Lp2*P1u*P2u*P0v*PTv + 4*Lp2*P1u*P2u*PTv^2 - 4*Lp2*P1u*P0v^2*PTu + 4*Lp2*P1u*P0v*P2v*PTu + 4*Lp2*P1u*P0v*PTu*PTv - 4*Lp2*P1u*P2v*PTu*PTv - 4*Lp2*P2u*P0v^2*PTu + 4*Lp2*P2u*P0v*P1v*PTu + 4*Lp2*P2u*P0v*PTu*PTv - 4*Lp2*P2u*P1v*PTu*PTv + 4*Lp2*P0v^2*PTu^2 - 4*Lp2*P0v*P1v*PTu^2 - 4*Lp2*P0v*P2v*PTu^2 + 4*Lp2*P1v*P2v*PTu^2 + Lq1^2*P0u^2*P1v^2 - 2*Lq1^2*P0u^2*P1v*PTv + Lq1^2*P0u^2*PTv^2 - 2*Lq1^2*P0u*P1u*P0v*P1v + 2*Lq1^2*P0u*P1u*P0v*PTv + 2*Lq1^2*P0u*P1u*P1v*PTv - 2*Lq1^2*P0u*P1u*PTv^2 + 2*Lq1^2*P0u*P0v*P1v*PTu - 2*Lq1^2*P0u*P0v*PTu*PTv - 2*Lq1^2*P0u*P1v^2*PTu + 2*Lq1^2*P0u*P1v*PTu*PTv + Lq1^2*P1u^2*P0v^2 - 2*Lq1^2*P1u^2*P0v*PTv + Lq1^2*P1u^2*PTv^2 - 2*Lq1^2*P1u*P0v^2*PTu + 2*Lq1^2*P1u*P0v*P1v*PTu + 2*Lq1^2*P1u*P0v*PTu*PTv - 2*Lq1^2*P1u*P1v*PTu*PTv + Lq1^2*P0v^2*PTu^2 - 2*Lq1^2*P0v*P1v*PTu^2 + Lq1^2*P1v^2*PTu^2)^(1/2) + Lp1*P0u*P2v - Lp1*P2u*P0v + Lp2*P0u*P1v - Lp2*P1u*P0v - Lp2*P0u*P2v + Lp2*P2u*P0v + Lp2*P1u*P2v - Lp2*P2u*P1v - Lq1*P0u*P1v + Lq1*P1u*P0v - Lp1*P0u*PTv + Lp1*P0v*PTu + Lp1*P2u*PTv - Lp1*P2v*PTu + Lq1*P0u*PTv - Lq1*P0v*PTu - Lq1*P1u*PTv + Lq1*P1v*PTu)/(2*(P0u*P1v - P1u*P0v - P0u*PTv + P0v*PTu + P1u*PTv - P1v*PTu - Lp1*P0u*P1v + Lp1*P1u*P0v + Lp1*P0u*P2v - Lp1*P2u*P0v - Lp1*P1u*P2v + Lp1*P2u*P1v));
end

level = rat11(dR, rightFit, rightInversed);

end

