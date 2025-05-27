module Vivid.Maths.Vec3

class Vec3

    float X,Y,Z;

    func Vec3()

        X=0.0;
        Y=0.0;
        Z=0.0;

    end 

    func Plus(Vec3 r)

        Vec3 res = new Vec3();
        res.X = X+r.X;
        res.Y = Y+r.Y;
        res.Z = Z+r.Z;
        return res;

    end 

    func Minus(Vec3 r)

        Vec3 res = new Vec3();
        res.X = X-r.X;
        res.Y = Y-r.Y;
        res.Z = Z-r.Z;
        return res;

    end 

    func Times(Vec3 r)

        Vec3 res = new Vec3();
        res.X = X*r.X;
        res.Y = Y*r.Y;
        res.Z = Z*r.Z;
        return res;

    end 

    func Times(Matrix r)

        Vec3 l = new Vec3();
        l.X = X;
        l.Y = Y;
        l.Z = Z;

        return VecTimesMatrix(l,r);


    end 

end 

