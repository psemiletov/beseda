/***********************************************************
 *   this code by Peter Semiletov is Public Domain         *
 **********************************************************/

#include <algorithm>

#include "filelist.h"
#include "utl.h"



CFileList::CFileList()
{
  exts.push_back ("txt");
  exts.push_back ("odt");
  exts.push_back ("docx");
  exts.push_back ("abw");
  exts.push_back ("fb2");
  exts.push_back ("fbz");
  exts.push_back ("fb2.zip");
  exts.push_back ("html");
  exts.push_back ("xhtml");
  exts.push_back ("htm");
  exts.push_back ("epub");
  exts.push_back ("kwd");

  current_index = 0;
  no_more_files = true;
}


void CFileList::update_for_directory (const std::string &path)
{
  if (path.empty())
      return;

  files.clear();

  files = files_get_list (path, exts);

  if (files.size() > 1)
     {
      sort (files.begin(), files.end());
      no_more_files = false;
     } 
}


void CFileList::update_for_file (const std::string &path)
{
  if (path.empty())
      return;

  files.clear();

  std::string dir = get_file_path (path);
  files = files_get_list (dir, exts);

  if (files.size() > 1)
     {
      sort (files.begin(), files.end());
      no_more_files = false;
     } 
}


int CFileList::get_list_index_from_fname (const std::string &path)
{
  if (path.empty() || no_more_files)
      return 0;

  for (size_t i = 0; i < files.size(); i++)
      if (files[i] == path)
         return (int)i;

  return 0;
}


void CFileList::left()
{
  if (no_more_files || current_index == 0)
     return;
 
  current_index--;
}


void CFileList::right()
{
  if (no_more_files || current_index == files.size() -1)
     return;

  current_index++;
}
