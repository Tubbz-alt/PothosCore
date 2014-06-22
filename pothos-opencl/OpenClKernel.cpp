// Copyright (c) 2014-2014 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include "OpenClKernel.hpp"
#include <Pothos/Framework.hpp>
#include <Poco/NumberParser.h>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Array.h>
#include <iostream>

/***********************************************************************
 * |PothosDoc OpenCL Kernel
 *
 * The OpenCL Kernel block executes a kernel on supported devices.
 * The kernel source is just in time (JIT) compiled by the OpenCL API.
 * This block exclusively computes kernels of one dimensional arrays.
 * Two and three dimensional kernels and others others are not handled by this block
 *
 * |category /Kernels
 * |keywords kernel jit opencl
 *
 * |param deviceId[Device ID] A markup to specify OpenCL platform and device.
 * The markup takes the format [platform index]:[device index]
 * The platform index represents a platform ID found in clGetPlatformIDs().
 * The device index represents a device ID found in clGetDeviceIDs().
 * |default "0:0"
 *
 * |param portMarkup[Port Markup] A JSON markup of input, output port sizes.
 * The port markup is an array of two arrays: [[input sizes], [output sizes]].
 * The port markup is used to determine the number of input and output ports,
 * and to specify the size in bytes per element for each input and output port.
 * |unit bytes
 * |default "[[4, 4], [4]]"
 *
 * |param kernelName[Kernel Name] The name of a kernel in the source.
 * |default ""
 *
 * |param kernelSource[Kernel Source] Source code for an OpenCL kernel.
 * |default ""
 *
 * |param localSize[Local Size] The number of work units/resources to allocate.
 * This controls the parallelism of the kernel execution.
 * |default 2
 *
 * |param globalFactor[Global Factor] This factor controls the global size.
 * The global size is the number of kernel iterarions per call.
 * Global size = number of input elements * global factor.
 * |default 1.0
 *
 * |param productionFactor[Production Factor] This factor controls the elements produced.
 * For each call to work, elements produced = number of input elements * production factor.
 * |default 1.0
 *
 * |factory /blocks/opencl/opencl_kernel(deviceId, portMarkup)
 * |setter setSource(kernelName, kernelSource)
 * |setter setLocalSize(localSize)
 * |setter setGlobalFactor(globalFactor)
 * |setter setProductionFactor(productionFactor)
 **********************************************************************/
class OpenClKernel : public Pothos::Block
{
public:
    static Pothos::Block *make(const std::string &deviceId, const std::string &portMarkup)
    {
        return new OpenClKernel(deviceId, portMarkup);
    }

    OpenClKernel(const std::string &deviceId, const std::string &portMarkup);

    ~OpenClKernel(void)
    {
        //reset in order of creation
        _kernel.reset();
        _queue.reset();
        _program.reset();
        _context.reset();
    }

    void setSource(const std::string &name, const std::string &source);

    void setLocalSize(const size_t size)
    {
        _localSize = size;
    }

    size_t getLocalSize(void) const
    {
        return _localSize;
    }

    void setGlobalFactor(const double factor)
    {
        _globalFactor = factor;
    }

    double getGlobalFactor(void) const
    {
        return _globalFactor;
    }

    void setProductionFactor(const double factor)
    {
        _productionFactor = factor;
    }

    double getProductionFactor(void) const
    {
        return _productionFactor;
    }

    Pothos::BufferManager::Sptr getInputBufferManager(const std::string &, const std::string &domain)
    {
        if (domain.empty())
        {
            OpenClBufferContainerArgs args;
            args.mem_flags = CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR;
            args.map_flags = CL_MAP_WRITE;
            args.context = _context;
            args.queue = _queue;
            return makeOpenClBufferManager(args);
        }
        if (domain == _myDomain)
        {
            return Pothos::BufferManager::Sptr();
        }
        throw Pothos::PortDomainError();
    }

    Pothos::BufferManager::Sptr getOutputBufferManager(const std::string &, const std::string &domain)
    {
        if (domain.empty() or domain == _myDomain)
        {
            OpenClBufferContainerArgs args;
            args.mem_flags = CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR;
            args.map_flags = 0;
            args.context = _context;
            args.queue = _queue;
            return makeOpenClBufferManager(args);
        }
        throw Pothos::PortDomainError();
    }

    void work(void);

private:
    std::string _myDomain;
    cl_platform_id _platform;
    cl_device_id _device;
    std::shared_ptr<cl_context> _context;
    std::shared_ptr<cl_program> _program;
    std::shared_ptr<cl_kernel> _kernel;
    std::shared_ptr<cl_command_queue> _queue;
    size_t _localSize;
    double _globalFactor;
    double _productionFactor;
};

OpenClKernel::OpenClKernel(const std::string &deviceId, const std::string &portMarkup):
    _localSize(1),
    _globalFactor(1.0),
    _productionFactor(1.0)
{
    const auto colon = deviceId.find(":");
    const auto platformIndex = Poco::NumberParser::parseUnsigned(deviceId.substr(0, colon));
    const auto deviceIndex = Poco::NumberParser::parseUnsigned(deviceId.substr(colon+1));

    /* Identify a platform */
    cl_int err = 0;
    cl_uint num_platforms = 0;
    cl_platform_id platforms[64];
    err = clGetPlatformIDs(64, platforms, &num_platforms);
    if (err < 0) throw Pothos::Exception("OpenClKernel::clGetPlatformIDs()", clErrToStr(err));
    if (platformIndex >= num_platforms) throw Pothos::Exception("OpenClKernel()", "platform index does not exist");
    _platform = platforms[platformIndex];

    /* Access a device */
    cl_uint num_devices = 0;
    cl_device_id devices[64];
    err = clGetDeviceIDs(_platform, CL_DEVICE_TYPE_ALL, 64, devices, &num_devices);
    if (err < 0) throw Pothos::Exception("OpenClKernel::clGetDeviceIDs()", clErrToStr(err));
    if (deviceIndex >= num_devices) throw Pothos::Exception("OpenClKernel()", "device index does not exist");
    _device = devices[deviceIndex];

    /* Create context */
    _context = lookupContextCache(_device);

    /* Create ports */
    _myDomain = "OpenCl_"+std::to_string(size_t(_device));
    Poco::JSON::Parser p; p.parse(portMarkup);
    const auto ports = p.getHandler()->asVar().extract<Poco::JSON::Array::Ptr>();
    const auto inputs = ports->getArray(0);
    const auto outputs = ports->getArray(1);
    for (size_t i = 0; i < inputs->size(); i++)
    {
        this->setupInput(i, Pothos::DType("custom", inputs->getElement<int>(i)), _myDomain);
    }
    for (size_t i = 0; i < outputs->size(); i++)
    {
        this->setupOutput(i, Pothos::DType("custom", outputs->getElement<int>(i)), _myDomain);
    }

    this->registerCall(POTHOS_FCN_TUPLE(OpenClKernel, setSource));
    this->registerCall(POTHOS_FCN_TUPLE(OpenClKernel, setLocalSize));
    this->registerCall(POTHOS_FCN_TUPLE(OpenClKernel, getLocalSize));
    this->registerCall(POTHOS_FCN_TUPLE(OpenClKernel, setGlobalFactor));
    this->registerCall(POTHOS_FCN_TUPLE(OpenClKernel, getGlobalFactor));
    this->registerCall(POTHOS_FCN_TUPLE(OpenClKernel, setProductionFactor));
    this->registerCall(POTHOS_FCN_TUPLE(OpenClKernel, getProductionFactor));
}

void OpenClKernel::setSource(const std::string &kernelName, const std::string &kernelSource)
{
    cl_int err = 0;

    /* Create program from source */
    if (kernelSource.empty()) throw Pothos::Exception("OpenClKernel::activate::createProgram()", "no source specified");
    const char *sourcePtr = kernelSource.data();
    const size_t sourceSize = kernelSource.size();
    auto program = clCreateProgramWithSource(*_context, 1, &sourcePtr, &sourceSize, &err);
    if(err < 0) throw Pothos::Exception("OpenClKernel::clCreateProgramWithSource()", clErrToStr(err));
    _program.reset(new cl_program(program), clReleaseProgramPtr);

    /* Build program */
    err = clBuildProgram(*_program, 0, nullptr, nullptr, nullptr, nullptr);
    if (err < 0)
    {
        /* Find size of log and print to std output */
        size_t logSize = 0;
        clGetProgramBuildInfo(*_program, _device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);
        std::vector<char> errorLog(logSize);
        clGetProgramBuildInfo(*_program, _device, CL_PROGRAM_BUILD_LOG, logSize, errorLog.data(), nullptr);

        std::string errorString(errorLog.begin(), errorLog.end());
        throw Pothos::Exception("OpenClKernel::clBuildProgram()", errorString);
    }

    /* Create a command queue */
    auto queue = clCreateCommandQueue(*_context, _device, 0, &err);
    if (err < 0) throw Pothos::Exception("OpenClKernel::clCreateCommandQueue()", clErrToStr(err));
    _queue.reset(new cl_command_queue(queue), clReleaseCommandQueuePtr);

    /* Create a kernel */
    auto kernel = clCreateKernel(*_program, kernelName.c_str(), &err);
    if (err < 0) throw Pothos::Exception("OpenClKernel::clCreateKernel()", clErrToStr(err));
    _kernel.reset(new cl_kernel(kernel), clReleaseKernelPtr);
}

void OpenClKernel::work(void)
{
    const auto &inputs = this->inputs();
    const auto &outputs = this->outputs();

    std::vector<cl_mem> inputBuffs(inputs.size());
    std::vector<cl_mem> outputBuffs(outputs.size());

    cl_int err = 0;

    if (this->workInfo().minElements == 0) return;

    //calculate number of elements
    size_t inputElems = this->workInfo().minInElements;
    size_t outputElems = this->workInfo().minOutElements;
    if (_productionFactor > 1.0)
    {
        outputElems = std::min<size_t>(inputElems*_productionFactor, outputElems);
        inputElems = outputElems/_productionFactor;
    }
    else
    {
        inputElems = std::min<size_t>(outputElems/_productionFactor, inputElems);
        outputElems = inputElems*_productionFactor;
    }
    size_t globalSize = inputElems*_globalFactor;

    /* Create data buffer */
    size_t argNo = 0;
    for (size_t i = 0; i < inputs.size(); i++)
    {
        inputBuffs[i] = getClBufferFromManaged(inputs[i]->buffer().getManagedBuffer());
        err = clSetKernelArg(*_kernel, argNo++, sizeof(cl_mem), &inputBuffs[i]);
        if (err < 0) throw Pothos::Exception("OpenClKernel::work::clSetKernelArg()", clErrToStr(err));
    }
    for (size_t i = 0; i < outputs.size(); i++)
    {
        outputBuffs[i] = getClBufferFromManaged(outputs[i]->buffer().getManagedBuffer());
        err = clSetKernelArg(*_kernel, argNo++, sizeof(cl_mem), &outputBuffs[i]);
        if (err < 0) throw Pothos::Exception("OpenClKernel::work::clSetKernelArg()", clErrToStr(err));
    }

    /* Enqueue kernel */
    err = clEnqueueNDRangeKernel(*_queue, *_kernel, 1, nullptr, &globalSize, &_localSize, 0, nullptr, nullptr);
    if (err < 0) throw Pothos::Exception("OpenClKernel::work::enqueueKernel()", clErrToStr(err));

    /* Read the kernel's output */
    for (size_t i = 0; i < inputs.size(); i++)
    {
        inputs[i]->consume(inputElems);
    }
    for (size_t i = 0; i < outputs.size(); i++)
    {
        err = clEnqueueReadBuffer(*_queue, outputBuffs[i], CL_TRUE, 0,
            outputElems*outputs[i]->dtype().size(), outputs[i]->buffer().as<void *>(), 0, nullptr, nullptr);
        if (err < 0) throw Pothos::Exception("OpenClKernel::work::clEnqueueReadBuffer()", clErrToStr(err));
        outputs[i]->produce(outputElems);
    }
}

static Pothos::BlockRegistry registerOpenClKernel(
    "/blocks/opencl/opencl_kernel", &OpenClKernel::make);
