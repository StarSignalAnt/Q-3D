#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
// Include the OpenCL header file provided by NVIDIA
#include <CL/cl2.hpp>

// Function to check for OpenCL errors
#define CHECK_ERROR(err) \
    if (err != CL_SUCCESS) { \
        std::cerr << "Error: " << err << std::endl; \
        exit(1); \
    }



class CLBase
{
public:

	CLBase();
    void LoadProgram(std::string path);

protected:

    cl::Platform platform;
    cl::Device device;
    cl::Context context;
    cl::CommandQueue queue;
    cl::Program program;
    cl::Kernel kernel;
};

