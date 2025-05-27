module Vivid.Maths.Maths

class Maths

    static func Clamp(float value,float min,float max)

        if(value<min)

            value = min;

        end 

        if(value>max)

            value = max;

        end 

        return value;

    end 

end 