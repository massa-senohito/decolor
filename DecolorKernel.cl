//#pragma OPENCL EXTENSION cl_amd_printf : enable
/*
get_work_dim	Number of dimensions in use
get_global_size	Number of global work items
get_global_id	Global work item ID value
get_local_size	Number of local work items
get_local_id	Local work item ID
get_num_groups	Number of work groups
get_group_id
*/
__kernel void findNear3
  (__global uchar3* cs, __global uchar3* hist, __global int* n, __global uchar3* nearest,
    __global uint* pal,__global int* dests)
{

    unsigned int w = get_global_id(0);
    unsigned int s = get_global_size(0);
    unsigned int he = get_global_id(1);
    int nv=999;
    int3 c= convert_int3(cs[w+s*he]);
    //dests[0] = get_global_size(0);//w
    //dests[1] = get_global_size(1);//h
    //dests[2] = get_local_size(0);//w
    //dests[3] = get_local_size(1);
    //dests[4] = get_num_groups(0);
    //dests[5] = get_num_groups(1);//h
    for (unsigned int j = 0; j< (*n); j++) 
    {
      int3 h  = convert_int3(hist[j]);
      int3 dec= ( c-h);
      int dest=dec.x*dec.x+dec.y*dec.y+dec.z*dec.z;
      if (nv>dest){ 
        //vstore3(h,0,nearest[i]);
        nearest[w+s*he]   = hist[j];
        nv=dest;
      }
    }
}
__kernel void findNear( __global uchar* cs,__global uchar* hist,__global int* n,__global uchar* nearest,
  __global unsigned int* decd)
{
    unsigned int w = get_global_id(0);
    unsigned int s = get_global_size(0);
    unsigned int he = get_global_id(1);//get_global_id(1);
    unsigned int wh = w * 3 + s * he*3;
    int nv=999;
    uchar3 c=(uchar3)(cs[wh],cs[wh+1],cs[wh+2]);
    decd[0] = (get_global_size(0)-1)*3+s*(get_global_size(1)-1)*3;
    decd[1] = s;
    decd[2] = he;
    decd[3] = wh;
    //decd[wh] = wh;
    for(unsigned int j=0;j< (*n)*3 ;j+=3)
    {
      uchar3 h=(uchar3)(hist[j],hist[j+1],hist[j+2]);
      int3 dec=convert_int3(c)-convert_int3(h);
      int dest=dec.x*dec.x+dec.y*dec.y+dec.z*dec.z;
      //decd[wh] = dest;//‹——£‚ª³‚µ‚­‹‚Ü‚Á‚Ä‚¢‚È‚¢,
      //uchar3 nears=nearest[i];
      //int near=nears.x*nears.x+nears.y*nears.y+nears.z*nears.z;
      if (nv>dest){ 
        //vstore3(h,0,nearest[i]);
        nearest[wh]   = h.x;
        nearest[wh+1] = h.y;
        nearest[wh+2] = h.z;
        nv=dest;
      }
    }
}

//__kernel void getHue(__global uchar hue,__global uchar3 cs)
//{
//  uchar minv=min(min(cs[0],cs[1]),cs[2]);
//  uchar maxv=max(max(cs[0],cs[1]),cs[2]);
//  uchar r = cs[0], g = cs[1], b = cs[2];
//  uchar mb=maxv-minv;
//  if(maxv==0) {hue=0;return;}
//    else if (md == 0) { hue=0;return ; }
//  else
//  {
//    int six=60;
//    if (maxv == r)
//    {
//      hue = six*(g - b) / md;
//    }
//    else if (maxv == g)
//    {
//      hue = six*(b - r) / md + 120;
//    }
//    else
//    {
//      hue = six*r - g / md + 240;
//    }
//    if (hue < 0)hue += 360;
//    return ;
//  }
//}
//
//__kernel void getHueAll(__global uchar* hue,__global uchar3* cs)
//{
//  unsigned int i = get_global_id(0);
//  getHue(hue[i],cs[i]);
//}
////TrueColorMap ,HistgramToByte
//
//__kernel void getHueAll(__global uchar* hue, uchar3* cs)
