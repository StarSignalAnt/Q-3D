#include "pch.h"
#include "Intersections.h"
#include "BasicMath.hpp"
#include "TerrainMesh.h"
#include "StaticMeshComponent.h"

Intersections::Intersections() {
    LoadProgram("engine/cl/intersects/intersects.cl");
    kernel = cl::Kernel(program, "findClosestIntersection");
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

bool Intersections::CheckCLError(cl_int err, const char* operation) const {
    if (err != CL_SUCCESS) {
        std::cerr << "OpenCL Error in " << operation << ": " << err << std::endl;
        return false;
    }
    return true;
}

void Intersections::InitializeMeshBuffers() {
    if (!m_MeshBuffers.initialized) {
        cl_int err;

        m_MeshBuffers.posBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(float3), nullptr, &err);
        if (!CheckCLError(err, "InitializeMeshBuffers - posBuffer")) return;

        m_MeshBuffers.dirBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(float3), nullptr, &err);
        if (!CheckCLError(err, "InitializeMeshBuffers - dirBuffer")) return;

        m_MeshBuffers.resultBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(int), nullptr, &err);
        if (!CheckCLError(err, "InitializeMeshBuffers - resultBuffer")) return;

        m_MeshBuffers.hitPointBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(float3), nullptr, &err);
        if (!CheckCLError(err, "InitializeMeshBuffers - hitPointBuffer")) return;

        m_MeshBuffers.initialized = true;
    }
}

void Intersections::InitializeTerrainBuffers() {
    if (!m_TerrainBuffers.initialized) {
        cl_int err;

        m_TerrainBuffers.posBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(float3), nullptr, &err);
        if (!CheckCLError(err, "InitializeTerrainBuffers - posBuffer")) return;

        m_TerrainBuffers.dirBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, sizeof(float3), nullptr, &err);
        if (!CheckCLError(err, "InitializeTerrainBuffers - dirBuffer")) return;

        m_TerrainBuffers.resultBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(int), nullptr, &err);
        if (!CheckCLError(err, "InitializeTerrainBuffers - resultBuffer")) return;

        m_TerrainBuffers.hitPointBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(float3), nullptr, &err);
        if (!CheckCLError(err, "InitializeTerrainBuffers - hitPointBuffer")) return;

        m_TerrainBuffers.initialized = true;
    }
}

CastResult Intersections::CastMesh(float3 pos, float3 dir, SubMesh* mesh) {
    // Thread safety - prevent interference between different cast operations
    std::lock_guard<std::mutex> lock(m_CastMutex);

    // Initialize mesh-specific buffers
    InitializeMeshBuffers();

    const size_t numTris = mesh->m_LODs[0]->m_Triangles.size();

    // Early exit for empty meshes
    if (numTris == 0) {
        return CastResult{ false };
    }

    // Initialize result with FLT_MAX converted to int
    int initialResult = float_to_int(FLT_MAX);
    float3 initialHitPoint = { 0.0f, 0.0f, 0.0f };

    // Write data to mesh-specific buffers with error checking
    cl_int err;
    err = queue.enqueueWriteBuffer(m_MeshBuffers.posBuffer, CL_TRUE, 0, sizeof(float3), &pos);
    if (!CheckCLError(err, "CastMesh - write posBuffer")) return CastResult{ false };

    err = queue.enqueueWriteBuffer(m_MeshBuffers.dirBuffer, CL_TRUE, 0, sizeof(float3), &dir);
    if (!CheckCLError(err, "CastMesh - write dirBuffer")) return CastResult{ false };

    err = queue.enqueueWriteBuffer(m_MeshBuffers.resultBuffer, CL_TRUE, 0, sizeof(int), &initialResult);
    if (!CheckCLError(err, "CastMesh - write resultBuffer")) return CastResult{ false };

    err = queue.enqueueWriteBuffer(m_MeshBuffers.hitPointBuffer, CL_TRUE, 0, sizeof(float3), &initialHitPoint);
    if (!CheckCLError(err, "CastMesh - write hitPointBuffer")) return CastResult{ false };

    // Handle triangle buffer
    cl::Buffer* triBuffer = nullptr;
    auto bufferIt = m_TriBuffers.find(mesh);

    if (mesh->RebuildIf()) {
        printf("Rebuilding Mesh Geo.\n");
        // Rebuild triangle buffer
        auto geoData = mesh->BuildGeo();
        size_t bufferSize = sizeof(float3) * numTris * 3;

        // Create new buffer and store it
        m_TriBuffers[mesh] = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            bufferSize, (void*)geoData, &err);
        if (!CheckCLError(err, "CastMesh - create triangle buffer")) return CastResult{ false };

        triBuffer = &m_TriBuffers[mesh];

        // Update legacy buffer cache for compatibility
        m_Buffers[mesh] = m_TriBuffers[mesh];
    }
    else {
        printf("NOT rebuilding mesh geo flag, but updating buffer data.\n");

        // Update buffer with current geometry data
        auto geoData = mesh->BuildGeo();
        size_t bufferSize = sizeof(float3) * numTris * 3;

        if (bufferIt != m_TriBuffers.end()) {
            // Buffer exists - update it with current geometry data
            err = queue.enqueueWriteBuffer(bufferIt->second, CL_TRUE, 0, bufferSize, (void*)geoData);
            if (!CheckCLError(err, "CastMesh - update triangle buffer")) return CastResult{ false };
            triBuffer = &bufferIt->second;
        }
        else {
            // Buffer doesn't exist yet, create it
            m_TriBuffers[mesh] = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                bufferSize, (void*)geoData, &err);
            if (!CheckCLError(err, "CastMesh - create new triangle buffer")) return CastResult{ false };
            triBuffer = &m_TriBuffers[mesh];

            // Update legacy buffer cache
            m_Buffers[mesh] = m_TriBuffers[mesh];
        }
    }

    // Set kernel arguments for mesh casting
    err = kernel.setArg(0, m_MeshBuffers.posBuffer);
    if (!CheckCLError(err, "CastMesh - setArg 0")) return CastResult{ false };

    err = kernel.setArg(1, m_MeshBuffers.dirBuffer);
    if (!CheckCLError(err, "CastMesh - setArg 1")) return CastResult{ false };

    err = kernel.setArg(2, m_MeshBuffers.resultBuffer);
    if (!CheckCLError(err, "CastMesh - setArg 2")) return CastResult{ false };

    err = kernel.setArg(3, m_MeshBuffers.hitPointBuffer);
    if (!CheckCLError(err, "CastMesh - setArg 3")) return CastResult{ false };

    err = kernel.setArg(4, *triBuffer);
    if (!CheckCLError(err, "CastMesh - setArg 4")) return CastResult{ false };

    // Execute kernel
    cl::NDRange globalSize(numTris);
    err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, globalSize, cl::NullRange);
    if (!CheckCLError(err, "CastMesh - enqueueNDRangeKernel")) return CastResult{ false };

    // Read results with error checking
    int intResult;
    float3 hitPoint;
    err = queue.enqueueReadBuffer(m_MeshBuffers.resultBuffer, CL_TRUE, 0, sizeof(int), &intResult);
    if (!CheckCLError(err, "CastMesh - read resultBuffer")) return CastResult{ false };

    err = queue.enqueueReadBuffer(m_MeshBuffers.hitPointBuffer, CL_TRUE, 0, sizeof(float3), &hitPoint);
    if (!CheckCLError(err, "CastMesh - read hitPointBuffer")) return CastResult{ false };

    // Convert back to float
    float distance = int_to_float(intResult);

    // Process result
    CastResult result;
    result.Hit = false;
    result.Distance = -1.0f;
    result.HitPoint = { 0.0f, 0.0f, 0.0f };

    if (distance > -1.0f && distance < 1000.0f) {
        result.Hit = true;
        result.Distance = distance;
        result.HitPoint = hitPoint;
    }

    return result;
}

CastResult Intersections::CastTerrainMesh(float3 pos, float3 dir, TerrainMesh* mesh) {
    // Thread safety - prevent interference between different cast operations
    std::lock_guard<std::mutex> lock(m_CastMutex);

    // Initialize terrain-specific buffers
    InitializeTerrainBuffers();

    const size_t numTris = mesh->GetTriangles().size();

    // Early exit for empty meshes
    if (numTris == 0) {
        return CastResult{ false };
    }

    // Initialize result with FLT_MAX converted to int
    int initialResult = float_to_int(FLT_MAX);
    float3 initialHitPoint = { 0.0f, 0.0f, 0.0f };

    // Write ray and initial result data to terrain-specific buffers with error checking
    cl_int err;
    err = queue.enqueueWriteBuffer(m_TerrainBuffers.posBuffer, CL_TRUE, 0, sizeof(float3), &pos);
    if (!CheckCLError(err, "CastTerrainMesh - write posBuffer")) return CastResult{ false };

    err = queue.enqueueWriteBuffer(m_TerrainBuffers.dirBuffer, CL_TRUE, 0, sizeof(float3), &dir);
    if (!CheckCLError(err, "CastTerrainMesh - write dirBuffer")) return CastResult{ false };

    err = queue.enqueueWriteBuffer(m_TerrainBuffers.resultBuffer, CL_TRUE, 0, sizeof(int), &initialResult);
    if (!CheckCLError(err, "CastTerrainMesh - write resultBuffer")) return CastResult{ false };

    err = queue.enqueueWriteBuffer(m_TerrainBuffers.hitPointBuffer, CL_TRUE, 0, sizeof(float3), &initialHitPoint);
    if (!CheckCLError(err, "CastTerrainMesh - write hitPointBuffer")) return CastResult{ false };

    // Handle the triangle buffer for the terrain mesh
    cl::Buffer* triBuffer = nullptr;
    auto bufferIt = m_TBuffers.find(mesh);

    // Always ensure geometry is up to date
    if (mesh->NeedRebuild()) {
        mesh->RebuildGeo();
        printf("Rebuilt terrain geometry.\n");
    }

    // Get the geometry data and convert to consistent format
    auto geoData = mesh->GetGeo();

    // Ensure there is geometry data to process
    if (geoData.empty()) {
        return CastResult{ false };
    }

    // Convert glm::vec3 to float3 for consistent data format
    std::vector<float3> float3Data;
    float3Data.reserve(geoData.size());

    for (const auto& vertex : geoData) {
        float3Data.push_back({ vertex.x, vertex.y, vertex.z });
    }

    size_t bufferSize = sizeof(float3) * float3Data.size();

    if (bufferIt != m_TBuffers.end()) {
        // Buffer already exists, update it with the latest geometry data
        err = queue.enqueueWriteBuffer(bufferIt->second, CL_TRUE, 0, bufferSize, (void*)float3Data.data());
        if (!CheckCLError(err, "CastTerrainMesh - update triangle buffer")) return CastResult{ false };
        triBuffer = &bufferIt->second;
    }
    else {
        // Buffer doesn't exist, create a new one
        m_TBuffers[mesh] = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            bufferSize, (void*)float3Data.data(), &err);
        if (!CheckCLError(err, "CastTerrainMesh - create triangle buffer")) return CastResult{ false };
        triBuffer = &m_TBuffers[mesh];
    }

    // Set the arguments for the OpenCL kernel (terrain-specific buffers)
    err = kernel.setArg(0, m_TerrainBuffers.posBuffer);
    if (!CheckCLError(err, "CastTerrainMesh - setArg 0")) return CastResult{ false };

    err = kernel.setArg(1, m_TerrainBuffers.dirBuffer);
    if (!CheckCLError(err, "CastTerrainMesh - setArg 1")) return CastResult{ false };

    err = kernel.setArg(2, m_TerrainBuffers.resultBuffer);
    if (!CheckCLError(err, "CastTerrainMesh - setArg 2")) return CastResult{ false };

    err = kernel.setArg(3, m_TerrainBuffers.hitPointBuffer);
    if (!CheckCLError(err, "CastTerrainMesh - setArg 3")) return CastResult{ false };

    err = kernel.setArg(4, *triBuffer);
    if (!CheckCLError(err, "CastTerrainMesh - setArg 4")) return CastResult{ false };

    // Execute the kernel with one work-item per triangle
    cl::NDRange globalSize(numTris);
    err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, globalSize, cl::NullRange);
    if (!CheckCLError(err, "CastTerrainMesh - enqueueNDRangeKernel")) return CastResult{ false };

    // Read the intersection distance and hit point back from the GPU
    int intResult;
    float3 hitPoint;
    err = queue.enqueueReadBuffer(m_TerrainBuffers.resultBuffer, CL_TRUE, 0, sizeof(int), &intResult);
    if (!CheckCLError(err, "CastTerrainMesh - read resultBuffer")) return CastResult{ false };

    err = queue.enqueueReadBuffer(m_TerrainBuffers.hitPointBuffer, CL_TRUE, 0, sizeof(float3), &hitPoint);
    if (!CheckCLError(err, "CastTerrainMesh - read hitPointBuffer")) return CastResult{ false };

    // Convert the integer result back to a float
    float distance = int_to_float(intResult);

    // Process the result
    CastResult result;
    result.Hit = false;
    result.Distance = -1.0f;
    result.HitPoint = { 0.0f, 0.0f, 0.0f };

    if (distance > -1.0f && distance < 1000.0f) {
        result.Hit = true;
        result.Distance = distance;
        result.HitPoint = hitPoint;
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