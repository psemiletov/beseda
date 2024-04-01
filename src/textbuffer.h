#ifndef TEXTBUFFER_H
#define TEXTBUFFER_H

//#include <iostream>
#include <string>
#include <vector>

#include "fio.h"

class CTextBuffer
{
  public:

  CFIOList loader;


  int pos; //current position

  bool loaded;

  std::vector <std::string> lines; //paragraphs of the text file

  CTextBuffer();
  bool load (std::string fname);

};


#endif

