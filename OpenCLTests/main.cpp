#ifdef __APPLE__
#include <OpenCL/opencl.hpp>
#else
#include <CL/cl.hpp>
#endif

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <random>

#include "Circle.h"
#include "WindowManager.h"

const int FPS = 75;
const float MS_DELAY = 1000.0F / FPS;

std::string loadKernel(const std::string name)
{
    std::ifstream in(name);
    std::string result(
        (std::istreambuf_iterator<char>(in)),
        std::istreambuf_iterator<char>());
    return result;
}

void CheckError(cl_int error, int line)
{
    if (error != CL_SUCCESS)
    {
        std::cerr << "OpenCL call on line " << line << " failed with error " << error << std::endl;
        std::exit(1);
    }
}

int getRandBetween(const int min, const int max)
{
    return (rand() % (min + max)) + min;
}

int main()
{
    srand(time(nullptr));
    std::vector<cl::Platform> plats;
    cl::Platform::get(&plats);
    for (cl::Platform plat : plats)
    {
        std::cout << plat.getInfo<CL_PLATFORM_NAME>() << " " << plat.getInfo<CL_PLATFORM_VERSION>() << std::endl;
        std::vector<cl::Device> d;
        plat.getDevices(CL_DEVICE_TYPE_ALL, &d);
        for (cl::Device dev : d)
        {
            std::cout << "  " << dev.getInfo<CL_DEVICE_NAME>() << " " << dev.getInfo<CL_DEVICE_VERSION>() << std::endl;
        }
    }

    cl_int error = 0;;

    std::vector<cl::Device> devs;
    plats[0].getDevices(CL_DEVICE_TYPE_ALL, &devs);
    std::cout << "Using " << devs[0].getInfo<CL_DEVICE_NAME>() << " from platform "
        << plats[0].getInfo<CL_PLATFORM_NAME>() << std::endl;

    cl::Platform platform = plats[0];
    cl::Device device = devs[0];

    cl::Context c = cl::Context::getDefault();

    cl::Program program = cl::Program(loadKernel("kernel.cl"));
    error = program.build(devs, "-cl-std=CL1.2");
    char* log = new char[10000];
    error = clGetProgramBuildInfo(program(), device(), CL_PROGRAM_BUILD_LOG, 10000, log, nullptr);
    std::cout << log << std::endl;
    CheckError(error, __LINE__);
    cl::Kernel kernel = cl::Kernel(program, "META", &error);
    CheckError(error, __LINE__);

    cl::ImageFormat format = cl::ImageFormat(CL_RGBA, CL_UNORM_INT8);
    cl::Image2D imageOut = cl::Image2D(c, CL_MEM_WRITE_ONLY, format, 800, 600, 0, nullptr, &error);
    CheckError(error, __LINE__);


    //Set up circles
    const int NUM_CIRCLES = 5;
    std::vector<Circle> vect;
    for (int i = 0; i < NUM_CIRCLES; i++)
    {
        int r = getRandBetween(20, 40);
        float x = static_cast<float>(getRandBetween(r, 750 - r));
        float y = static_cast<float>(getRandBetween(r, 550 - r));
        int xV = getRandBetween(30, 80);
        int yV = getRandBetween(30, 80);
        Circle circle{ x, y, r, xV, yV };
        vect.push_back(circle);
    }

    cl::Buffer circs = cl::Buffer(c, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, sizeof(Circle) * vect.size(), vect.data(), &error);
    CheckError(error, __LINE__);

    cl::CommandQueue cq = cl::CommandQueue(c, device, nullptr, &error);
    CheckError(error, __LINE__);
    WindowManager w{};
    uint8_t* out = new uint8_t[800 * 600 * 4];
    cl::NDRange r(800, 600);
    
    cl::size_t<3> origin = cl::size_t<3>();
    cl::size_t<3> region = cl::size_t<3>();
    region[0] = 800;
    region[1] = 600;
    region[2] = 1;

    Uint64 lastUpdate = SDL_GetPerformanceCounter();

    while (!w.process())
    {
        Uint64 current = SDL_GetPerformanceCounter();

        double dT = (current - lastUpdate) / static_cast<double>(SDL_GetPerformanceFrequency());

        for (Circle& c : vect)
        {
            updateCircle(c, 800, 600, dT);
        }
        
        error = cq.enqueueWriteBuffer(circs, CL_TRUE, 0, sizeof(Circle) * vect.size(), vect.data());
        CheckError(error, __LINE__);
        error = kernel.setArg(0, sizeof(circs), &circs);
        CheckError(error, __LINE__);
        static int size = vect.size();
        error = kernel.setArg(1, sizeof(size), &size);
        CheckError(error, __LINE__);
        error = kernel.setArg(2, sizeof(imageOut), &imageOut);
        CheckError(error, __LINE__);
        error = cq.enqueueNDRangeKernel(kernel, cl::NDRange(0, 0), r);
        CheckError(error, __LINE__);
        error = cq.enqueueReadImage(imageOut, CL_TRUE, origin, region, 800 * 4, 0, out);
        CheckError(error, __LINE__);

        w.updateScreen(out, 800 * 4);
        Uint64 end = SDL_GetPerformanceCounter();
        float elapsed = (end - current) / static_cast<float>(SDL_GetPerformanceFrequency());
        float elapsedMS = elapsed * 1000.0f;
        SDL_Delay(floor(MS_DELAY - elapsedMS));
        end = SDL_GetPerformanceCounter();
        elapsed = (end - current) / static_cast<float>(SDL_GetPerformanceFrequency());
        
        float fps = 1.0f / elapsed;
        w.updateTitle("OpenCL Stuff (" + std::to_string(fps) + " fps)");
        lastUpdate = current;
    }
    delete[] out;
    return 0;
}