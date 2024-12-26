/***********************************************************
 *   this code by Peter Semiletov is Public Domain         *
 **********************************************************/

#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>


#include "pugixml.hpp"
#include "zip.h"

#include "fio.h"
#include "utl.h"


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

  if (std::find (paragraphs.begin(), paragraphs.end(), node_name) != paragraphs.end())
     {
      // Element in vector.
      std::string t = node.text().as_string();
//      std::cout << "t:" << t << std::endl;
      if (! t.empty())
          if (t.size() != 1 && t[0] != ' ' && t[0] != '\n')
              lines.push_back (t);
     }

  return true;
}


std::vector <std::string> extract_text_from_xml_pugi (const char *string_data, size_t datasizebytes, std::vector <std::string> tags)
{
  std::vector <std::string> result_lines;

  std::string enc = string_between (string_data, "encoding=\"", "\"");

  if (enc.empty())
      enc = "UTF-8";

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

  walker.paragraphs.reserve (tags.size());
  std::copy (tags.begin(), tags.end(), back_inserter(walker.paragraphs));

  doc.traverse (walker);

  if (enc.find ("1251") != std::string::npos)
     {
      for (size_t i = 0; i < walker.lines.size(); i++)
         {
          std::string s = walker.lines.at(i);
          char16_t *t_utf16 = ConvertFromCP1251ToUTF16 (s.c_str());
          std::string t_utf8 = ConvertUTF16ToUTF8 (t_utf16);
          walker.lines[i] = t_utf8;

          delete [] t_utf16;
         }
     }

  return walker.lines;
}


CFIOList::CFIOList()
{
  loaders.push_back (new CFIOPlainText);
  loaders.push_back (new CFIOABW);
  loaders.push_back (new CFIOHTML);

  loaders.push_back (new CFIOXMLZipped);
  loaders.push_back (new CFIOFB2);
  loaders.push_back (new CFIOEPUB);
}


CFIOList::~CFIOList()
{
  for (size_t i = 0; i < loaders.size(); i++)
       delete loaders[i];
}


CFIO* CFIOList::get_loader_for_file (const std::string &fname)
{
  for (size_t i = 0; i < loaders.size(); i++)
      {
       CFIO* fio = loaders[i];
       if (fio->understand (fname))
          return fio;
      }

  return 0;
}


std::vector <std::string> CFIOPlainText::load (const std::string &fname)
{
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


std::vector <std::string> CFIOHTML::load (const std::string &fname)
{
  std::string temp = string_file_load (fname);
  std::string cleared = html_strip (temp);
  std::vector<std::string> lines = split_string_to_vector (cleared, "\n", false);

  return lines;
}


bool CFIOHTML::understand (const std::string &fname)
{
  std::string ext = get_file_ext (fname);

  if (ext == "htm" || ext == "html" || ext == "xhtml"  )
     return true;

  return false;
}


std::vector <std::string> CFIOABW::load (const std::string &fname)
{
  std::string temp = string_file_load (fname);

  std::vector <std::string> tags;

  tags.push_back ("c");

  std::vector<std::string> lines;
  lines = extract_text_from_xml_pugi (temp.c_str(), temp.size(), tags);

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
  std::string ext = get_file_ext (fname);

  if (ext == "odt" || ext == "docx" || ext == "kwd" || ext == "sxw")
     return true;

  return false;
}


std::vector <std::string> CFIOXMLZipped::load (const std::string &fname)
{
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

  void *buf = NULL;
  size_t bufsize = 0;

  struct zip_t *zip = zip_open(fname.c_str(), 0, 'r');

  if (! zip)
     return lines;

  if (zip_entry_open (zip, source_fname.c_str()) < 0)
     return lines;

  zip_entry_read (zip, &buf, &bufsize);
  zip_entry_close (zip);

  if (bufsize == 0)
     return lines;

  lines = extract_text_from_xml_pugi ((char*)buf, bufsize, tags);

  zip_close (zip);
  free (buf);

  return lines;
}


bool CFIOFB2::understand (const std::string &fname)
{
  std::string ext = get_file_ext (fname);

  if (ext == "fb2" || ext == "fbz" || ends_with (fname, "fb2.zip"))
     return true;

  return false;
}


std::vector <std::string> CFIOFB2::load (const std::string &fname)
{
  std::vector <std::string> tags;
  std::vector <std::string> lines;
  std::string ext = get_file_ext (fname);

  tags.push_back ("p");

  if (ext == "fb2")
     {
      std::string temp = string_file_load (fname);
      lines = extract_text_from_xml_pugi (temp.c_str(), temp.size(), tags);
      return lines;
     }

  //else zipped fb2.zip or fbz

  std::string source_fname;

  //we can have malformed internal filename, so find the first fb2 at the archive

  struct zip_t *zip = zip_open (fname.c_str(), 0, 'r');

  if (! zip)
     return lines;

  int n = zip_entries_total (zip);
  for (int i = 0; i < n; ++i)
      {
       zip_entry_openbyindex (zip, i);

       const char *name = zip_entry_name (zip);

       std::string tname = name;
       if (ends_with (tname, "fb2"))
          {
           source_fname = tname;
           zip_entry_close (zip);
           break;
          }

       //int isdir = zip_entry_isdir(zip);
       //unsigned long long size = zip_entry_size(zip);
       //unsigned int crc32 = zip_entry_crc32(zip);
       zip_entry_close(zip);
      }


  if (source_fname.empty())
      return lines;

  void *buf = NULL;
  size_t bufsize;

  if (zip_entry_open (zip, source_fname.c_str()) < 0)
      return lines;

  zip_entry_read (zip, &buf, &bufsize);
  zip_entry_close (zip);

  if (bufsize == 0)
      return lines;

  lines = extract_text_from_xml_pugi ((char*)buf, bufsize, tags);

  zip_close (zip);
  free (buf);

  return lines;
}


bool CFIOEPUB::understand (const std::string &fname)
{
  if (ends_with (fname, ".epub"))
     return true;

  return false;
}


std::vector <std::string> CFIOEPUB::load (const std::string &fname)
{
  std::vector <std::string> tags;
  std::vector <std::string> lines; //lines from all html contained at epub

  struct zip_t *zip = zip_open (fname.c_str(), 0, 'r');

  if (! zip)
     return lines;

  //read toc.ncx

  void *contenttoc = NULL;
  size_t bufsize;

  std::string subdir = "OEBPS/";

  if (zip_entry_open (zip, "OEBPS/toc.ncx") < 0)
     {
      if (zip_entry_open (zip, "OPS/toc.ncx") < 0)
          return lines;

      subdir = "OPS/";
     }

  zip_entry_read (zip, &contenttoc, &bufsize);
  zip_entry_close (zip);

  if (bufsize == 0)
    return lines;

 // done with toc.ncx

  std::string content ((char*)contenttoc);
  free (contenttoc);

  std::vector <std::string> urls = extract_src_from_toc (content, subdir);

  //HERE WE ALREADY PARSED URLS

  if (urls.size() == 0)
      return lines;

  tags.push_back ("p");

  //read urls from epub

  for (size_t i = 0; i < urls.size(); i++)
      {
       if (i + 1 != urls.size())
           if (urls[i] == urls[i+1])
              continue;

       void *temp = NULL;

       if (zip_entry_open (zip, urls[i].c_str()) >= 0)
          {
         //  std::cout << "open: " << urls[i] << std::endl;

           zip_entry_read (zip, &temp, &bufsize);
           zip_entry_close (zip);

           if (bufsize == 0)
              continue;

  //         std::cout << "bufsize: " << bufsize << std::endl;
           //std::cout << (char*) temp << std::endl;

          // std::string st = (char*) temp;
            
            std::string st; 
           
           if (temp)
              st = (char*) temp;
            
            
         //  std::string st_cleaned = html_strip (st);
           
            std::string st_cleaned = xml_strip_remove_empty_lines (st);
           

//           std::cout << st_cleaned << std::endl;

           std::vector <std::string> file_lines = split_string_to_vector (st_cleaned, "\n", false);

           //print_lines (file_lines);

           if (file_lines.size() > 0)
              lines.insert(std::end(lines), std::begin(file_lines), std::end(file_lines));

           free (temp);
          }
       }

  zip_close (zip);

 // print_lines (lines);

  return lines;
}

