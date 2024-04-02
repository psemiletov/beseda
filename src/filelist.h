#ifndef FILELIST_H
#define FILELIST_H


#include <string>
#include <vector>


class CFileList
{
public:

  int current_index;

  std::vector <std::string> exts;
  std::vector <std::string> files;

  CFileList();

  void update_for_directory (const std::string &path);
  void update_for_file (const std::string &path);
  int get_list_index_from_fname (const std::string &path);

  void left();
  void right();


};




#endif
