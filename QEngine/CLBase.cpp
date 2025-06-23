#include "pch.h"
#include "CLBase.h"

std::string readFileToString(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

CLBase::CLBase() {

    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);

    if (platforms.empty()) {
        std::cerr << "No OpenCL platforms found." << std::endl;
        return;
    }

    // Choose the first platform
    platform = platforms[0];
    std::string platformName;
    platform.getInfo(CL_PLATFORM_NAME, &platformName);
    std::cout << "Selected OpenCL platform: " << platformName << std::endl;
    // Get devices on the chosen platform
    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);

    if (devices.empty()) {
        std::cerr << "No GPU devices found on the platform." << std::endl;
        return;
    }

    // Choose the first device
    device = devices[0];

    std::string deviceName;
    device.getInfo(CL_DEVICE_NAME, &deviceName);
    std::string deviceVersion;
    device.getInfo(CL_DEVICE_VERSION, &deviceVersion);
    std::cout << "Selected OpenCL device: " << deviceName << std::endl;
    std::cout << "Device OpenCL version: " << deviceVersion << std::endl;

    context = cl::Context(device);

    // Create a command queue for the context and device
    queue = cl::CommandQueue(context, device);

    std::cout << "OpenCL initialized successfully." << std::endl;

}

void CLBase::LoadProgram(std::string path) {

    std::string kernelFile = "path_to_kernel_file.cl";
    std::string kernelSource = readFileToString(path);
    if (kernelSource.empty()) {
        std::cerr << "Failed to load kernel source from file." << std::endl;
        return;
    }

    cl::Program::Sources sources;
    sources.push_back({ kernelSource.c_str(), kernelSource.length() });

    program = cl::Program(context, sources);

    if (program.build(device) != CL_SUCCESS) {
        std::cerr << "Error building program: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;
        return;
    }


}