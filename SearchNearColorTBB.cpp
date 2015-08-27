#include "stdafx.h"
#include "SearchNearColorTBB.h"


//SearchNearColorTBB::SearchNearColorTBB()
//{
//}
//
//
//SearchNearColorTBB::~SearchNearColorTBB()
//{
//}
cl_platform_id cpPlatform;
void (*pCleanup)(int) = &Cleanup;
cl_context cxGPUContext;
cl_device_id* cdDevices;
cl_uint uiDevCount;
cl_command_queue cqCommandQueue;
cl_kernel ckKernel;
cl_mem vbo_cl;
cl_program cpProgram;
char* cSourceCL = NULL;             // Buffer to hold source for compilation 
ErrorCode ciErrNum;
bool testPlatFormAndInitCL()
{
  oclGetPlatformID(&cpPlatform);
  oclCheckErrorEX(ciErrNum, CL_SUCCESS, pCleanup);
  ciErrNum = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 0, NULL, &uiDevCount);
  oclCheckErrorEX(ciErrNum, CL_SUCCESS, pCleanup);
  if (ciErrNum != CL_SUCCESS) { return false; }
  // Create the device list
  cdDevices = new cl_device_id [uiDevCount];
  ciErrNum = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, uiDevCount, cdDevices, NULL);
  oclCheckErrorEX(ciErrNum, CL_SUCCESS, pCleanup);
  //デバイス情報は取らない
  cl_context_properties props[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)cpPlatform, 0};
  cxGPUContext = clCreateContext(props, 1, &cdDevices[0], NULL, NULL, &ciErrNum);
  cqCommandQueue = clCreateCommandQueue(cxGPUContext, cdDevices[0], 0, &ciErrNum);
  shrCheckErrorEX(ciErrNum, CL_SUCCESS, pCleanup);

    // Program Setup
  size_t program_length;
  auto cPathAndName = "./DecolorKernel.cl"; //shrFindFilePath("simpleGL.cl", argv[0]);
  shrCheckErrorEX(cPathAndName != NULL, shrTRUE, pCleanup);
  cSourceCL = oclLoadProgSource(cPathAndName, "", &program_length);
  if (cSourceCL == NULL) { pCleanup(0); exit(0); }
  shrCheckErrorEX(cSourceCL != NULL, shrTRUE, pCleanup);
// create the program
  cpProgram = clCreateProgramWithSource(cxGPUContext, 1,
		(const char **) &cSourceCL, &program_length, &ciErrNum);
  shrCheckErrorEX(ciErrNum, CL_SUCCESS, pCleanup);

  // build the program
  ciErrNum = clBuildProgram(cpProgram, 0, NULL, "-cl-fast-relaxed-math", NULL, NULL);
  if (ciErrNum != CL_SUCCESS)
  {
    // write out standard error, Build Log and PTX, then cleanup and exit
    shrLogEx(LOGBOTH | ERRORMSG, ciErrNum, STDERROR);
    oclLogBuildInfo(cpProgram, oclGetFirstDev(cxGPUContext));
    oclLogPtx(cpProgram, oclGetFirstDev(cxGPUContext), "log.ptx");
    Cleanup(EXIT_FAILURE); 
  }

  // create the kernel
  ckKernel = clCreateKernel(cpProgram, "findNear", &ciErrNum);
  shrCheckErrorEX(ciErrNum, CL_SUCCESS, pCleanup);
  return true;
}
//CL.Init();plog=CL.setupCLProg(".hoge.cl");prog.createKernel("kfun",ref kid)
//CLHost::
ErrorCode setupCLProg(const char* fileName,cl_program* pid)
{
  return ciErrNum;
}
ErrorCode readWaitQue(cl_mem m, size_t s, void* p)
{

  return clEnqueueReadBuffer(cqCommandQueue, m, true, 0, s, p, 0, nullptr, nullptr);
}
void printByte(byte* p,size_t s) 
{
  for (size_t i = 0; i < s; i+=3)
  {
    std::cout << (int)p[i] << "," <<(int)p[i+1]<< ","<<(int)p[i+2] << std::endl;
  }
}
ErrorCode writeWaitQue(cl_mem m, size_t s, void* p)
{
  return clEnqueueWriteBuffer(cqCommandQueue, m, true, 0, s, p, 0, nullptr, nullptr);
}
ErrorCode createKernel(const char* name,cl_kernel* kernel,cl_program pid)
{
  *kernel = clCreateKernel(pid, name, &ciErrNum);
  return ciErrNum;
}
ErrorCode setKernelArg(cl_kernel kernel,size_t num, size_t datasize, const void* data)
{
  for (size_t i = 0; i < num; i++)
  {
    ciErrNum|= clSetKernelArg(kernel, i, datasize, data);
  }
  return ciErrNum;
}
typedef unsigned char uchar;
uchar* searchNearColorCL(uchar* cs,uchar* rank,size_t bitmapwid,size_t bitmaphei,size_t reduceColorSize)
{
  uchar* resultD = new uchar[bitmapwid*bitmaphei*3]; //std::fill(resultD, resultD + bitmapSize, 255);

  if (!testPlatFormAndInitCL()) { Cleanup(-1); }
  //auto colsize = sizeof(byte)*3;
  //auto usize = sizeof(uchar3);

  typedef cl_mem mem;
  auto ranksize = reduceColorSize*3;//バイトで指定
  auto bitmapsize = bitmapwid*bitmaphei*3;
  auto readAndHost = CL_MEM_COPY_HOST_PTR | CL_MEM_READ_WRITE;
  auto read=  CL_MEM_READ_WRITE;
  auto si = sizeof(mem);
  
  auto m=clCreateBuffer(cxGPUContext, readAndHost, bitmapsize, cs, &ciErrNum);


  //ciErrNum =  writeWaitQue(m, ranksize, rank);
  cl_mem result=clCreateBuffer(cxGPUContext, read, bitmapsize, nullptr, &ciErrNum);
  //cl_mem pal=clCreateBuffer(cxGPUContext, read, bitmapsize, nullptr, &ciErrNum);
  cl_mem dests=clCreateBuffer(cxGPUContext, read, sizeof(unsigned int)*bitmaphei*bitmapwid, nullptr, &ciErrNum);

  auto mm=clCreateBuffer(cxGPUContext, readAndHost, ranksize,rank , &ciErrNum);
  //ciErrNum =  readWaitQue(mm, ranksize, resultD);//cs-m
  //auto ts = ranksize;
  //printByte(resultD, ts);
  auto n=clCreateBuffer(cxGPUContext, readAndHost, sizeof(reduceColorSize),&reduceColorSize,  &ciErrNum);

  ciErrNum= clSetKernelArg(ckKernel, 0, si, &m);
  ciErrNum= clSetKernelArg(ckKernel, 1, si, &mm);
  ciErrNum= clSetKernelArg(ckKernel, 2, si, &n);
  ciErrNum= clSetKernelArg(ckKernel, 3, si, &result);
  ciErrNum= clSetKernelArg(ckKernel, 4, si, &dests);
  //ここでワークサイズを指定、カーネルにいくつ並列させるか
  size_t work[] = { bitmapwid,bitmaphei };
  clEnqueueNDRangeKernel(cqCommandQueue, ckKernel, 2, nullptr, work, nullptr, 0, nullptr, nullptr);
  //結果が返るまでブロックしてくれる
  clEnqueueReadBuffer(cqCommandQueue, result, CL_TRUE, 0, bitmapsize, resultD, 0, nullptr, nullptr); //GPU ⇒ CPU のデータ転送
  auto p = std::unique_ptr<uchar3[]>(new uchar3[bitmapsize]);
  auto d = std::unique_ptr<unsigned int[]>(new unsigned int[bitmapsize]);
  readWaitQue(dests, bitmapsize/3, d.get());//7つまでしか並列が動いていない
  for (size_t i = 0; i < bitmapsize/3; i++)
  {
    auto pp = d.get()[i];
    //std::cout <<(int) pp.x<<","<< (int)pp.y<<","<<(int)pp.z<< std::endl;
    //std::cout << pp<< std::endl;
    
  }
  if(m)clReleaseMemObject(m);
  if(n)clReleaseMemObject(n);
  if(result)clReleaseMemObject(result);
  if(mm)clReleaseMemObject(mm);
  Cleanup(0);
  return resultD;
}
//void hueTest() {}
void Cleanup(int iExitCode)
{
    // Cleanup allocated objects
    shrLog("\nStarting Cleanup...\n\n");
    if(ckKernel)       clReleaseKernel(ckKernel); 
    if(cpProgram)      clReleaseProgram(cpProgram);
    if(cqCommandQueue) clReleaseCommandQueue(cqCommandQueue);
    if(cxGPUContext)clReleaseContext(cxGPUContext);
    if(cSourceCL)free(cSourceCL);
    if(cdDevices)delete[]cdDevices;
    shrExit();
    //exit (iExitCode);
}