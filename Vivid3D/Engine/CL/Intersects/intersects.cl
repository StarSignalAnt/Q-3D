#define EPSILON 1.19209290e-07F

float rayTri(float3 pos, float3 dir, float3 v0, float3 v1, float3 v2) {
     float3 edge1, edge2, h, s, q;
  float a, f, u, v;
 edge1 = v1 - v0;// vertex1 - vertex0;
 edge2 = v2 - v0;
 h = cross(dir, edge2);
  a = dot(edge1, h);
  if (a > -EPSILON && a < EPSILON)
      return 10000;

 f = 1.0f / a;
 s =pos - v0;
 u = f *dot(s, h);
 if (u < 0.0f || u > 1.0f)
     return 10000;
 q = cross(s, edge1);
 v = f * dot(dir, q);
 if (v < 0.0f || u + v > 1.0f)
     return 10000;
 // At this stage we can compute t to find out where the intersection point is on the line.
 float t = f * dot(edge2, q);
 
 if (t > EPSILON) // ray intersection
{

    float3 m_Point =pos + dir * t;
    return length(m_Point - pos);

}
else // This means that there is a line intersection but not a ray intersection.
    return 10000;

    // TODO: Implement the logic here
    // This is just an example to avoid unused variable warnings
  //  float3 temp = pos + dir + v0 + v1 + v2;
 //   return length(temp); // This is just a placeholder; replace with actual logic
}

__kernel void findClosestIntersection(
   
    __global float3* origin,
   __global float3* dir,


    __global int* result,
     __global float* points
) {
    int gid = get_global_id(0);
      int tid = gid * (3*3);  // Calculate the starting index of the current triangle

    //float x = points[tid];     // First vertex of the triangle
    //float y= points[tid+1];
    //float z= points[tid+2];
    float3 v0 = (float3)(points[tid],points[tid+1],points[tid+2]);
    float3 v1 = (float3)(points[tid+3],points[tid+4],points[tid+5]);
    float3 v2 = (float3)(points[tid+6],points[tid+7],points[tid+8]);



    //float3 v1 = points[tid + 1]; // Second vertex of the triangle
    //float3 v2 = points[tid + 2]; // Third vertex of the triangle
  float res = rayTri(origin[0],dir[0],v0,v1,v2);

  union { float f; int i; } u;
    u.f = res;

    // Atomic compare-and-swap to find the minimum
    int old = *result;
    while (u.i < old) {
        int prev = atomic_cmpxchg(result, old, u.i);
        if (prev == old) break;
        old = prev;
    }


//   if(res>-1){
    
 //  if(res<minResult[0]){
  //    minResult[0] =res;
  // }
  // }
   



  
}