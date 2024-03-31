#include <fstream>
#include <iostream>
#include <algorithm>


#include "fio.h"
#include "utl.h"


#include "zip.h"
#include "pugixml.hpp"


class CXML_walker: public pugi::xml_tree_walker
{
public:

  //std::string *text;

  std::vector <std::string> paragraphs; //text paragraph element, i.e. p para etc
  std::vector <std::string> lines;

  //QStringList paragraphs;
  bool fine_spaces;
  bool for_each (pugi::xml_node& node);
};


bool CXML_walker::for_each (pugi::xml_node &node)
{
  if (node.type() != pugi::node_element)
      return true;

  std::string node_name = node.name();

 if (std::find(paragraphs.begin(), paragraphs.end(), node_name) != paragraphs.end())
    {
  // Element in vector.

  //if (paragraphs.contains (node_name, Qt::CaseInsensitive))
      std::string t = node.text().as_string();

      if (! t.empty())
         {
          //if (fine_spaces)
            //  text->append ("   ");

           lines.push_back (t);
          //text->append (t);


          if (t.size() > 1)
             //text->append ("\n");
             lines.push_back ("\n");


         }
      }

  return true;
}


std::vector <std::string> extract_text_from_xml_pugi (const char *string_data, size_t datasizebytes, std::vector <std::string> tags)
{
  std::vector <std::string> result_lines;
  //  QString data;

  pugi::xml_document doc;

  pugi::xml_parse_result result = doc.load_buffer (string_data,
                                                   datasizebytes,
                                                   pugi::parse_default,
                                                   pugi::encoding_utf8);

  if (! result)
     {
      std::cout << "NOT PARSED" << std::endl;
      return result_lines;
     }

  CXML_walker walker;
  //walker.text = &data;
  //walker.fine_spaces = settings->value ("show_ebooks_fine", "0").toBool();

  walker.paragraphs.reserve (tags.size());
  std::copy (tags.begin(), tags.end(), back_inserter(walker.paragraphs));

  doc.traverse (walker);

  return walker.lines;
}






CFIOList::CFIOList()
{

  loaders.push_back (new CFIOPlainText);
  loaders.push_back (new CFIOABW);


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




std::vector <std::string> CFIOABW::load (const std::string fname)
{
  std::cout << "CFIO* CFIOABW::load: " << fname;

  std::string temp = string_file_load (fname);

  std::vector<std::string> tags;
  tags.push_back ("p");


  std::vector<std::string> lines;

  lines = extract_text_from_xml_pugi (temp.c_str(), temp.size(), tags);

  return lines;
}




bool CFIOABW::understand (const std::string fname)
{
  std::string ext = get_file_ext (fname);

  if (ext == "abw")
     return true;

  return false;
}



