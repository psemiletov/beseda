#include <fstream>
#include <iostream>
#include <algorithm>


#include "fio.h"
#include "utl.h"


#include "pugixml.hpp"
#include "zip.h"


class CXML_walker: public pugi::xml_tree_walker
{
public:

  std::vector <std::string> paragraphs; //text paragraph element, i.e. p para etc
  std::vector <std::string> lines;

  bool fine_spaces;
  bool for_each (pugi::xml_node& node);
};


bool CXML_walker::for_each (pugi::xml_node &node)
{
  if (node.type() != pugi::node_element)
      return true;

  std::string node_name = node.name();

//  std::cout << "node_name:" << node_name << std::endl;

//  std::string node_value = node.value();

 //std::cout << "node_value:" << node_value << std::endl;

 //std::cout << "node_txt:" << node.text().as_string() << std::endl;

  //if (paragraphs.contains (node_name, Qt::CaseInsensitive))
  if (std::find(paragraphs.begin(), paragraphs.end(), node_name) != paragraphs.end())
     {
      // Element in vector.
      std::string t = node.text().as_string();

      std::cout << "t:" << t << std::endl;

      if (! t.empty())
         {
           if (t.size() != 1 && t[0] != ' ' && t[0] != '\n')
               lines.push_back (t);
         }
      }

  return true;
}


std::vector <std::string> extract_text_from_xml_pugi (const char *string_data, size_t datasizebytes, std::vector <std::string> tags)
{
  std::cout << "extract_text_from_xml_pugi 1" << std::endl;

  std::vector <std::string> result_lines;
  //  QString data;

  pugi::xml_document doc;

  pugi::xml_parse_result result = doc.load_buffer (string_data,
                                                   datasizebytes,
                                                   pugi::parse_default,
                                                   pugi::encoding_utf8);


  std::cout << "extract_text_from_xml_pugi 2" << std::endl;


  if (! result)
     {
      std::cout << "NOT PARSED" << std::endl;
      return result_lines;
     }


 std::cout << "extract_text_from_xml_pugi 3" << std::endl;

  CXML_walker walker;
  //walker.text = &data;
  //walker.fine_spaces = settings->value ("show_ebooks_fine", "0").toBool();

  walker.paragraphs.reserve (tags.size());
  std::copy (tags.begin(), tags.end(), back_inserter(walker.paragraphs));

  std::cout << "extract_text_from_xml_pugi 4" << std::endl;


  doc.traverse (walker);


  std::cout << "extract_text_from_xml_pugi 5" << std::endl;

  return walker.lines;
}


CFIOList::CFIOList()
{
  std::cout << "CFIOList::CFIOList() - 1\n";


  loaders.push_back (new CFIOPlainText);
  loaders.push_back (new CFIOABW);
   loaders.push_back (new CFIOXMLZipped);

  std::cout << "CFIOList::CFIOList() - 2\n";

}


CFIOList::~CFIOList()
{
  for (int i = 0; i < loaders.size(); i++)
       delete loaders[i];

}



CFIO* CFIOList::get_loader_for_file (const std::string &fname)
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


std::vector <std::string> CFIOPlainText::load (const std::string &fname)
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


bool CFIOPlainText::understand (const std::string &fname)
{
  std::string ext = get_file_ext (fname);

  if (ext == "txt")
     return true;

  return false;
}




std::vector <std::string> CFIOABW::load (const std::string &fname)
{

  std::string temp = string_file_load (fname);



  std::vector<std::string> tags;
//  tags.push_back ("p");

    tags.push_back ("c");



  std::vector<std::string> lines;

  lines = extract_text_from_xml_pugi (temp.c_str(), temp.size(), tags);

  //  std::cout << "CFIO* CFIOABW::load 3\n";


    std::cout << "lines.size:\n" << lines.size();

  return lines;
}




bool CFIOABW::understand (const std::string &fname)
{
  std::string ext = get_file_ext (fname);

  if (ext == "abw")
     return true;

  return false;
}




bool CFIOXMLZipped::understand (const std::string &fname)
{
  std::cout  << "CFIOXMLZipped::understand (const std::string &fname)\n";

  std::string ext = get_file_ext (fname);


  if (ext == "odt" || ext == "docx" || ext == "kwd" || ext == "sxw")
     return true;

  return false;
}



std::vector <std::string> CFIOXMLZipped::load (const std::string &fname)
{

  std::cout << "^^^^^^^^^^^^^^^ CFIOXMLZipped::load (const std::string &fname) " << std::endl;

  std::vector <std::string> tags;
  std::vector <std::string> lines;

  std::string source_fname;
  std::string ext = get_file_ext (fname);

  if (ext == "kwd")
     {
      source_fname = "maindoc.xml";
      tags.push_back ("text");
      tags.push_back ("TEXT");

     }
  else
  if (ext == "docx")
     {
      source_fname = "word/document.xml";
      tags.push_back ("w:t");
     }
  else
  if (ext == "odt" || ext == "sxw" )
     {
      source_fname = "content.xml";
      tags.push_back ( "text:p");
      tags.push_back ( "text:s");
     }

     std::cout << "^^^^^^^^^^^^^^^ 1\n";


//  CZipper zipper;
//  if (! zipper.read_as_utf8 (fname, source_fname))
  //    return false;


  void *buf = NULL;
  size_t bufsize;

  struct zip_t *zip = zip_open(fname.c_str(), 0, 'r');

  std::cout << "^^^^^^^^^^^^^^^ 2\n";


  if (! zip)
     return lines;


  std::cout << "^^^^^^^^^^^^^^^ 3\n";

  if (zip_entry_open(zip, source_fname.c_str()) < 0)
     return lines;

    std::cout << "^^^^^^^^^^^^^^^ 4\n";

  zip_entry_read(zip, &buf, &bufsize);

  std::cout << "^^^^^^^^^^^^^^^ 5\n";


  zip_entry_close(zip);


    std::cout << "^^^^^^^^^^^^^^^ 6\n";

  lines = extract_text_from_xml_pugi ((char*)buf, bufsize, tags);


    std::cout << "^^^^^^^^^^^^^^^ 7\n";


  zip_close(zip);

     std::cout << "^^^^^^^^^^^^^^^ 8\n";

  free(buf);


     std::cout << "^^^^^^^^^^^^^^^ 9\n";

  return lines;
}

