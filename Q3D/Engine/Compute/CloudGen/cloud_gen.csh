// The output texture, bound as an Unordered Access View (UAV)
RWTexture3D<float> g_OutputTexture;

// Input constants from the C++ side
cbuffer Constants
{
    float g_Time;
    float g_Coverage;
    uint3 g_TexDimensions;
};

// --- HASH & NOISE FUNCTIONS (HLSL port of common noise algorithms) ---
// These helper functions replicate what FastNoiseLite was doing on the CPU.

// 3D hash function
float3 hash33(float3 p)
{
    p = float3(dot(p, float3(127.1, 311.7, 74.7)),
               dot(p, float3(269.5, 183.3, 246.1)),
               dot(p, float3(113.5, 271.9, 124.6)));
    return frac(sin(p) * 43758.5453123);
}

// 3D Perlin Noise
float perlin_noise(float3 p)
{
    float3 pi = floor(p);
    float3 pf = p - pi;
    float3 w = pf * pf * (3.0 - 2.0 * pf);

    return lerp(
        lerp(
            lerp(dot(hash33(pi + float3(0, 0, 0)), pf - float3(0, 0, 0)), dot(hash33(pi + float3(1, 0, 0)), pf - float3(1, 0, 0)), w.x),
            lerp(dot(hash33(pi + float3(0, 1, 0)), pf - float3(0, 1, 0)), dot(hash33(pi + float3(1, 1, 0)), pf - float3(1, 1, 0)), w.x),
            w.y),
        lerp(
            lerp(dot(hash33(pi + float3(0, 0, 1)), pf - float3(0, 0, 1)), dot(hash33(pi + float3(1, 0, 1)), pf - float3(1, 0, 1)), w.x),
            lerp(dot(hash33(pi + float3(0, 1, 1)), pf - float3(0, 1, 1)), dot(hash33(pi + float3(1, 1, 1)), pf - float3(1, 1, 1)), w.x),
            w.y),
        w.z);
}



// Define how many threads will run in a single group. 8x8x8 is a good default.
[numthreads(8, 8, 8)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    // DTid is the unique ID of the current thread, which maps to our texture coordinate (x,y,d)
    
    // Safety check: Don't write outside the texture bounds
    if (DTid.x >= g_TexDimensions.x || DTid.y >= g_TexDimensions.y || DTid.z >= g_TexDimensions.z)
    {
        return;
    }

    // --- The logic below is a direct translation of your C++ function ---

     // --- 1. Define Anisotropic Frequency Scale ---
    // Using a smaller Y-value for the frequency "stretches" the noise
    // vertically, creating taller, more billowy cloud shapes.
    // A value of 1.0 would be uniform (puffy), smaller values are taller.
    float3 frequency_scale = float3(1.0, 0.35, 1.0);

    // Setup base noise frequencies
    float base_freq = 0.02;
    float detail_freq = 0.1;

    // --- 2. Sample Noise using the new scale ---
    float3 base_coords = ((float3) DTid * frequency_scale) * base_freq + float3(0, 0, g_Time);
    float baseValue = perlin_noise(base_coords);
    baseValue = (baseValue + 1.0) / 2.0;

    float3 detail_coords = ((float3) DTid * frequency_scale) * detail_freq + float3(0, 0, g_Time);
    float detailValue = perlin_noise(detail_coords);
    detailValue = (detailValue + 1.0) / 2.0;
    
    // --- 3. Combine and Apply Effects (Logic is the same) ---
    float cloudCoverage = smoothstep(1.0 - g_Coverage, 1.0, baseValue);
    float density = saturate(cloudCoverage - detailValue * 0.5);

    float normY = (float) DTid.y / g_TexDimensions.y;
    density *= smoothstep(0.2, 0.45, normY);

    float3 norm_pos = (float3) DTid / g_TexDimensions;
    float dist_from_center = distance(norm_pos, float3(0.5, 0.5, 0.5)) * 2.0;
    density *= 1.0 - smoothstep(1.4, 1.7, dist_from_center);
    
    g_OutputTexture[DTid] = density;
}