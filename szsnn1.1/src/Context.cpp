/**
 * \file Context.cpp
 *
 * \date 07-12-2012
 * \author pawel
 * \description
 */

#include "Context.hpp"



namespace szsnn {

cl::Device
Context::findTargetDevice( // TODO template i by name
		std::vector<cl::Device>	platformDevices,
		std::vector<std::string>	targetDevices)
{

	std::vector<cl::Device>::iterator itpd;
	std::vector<std::string>::iterator ittd;

	for(ittd = targetDevices.begin(); ittd != targetDevices.end(); ittd++)
	{
		for(itpd = platformDevices.begin(); itpd != platformDevices.end(); itpd++)
		{
			std::string deviceName = (*itpd).getInfo<CL_DEVICE_NAME>();
			if ((*ittd).compare(deviceName) == 0)
			{
				return cl::Device((*itpd));
			}
		}
	}

	throw szsnn::SzsnnException("Context::findTargetDevice error");
}


std::string
getPath
()
/**************************************************************************************************/
{
	//TODO
	return std::string("/home/pawel/Studia/PDI/SZSNN/szsnn1.1/src/");
	//return std::string("D:\\pawel\\PDI\\SZSNN\\win\\szsnn\\szsnn\\src\\");
	//return std::string("/home/pawel/Studia/PDI/SZSNN/szsnn1.0/src/");

//#ifdef _WIN32
//  char buffer[MAX_PATH];
//#ifdef UNICODE
//  if(!GetModuleFileName(NULL, (LPWCH)buffer, sizeof(buffer)))
//      throw std::string("GetModuleFileName() failed!");
//#else
//  if(!GetModuleFileName(NULL, buffer, sizeof(buffer)))
//      throw std::string("GetModuleFileName() failed!");
//#endif
//  std::string str(buffer);
//  /* '\' == 92 */
//  size_t last = str.find_last_of((char)92);
//#else
//  char buffer[PATH_MAX + 1];
//  ssize_t len;
//  if((len = readlink("/proc/self/exe",buffer, sizeof(buffer) - 1)) == -1)
//      throw std::string("readlink() failed!");
//  buffer[len] = '\0';
//  std::string str(buffer);
//  /* '/' == 47 */
//  size_t last = str.find_last_of((char)47);
//#endif
//  return str.substr(0, last + 1);
}

void
replaceNewlineWithSpaces
(
  std::string &string
)
/**************************************************************************************************/
{

	std::replace( string.begin(), string.end(), '\n', ' ');
	std::replace( string.begin(), string.end(), '\r', ' ');
	return ;
//  size_t pos = string.find_first_of('\n', 0);
//
//  while(pos != string::npos)
//  {
//      string.replace(pos, 1, " ");
//      pos = string.find_first_of('\n', pos + 1);
//  }
//
//  pos = string.find_first_of('\r', 0);
//
//  while(pos != string::npos)
//  {
//      string.replace(pos, 1, " ");
//      pos = string.find_first_of('\r', pos + 1);
//  }
}

bool
getSource
(
  const char  *fileName,
  std::string &source
)
/**************************************************************************************************/
{
	std::string tmp;
  std::streampos size;
 // char*         str;

  /* Open file stream*/
  std::ifstream f(fileName, (std::fstream::in | std::fstream::binary));

  /* Check if we have opened file stream*/
  if (f.is_open())
  {
	  /* male pliki szybsze */
	  std::string tmp((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
	  /* duże pliki szybsze */
	  //string tmp(static_cast<stringstream const&>(stringstream() << ifs.rdbuf()).str());

	  source = tmp;
	  std::cout << source << std::endl;
	  f.close();
	  return true;
  }

  return false;
}


void Context::setup()
{
	cl_int err = CL_SUCCESS;
	cl_device_type dType;
	cl_uint myDeviceId;

	bool found = false;
	err = cl::Platform::get(&platforms);
	CHECK_CL_SUCCESS(err, "Platform::get() error.")

	/*Find target platform*/
	found = false;
	std::vector<cl::Platform>::iterator i;
	for (i = this->platforms.begin(); i != this->platforms.end(); ++i)
	{
		/*if(!strcmp((*i).getInfo<CL_PLATFORM_VENDOR>().c_str(), TARGET_PLATFORM_VENDOR))
		{
		  found = true; break;
		}*/
		//std::cout << (*i).getInfo<CL_PLATFORM_VENDOR>() << std::endl;
		found = true; platform = (*i); break;

	}

	if(!found)
	{
		std::stringstream ss;
		ss << "nie znalazl platformy " <<
		  (*i).getInfo<CL_PLATFORM_VENDOR>().c_str() << "\n";
		throw szsnn::SzsnnException(ss.str());
	}

	(*i).getDevices(CL_DEVICE_TYPE_ALL, &devices);

	std::vector<cl::Device>::iterator d;

	FIND_TARGET_DEVICE(devices, TARGET_DEVICE_NAME, d, found);

	if(!found)
	{
	std::stringstream ss;
	ss << "Neurosim::setup: Unable to find target devices " << TARGET_DEVICE_NAME
	  << " on platform from vendor "  << (*i).getInfo<CL_PLATFORM_VENDOR>().c_str() << "\n";
	throw szsnn::SzsnnException(ss.str());
	}

	LOG_SZSNN("Found device " << (*d).getInfo<CL_DEVICE_NAME>());

	dType = (*d).getInfo<CL_DEVICE_TYPE>();
	myDeviceId = (*d).getInfo<CL_DEVICE_VENDOR_ID>();

	cl_context_properties cps[3] =
	{
	  CL_CONTEXT_PLATFORM,
	  (cl_context_properties)(*i)(),
	  0
	};

	err = 0;
	context = cl::Context(dType, cps, NULL, NULL, &err);
	CHECK_CL_SUCCESS(err, "Neurosim::setupCL: Context::Context() failed.")

	devices = context.getInfo<CL_CONTEXT_DEVICES>();
	CHECK_CL_SUCCESS(err, "Neurosim::setupCL: Context::getInfo() failed.")

	FIND_TARGET_DEVICE(devices, TARGET_DEVICE_NAME, d, found);

	if(!found)
	{
	std::stringstream ss;
	ss << "Neurosim::setup: Unable to find target devices " << TARGET_DEVICE_NAME
	  << " within created context on platform from vendor "
	  << (*i).getInfo<CL_PLATFORM_VENDOR>().c_str() << "\n";
	throw szsnn::SzsnnException(ss.str());
	}

	LOG_SZSNN("Creating command queue for device " << (*d).getInfo<CL_DEVICE_NAME>());
	commandQueue = cl::CommandQueue(context, *d, 0, &err);
	CHECK_CL_SUCCESS(err, "Neurosim::setupCL: CommandQueue::CommandQueue() failed.")

	device = (*d);


	deviceMgr.setup(device);
//	std::stringstream ss1;
//	ss1 << "PRINTF " << device.getInfo<CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE>() << std::endl;
//	ss1 << "other " << device.getInfo<CL_DEVICE_MEM_BASE_ADDR_ALIGN>() << std::endl;
//	throw szsnn::SzsnnException(ss1.str());
}

void Context::createKernel(
		cl::Kernel    		&kernel,
		std::string         kernelFileName,
		std::string         kernelName,
		std::string         flagsStr,
		size_t              blockSizeX,
		size_t              blockSizeY)
{
  cl_int err = CL_SUCCESS;
  cl::Program program;

  /* create a CL program using the kernel source */

  std::string kernelPath = getPath();
  //kernelPath.append("cl\\");
  kernelPath.append("cl/");
  std::string kernelSrc("");

  std::string defPath = getPath();
  std::string defSrc("");

  //TODO
  //std::string flagsPath = "D:\\pawel\\PDI\\SZSNN\\win\\szsnn\\config\\oclCompilerOptions.txt";
  std::string flagsPath = "/home/pawel/Studia/PDI/SZSNN/szsnn1.1/config/oclCompilerOptions.txt";
  std::string flagsSrc("");

  kernelPath.append(kernelFileName);

  if(!getSource(kernelPath.c_str(), kernelSrc))
  {
    THROW_SZSNNEX("Neurosim::createKernel: Failed to load kernel file: " << kernelPath);
  }

  //defPath.append("Definitions.h");
  defPath.append("locales.h");
  if(!getSource(defPath.c_str(), defSrc))
  {
    THROW_SZSNNEX("Neurosim::createKernel: Failed to load kernel file: " << defPath);
  }

  /*To nearest 4B chunk*/
  size_t sourceCodeSize = ((defSrc.size() + kernelSrc.size())/sizeof(int)+1)*sizeof(int);
  /*size_t sourceCodeSize = defSrc.size() + kernelSrc.size();*/
  LOG_SZSNN("Source code size (" << kernelName << "): " << ((float)sourceCodeSize)/1024.0 << " KB");

  char *sourceCode = (char *) calloc(sourceCodeSize, sizeof(char));
  sourceCode[0] = '\0';
  strcat ( sourceCode, defSrc.data() );
  strcat ( sourceCode, kernelSrc.data() );

  /*std::cout << "Source code:\n" << sourceCode << std::endl;*/
  cl::Program::Sources programSource(1, std::make_pair(sourceCode, sourceCodeSize));

  program = cl::Program(context, programSource, &err);

  free(sourceCode);

  if(err != CL_SUCCESS)
  {
    THROW_SZSNNEX("Neurosim::createKernel: Failed cl::Program(Source) due to error code " << err);
  }

  if(!getSource(flagsPath.c_str(), flagsSrc))
  {
    THROW_SZSNNEX("Neurosim::createKernel: Failed to load flags file: " << flagsPath);
  }
  replaceNewlineWithSpaces(flagsSrc);
  const char *flags = flagsSrc.c_str();
  flagsStr.append(flags);
  if(flagsStr.size() != 0)
  {
    LOG_SZSNN("Build Options (" << kernelName << "): " << flagsStr.c_str());
  }

  std::vector<cl::Device> deviceVector;
  deviceVector.push_back(device);
  LOG_SZSNN("Building for device: " << (device).getInfo<CL_DEVICE_NAME>());
  err = program.build(deviceVector, flagsStr.c_str());

  if(err != CL_SUCCESS)
  {
    if(err == CL_BUILD_PROGRAM_FAILURE)
    {
      std::string str = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);

      std::cout << " \n\tBUILD LOG for kernel " << kernelName << "\n";
      std::cout << "*******************************************************************\n";
      std::cout << str << std::endl;
      std::cout << "*******************************************************************\n";
    }
  }

  if(err != CL_SUCCESS)
  {
    THROW_SZSNNEX("Neurosim::createKernel: Failed cl::Program:build due to error code " << err);
  }

  /* Create kernel */
  kernel = cl::Kernel(program, kernelName.c_str(), &err);

  if(err != CL_SUCCESS)
  {
    THROW_SZSNNEX("Neurosim::createKernel: Failed cl::Kernel() due to error code " << err);
  }

  /* Check group size against group size returned by kernel */
  size_t kernelWorkGroupSize =
    kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(device, &err);

  if(err != CL_SUCCESS)
  {
    THROW_SZSNNEX("Neurosim::createKernel: Failed Kernel::getWorkGroupInfo() due to error code "
      << err);
  }

  if((blockSizeX * blockSizeY) > kernelWorkGroupSize)
  {
    THROW_SZSNNEX("Neurosim::createKernel: Out of Resources!" << std::endl
      << "Group Size specified : " << blockSizeX * blockSizeY << std::endl
      << "Max Group Size supported on the kernel : " << kernelWorkGroupSize);
  }

  if(registerON)
  {
	  deviceMgr.registerKernel(kernelName);
  }
}


void Context::createKernel(
	KernelWrapper&		wrap)
{
	createKernel(
			wrap.kernel,
			wrap.kernelFileName,
			wrap.kernelName,
			wrap.flags,
			wrap.blockSizeX,
			wrap.blockSizeY);
}

void Context::enqueueKernel(
	const cl::Kernel&				kernel,
	std::string						kernelName,
	const cl::NDRange& 				global,
	const cl::NDRange& 				local,
	const std::vector<cl::Event>* 	events,
	cl::Event*						event)
{
    cl_int status;
    status = commandQueue.enqueueNDRangeKernel
      (kernel, cl::NullRange, global, local, events, event);
    if(status != CL_SUCCESS)
    {
      THROW_SZSNNEX("ENQUEUE_KERNEL: Failed to enqueue kernel " << kernelName
    		  << " due to error code " << status)
    }
}

void Context::enqueueKernel(
	const KernelWrapper&			wrap,
	const std::vector<cl::Event>* 	events,
	cl::Event* 						event)
{
	enqueueKernel(
			wrap.kernel,
			wrap.kernelName,
			wrap.globalRange,
			wrap.localRange,
			events,
			event);
}
void Context::createBuffer(
		cl::Buffer&					buffer,
		std::string 				bufferName,
		std::vector<std::string>	kernelNames,
		cl_mem_flags 				flags,
		size_t						size)
{
	cl_int err = CL_SUCCESS;
	buffer = cl::Buffer(context, flags, size, NULL, &err);
	if(err != CL_SUCCESS)
	{
		THROW_SZSNNEX("SZSNN::CONTEXT::createBuffer Failed to allocate "
				<< bufferName << " due to error code " << err);
	}
	 if(registerON)
	 {
		 //TODO -> wcześnie i sprawdzić z CL_DEVICE_MAX_MEM_ALLOC_SIZE
		 deviceMgr.registerMemory(kernelNames, bufferName, DeviceManager::MEM_GLOBAL, size);
	 }
}



void Context::writeBuffer(
	const cl::Buffer&	buffer,
	std::string 		bufferName,
	cl_bool				block,
	cl_uint				offset,
	cl_uint				size,
	const void*			ptr)
{
	if (size == 0)
	{
		THROW_SZSNNEX("Context::writeBuffer: Failed to enqueue buffer " << bufferName
				<< " for write, buffer size is 0");
	}

	cl_int status;
	cl::Event writeEvt;
	status = commandQueue.enqueueWriteBuffer(buffer, block, offset, size, ptr, NULL, &writeEvt);
	if(status != CL_SUCCESS)
	{
	  THROW_SZSNNEX("Context::writeBuffer: Failed to enqueue buffer " << bufferName
		<< " for write due to error code " << status);
	}

	status = commandQueue.flush();
	if(status != CL_SUCCESS)
	{
		THROW_SZSNNEX("Context::writeBuffer: Failed to flush buffer" << bufferName
		<< " for write due to error code " << status);
	}

	cl_int eventStatus = CL_QUEUED;
	while(eventStatus != CL_COMPLETE)
	{
	  status = writeEvt.getInfo<cl_int>(CL_EVENT_COMMAND_EXECUTION_STATUS,&eventStatus);
	  if(status != CL_SUCCESS)
	  {
		  THROW_SZSNNEX("Context::writeBuffer: Failed to get successefull write event status "
		  << "for buffer " << bufferName << " due to error code " << status);
	  }
	}
}

void Context::readBuffer(
	const cl::Buffer&	buffer,
	std::string 		bufferName,
	cl_bool				block,
	cl_uint				offset,
	cl_uint				size,
	void*				ptr)
{

	if (size == 0)
	{
		THROW_SZSNNEX("Context::readBuffer: Failed to enqueue buffer " << bufferName
				<< " for read, buffer size is 0");
	}
	cl_int status;
	cl::Event readEvt;
	//std::cout<<bufferName<<" "<<size<<std::endl;
	//system("pause");
	status = commandQueue.enqueueReadBuffer(buffer, block, offset, size, ptr, NULL, &readEvt);
	if(status != CL_SUCCESS)
	{
	  THROW_SZSNNEX("Context::readBuffer: Failed to enqueue buffer " << bufferName
		<< " for read due to error code " << status);
	}

	status = commandQueue.flush();
	if(status != CL_SUCCESS)
	{
		THROW_SZSNNEX("Context::readBuffer: Failed to flush buffer" << bufferName
		<< " for read due to error code " << status);
	}

	cl_int eventStatus = CL_QUEUED;
	while(eventStatus != CL_COMPLETE)
	{
	  status = readEvt.getInfo<cl_int>(CL_EVENT_COMMAND_EXECUTION_STATUS,&eventStatus);
	  if(status != CL_SUCCESS)
	  {
		  THROW_SZSNNEX("Context::readBuffer: Failed to get successefull read event status "
		  << "for buffer " << bufferName << " due to error code " << status);
	  }
	}
}



} /* namespace szsnn */
