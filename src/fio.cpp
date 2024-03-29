#include "fio.h"
#include "utl.h"


#include <fstream>
#include <iostream>

CFIOList::CFIOList()
{

  loaders.push_back (new CFIOPlainText);


}


CFIOList::~CFIOList()
{
  for (int i = 0; i < loaders.size(); i++)
       delete loaders[i];

}



CFIO* CFIOList::get_loader_for_file (const std::string fname)
{
  std::cout << "CFIO* CFIOList::get_loader_for_file: " << fname;

  for (int i = 0; i < loaders.size(); i++)
      {
       CFIO* fio = loaders[i];
       if (fio->understand (fname))
          return fio;
      }

  return 0;
}


std::vector <std::string> CFIOPlainText::load (const std::string fname)
{
  std::cout << "CFIO* CFIOPlainText::load: " << fname;


  std::vector<std::string> lines;

  std::string line;

  std::ifstream infile (fname, std::ios_base::in);

  while (getline (infile, line, '\n'))
        {
         if (line.size() > 1)
            lines.push_back (line);
        }

  return lines;
}


bool CFIOPlainText::understand (const std::string fname)
{
  std::string ext = get_file_ext (fname);

  if (ext == "txt")
     return true;

  return false;
}

