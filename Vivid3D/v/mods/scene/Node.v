module Vivid.Scene.Node

class Node

    CObject C;
    Vec3 Position;

    func Node()

        Position = new Vec3();
    
    end 

    func Turn(float p,float y,float r)

        TurnNode(C,p,y,r);

    end 

    func GetPosition()

        Vec3 res = GetPositionNode(C);

        return res;

    end 

    func SetPosition(Vec3 position)

        SetPositionNode(C,position);

    end 

    func SetAnimation(string name,int restart)

        PlayAnimNode(C,name,restart);

    end 

end 