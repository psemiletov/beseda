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
}

void CFileList::update_for_directory (const std::string &path)
{
  files = files_get_list (path, exts);

}

void CFileList::update_for_file (const std::string &path)
{
  std::string dir = get_file_path (path);
  files = files_get_list (dir, exts);
}


int CFileList::get_list_index_from_fname (const std::string &path)
{
  if (files.size() == 0)
     return -1;

  int result = -1;
  for (int i = 0; i < files.size(); i++)
      if (files[i] == path)
         return i;

  return -1;

}
