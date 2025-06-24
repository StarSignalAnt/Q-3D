using GlmNet;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace QNet.Maths
{
    public class Maths
    {
        public static mat3 ToMat3(mat4 modelMatrix)
        {
            mat3 upper3x3 = new mat3(
        new vec3(modelMatrix[0].x, modelMatrix[0].y, modelMatrix[0].z),
        new vec3(modelMatrix[1].x, modelMatrix[1].y, modelMatrix[1].z),
        new vec3(modelMatrix[2].x, modelMatrix[2].y, modelMatrix[2].z)
        );
            return upper3x3;
        }

    
        public static mat3 Transpose(mat3 m)
        {
            return new mat3(
                new vec3(m[0].x, m[1].x, m[2].x),
                new vec3(m[0].y, m[1].y, m[2].y),
                new vec3(m[0].z, m[1].z, m[2].z)
            );
        }
        public static mat4 Transpose(mat4 m)
        {
            return new mat4(
                new vec4(m[0].x, m[1].x, m[2].x, m[3].x),
                new vec4(m[0].y, m[1].y, m[2].y, m[3].y),
                new vec4(m[0].z, m[1].z, m[2].z, m[3].z),
                new vec4(m[0].w, m[1].w, m[2].w, m[3].w)
            );
        }
    }
}
