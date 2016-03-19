classdef Led

    properties
        name;
        spd;
        cct;
        lumens;
        ler;
        power;
        maxCoeff;
    end
    
    methods
        function this = Led( name, spd, lumens, maxCoeff )
            this.name = name;
            this.spd = spd;
            this.lumens = lumens;
            this.ler = spdToLER(spd);
            this.power = this.lumens / this.ler;
            this.cct = spdToCct(spd);
            if exist('maxCoeff', 'var')
                this.maxCoeff = maxCoeff;
            else
                this.maxCoeff = 1;
            end
        end
    end
    
end

