#include "pch.h"
#include "Intersections.h"
//#include "Mesh3D.h"
#include "BasicMath.hpp"
//#include "TerrainMesh.h"
#include "StaticMeshComponent.h"

Intersections::Intersections() {
    LoadProgram("engine/cl/intersects/intersects.cl");
    kernel = cl::Kernel(program, "findClosestIntersection");
    int check = 1;
}

void CL_CALLBACK errorCallback(cl_int err, const char* msg, void* data) {
    std::cerr << "Error " << err << ": " << msg << std::endl;
}

int float_to_int(float f) {
    int i;
    memcpy(&i, &f, sizeof(int));
    return i;
}

float int_to_float(int i) {
    float f;
    memcpy(&f, &i, sizeof(float));
    return f;
}

void Intersections::InitializeBuffers() {
    if (!m_BuffersInitialized) {
        // Create persistent buffers - don't allocate specific sizes yet
        m_PosBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(float3));
        m_DirBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(float3));
        m_ResultBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(int));
        m_HitPointBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(float3)); // Add this line
        m_BuffersInitialized = true;
    }
}

CastResult Intersections::CastMesh(float3 pos, float3 dir, SubMesh* mesh) {
    // Initialize persistent buffers on first use
    InitializeBuffers();

    const size_t numTris = mesh->m_Triangles.size();

    // Early exit for empty meshes
    if (numTris == 0) {
        return CastResult{ false };
    }

    // Initialize result with FLT_MAX converted to int (exactly like original)
    int initialResult = float_to_int(FLT_MAX);

    float3 initialHitPoint = { 0.0f, 0.0f, 0.0f }; // Add this line
    // Write data to persistent buffers - use BLOCKING writes to ensure data is ready
    queue.enqueueWriteBuffer(m_PosBuffer, CL_TRUE, 0, sizeof(float3), &pos);
    queue.enqueueWriteBuffer(m_DirBuffer, CL_TRUE, 0, sizeof(float3), &dir);
    queue.enqueueWriteBuffer(m_ResultBuffer, CL_TRUE, 0, sizeof(int), &initialResult);
    queue.enqueueWriteBuffer(m_HitPointBuffer, CL_TRUE, 0, sizeof(float3), &initialHitPoint);

    // Handle triangle buffer - FIXED LOGIC
    cl::Buffer* triBuffer = nullptr;
    auto bufferIt = m_TriBuffers.find(mesh);

    if (mesh->RebuildIf()) {
        printf("Rebuilding Geo.\n");
        // Rebuild triangle buffer
        auto geoData = mesh->BuildGeo();
        size_t bufferSize = sizeof(float3) * numTris * 3;

        // Create new buffer and store it
        m_TriBuffers[mesh] = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            bufferSize, (void*)geoData);
        triBuffer = &m_TriBuffers[mesh];

        // Also update legacy buffer cache for compatibility
        m_Buffers[mesh] = m_TriBuffers[mesh];
    }
    else {
        printf("NOT rebuilding geo flag, but updating buffer data.\n");

        // Even if RebuildIf() returns false, we still need to update the buffer
        // with current geometry data in case the mesh has been modified
        auto geoData = mesh->BuildGeo();
        size_t bufferSize = sizeof(float3) * numTris * 3;

        if (bufferIt != m_TriBuffers.end()) {
            // Buffer exists - update it with current geometry data
            queue.enqueueWriteBuffer(bufferIt->second, CL_TRUE, 0, bufferSize, (void*)geoData);
            triBuffer = &bufferIt->second;
        }
        else {
            // Buffer doesn't exist yet, create it
            m_TriBuffers[mesh] = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                bufferSize, (void*)geoData);
            triBuffer = &m_TriBuffers[mesh];

            // Also update legacy buffer cache
            m_Buffers[mesh] = m_TriBuffers[mesh];
        }
    }

    // Set kernel arguments
    kernel.setArg(0, m_PosBuffer);
    kernel.setArg(1, m_DirBuffer);
    kernel.setArg(2, m_ResultBuffer);
    kernel.setArg(3, m_HitPointBuffer); // Add this line
    kernel.setArg(4, *triBuffer); // This changes from arg 3 to arg 4

    // Execute kernel - start simple, optimize work groups later
    cl::NDRange globalSize(numTris);
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, globalSize, cl::NullRange);


    // Read result as int (exactly like original)
    int intResult;
    float3 hitPoint; // Add this line
    queue.enqueueReadBuffer(m_ResultBuffer, CL_TRUE, 0, sizeof(int), &intResult);
    queue.enqueueReadBuffer(m_HitPointBuffer, CL_TRUE, 0, sizeof(float3), &hitPoint);

    // Convert back to float (exactly like original)
    float distance = int_to_float(intResult);

    // Process result (exactly like original logic)
    CastResult result;
    result.Hit = false;
    result.Distance = -1.0f;
    result.HitPoint = { 0.0f, 0.0f, 0.0f }; // Add this line

    if (distance > -1.0f && distance < 1000.0f) {
        result.Hit = true;
        result.Distance = distance;
        result.HitPoint = hitPoint; // Add this line
    }

    return result;
}

size_t Intersections::GetOptimalWorkGroupSize(size_t numTris) const {
    // Get device info for optimal work group sizing
    size_t maxWorkGroupSize;
    kernel.getWorkGroupInfo(device, CL_KERNEL_WORK_GROUP_SIZE, &maxWorkGroupSize);

    // Common optimal sizes for AMD GPUs
    const size_t commonSizes[] = { 64, 128, 256 };

    for (size_t size : commonSizes) {
        if (size <= maxWorkGroupSize && numTris >= size) {
            return size;
        }
    }

    return std::min(maxWorkGroupSize, numTris);
}

size_t Intersections::RoundUpToMultiple(size_t value, size_t multiple) const {
    return ((value + multiple - 1) / multiple) * multiple;
}

CastResult Intersections::CastTerrainMesh(float3 pos, float3 dir, TerrainMesh* mesh) {
    // Keep existing implementation or update similarly
    return CastResult();
}