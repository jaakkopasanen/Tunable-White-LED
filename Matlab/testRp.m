% Test Rp function
Rp = zeros((140-60)*5+1, (100-50)*5+1);
x = 1; y = 1;
for Rg = 60:0.2:140
    x = 1;
    for Rf = 50:0.2:100-abs(100-Rg)
        Rp(y,x) = RfRgToRp(Rf,Rg);
        x = x + 1;
    end
    y = y + 1;
end
Rp = Rp./100;
imagesc([50 100], [60 140], Rp);
set(gca, 'ydir', 'normal');