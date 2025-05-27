struct HitAttributes
{
    float2 barycentrics;
};


[shader("intersection")]
void main()
{

  float3 origin = ObjectRayOrigin();
    float3 direction = ObjectRayDirection();
    
    // Sphere parameters
    float3 center = float3(0, 0, 0);  // Assuming sphere is centered at origin
    float radius = 1.0;  // Assuming unit sphere
    
    // Calculate quadratic equation coefficients
    float a = dot(direction, direction);
    float b = 2.0 * dot(origin - center, direction);
    float c = dot(origin - center, origin - center) - radius * radius;
    
    // Solve quadratic equation
    float discriminant = b * b - 4 * a * c;
    
    if (discriminant < 0)
    {
        // No intersection
        return;
    }
    
    float t0 = (-b - sqrt(discriminant)) / (2 * a);
    float t1 = (-b + sqrt(discriminant)) / (2 * a);
    
    // Prepare hit attributes
    HitAttributes attr;
    attr.barycentrics = float2(0, 0);  // For a sphere, we don't use barycentrics

    // Check if intersection is within ray parameters
    if (t0 > 0 && t0 < RayTCurrent())
    {
        ReportHit(t0, 0, attr);
    }
    else if (t1 > 0 && t1 < RayTCurrent())
    {
        ReportHit(t1, 0, attr);
    }
}