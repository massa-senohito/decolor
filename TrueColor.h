struct TrueColor
{
  unsigned char r;
  unsigned char g;
  unsigned char b;
  int Sum() const
  {
    return r + g + b;
  }

  bool operator==(const TrueColor& c)const
  {
    return this->r == c.r && this->g==c.g && this->b ==c.b;
  }
  bool operator<(const TrueColor& c)const
  {
    return this->r < c.r //&&
      //this->g<c.g //&& 
      //this->b <c.b
      ;
  }
};