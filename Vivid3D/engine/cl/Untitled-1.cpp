#include "pch.h"
#include "Intersections.h"
#include "Mesh3D.h"
#include "BasicMath.hpp"
#include "TerrainMesh.h"

Intersections::Intersections() {

	LoadProgram("engine/cl/intersects.cl");

     kernel = cl::Kernel(program, "findClosestIntersection");

         int check = 1;


}

void CL_CALLBACK errorCallback(cl_int err, const char* msg, void* data) {
    std::cerr << "Error " << err << ": " << msg << std::endl;
    // Handle error as needed
}


CastResult Intersections::CastTerrainMesh(float3 pos, float3 dir, TerrainMesh* mesh) {

    
    int cl = clock();
   // if (mesh) {
        mesh->RebuildGeo();
    //}

   


    


        int result_byte_size = num_tris * sizeof(float);
    int b = 5;
    // Create OpenCL buffers
    cl::Buffer posBuf(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float3), &pos);
    cl::Buffer dirBuf(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float3), &dir);

    if (first) {
        num_tris = mesh->GetTriangles().size();

        std::vector<float3> tri_data(num_tris * 3);
        int byte_size = (sizeof(float3) * 3);

      

        tri_data = mesh->GetGeo();
        triBuf = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof(float3) * tri_data.size(), (void*)tri_data.data());

         resBuf = cl::Buffer(context, CL_MEM_READ_WRITE, result_byte_size);
      
    }
    else {


    }


    // Check for errors


    // Set kernel arguments

    if (first) {
        kernel.setArg(0, posBuf);
        kernel.setArg(1, dirBuf);
        kernel.setArg(2, resBuf);
        kernel.setArg(3, triBuf);
    }
    // Execute the kernel (one work-item per triangle)
    cl::NDRange globalSize(num_tris);
   
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, globalSize, cl::NullRange);
    queue.finish();
   
    // Read the result
    std::vector<float> cd(num_tris);
    queue.enqueueReadBuffer(resBuf, CL_TRUE, 0, result_byte_size, cd.data());

    CastResult result;
    result.Hit = false;
    result.Distance = -1;
    // Print or process the results
    for (int i = 0; i < num_tris; ++i) {
        //    std::cout << "Result " << i << ": " << cd[i] << std::endl;
        if (cd[i] > -1)
        {
            result.Hit = true;
            result.Distance = cd[i];
        }
    }

    first = false;
    int ts = clock() - cl;

    printf("TMesh:%d\n", ts);
    return result;
}

CastResult Intersections::CastMesh(float3 pos, float3 dir, Mesh3D* mesh) {

 
    if (mesh->RebuildIf()) {
        mesh->BuildGeo();
    }
    int num_tris = mesh->GetTris().size();
    std::vector<float3> tri_data(num_tris * 3);


    tri_data = mesh->GetGeo();


    int byte_size = (sizeof(float3) * 3);

    int result_byte_size = num_tris * sizeof(float);

    int b = 5;
    // Create OpenCL buffers
    
    cl::Buffer trianglesBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
     sizeof(float3)*tri_data.size(), (void*)tri_data.data());
    cl::Buffer posBuf(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float3), &pos);
    cl::Buffer dirBuf(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float3), &dir);
    cl::Buffer minResultBuffer(context, CL_MEM_READ_WRITE,result_byte_size);

    

    // Check for errors
 

    // Set kernel arguments
 
    kernel.setArg(0, posBuf);
     kernel.setArg(1, dirBuf);
    kernel.setArg(2, minResultBuffer);
    kernel.setArg(3, trianglesBuffer);
    // Execute the kernel (one work-item per triangle)
    cl::NDRange globalSize(num_tris);
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, globalSize, cl::NullRange);
    queue.finish();

    // Read the result
    std::vector<float> cd(num_tris);
    queue.enqueueReadBuffer(minResultBuffer, CL_TRUE, 0,result_byte_size, cd.data());

    CastResult result;
    result.Hit = false;
    result.Distance = -1;
    // Print or process the results
    for (int i = 0; i < num_tris; ++i) {
    //    std::cout << "Result " << i << ": " << cd[i] << std::endl;
        if (cd[i] > -1)
        {
            result.Hit = true;
            result.Distance = cd[i];
        }
    }
    
    return result;
}