#include "textbuffer.h"
#include "utl.h"
#include <iostream>

bool CTextBuffer::load (std::string fname)
{

 // std::cout << "CTextBuffer::load: " << fname;


   CFIO *f = loader.get_loader_for_file (fname);
   if (f)
      {
       lines = f->load (fname);
       return true;
      }


   return false;

}

