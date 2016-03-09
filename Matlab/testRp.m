% Test Rp function
p = zeros(141, 101);
for Rg = 60:140
    for Rf = 50:100-abs(100-Rg)
        p(Rg,Rf) = RfRgToRp(Rf,Rg);
    end
end
mesh(p);
axis([50 100 60 140]);