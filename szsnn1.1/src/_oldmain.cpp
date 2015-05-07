//#include <iostream>
//#include <stdlib.h>
//#include <time.h>
//#include <string.h>
//#include <malloc.h>
//
//
////#include <cuda_runtime.h>
//
//#include <stdio.h>
//
//
//#include <sstream>
//
//#include "opencl_header.h"
//
//#include "utils.h"
//#include "locales.h"
//#include "types.h"
//
//
//#include "debug.hpp"
//#include "neuron_model_traits.hpp"
//#include "locales.h"
//#include "NetworkGraph.hpp"
//#include "SzsnnException.hpp"
//#include "Context.hpp"
//#include "Connection.hpp"
//
///*
// * main.cpp
// *
// *  Created on: 03-03-2012
// *      Author: pawel
// */
//
////#define THROW_SIMEX(msg)\
////  {\
////    std::stringstream ss;\
////    ss << msg << std::endl;\
////    throw szsnn::SzsnnException(ss.str());\
////  }
////
////#define TARGET_DEVICE_NAME                                  "GeForce 8400M GS;Tahiti;Cayman"
////
////#define LOG_SIM(message)\
////{\
////  time_t t; time (&t); char *s=ctime(&t); s[strlen(s)-1]=0;\
////  cout << ctime(&t) << " " << message << endl;\
////}
////
////#define ASSERT_CL_SUCCESS(tested_variable, message)\
////  if(tested_variable != CL_SUCCESS)\
////  {\
////    throw szsnn::SzsnnException(message);\
////  }
////
////#define FIND_TARGET_DEVICE(platformDevices, targetDevices, deviceIterator, found)\
////  {\
////    const char *tD = targetDevices;\
////    found = false;\
////    char *str = (char *) calloc(0xFFFF, sizeof(char));\
////    strcpy(str, tD);\
////    char *pch;\
////    pch = strtok (str, ";");\
////    while (pch != NULL)\
////    {\
////      for(deviceIterator = platformDevices.begin(); deviceIterator != platformDevices.end();\
////      ++deviceIterator)\
////      {\
////        std::string deviceName = (*deviceIterator).getInfo<CL_DEVICE_NAME>();\
////        std::cout << deviceName << std::endl;\
////        if(strcmp(deviceName.c_str(), pch) == 0)\
////        {\
////          found = true; break;\
////        }\
////      }\
////      if(found){break;}\
////      pch = strtok (NULL, ";");\
////    }\
////    free(str);\
////  }
//
//using namespace std;
//
////vector<cl::Platform> platforms;
////
////vector<cl::Device> devices;
////
////cl::Context context;
////cl::CommandQueue commandQueue;
//
//
//
//
////std::string
////getPath
////()
/////**************************************************************************************************/
////{
////	return std::string("/home/pawel/Studia/PDI/SZSNN/szsnn1.0/src/cl/");
////#ifdef _WIN32
////  char buffer[MAX_PATH];
////#ifdef UNICODE
////  if(!GetModuleFileName(NULL, (LPWCH)buffer, sizeof(buffer)))
////      throw std::string("GetModuleFileName() failed!");
////#else
////  if(!GetModuleFileName(NULL, buffer, sizeof(buffer)))
////      throw std::string("GetModuleFileName() failed!");
////#endif
////  std::string str(buffer);
////  /* '\' == 92 */
////  size_t last = str.find_last_of((char)92);
////#else
////  char buffer[PATH_MAX + 1];
////  ssize_t len;
////  if((len = readlink("/proc/self/exe",buffer, sizeof(buffer) - 1)) == -1)
////      throw std::string("readlink() failed!");
////  buffer[len] = '\0';
////  std::string str(buffer);
////  /* '/' == 47 */
////  size_t last = str.find_last_of((char)47);
////#endif
////  return str.substr(0, last + 1);
////}
////
////#define OCL_COMPILER_OPTIONS_FILE_NAME                        "oclCompilerOptions.txt"
////
////
//
//
////bool
////findTargetDevice
////(
////  vector<cl::Device>                  platformDevices,
////  const char                          *targetDevices,
////  std::vector<cl::Device>::iterator   *d
////)
/////**************************************************************************************************/
////{
////  bool found = false;
////  char *str = (char *) calloc(0xFFFF, sizeof(char));
////  strcpy(str, targetDevices);
////  char *pch;
////  pch = strtok (str, ",");
////
////  while (pch != NULL)
////  {
////    for(*d = platformDevices.begin(); *d != platformDevices.end(); ++(*d))
////    {
////      std::string deviceName = (*(*d)).getInfo<CL_DEVICE_NAME>();
////
////      if(strcmp(deviceName.c_str(), pch) == 0)
////      {
////        found = true; break;
////      }
////    }
////    if(found){break;}
////    pch = strtok (NULL, ",");
////  }
////
////  free(str);
////  return found;
////}
//#include <fstream>
//#include <algorithm>
//
////void
////replaceNewlineWithSpaces
////(
////  std::string &string
////)
/////**************************************************************************************************/
////{
////
////	std::replace( string.begin(), string.end(), '\n', ' ');
////	std::replace( string.begin(), string.end(), '\r', ' ');
////	return ;
////  size_t pos = string.find_first_of('\n', 0);
////
////  while(pos != string::npos)
////  {
////      string.replace(pos, 1, " ");
////      pos = string.find_first_of('\n', pos + 1);
////  }
////
////  pos = string.find_first_of('\r', 0);
////
////  while(pos != string::npos)
////  {
////      string.replace(pos, 1, " ");
////      pos = string.find_first_of('\r', pos + 1);
////  }
////}
////
////bool
////getSource
////(
////  const char  *fileName,
////  std::string &source
////)
/////**************************************************************************************************/
////{
////	std::string tmp;
////  std::streampos size;
////  char*         str;
////
////  /* Open file stream*/
////  std::ifstream f(fileName, (std::fstream::in | std::fstream::binary));
////
////  /* Check if we have opened file stream*/
////  if (f.is_open())
////  {
////	  /* male pliki szybsze */
////	  string tmp((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
////	  /* duże pliki szybsze */
////	  //string tmp(static_cast<stringstream const&>(stringstream() << ifs.rdbuf()).str());
////
////	  source = tmp;
////	  std::cout << source << std::endl;
////	  f.close();
////	  return true;
////    std::streampos sizeFile;
////    /* Find the stream size*/
////    f.seekg(0, std::fstream::end);
////    size = sizeFile = f.tellg();
////    f.seekg(0, std::fstream::beg);
////
////    if(size < 0)
////    {
////      f.close();
////      return  false;
////    }
////
////    str = new char[(size_t)size + 1];
////
////    if(!str)
////    {
////      f.close();
////      return  false;
////    }
////
////    /* Read file*/
//////    f.read(str, sizeFile);
////    f.close();
////    str[size] = '\0';
////
////    source = str;
////
////    delete[] str;
////
////    return true;
////  }
////
////  return false;
////}
////void
////createKernel
////(
////#if LOG_SIMULATION
////  std::stringstream   *dataToSimulationLogFile,
////#endif
////  cl::Context         &context,
////  cl::Device          &device,
////  cl::Kernel          &kernel,
////  std::string         kernelFileName,
////  const char          *kernelName,
////  std::string         flagsStr,
////  size_t              blockSizeX,
////  size_t              blockSizeY
////)
/////**************************************************************************************************/
////{
////  cl_int err = CL_SUCCESS;
////  cl::Program program;
////
////  /* create a CL program using the kernel source */
////
////  std::string kernelPath = getPath();
////  std::string kernelSrc("");
////
////  std::string defPath = getPath();
////  std::string defSrc("");
////
////  std::string flagsPath = "/home/pawel/Studia/PDI/SZSNN/szsnn1.0/src/cl/oclCompilerOptions.txt";
////  std::string flagsSrc("");
////
////  kernelPath.append(kernelFileName);
////
////  if(!getSource(kernelPath.c_str(), kernelSrc))
////  {
////    THROW_SIMEX("Neurosim::createKernel: Failed to load kernel file: " << kernelPath);
////  }
////
////  //defPath.append("Definitions.h");
////  defPath.append("locales.h");
////  if(!getSource(defPath.c_str(), defSrc))
////  {
////    THROW_SIMEX("Neurosim::createKernel: Failed to load kernel file: " << defPath);
////  }
////
////  cout << "QQQQQQQQQqq " << defSrc << endl;
////  /*To nearest 4B chunk*/
////  size_t sourceCodeSize = ((defSrc.size() + kernelSrc.size())/sizeof(int)+1)*sizeof(int);
////  /*size_t sourceCodeSize = defSrc.size() + kernelSrc.size();*/
////  LOG_SIM("Source code size (" << kernelName << "): " << ((float)sourceCodeSize)/1024.0 << " KB");
////
////  char *sourceCode = (char *) calloc(sourceCodeSize, sizeof(char));
////  sourceCode[0] = '\0';
////  strcat ( sourceCode, defSrc.data() );
////  strcat ( sourceCode, kernelSrc.data() );
////
////  /*std::cout << "Source code:\n" << sourceCode << std::endl;*/
////  cl::Program::Sources programSource(1, std::make_pair(sourceCode, sourceCodeSize));
////
////  program = cl::Program(context, programSource, &err);
////
////  free(sourceCode);
////
////  if(err != CL_SUCCESS)
////  {
////    THROW_SIMEX("Neurosim::createKernel: Failed cl::Program(Source) due to error code " << err);
////  }
////
////  if(!getSource(flagsPath.c_str(), flagsSrc))
////  {
////    THROW_SIMEX("Neurosim::createKernel: Failed to load flags file: " << flagsPath);
////  }
////  replaceNewlineWithSpaces(flagsSrc);
////  const char *flags = flagsSrc.c_str();
////  flagsStr.append(flags);
////  if(flagsStr.size() != 0)
////  {
////    LOG_SIM("Build Options (" << kernelName << "): " << flagsStr.c_str());
////  }
////
////  vector<cl::Device> deviceVector;
////  deviceVector.push_back(device);
////  LOG_SIM("Building for device: " << (device).getInfo<CL_DEVICE_NAME>());
////  err = program.build(deviceVector, flagsStr.c_str());
////
////  if(err != CL_SUCCESS)
////  {
////    if(err == CL_BUILD_PROGRAM_FAILURE)
////    {
////      std::string str = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
////
////      std::cout << " \n\tBUILD LOG for kernel " << kernelName << "\n";
////      std::cout << "*******************************************************************\n";
////      std::cout << str << std::endl;
////      std::cout << "*******************************************************************\n";
////    }
////  }
////
////  if(err != CL_SUCCESS)
////  {
////    THROW_SIMEX("Neurosim::createKernel: Failed cl::Program:build due to error code " << err);
////  }
////
////  /* Create kernel */
////  kernel = cl::Kernel(program, kernelName, &err);
////
////  if(err != CL_SUCCESS)
////  {
////    THROW_SIMEX("Neurosim::createKernel: Failed cl::Kernel() due to error code " << err);
////  }
////
////  /* Check group size against group size returned by kernel */
////  size_t kernelWorkGroupSize =
////    kernel.getWorkGroupInfo<CL_KERNEL_WORK_GROUP_SIZE>(device, &err);
////
////  if(err != CL_SUCCESS)
////  {
////    THROW_SIMEX("Neurosim::createKernel: Failed Kernel::getWorkGroupInfo() due to error code "
////      << err);
////  }
////
////  if((blockSizeX * blockSizeY) > kernelWorkGroupSize)
////  {
////    THROW_SIMEX("Neurosim::createKernel: Out of Resources!" << std::endl
////      << "Group Size specified : " << blockSizeX * blockSizeY << std::endl
////      << "Max Group Size supported on the kernel : " << kernelWorkGroupSize);
////  }
////}
///**************************************************************************************************/
//
//#include <boost/shared_ptr.hpp>
//#include "TestNeuronModel.hpp"
//
//#include <map>
//#include "Simulation.hpp"
//#include "neuron_model_traits.hpp"
//#include "DeviceManager.hpp"
//#include "SynapseData.hpp"
//#include "NeuronData.hpp"
//#include "SpikeBuffer.hpp"
//#include "synapse_model.h"
//#include "SzsnnTimer.hpp"
//#include "IF_curr_exp_NeuronModel.hpp"
//#include "Population.hpp"
//#include "NetworkCreator.hpp"
//#include "SpikeRecorder.hpp"
//#include "Izhikevich_NeuronModel.hpp"
//
//void izih()
//{
//	typedef float floatT;
//	size_t n = 1000;
//	size_t n08 = (size_t) n * 0.8;
//	size_t n02 = (size_t) n * 0.2;
//	szsnn::construction::Population<szsnn::Izhikevich_NeuronModel<floatT> > p1(n08, 0, "excictatory");
//	szsnn::construction::Population<szsnn::Izhikevich_NeuronModel<floatT> >  p2(n02, n08, "inhibitory");
//	szsnn::construction::Connection<floatT> con1(0u, 0u);
//	szsnn::construction::Connection<floatT> con2(0u, 0u);
//	con1.loadFromFile("izi1.conn");
//	con2.loadFromFile("izi2.conn");
//	szsnn::construction::NetworkCreator<szsnn::Izhikevich_NeuronModel<floatT> > netC;
//	netC.addConnection(con1);
//	netC.addConnection(con2);
//	typedef szsnn::Izhikevich_NeuronModel<floatT> neuronT;
//	p1.setNeuronParameter(neuronT::p_D, 8.0);
//	p2.setNeuronParameter(neuronT::p_D, 8.0);
//
//	unsigned STEPS = 340;
//	netC.addPopulation(p1);
//	netC.addPopulation(p2);
//	szsnn::NetworkGraph<szsnn::Izhikevich_NeuronModel<floatT> > ngg;
//	netC.fillNetworkGraph(ngg);
//	szsnn::Context cc;
//	cc.setup();
//	szsnn::Simulation<floatT> sim(cc, ngg);
//	szsnn::record::SpikeRecorder r(0, STEPS);
//	szsnn::Simulation<floatT>::firing_input inputer;
//	szsnn::Simulation<floatT>::firing_output foutput;
//	for(unsigned nn = 0; n<100; ++n)
//	{
//		inputer.push_back(nn);
//	}
//	for(unsigned c = 0; c < STEPS; c++)
//	{
//		if(c == 50)
//		{
//			foutput = sim.stepAndFired(inputer);
//		}
//		else
//		{
//			foutput = sim.stepAndFired();
//		}
//		r.recordCycleSpikes(foutput);
//		std::cout<< "runda " << c<< std::endl;
//		//std::cin.sync();std::cin.get();
//		if(!foutput.empty()){
//			std::cout<< "strzalilo " << foutput.size();
//		}
//		else {continue;}
//		for(unsigned i = 0; i < foutput.size(); i ++)
//		{
//			std::cout<< " " << foutput[i] << std::endl;
//		}
//		//std::cin.sync();std::cin.get();
//	}
//	r.saveToFileByNeurons("my_spikes_izi.txt");
//
//}
//int _oldmain(int argc, char **argv) {
//
//	//izih();
//	//return 0;
//	typedef float floatT;
//	szsnn::SzsnnTimer t;
//	size_t n = 1000;
//	size_t n08 = (size_t) n * 0.8;
//	size_t n02 = (size_t) n * 0.2;
//	szsnn::construction::Population<szsnn::IF_curr_exp_NeuronModel<floatT> > p1(n08, 0, "excictatory");
//	szsnn::construction::Population<szsnn::IF_curr_exp_NeuronModel<floatT> >  p2(n02, n08, "inhibitory");
//	szsnn::construction::Connection<floatT> con1(0u, 0u);
//	szsnn::construction::Connection<floatT> con2(0u, 0u);
//	con1.loadFromFile("inhib.conn");
//	con2.loadFromFile("excit.conn");
//	szsnn::construction::NetworkCreator<szsnn::IF_curr_exp_NeuronModel<floatT> > netC;
//	netC.addConnection(con1);
//	netC.addConnection(con2);
//	typedef szsnn::IF_curr_exp_NeuronModel<floatT> neuronT;
//	p1.setNeuronParameter(neuronT::p_V_REST, -49.0);
//	p2.setNeuronParameter(neuronT::p_V_REST, -49.0);
//	p1.setNeuronParameter(neuronT::p_TAU_SYN_I, 10.0);
//	p2.setNeuronParameter(neuronT::p_TAU_SYN_I, 10.0);
//	p1.setNeuronParameter(neuronT::p_TAU_SYN_E, 5.0);
//	p2.setNeuronParameter(neuronT::p_TAU_SYN_E, 5.0);
//	p1.setNeuronParameter(neuronT::p_V_RESET, -70.0);
//	p2.setNeuronParameter(neuronT::p_V_RESET, -70.0);
//	p1.setNeuronParameter(neuronT::p_V_THRESH, -50.0);
//	p2.setNeuronParameter(neuronT::p_V_THRESH, -50.0);
//	p1.setNeuronVariable(neuronT::v_V, -70.0);
//	p2.setNeuronVariable(neuronT::v_V, -70.0);
//	p1.setNeuronVariable(neuronT::v_IE, 10.0);
//	p2.setNeuronVariable(neuronT::v_IE, 10.0);
//
//	netC.addPopulation(p1);
//	netC.addPopulation(p2);
//	szsnn::NetworkGraph<szsnn::IF_curr_exp_NeuronModel<floatT> > ngg;
//	netC.fillNetworkGraph(ngg);
//	szsnn::Context cc;
//	cc.setup();
//	szsnn::Simulation<floatT> sim(cc, ngg);
//
//
////	std::cout << ngg.neuronsToString() << std::endl;
////	std::cout << ngg.synapsesToString() << std::endl;
//
////	std::cout << sim.neurons.neuronParamsToString() << std::endl;
////	std::cout << sim.neurons.neuronVarsToString() << std::endl;
////	std::cin.sync();std::cin.get();
//
//	//return 0;
//	//return 0;
//	szsnn::Simulation<floatT>::firing_output foutput;
////#if SZSNN_DEBUG
//	//freopen ("myfile.txt","w",stdout);
////#endif
//	szsnn::record::SpikeRecorder r(0, 1000);
//	for(int c = 0; c < 1000; c++)
//	{
//
//		foutput = sim.stepAndFired();
//		r.recordCycleSpikes(foutput);
//		std::cout<< "runda " << c<< std::endl;
//		//std::cin.sync();std::cin.get();
//		if(!foutput.empty()){
//			std::cout<< "strzalilo " << foutput.size();
//		}
//		else {continue;}
//		for(unsigned i = 0; i < foutput.size(); i ++)
//		{
//			std::cout<< " " << foutput[i] << std::endl;
//		}
//		//std::cin.sync();std::cin.get();
//	}
//	//netC.saveAllConnections();
////#if SZSNN_DEBUG
//	//fclose (stdout);
////#endif
//	r.saveToFileByNeurons("my_spikes_norm2.txt");
//	std::cout << double(sim.getTime()) / 1000 << "s" << std::endl;
//	return 0;
//	//szsnn::SzsnnException ex("hahaha");
//	//throw ex;
//	try {
//	szsnn::Context c;
//	//boost::shared_ptr<szsnn::Context> cp(new szsnn::Context());
//
////
////	cl::Event ev;
////	cp.get()->setup();
////	szsnn::SpikeBuffer sb(cp);
////
////	sb.initialize(2, "byleco");
////	cl_uint vv[7] = {1, 2, 31, 20, 0, 61, 32};
////	std::vector<cl_uint> vec(&vv[0], &vv[0] + 7);
////	sb.setInput(vec, ev);
////	cp.get()->writeBuffer(sb.getFiredInput(), CL_TRUE);
////
////	std::vector<cl_uint>& bitvec = sb.getFiredInput().getDataRef();
////	std::fill(bitvec.begin(), bitvec.end(), 0);
////	//cp.get()->readBuffer(sb.getFiredInput(), CL_TRUE);
////	unsigned nidx = 0;
////	for(std::vector<cl_uint>::const_iterator it = bitvec.begin(); it != bitvec.end(); ++it)
////	{
////		for(int i=0; i<32; i++ )
////		{
////			bool bit = ((*it) & (1<<i));
////			std::cout << nidx << "  " <<  bit << std::endl;
////			++nidx;
////		}
////	}
//	szsnn::NetworkGraph<szsnn::IF_curr_exp_NeuronModel<float> > ng;
//	unsigned networkSize = 10;
//	for(int i =0 ; i <networkSize; i++)
//	{
//		szsnn::IF_curr_exp_NeuronModel<float> n;
//		ng.addNeuron(n);
//	}
//
//	for(int i =0 ; i <networkSize; i++)
//	{
//		unsigned j = (i + 3) % networkSize;
//		szsnn::Synapse<float> sy;
//		sy.sourceNeuronIdx = i;
//		sy.targetNeuronIdx = j;
//		sy.delay = 1;
//		sy.weight = 1.62;
//		ng.addSynapse(0, sy);
//	}
//	//ng.createRandomNetwork(100, 4, 100, 0, 1, 0.01, 1, 4, unsigned(time(0)));
////
////
////	//************ TESTY *******************
////	unsigned partSize = 50;
////	for(int i =0 ; i <partSize; i++)
////	{
////		szsnn::TestNeuronModel<float> n;
////		float p[2] = {2,3};
////		float v[2] = {0,0};
////		//if (!((i+1)%33)) p[0] = 1;
////		if (i == 1) p[0] = 1;
////		if (i == 2) p[0] = 1;
////		if(i == MAX_PARTITION_SIZE) p[0] = 1;
////		//if (i != 1024) v[1] = 1;
////		//p[0] = 1;
////		n.setAll(p, v);
////		ng.addNeuron(n);
////	}
////	unsigned nn = 100000;
////	for(int i = 0; i < partSize ; i++)
////	{
////		for(int j = 0; j < partSize; j++)
////		{
////			if(i == (j - nn))
////			{
////				//for(int k = i, d = 0; k < j; k++, d++)
////				for(int k = i, d = nn-1; k < j; k++, d--)
////				{
////					szsnn::Synapse<float> sy;
////					sy.weight = 1.0/nn;
////					if (k == j-1) sy.weight += 1.0/(nn*2);
////					sy.delay = (d % MAX_SYNAPTIC_DELAY) + 1;
////					sy.sourceNeuronIdx = k;
////					sy.targetNeuronIdx = j;
////					sy.synapseIdx = 0;
////					//std::cout << "ADDING SYNAPSE " << k << " -> " << j << " delay "<<sy.delay<< " weight " << sy.weight << std::endl;
////					//system("pause");
////					ng.addSynapse(0, sy);
////
////
////				}
////	/*			szsnn::Synapse<float> sy;
////				sy.weight = 0.4;
////				sy.delay = 62;
////				sy.sourceNeuronIdx = i;
////				sy.targetNeuronIdx = j;
////				sy.synapseIdx = 0;
////				ng.addSynapse(0, sy);
////				sy.sourceNeuronIdx = i+1;
////				sy.delay = 63;
////				ng.addSynapse(0, sy);
////				sy.sourceNeuronIdx = i+2;
////				sy.delay = 64;
////				ng.addSynapse(0, sy);*/
////			}
////
////		}
////	}
////	//for(int i = 0; i < partSize-1; i++)
////	//{
////	//	szsnn::Synapse<float> sy;
////	//	sy.sourceNeuronIdx = i;
////	//	sy.targetNeuronIdx = 1024;
////	//	sy.weight = 1.0/1024;
////	//	sy.delay = 1;
////	//	sy.synapseIdx = 0;
////	//	ng.addSynapse(0, sy);
////	//}
////	for(int i = 0; i < partSize ; i++)
////	{
////		for(int j = 0; j < partSize; j++)
////		{
////			if (j == i) continue;
////			szsnn::Synapse<float> sy;
////			sy.weight = 1.0/(partSize - 1) + (1.0/((partSize - 1) * (partSize - 1)));
////			sy.delay =  1;
////			sy.sourceNeuronIdx = i;
////			sy.targetNeuronIdx = j;
////			sy.synapseIdx = 0;
////			ng.addSynapse(0, sy);
////		}
////	}
////	/*szsnn::Synapse<float> sy;
////	sy.weight = 0.5;
////	sy.delay = 4;
////	sy.sourceNeuronIdx = 0;
////	sy.targetNeuronIdx = 32;
////	sy.synapseIdx = 0;
////	ng.addSynapse(0, sy);
////	sy.targetNeuronIdx = 4;
////	ng.addSynapse(0, sy);
////	sy.sourceNeuronIdx = 1;
////	sy.delay = 6;
////	ng.addSynapse(0, sy);
////	sy.targetNeuronIdx = 32;
////	ng.addSynapse(0, sy);*/
////	//for(int n1 = 0; n1 < MAX_PARTITION_SIZE; ++n1)
////	//{
////	//	for(int n2 = 0; n2 < 2*8; ++n2)
////	//	{
////	//		if (n1 == n2) continue;
////	//		for(int i =1 ; i <= 8; i++)
////	//		{
////	//			szsnn::Synapse<float> sy;
////	//			sy.weight = 1;
////	//			sy.delay = i;
////	//			sy.sourceNeuronIdx = n1;
////	//			sy.targetNeuronIdx = (n1 + n2 + 1) % MAX_PARTITION_SIZE;
////	//			sy.synapseIdx = 0;
////
////	//			ng.addSynapse(0,sy);
////	//		}
////	//	}
////	//}
////	//szsnn::Synapse<float> sy;
////	//sy.weight = 1;
////	//sy.delay = 1;
////	//sy.sourceNeuronIdx = 1;
////	//sy.targetNeuronIdx = 2;
////	//sy.synapseIdx = 0;
////
////	//ng.addSynapse(0,sy);
////	//sy.targetNeuronIdx = 3;
////	//ng.addSynapse(0,sy);
////	//sy.targetNeuronIdx = 2;
////	//sy.delay = 2;
////	//ng.addSynapse(0,sy);
////	//sy.sourceNeuronIdx = 2;
////	//sy.targetNeuronIdx = 4;
////	//sy.weight = -2;
////	//sy.delay = 1;
////	//ng.addSynapse(0, sy);
////	//sy.sourceNeuronIdx = 1;
////	//sy.targetNeuronIdx = MAX_PARTITION_SIZE;
////	//sy.weight = 3;
////	//ng.addSynapse(0, sy);
////	//sy.delay = 2;
////	//ng.addSynapse(0, sy);
////	//sy.delay = 1;
////	//sy.sourceNeuronIdx = MAX_PARTITION_SIZE;
////	//sy.targetNeuronIdx = 1;
////	//sy.weight = -4;
////	//ng.addSynapse(0, sy);
//////	for(std::vector<unsigned>::const_iterator it = ng.getPartitionSizes().begin();
//////			it != ng.getPartitionSizes().end(); ++it)
//////	{
//////		std::cout << (*it) <<std::endl;
//////	}
////	//************ TESTY *******************
//	c.setup();
//	szsnn::Simulation<float> s(c, ng);
//	FILE *stream ;
//	//if((stream = freopen("file.txt", "w", stdout)) == NULL)
//	//	exit(-1);
//	//szsnn::Simulation<float>::firing_input finput;
//	//for(int i = 0; i < partSize-1; i++){
//	//	finput.push_back(i);
//	//}
//
//	//finput.push_back(2);
//	//szsnn::Simulation<float>::firing_output foutput = s.stepAndFired(finput);
//	szsnn::Simulation<float>::firing_output foutput;
//	for(int c = 0; c < 1000; c++)
//	{
//		//foutput = s.stepAndFired();
//		foutput = s.stepAndFired();
//		//s.stepAndFired();
//		//if((c+1) % 64) continue;
//		//if(foutput.size() == 0) continue;
//		//if((c+1) < 1024 ) continue;
//		//if(std::find(foutput.begin(), foutput.end(), 1024)==foutput.end())
//		std::cout<< "runda " << c+1<< std::endl;
//		if(!foutput.empty()){
//			std::cout<< "strzalilo " << foutput.size() << std::endl;
//		}
//		else {continue;}
//		//system("pause");
//		//continue;
//		for(int i = 0; i < foutput.size(); i ++)
//		{
//
//			std::cout<< "strzal " << foutput[i] << std::endl;
//		}
//		//system("pause");
//	}
//	//fclose(stream);
//	} catch(const szsnn::SzsnnException e)
//	{
//		std::cout<<e.what()<<std::endl;
//	} catch(const std::bad_alloc ba)
//	{
//		std::cout<<ba.what()<<std::endl;
//	} catch (...)
//	{
//		std::cout<<"Unknown"<<std::endl;
//	}
//	//system("pause");
//
//	//std::cout << s.context.get()->getDevice().getInfo<CL_DEVICE_NAME>() << " <- to" <<std::endl;
////	boost::shared_ptr<szsnn::Context> cp(new szsnn::Context);
////	//boost::shared_ptr<szsnn::NetworkGraph> ntgr(new szsnn::NetworkGraph);
////	cp.get()->setup();
////	cl::Kernel ker;
////	cp.get()->createKernel(ker, "lesson1_kernel.cl", "DEFAULT","",64, 1);
////	//ntgr.get()->createRandomNetwork(30, 4, 20, 0, 1, 0.4, 1, 10, unsigned(time(0)));
////
////	szsnn::TestNeuronModel<float> tnm(1.0, 2.0, 3, 4);
////	std::cout << szsnn::TestNeuronModel<float>::base_type::INHIBITORY << std::endl;
////	//tnm.debug(szsnn::TestNeuronModel<float>::base_type::INHIBITORY);
////	tnm.randomizeMe(szsnn::TestNeuronModel<float>::base_type::INHIBITORY);
////	//tnm = tnm._genRandom(szsnn::NeuronModel::EXCITATORY);
////	std::cout << szsnn::neuron_model_traits<szsnn::TestNeuronModel<float> >::param_count << std::endl;
////	std::cout << tnm.getVar<1>() << " asda";
////	//szsnn::TestNeuronModel tnm(1.0, 2.0);
////	//std::cout << szsnn::neuron_model_traits<szsnn::TestNeuronModel >::param_count << std::endl;
//////	std::cout << tnm.getParam<0>() << " "; //<<  szsnn::is_szsnn_supported_floating_point<float>::bla << std::endl;
////
//////
//////	szsnn::SynapseData sd(cp);
//////
////	szsnn::NetworkGraph<szsnn::TestNeuronModel<float> > ng;
////	for(int i=0; i < 31; ++i)
////	{
////		szsnn::TestNeuronModel<float> n;
////		n.randomizeMe(false);
////		ng.addNeuron(n);
////	}
////	synapseData s1;
////	s1.delay = 1;
////	s1.sourceNeuronIdx = 0;
////	s1.targetNeuronIdx = 2;
////	s1.weight = 12;
////	for(int i = 0; i < 29; i++)
////	{
////		ng.addSynapse(0, s1);
////	}
////	s1.delay = 2;
////	s1.targetNeuronIdx = 21;
////	ng.addSynapse(0, s1);
////	s1.sourceNeuronIdx = 2;
////	ng.addSynapse(0, s1);
////
////	s1.delay = 4;
////	s1.sourceNeuronIdx = 22;
////	s1.targetNeuronIdx = 3;
////	ng.addSynapse(0, s1);
////	std::cout << ng.neuronCount << std::endl;
//////
//////
////	//ng.createRandomNetwork(30, 4, 20, 0, 1, 0.4, 1, 4, unsigned(time(0)));
////	std::cout << ng.neuronsToString() << std::endl;
////	szsnn::NeuronData<float> nd(cp);
////	nd.initialize(ng);
////
////	std::vector<std::string> kernelNames;
////	kernelNames.push_back("DEFAULT"); //TODO;
////	szsnn::DataBuffer<float> tbuff(64, CL_MEM_READ_WRITE, "test", kernelNames);
////	cp.get()->createBuffer(tbuff);
////	nd.test();
////	if (nd.test())
////		std::cout << "OK";
////	else
////		std::cout << "WRONG";
////
////	nd.uploadToDevice();
////	cp.get()->setKernelArg(ker, "DEFAULT", 0, nd.getNeuronParametersRef().buffer, nd.getNeuronParametersRef().getBufferName());
////	cp.get()->setKernelArg(ker, "DEFAULT", 1, nd.getNeuronVariablessRef().buffer, nd.getNeuronVariablessRef().getBufferName());
////	cp.get()->setKernelArg(ker, "DEFAULT", 2, tbuff.buffer, tbuff.getBufferName());
////	cp.get()->setKernelArg(ker, "DEFAULT", 3, 64, "count");
////
////	cl::Event event;
////	cp.get()->enqueueKernel(ker, "DEFAULT",cl::NDRange(64), cl::NDRange(1,1), NULL,  &event);
////	event.wait();
////	cp.get()->readBuffer(tbuff, CL_TRUE);
////	for(std::vector<float>::iterator it = tbuff.data.begin(); it != tbuff.data.end(); ++it)
////	{
////		std::cout << (*it) << std::endl;
////	}
////
////	std::cout << ng.neuronsToString() << std::endl;
////	std::cout << ng.synapsesToString() << std::endl;
////	szsnn::SynapseData<float> sd(cp);
////	sd.initialize(ng);
////	std::cout << sd.groups.printGroupPtrs() << std::endl;
////	std::cout << sd.groups.printGroupMatrix() << std::endl;
////
////	std::cout << sd.printTargets() << std::endl;
////	std::cout << sd.printDelaysBitmap() << std::endl;
//	return 0;
//
//	//////////////////////////////////////////////////////////////////////
////	for(int i=0; i < 31; ++i)
////	{
////		CPU_neuronType n = createRandomNeuron(1);
////		ng.addNeuron(n);
////	}
////
////	synapseData s1;
////	s1.delay = 1;
////	s1.sourceNeuronIdx = 0;
////	s1.targetNeuronIdx = 2;
////	s1.weight = 12;
////	for(int i = 0; i < 29; i++)
////	{
////		ng.addSynapse(0, s1);
////	}
////	s1.delay = 2;
////	s1.targetNeuronIdx = 21;
////	ng.addSynapse(0, s1);
////	s1.sourceNeuronIdx = 2;
////	ng.addSynapse(0, s1);
////
////	s1.delay = 4;
////	s1.sourceNeuronIdx = 22;
////	s1.targetNeuronIdx = 3;
////	ng.addSynapse(0, s1);
////
////	std::cout << ng.neuronCount << std::endl;
////
////	sd.initialize(ng);
////	std::cout << ng.synapsesToString() << std::endl;
////	std::cout << std::endl;
////	std::cout << sd.groups.printGroupPtrs() << std::endl;
////	std::cout << sd.groups.printGroupMatrix() << std::endl;
////
////	std::cout << sd.printTargets() << std::endl;
////	//std::cout << sd.printDelaysBitmap() << std::endl;
////	//std::cout << ng.neuronsToString() << std::endl;
////	//std::cout << std::endl << std::endl << std::endl << std::endl;
////	//std::cout << ng.synapsesToString() << std::endl;
////
////	return 0;
////	cout << "ASDASD" << endl;
////	//cout << getPath() << endl;
////	szsnn::Context c;
////	c.setup();
////	cout << c.getPlatform().getInfo<CL_PLATFORM_VENDOR>() << endl;
////	cout << c.getDevice().getInfo<CL_DEVICE_NAME>() << endl;
//
////	string s = "asd\nasdwasd\nqsd\rdasd\n";
////	replaceNewlineWithSpaces(s);
////	cout << s << endl;
////	cl_int err = CL_SUCCESS;
////	cl_device_type dType;
////	cl_uint myDeviceId;
////	bool found = false;
////
////	err = cl::Platform::get(&platforms);
////	ASSERT_CL_SUCCESS(err, "Neurosim::setupCL: Platform::get() failed.")
////
////	/*Find target platform*/
////	found = false;
////	std::vector<cl::Platform>::iterator i;
////	for (i = platforms.begin(); i != platforms.end(); ++i)
////	{
////	/*if(!strcmp((*i).getInfo<CL_PLATFORM_VENDOR>().c_str(), TARGET_PLATFORM_VENDOR))
////	{
////	  found = true; break;
////	}*/
////	found = true; break;
////	}
////
////	if(!found)
////	{
////	std::stringstream ss;
////	ss << "Neurosim::setup: Unable to find target platform with vendor " <<
////	  (*i).getInfo<CL_PLATFORM_VENDOR>().c_str() << "\n";
////	throw szsnn::SzsnnException(ss.str());
////	}
////
////	(*i).getDevices(CL_DEVICE_TYPE_ALL, &devices);
////
////	std::vector<cl::Device>::iterator d;
////
////	FIND_TARGET_DEVICE(devices, TARGET_DEVICE_NAME, d, found);
////
////	if(!found)
////	{
////	std::stringstream ss;
////	ss << "Neurosim::setup: Unable to find target devices " << TARGET_DEVICE_NAME
////	  << " on platform from vendor "  << (*i).getInfo<CL_PLATFORM_VENDOR>().c_str() << "\n";
////	throw szsnn::SzsnnException(ss.str());
////	}
////
////	LOG_SIM("Found device " << (*d).getInfo<CL_DEVICE_NAME>());
////
////	dType = (*d).getInfo<CL_DEVICE_TYPE>();
////	myDeviceId = (*d).getInfo<CL_DEVICE_VENDOR_ID>();
////
////	cl_context_properties cps[3] =
////	{
////	  CL_CONTEXT_PLATFORM,
////	  (cl_context_properties)(*i)(),
////	  0
////	};
////
////	context = cl::Context(dType, cps, NULL, NULL, &err);
////	ASSERT_CL_SUCCESS(err, "Neurosim::setupCL: Context::Context() failed.")
////
////	devices = context.getInfo<CL_CONTEXT_DEVICES>();
////	ASSERT_CL_SUCCESS(err, "Neurosim::setupCL: Context::getInfo() failed.")
////
////	FIND_TARGET_DEVICE(devices, TARGET_DEVICE_NAME, d, found);
////
////	if(!found)
////	{
////	std::stringstream ss;
////	ss << "Neurosim::setup: Unable to find target devices " << TARGET_DEVICE_NAME
////	  << " within created context on platform from vendor "
////	  << (*i).getInfo<CL_PLATFORM_VENDOR>().c_str() << "\n";
////	throw szsnn::SzsnnException(ss.str());
////	}
////
////	LOG_SIM("Creating command queue for device " << (*d).getInfo<CL_DEVICE_NAME>());
////	commandQueue = cl::CommandQueue(context, *d, 0, &err);
////	ASSERT_CL_SUCCESS(err, "Neurosim::setupCL: CommandQueue::CommandQueue() failed.")
////
////	std::stringstream *dataToReportLogFile;
//
////	cl_int err;
////
////	std::string hw("Hello World\n");
////
////	std::vector<char> outH_vec(hw.length()+1, 'a');
////	std::cout <<  outH_vec[0] << " 111111111111" <<std::endl;
////
////	char * outH = &outH_vec[0];
////	cl::Buffer outCL;
////	c.getDeviceMgr()->registerKernel("ker");
////	c.getDeviceMgr()->registerMemory("ker", "outCL", szsnn::DeviceManager::MEM_GLOBAL, 13);
////	c.createBuffer(outCL, "outCL", CL_MEM_READ_WRITE, outH_vec.size());
////	//c.writeBuffer(outCL, CL_TRUE, 0, hw.length()+1, outH);
////	c.writeBuffer(outCL, "outCL", CL_TRUE, outH_vec);
//////	//	cl::Buffer outCL(
//////		 c.getContext(),
//////		 CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
//////		 hw.length()+1,
//////		 outH,
//////		 &err);
//////	CHECK_CL_SUCCESS(err, "BUFFER");
////
////
////
////	cl::Kernel ker;
////	//c.registerMemory("ker", "outCL", DeviceManager::MEM_GLOBAL, outH_vec.size())
////	c.createKernel(ker, "lesson1_kernel.cl", "hello","",4*64, 1);
////
////	c.setKernelArg(ker, "ker", 0, outCL, "outCL");
//////	err = ker.setArg(0, outCL);
//////	CHECK_CL_SUCCESS(err, "Kernel::setArg()");
////
////	cl::Event event;
////	c.enqueueKernel(ker, "ker", cl::NDRange(hw.length()+1), cl::NDRange(1, 1), NULL, &event);
//////		err = c.getCommandQueue().enqueueNDRangeKernel(
//////		   ker,
//////		   cl::NullRange,
//////		   cl::NDRange(hw.length()+1),
//////		   cl::NDRange(1, 1),
//////		   NULL,
//////		   &event);
//////
//////		CHECK_CL_SUCCESS(err, "ComamndQueue::enqueueNDRangeKernel()");
////		event.wait();
////		//c.readBuffer(outCL, CL_FALSE, 0, hw.length()+1, outH);
////		c.readBuffer(outCL, "outCL", CL_FALSE, outH_vec);
////		std::cout << outH_vec[1] << " A" <<std::endl;
//////		err = c.getCommandQueue().enqueueReadBuffer(
//////		   outCL,
//////		   CL_TRUE,
//////		   0,
//////		   hw.length()+1,
//////		   outH);
//////
//////		CHECK_CL_SUCCESS(err, "ComamndQueue::enqueueReadBuffer()");
////		//std::cout << outH_vec[0]<< outH_vec[1]<< outH_vec[2] << " aB"<<std::endl;
////		//std::cout << c.getDeviceMgr()->getKernelMemoryUsage("ker") << std::endl;
////
////
////
//////		srand(time(0));
//////
//////		int range = (20 - 1) + 1;
//////		for(int i  = 0; i < 10; i++)
//////		{
//////			std::cout << 1 + (unsigned)((float)rand() / (RAND_MAX + 1.0) * range) << " ";
//////
//////		}
//////		return 0;
//
//
//}
