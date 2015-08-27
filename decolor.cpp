

#include "stdafx.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "stb_image.h"
#include "TrueColor.h"
#include "SearchNearColorTBB.h"
struct Image
{//std::unique_ptr< TrueColor[]>
  TrueColor* tc; int x; int y;
  ~Image() { delete tc; }
};
//typedef uchar* TrueColor;
typedef std::map<TrueColor, int> Histgram;

typedef const char* Cchar;
int getH(const TrueColor& c)
{
  //auto r = c[0], g = c[1], b = c[2];
  auto r = c.r, g = c.g, b = c.b;

  uchar minv = (std::min)({ r, g, b });
  uchar maxv = (std::max)({ r, g, b });
  uchar md = maxv - minv;
  if (maxv == 0) { return 0; }
  else if (md == 0) { return 0; }
  else
  {
    int h = 0,six=60;
    if (maxv == r)
    {
      h = six*(g - b) / md;
    }
    else if (maxv == g)
    {
      h = six*(b - r) / md + 120;
    }
    else
    {
      h = six*r - g / md + 240;
    }
    if (h < 0)h += 360;
    return h;
  }
}
int distanceOfColor(const TrueColor& c,const TrueColor& cc)
{
  auto h=getH(c) - getH(cc);
  return h*h;
}
int distanceOfRGB(const TrueColor& c,const TrueColor& cc)
{
  auto r = c.r - cc.r;
  auto g = c.g - cc.g;
  auto b = c.b - cc.b;
  return r*r + g*g + b*b;
}
typedef std::pair<TrueColor, Histgram*> mtype;
typedef std::pair<const TrueColor, int> ti;
//typedef const std::map<TrueColor, int> CHist;
typedef Histgram Hist;
typedef std::map <TrueColor,  Hist*> HistDic;
uchar* parralelCompute(Hist* hi, TrueColor* bd, size_t,size_t  , size_t palletsize);
void write(uchar* bit, int w, int h, std::string& name);
HistDic*
  getherColor(int d, Histgram* hist)
{
  auto memory = //std::unique_ptr<HistDic>
    (new HistDic());
  typedef decltype(*memory->begin()) inf;
  for (auto i: *hist)
  {
    auto ite=std::find_if(memory->begin(), memory->end(),
      [&i, d](inf& t){return distanceOfColor(t.first, i.first) < d; });
    if (ite == memory->end())
    {
      //似た色がメモリ中になかった

      auto p = new Hist();
      p->insert(std::make_pair(i.first,i.second));
      auto pp = std::make_pair(i.first, p);
      memory->insert(pp);
    }
    else
    {
      auto it = *ite;
      memory->at(it.first)->insert(i);
    }
  }
  return memory;
}
void deleteHistDic(HistDic* hdic)
{
  for (auto hp : *hdic)
  {
    hp.second->clear();
  }
  hdic->clear();
  delete hdic;
}
//std::unique_ptr<
Histgram *
//>
  arrangeMajorColor(std::map<TrueColor, Histgram*>* dic)
{ 
  typedef decltype(*dic->begin()) inf;
  auto his=new Histgram();
  //constなので怒られてる
  //std::transform(dic->begin(), dic->end(),his->begin(), []
    for(std::pair<TrueColor,Histgram*> d:*dic)
  {
    //持っているエントリの中で最大の値を選ぶ
    auto h = *d.second;
    auto ma = 0;
    std::pair<TrueColor,int> mti;
    for (decltype(mti) i : h)
    {
      if (i.second > ma)
      {
        ma = i.second;
        mti = i;
      }
    }
    his->insert( mti);
  };
  return his;
}

Histgram*
 makeHist(TrueColor* cols,int w,int h)
{
  auto hist=new Histgram();
  //auto hist = std::unique_ptr<Histgram>();
  for (auto c = 0; c < w*h;c+=1)
  {
    if (hist->find(cols[c]) == hist->end())
    {
      hist->emplace(cols[c],1);
    }
    else
    {
      ++hist->at(cols[c]);
    }
  }
  return hist;
}

//typedef std::pair < TrueColor, int > Pair ;
//int compairer(const void* ix,const void* iix)
//{
//  auto i = reinterpret_cast<Pair*>(const_cast<void*>(ix));
//  auto ii = reinterpret_cast<Pair*>(const_cast<void*>(iix));
//  return i->second > ii->second ? 1 : i->second == ii->second ? 0 : -1; 
//}
//std::vector<TrueColor>
//オペレータの定義がおかしいそうだ
//void ranking(Histgram& hist)
//{
//  auto pairv = std::vector<Pair>(hist.begin(),hist.end());
//  std::qsort(&hist, hist.size(), sizeof(Histgram), compairer);
//}
void writeBitMap(uchar* bit,int w,int h)
{
  //stbはv5形式で書き出すようだ、compはチャンネル数、rgbなら3,rgbaなら4
  //http://www.ruche-home.net/program/bmp/struct
  auto fileName = "procFile.bmp";
  //fopen_s(&file,fileName,"w");
  stbi_write_bmp(fileName, w, h, 3, bit);
  //fprintf_s(file, "%x",tmp);
}

void write3(uchar3* bit, int w, int h, std::string& name)
{
  uchar* u=new uchar[w*h*3];
  for (size_t i = 0; i < w*h*3; i+=3)
  {
    u[i]   = bit[i].x;
    u[i+1] = bit[i].y;
    u[i+2] = bit[i].z;
  }
  write(u, w, h, name);
  delete[] u;
}
void write(uchar* bit,int w,int h,std::string& name)
{
  //stbはv5形式で書き出すようだ、compはチャンネル数、rgbなら3,rgbaなら4
  //http://www.ruche-home.net/program/bmp/struct
  std::string fileName = 
     name+ "procFile.bmp";
  //fopen_s(&file,fileName,"w");
  stbi_write_bmp(fileName.c_str(), w, h, 3, bit);
  //fprintf_s(file, "%x",tmp);
}
void writeVec(std::vector<TrueColor>* v,int w,int h,std::string& n)
{
  auto tmp = new std::vector< uchar>();
  tmp->reserve(v->size()*3);
  for (auto i : *v)
  {
    tmp->emplace_back(i.r);
    tmp->emplace_back(i.g);
    tmp->emplace_back(i.b);
  }
  write(tmp->data(), w, h,n);
  delete tmp;
}
byte* truecol2Byte(const TrueColor* tc,size_t mapsize)
{
  auto tmp = new byte[mapsize*3];
  for (size_t i = 0; i < mapsize*3; i+=3)
  {
    tmp[i  ] = tc[i/3].r;
    tmp[i+1] = tc[i/3].g;
    tmp[i+2] = tc[i/3].b;
  }
  return tmp;
}
byte* hist2Byte(Hist* hist)
{
  auto tmp = new byte[hist->size()*3];
  int i = 0;
  for (auto h : *hist) 
  {
    auto c=h.first;
    tmp[i] = c.r;
    tmp[i+1] = c.g;
    tmp[i+2] = c.b;
    i+=3;
  }
  return tmp;
}
uchar3* truecol2u(const TrueColor* tc,size_t mapsize)
{
  auto tmp = new uchar3[mapsize];
  for (size_t i = 0; i < mapsize; ++i)
  {
    tmp[i  ].x = tc[i].r;
    tmp[i].y = tc[i].g;
    tmp[i].z = tc[i].b;
  }
  return tmp;
}
uchar3* hist2u(Hist* hist)
{
  auto tmp = new uchar3[hist->size()];
  int i = 0;
  for (auto h : *hist) 
  {
    auto c=h.first;
    tmp[i].x = c.r;
    tmp[i].y = c.g;
    tmp[i].z = c.b;
    ++i;
  }
  return tmp;
}
//std::unique_ptr<
  std::vector< TrueColor>*
  findNear(Histgram* hist,
  //std::vector<TrueColor*>
  TrueColor* bmp,size_t siz)
{

  //std::transform(hist->begin(), hist->end(), rgbSum->begin(),
  //  [](inf i){return i.first.Sum(); });
  
  auto nears=new std::vector<TrueColor>();
  nears->reserve(siz);
  for (size_t i = 0; i < siz; i++)
  {
    TrueColor tc = bmp[i];
    TrueColor nearest;
    int nv=999;
    //全部と比較している、rgbそれぞれについてソート、2分探索で速くなるが、
    for (auto b : *hist)
    {
      int v = //(b.first.Sum()) - tc.Sum();
        distanceOfRGB(b.first, tc);
      if (nv > v)
      {
        nv = v; nearest = //const_cast<TrueColor*>
          ( b.first);
      }
    }
    nears->push_back(nearest);
  }
  return nears;
}

//std::tuple<
//  std::unique_ptr< Image>//,int,int>
Image* loadFile(Cchar fileName)
{
  int x, y, req_comp;
  stbi_uc* data;
  //for (size_t i = 0; i < x*y*req_comp; i+=req_comp){ }
  data=stbi_load(fileName, &x, &y, &req_comp, STBI_rgb);
  if (!data) { std::cout << "ファイル"<<fileName <<"が見つかりません" << std::endl; exit(2); }
  //std::unique_ptr< TrueColor[]>
  auto* cdata(new TrueColor[x*y]);
  for (int i = 0; i < x*y*req_comp; i+=3)
  {
    cdata[i/3].r = data[i];
    cdata[i/3].g = data[i+1];
    cdata[i/3].b = data[i+2];
  }
  //writeTestMap(data,x,y,"t");
  stbi_image_free(data);
  return new Image{ ( cdata), x, y };
  //return //std::make_tuple
  //  i;
}
void saveHist(Hist* h,TrueColor* bd,int x, int y,std::string& n)
{
  auto reducedColor = //std::unique_ptr<std::vector<TrueColor*>> 
    //(findNear(h, bd,x*y));
    parralelCompute(h, bd, x,y, h->size());
  //writeVec
      write(reducedColor, x, y,n.substr(0,n.size()-4));
      delete reducedColor;
}
uchar* parralelCompute(Hist* hi,TrueColor* bd,size_t w,size_t h,size_t palletsize)
{
  auto b=std::unique_ptr<uchar[]>( truecol2Byte(bd,w*h));
  auto hg= std::unique_ptr<uchar[]>(hist2Byte(hi));
  return searchNearColorCL(b.get(), hg.get(), w,h, palletsize);
  
}
int main(int argc,char** argv)
{
  if (argc == 1) {
    std::cout << "ファイルを指定して実行してください" << std::endl;exit(1);
  }
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
  auto bmp = std::unique_ptr<Image> ( loadFile(argv[1]));
  auto x = bmp->x, y=bmp->y;
  auto bd = ( (bmp)->tc);
  auto hist = // std::shared_ptr<Histgram>
    (makeHist(bd,x,y));
  auto palletsize = hist->size();
  if (palletsize <= 256)
  {
    std::cout << "パレットは"<<palletsize<<"色になりました" << std::endl;
    saveHist(hist, bd, x, y,std::string( argv[1]));
    delete hist;
  }
  else
  {
    for (size_t i = 1; i < 30; i++)
    {
      auto gethercol = //std::unique_ptr<std::map<TrueColor,CHist>>( getherColor(i, *hist));
        (getherColor(i, hist));
      palletsize = gethercol->size();
      if (palletsize < 256)
      {
        //typedef decltype (*arrangeMajorColor(gethercol.get())) mmt;
        auto gcol = std::move(gethercol);
        auto arrangeCol = //std::unique_ptr<Histgram>
          (arrangeMajorColor(gcol));
        auto acol = std::move(arrangeCol);
        std::cout << "近隣" << i << "の色相を選び" << std::endl;
        std::cout << "パレットは" << palletsize << "色になりました" << std::endl;

        auto reducedColor = //std::unique_ptr<std::vector<TrueColor*>> 
          //(findNear(acol, bd, x*y));
          parralelCompute( acol,bd, x,y, palletsize);
        write(reducedColor, x, y,std::string( argv[1]));

        deleteHistDic(gcol);
        hist->clear();
        delete hist;
        acol->clear();
        delete acol;
        //reducedColor->clear();

        delete[] reducedColor;
        //delete bmp;
        break;
      }
    }//256色以下にならなかった
  }
}

