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

int float_to_int(float f) {
    int i;
    memcpy(&i, &f, sizeof(int));
    return i;
}

// Helper function to convert int to float for reading the result
float int_to_float(int i) {
    float f;
    memcpy(&f, &i, sizeof(float));
    return f;
}

CastResult Intersections::CastTerrainMesh(float3 pos, float3 dir, TerrainMesh* mesh) {

    //return CastResult();

   // if (mesh) {
    float size = 1000;
    int initial_result = float_to_int(FLT_MAX);
    cl::Buffer posBuf(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float3), &pos);
    cl::Buffer dirBuf(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float3), &dir);
    cl::Buffer minResultBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float), &initial_result);
    //}  num_tris = mesh->GetTris().size();

    num_tris = mesh->GetTriangles().size();
        int cl = clock();
        if (mesh->NeedRebuild()) {

            mesh->RebuildGeo();



            num_tris = mesh->GetTriangles().size();
            std::vector<float3> tri_data(num_tris * 3);


            tri_data = mesh->GetGeo();


            int byte_size = (sizeof(float3) * 3);

            int result_byte_size = num_tris * sizeof(float);

            int b = 5;
            // Create OpenCL buffers
           // float size = -10000;
            //i//nt cl = clock();
            triBuf = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                sizeof(float3) * tri_data.size(), (void*)tri_data.data());
            kernel.setArg(0, posBuf);
            kernel.setArg(1, dirBuf);
            kernel.setArg(2, minResultBuffer);
            kernel.setArg(3, triBuf);
            m_TBuffers[mesh] = triBuf;
        }
        else {
            triBuf = m_TBuffers[mesh];
            kernel.setArg(0, posBuf);
            kernel.setArg(1, dirBuf);
            kernel.setArg(2, minResultBuffer);
            kernel.setArg(3, triBuf);
        }

    //int ct = clock() - cl;

    //printf("CT:%d\n", ct);



    // Check for errors


    // Set kernel arguments



    // Execute the kernel (one work-item per triangle)
    cl::NDRange globalSize(num_tris);
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, globalSize, cl::NullRange);
    queue.finish();

    float ch = 0;
    // Read the result
    int int_r;
    queue.enqueueReadBuffer(minResultBuffer, CL_TRUE,0,4, &int_r);

    ch = int_to_float(int_r);



    CastResult result;
    result.Hit = false;
    result.Distance = -1;
    int ts = clock() - cl;
    //printf("RT:%d\n", ts);
    if (ch > -1 && ch<1000) {
        result.Hit = true;
        result.Distance = ch;
        return result;
    }
    

 


    return result;
}

CastResult Intersections::CastMesh(float3 pos, float3 dir, Mesh3D* mesh) {

 //   float size = 1000;
    float size = 1000;
    int initial_result = float_to_int(FLT_MAX);
    cl::Buffer posBuf(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float3), &pos);
    cl::Buffer dirBuf(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float3), &dir);
    cl::Buffer minResultBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float), &initial_result);
    //}
    num_tris = mesh->TriCount();
    int cl = clock();
    if (mesh->RebuildIf()) {

        mesh->BuildGeo();



 //       num_tris = mesh->GetTris().size();
     //   std::vector<float3> tri_data(num_tris * 3);


        int num_tris = mesh->TriCount();

        auto rdata = mesh->GetRawGeo();

        int byte_size = (sizeof(float3) * 3);

        int result_byte_size = num_tris * sizeof(float);

        int b = 5;
        // Create OpenCL buffers
       // float size = -10000;
        //i//nt cl = clock();
        triBuf = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof(float3) * num_tris*3, (void*)rdata);
        kernel.setArg(0, posBuf);
        kernel.setArg(1, dirBuf);
        kernel.setArg(2, minResultBuffer);
        kernel.setArg(3, triBuf);
        m_Buffers[mesh] = triBuf;
    }
    else {

        num_tris = mesh->TriCount();
        std::vector<float3> tri_data(num_tris * 3);


        //tri_data = mesh->GetGeo();



        int byte_size = (sizeof(float3) * 3);

        int result_byte_size = num_tris * sizeof(float);
        triBuf = m_Buffers[mesh]; // cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        //    sizeof(float3) * tri_data.size(), (void*)tri_data.data());

        kernel.setArg(0, posBuf);
        kernel.setArg(1, dirBuf);
        kernel.setArg(2, minResultBuffer);
        kernel.setArg(3, triBuf);
    }

    //int ct = clock() - cl;

    //printf("CT:%d\n", ct);



    // Check for errors


    // Set kernel arguments



    // Execute the kernel (one work-item per triangle)
    cl::NDRange globalSize(num_tris);
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, globalSize, cl::NullRange);
    queue.finish();

    // Read the result
    float ch = 0;
    // Read the result
    int int_r;
    queue.enqueueReadBuffer(minResultBuffer, CL_TRUE, 0, 4, &int_r);

    ch = int_to_float(int_r);

    CastResult result;
    result.Hit = false;
    result.Distance = -1;
    int ts = clock() - cl;
    //printf("RT:%d\n", ts);
    if (ch > -1 && ch < 1000) {
        result.Hit = true;
        result.Distance = ch;
        return result;
    }





    /*
    return CastResult();

    if (mesh->RebuildIf()) {
        mesh->BuildGeo();
    }
    int num_tris = mesh->GetTris().size();
    std::vector<float3> tri_data(num_tris * 3);


    tri_data = mesh->GetGeo();


    tri_data = mesh->GetGeo();


    int byte_size = (sizeof(float3) * 3);

    int result_byte_size = num_tris * sizeof(float);

    int b = 5;
    // Create OpenCL buffers
    float size = 10000;
    int cl = clock();
    // Create OpenCL buffers
    cl::Buffer trianglesBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(float3) * tri_data.size(), (void*)tri_data.data());
    cl::Buffer posBuf(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float3), &pos);
    cl::Buffer dirBuf(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float3), &dir);
    cl::Buffer minResultBuffer(context, CL_MEM_READ_WRITE, sizeof(float));
    int ct = clock() - cl;
    printf("CT:%d\n", ct);



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
    float ch = -10000;
    queue.enqueueReadBuffer(minResultBuffer, CL_TRUE, 0, 4, &ch);

    CastResult result;
    result.Hit = false;
    result.Distance = -1;

    if (ch > -1) {
        result.Hit = true;
        result.Distance = ch;
        return result;
    }


    // Print or process the results
    for (int i = 0; i < num_tris; ++i) {
        //    std::cout << "Result " << i << ": " << cd[i] << std::endl;
    //    if (cd[i] > -1)
        {
            //     result.Hit = true;
           //      result.Distance = cd[i];
        }
    }

    return result;
    */
}