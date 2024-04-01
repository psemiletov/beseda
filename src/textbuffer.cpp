#include "textbuffer.h"
#include "utl.h"
#include <iostream>

bool CTextBuffer::load (std::string fname)
{
   CFIO *f = loader.get_loader_for_file (fname);
   if (f)
      {
       lines = f->load (fname);

       if (lines.size() != 0)
           loaded = true;

       return true;
      }


  loaded = false;
  return false;
}


CTextBuffer::CTextBuffer()
{
  loaded = false;

}

