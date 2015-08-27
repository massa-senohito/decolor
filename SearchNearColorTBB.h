#pragma once
#include <tbb\parallel_reduce.h>
#include "tbb/blocked_range.h"
#include "oclUtils.h"
struct TrueColor;//https://www.threadingbuildingblocks.org/docs/help/reference/algorithms/parallel_reduce_func.htm

typedef unsigned char uchar ;
class SearchNearColorTBB
{
  TrueColor* nearest;
public:
  SearchNearColorTBB() :nearest(nullptr)
  {

  }
  ~SearchNearColorTBB()
  {
    delete nearest;
  }
  void join(SearchNearColorTBB&);
  void operator()(const tbb::blocked_range<int*>& r)
  {
    
  }
};

class HistMap
{
  TrueColor* nearest;
public:
  HistMap() :nearest(nullptr)
  {

  }
  ~HistMap()
  {
    delete nearest;
  }
  void join(SearchNearColorTBB&);
  void operator()(const tbb::blocked_range<int*>& r)
  {
    
  }
};
typedef cl_int ErrorCode;
struct uchar3
{
  unsigned char x, y, z;
};
void Cleanup(int iExitCode);
ErrorCode createKernel(const char* name, cl_kernel* kernel, cl_program pid);
bool testPlatFormAndInitCL();
ErrorCode setupCLProg(const char* fileName, cl_program* pid);
ErrorCode setKernelArg(cl_kernel kernel, size_t num, size_t datasize, const void* data);
uchar* searchNearColorCL(uchar* cs, uchar* rank, size_t ,size_t, size_t reduceColorSize);
