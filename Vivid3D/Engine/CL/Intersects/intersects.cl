#define EPSILON 1.19209290e-07F

// Updated rayTri function to work with point1 to point2
struct RayResult {
    float distance;
    float3 hitPoint;
};

struct RayResult rayTri(float3 point1, float3 point2, float3 v0, float3 v1, float3 v2) {
    struct RayResult result;
    result.distance = 10000;
    result.hitPoint = (float3)(0.0f, 0.0f, 0.0f);
    
    // Calculate ray direction from point1 to point2
    float3 rayDir = point2 - point1;
    
    float3 edge1, edge2, h, s, q;
    float a, f, u, v;
    edge1 = v1 - v0;
    edge2 = v2 - v0;
    h = cross(rayDir, edge2);
    a = dot(edge1, h);
    if (a > -EPSILON && a < EPSILON)
        return result;

    f = 1.0f / a;
    s = point1 - v0;
    u = f * dot(s, h);
    if (u < 0.0f || u > 1.0f)
        return result;
    q = cross(s, edge1);
    v = f * dot(rayDir, q);
    if (v < 0.0f || u + v > 1.0f)
        return result;
    
    float t = f * dot(edge2, q);
    
    // Check if intersection is within the line segment (between point1 and point2)
    if (t > EPSILON && t <= 1.0f) {
        float3 hitPoint = point1 + rayDir * t;
        result.distance = length(hitPoint - point1);
        result.hitPoint = hitPoint;
        return result;
    }
    
    return result;
}

__kernel void findClosestIntersection(
    __global float3* point1,    // Changed from origin
    __global float3* point2,    // Changed from dir
    __global int* result,
    __global float3* hitPoint,
    __global float* points
) {
    int gid = get_global_id(0);
    int tid = gid * (3*3);

    float3 v0 = (float3)(points[tid], points[tid+1], points[tid+2]);
    float3 v1 = (float3)(points[tid+3], points[tid+4], points[tid+5]);
    float3 v2 = (float3)(points[tid+6], points[tid+7], points[tid+8]);

    struct RayResult rayRes = rayTri(point1[0], point2[0], v0, v1, v2);

    union { float f; int i; } u;
    u.f = rayRes.distance;

    // Atomic compare-and-swap to find the minimum distance
    int old = *result;
    while (u.i < old) {
        int prev = atomic_cmpxchg(result, old, u.i);
        if (prev == old) {
            // We successfully updated the minimum distance, also update hit point
            hitPoint[0] = rayRes.hitPoint;
            break;
        }
        old = prev;
    }
}