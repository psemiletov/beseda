/***********************************************************
 *   this code by Peter Semiletov is Public Domain         *
 **********************************************************/


#include <sys/stat.h>
#include <algorithm>

#include <cstring>

#include <uchar.h>
/*
#ifndef USE_OPENSUSE
#include <filesystem>
#else
#include <experimental/filesystem>
#endif
*/

/*
#ifndef __has_include
  static_assert(false, "__has_include not supported");
#else
#  if __cplusplus >= 201703L && __has_include(<filesystem>)
#    include <filesystem>
     namespace fs = std::filesystem;
#  elif __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
     namespace fs = std::experimental::filesystem;
#  endif
#endif
*/


#include <cstdint>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdio.h>  // for FILENAME_MAX
#include <limits>
#include <unistd.h>


#if !defined(_WIN32) || !defined(_WIN64)

#include <unistd.h>
#include <dirent.h>
#include <sys/statvfs.h>

#define DIR_SEPARATOR '/'

#else

#include <Windows.h>
#include <shlobj.h>

#define GetCurrentDir _getcwd
#define DIR_SEPARATOR '\\'

#endif


#include "utl.h"



using namespace std;


bool is_dir (char *path)
{
  struct stat s;
 if (stat(path,&s) == 0)

    if (s.st_mode & S_IFDIR)
        return true;

  return false;

}


std::string get_file_name (std::string const& path)
{
   size_t index_of_slash = path.find_last_of ("/");
   return path.substr (index_of_slash + 1);
}


#if !defined(_WIN32) || !defined(_WIN64)

std::vector <std::string> files_get_list (const std::string &path)
{
  std::vector <std::string> result;

  if (path.empty())
     return result;

  DIR *directory;
  struct dirent *dir_entry = NULL;

  directory = opendir (path.c_str());
  if (! directory)
     return result;

  std::string t;

  while ((dir_entry = readdir (directory)))
        {
         t = dir_entry->d_name;

         if (t != "." && t != "..")
             result.push_back (path + "/" + t);
        }

  closedir (directory);

  return result;
}


std::vector <std::string> files_get_list (const std::string &path,
                                         std::vector <std::string> exts)
{
  std::vector <std::string> result;

  if (path.empty())
     return result;

  DIR *directory;


  directory = opendir (path.c_str());
  if (! directory)
      return result;

  std::string t;// = dir_entry->d_name;

  struct dirent *dir_entry = NULL;

   while (dir_entry = readdir (directory))
         {
          // std::cout << dir_entry->d_name << std::endl;

          if (dir_entry->d_type == DT_DIR)
             continue;

          t = dir_entry->d_name;

          for (auto & ext : exts)
             {
              if (t.rfind (ext) != string::npos)
                 result.push_back (path + "/" + t);
             }

         }

   closedir (directory);
   return result;
}


std::vector <std::string> files_get_list (const std::string &path, const std::string &ext) //ext with dot: ".txt"
{
  std::vector <std::string> result;

  if (path.empty())
     return result;

  DIR *directory;


  directory = opendir (path.c_str());
  if (! directory)
      return result;

  std::string t;// = dir_entry->d_name;

  struct dirent *dir_entry = NULL;

   while (dir_entry = readdir (directory))
         {
          // std::cout << dir_entry->d_name << std::endl;
          t = dir_entry->d_name;
          if (t.rfind (ext) != string::npos)
            result.push_back (path + "/" + t);
         }

   closedir (directory);
   return result;
}


#else


std::vector <std::string> files_get_list (const std::string &path)
{

  std::vector<std::string> result;

  if (path.empty())
     return result;


    WIN32_FIND_DATAA findData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    std::string full_path = path + "\\*";

    hFind = FindFirstFileA (full_path.c_str(), &findData);

    std::string t;

    if (hFind == INVALID_HANDLE_VALUE)
        //throw std::runtime_error("Invalid handle value! Please check your path...");
       return result;

    while (FindNextFileA(hFind, &findData) != 0)
      {
       t = findData.cFileName;

       if (t != "." && t != "..")
           result.push_back (path + "/" + t);
     }

    FindClose(hFind);

    return result;
}



std::vector <std::string> files_get_list (const std::string &path, const std::string &ext) //ext with dot: ".txt"
{

  std::vector<std::string> result;

  if (path.empty())
     return result;



    WIN32_FIND_DATAA findData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    std::string full_path = path + "\\*";


    hFind = FindFirstFileA (full_path.c_str(), &findData);

    std::string t;

    if (hFind == INVALID_HANDLE_VALUE)
        //throw std::runtime_error("Invalid handle value! Please check your path...");
       return result;

    while (FindNextFileA(hFind, &findData) != 0)
      {
        t = findData.cFileName;

      if (t.rfind (ext) != string::npos)
            result.push_back (path + "/" + t);

    }

    FindClose(hFind);

    return result;
}


#endif




size_t get_file_size (const string &fname)
{
  if (fname.empty())
      return 0;

  struct stat buf;
  stat (fname.c_str(), &buf);
  return buf.st_size;
}


string get_file_ext (const string &fname)
{
  std::string::size_type i = fname.rfind('.');
  if (i != std::string::npos)
     return fname.substr (i+1);
  else
      return string();
}


string replace_file_ext (const string &fname, const string &ext)
{
  string result = fname;
  string::size_type i = fname.rfind ('.', fname.length());
  if (i != string::npos)
      result.replace (i+1, ext.length(), ext);

  return result;
}


string get_file_path (const string &path)
{
  size_t i = path.rfind (DIR_SEPARATOR, path.length());

  if (i != string::npos)
     return path.substr (0, i);

  return string();
}


string get_home_dir()
{
  string result;

#if !defined(_WIN32) || !defined(_WIN64)

  const char *homedir = getenv ("HOME");

  if (homedir != NULL)
     result = homedir;

#else

  char homeDirStr[MAX_PATH];

 if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, homeDirStr)))
   result = homeDirStr;


#endif

  return result;

}

/*
string current_path()
{
  return fs::current_path().string();
}
*/


std::string current_path()
{
 std::string result;

 char buff[FILENAME_MAX];

#if defined(_WIN32) || defined(_WIN64)

  if (_getcwd (buff, FILENAME_MAX))
     result = buff;

#else

 if (getcwd (buff, FILENAME_MAX))
    result = buff;

#endif

 return result;
}


std::string get_tmp_dir()
{
  std::string result;

#if defined(_WIN32) || defined(_WIN64)


  TCHAR szPath[MAX_PATH + 1];
  DWORD d = GetTempPath(MAX_PATH + 1, szPath);
  if (d != ERROR_SUCCESS)
     result = "/tmp";
  else
      result = szPath;

 #else

  char const *d = getenv ("TMPDIR");
  if (d)
     result = d;
  else
      result = "/tmp";

#endif

  return result;
}


/*
string current_path()
{
  char path [FILENAME_MAX];
  string result;

  if (! get_cur_dir (path, sizeof (path)))
     return result;

  result = path;
  return result;
}

*/

size_t get_free_space (const string &path)
{
  if (path.empty())
     return 0;

  size_t result = 0;

#if defined(_WIN32) || defined(_WIN64)


     BOOL  fResult;
      unsigned __int64 i64FreeBytesToCaller,
                       i64TotalBytes,
                       i64FreeBytes;
         fResult = GetDiskFreeSpaceEx (path.c_str(),
                                 (PULARGE_INTEGER)&i64FreeBytesToCaller,
                                 (PULARGE_INTEGER)&i64TotalBytes,
                                 (PULARGE_INTEGER)&i64FreeBytes);
         if (fResult)
         {
            result = i64FreeBytesToCaller;
            /*printf ("\n\nGetDiskFreeSpaceEx reports\n\n");
            printf ("Available space to caller = %I64u MB\n",
                    i64FreeBytesToCaller / (1024*1024));
            printf ("Total space               = %I64u MB\n",
                    i64TotalBytes / (1024*1024));
            printf ("Free space on drive       = %I64u MB\n",
                    i64FreeBytes / (1024*1024));*/
         }

#else

  struct statvfs buf;

  int r = statvfs (path.c_str(), &buf);

  if (r < 0)
     result = 0;
  else
  result = buf.f_bavail * buf.f_bsize;

#endif

  return result;

}


/*
size_t get_free_space (const string &path)
{
  if (path.empty())
     return 0;

  fs::path p (path);
  const fs::space_info i = fs::space (p);
  return i.available;
}
*/

bool file_exists (const string &name)
{
  if (name.empty())
     return false;

  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}


bool is_program_exists (const string &appname)
{
  if (appname.empty())
     return false;

  string cm = "which " + appname + " > /dev/null 2>&1";
  if (system (cm.c_str()))
     return false;
  else
      return true;
}


bool is_path_abs (const string &path)
{
  if (path.empty())
     return false;

  if (path[0] == '/')
     return true;

  if (path.size() > 1)
     if (path[1] == ':') //windows
        return true;

  return false;
}


string str_replace (string &source, const string &text_to_find, const string &replace_with)
{
  if (source.empty())
      return source;

  if (text_to_find.empty())
      return source;

  if (replace_with.empty())
      return source;

  size_t pos = source.find (text_to_find);

  if (pos == string::npos)
     return source;

  source.replace (pos, text_to_find.size(), replace_with);

  return source;
}


string string_replace_all (const string &s, const string &from, const string &to)
{
  string result = s;
  size_t i = 0;
  do
    {
     i = result.find (from);
     if (i != string::npos)
         result.replace (i, from.length(), to);
    }
  while (i != string::npos);

  return result;
}


vector <string> split_string_to_vector (const string& s, const string& delimeter, const bool keep_empty)
{
  vector <string> result;

  if (delimeter.empty())
     {
      result.push_back (s);
      return result;
     }

  string::const_iterator substart = s.begin(), subend;

  while (true)
        {
         subend = search (substart, s.end(), delimeter.begin(), delimeter.end());

         string temp (substart, subend);

         if (keep_empty || ! temp.empty())
             result.push_back (temp);

         if (subend == s.end())
             break;

         substart = subend + delimeter.size();
        }

  return result;
}


string string_to_lower (const string &s)
{
  string result = s;

  std::for_each (
                 result.begin(),
                 result.end(),
                 [](char & c) {
                               c = ::tolower(c);
                              });

  return result;
}


size_t string_to_file_size (const string &val)
{
  size_t result = 0;

  char* end;

  const char *st = val.c_str();

  if (st)
     result = strtoull (st, &end, 10);

  string s = string_to_lower (val);

  if (s.find ("k") != string::npos)
     result = result * 1024;

  if (s.find ("m") != string::npos)
     result = result * 1048576;

  if (s.find ("g") != string::npos)
     result = result * 1073741824;

  return result;
}


string bytes_to_file_size (size_t val)
{
  if (val >= 1073741824)
     return std::to_string (val / 1073741824) + " gbytes";

  if (val >= 1048576)
     return std::to_string (val / 1048576) + " mbytes";

  if (val >= 1024)
     return std::to_string (val / 1024) + " kbytes";

  return std::to_string (val) + " bytes";;
}


string bytes_to_file_size3 (size_t val)
{
  if (val >= 1073741824)
     return format3 (std::to_string (val / 1073741824)) + " gbytes";

  if (val >= 1048576)
     return format3 (std::to_string (val / 1048576)) + " mbytes";

  if (val >= 1024)
     return format3 (std::to_string (val / 1024)) + " kbytes";

  return format3 (std::to_string (val)) + " bytes";;
}


std::string string_file_load (const std::string &fname)
{
 if (fname.empty())
    return std::string();

 std::ifstream t (fname.c_str());
 std::string s ((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());

 return s;
}


std::vector <std::string> vector_file_load (const std::string &fname)
{
  std::vector <std::string> v;

  if (fname.empty())
     return v;


  ifstream infile (fname.c_str());

  if (infile.fail())
     {
    //  cout << "CPairFile::CPairFile - Could not open file: " << fname << endl;
      return v;
     }

  std::string line;

  while (getline (infile, line))
        {
         v.push_back (line);
        }

   infile.close();
   return v;
}


std::string get_macro_name (const std::string &value)
{
 size_t pos = value.find_first_of (':');
 if (pos == string::npos)
     pos = value.size();

  return value.substr (0, pos);
}


void string_save_to_file (const string &fname, const string &s)
{
  std::ofstream file (fname);
  if (file.is_open())
  file << s;
  file.close();
}


bool path_exists (const string &fname)
{
  if (fname.empty())
     return false;

  std::string fpath = get_file_path (fname);
  if (fpath.empty())
     return false;

  return file_exists(fpath);
}


bool ends_with (std::string const & value, std::string const & ending)
{
    if (ending.size() > value.size())
       return false;

    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}



/*
bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}
*/



std::string format3 (size_t n)
{
  std::string number_str = std::to_string(n);

  for (int i = number_str.length() - 3; i > 0; i -= 3)
      number_str.insert(i, ",");

  return number_str;
}


std::string format3 (const std::string &s)
{
  std::string number_str = s;

  for (int i = number_str.length() - 3; i > 0; i -= 3)
      number_str.insert(i, ",");

  return number_str;
}


std::string html_strip (const std::string &source)
{
  std::string html = source;

    while (html.find("<") != std::string::npos)
    {
        auto startpos = html.find("<");
        auto endpos = html.find(">") + 1;

        if (endpos != std::string::npos)
        {
            html.erase(startpos, endpos - startpos);
        }
    }

   return html;
}


std::vector <std::string> extract_hrefs (const std::string &source, const std::string &prefix)
{
 std::vector <std::string> result;


 size_t i = 0;
 size_t limit = source.size() - 1;
 std::string signature_str = "<item href=\"";
 size_t signature_size = signature_str.size();

 while (i < limit)
       {
        size_t pos_start = source.find (signature_str, i);
        if (pos_start == string::npos)
           break;

        size_t pos_end = source.find ("\"", pos_start + signature_size);
        if (pos_end == string::npos)
           break;

        //else

        std::string url = source.substr (pos_start + signature_size, pos_end - (pos_start + signature_size));

        if (ends_with (url, "html") || ends_with (url, "xhtml"))
           if (url.rfind("wrap", 0) != 0)
              {
               result.push_back (prefix + url);
               std::cout << "url: " << url << std::endl;
              }

        i = pos_end + 1;
       }

   return result;
}



std::vector <std::string> extract_src_from_toc (const std::string &source, const std::string &prefix)
{
 std::vector <std::string> result;

 size_t i = 0;
 size_t limit = source.size() - 1;
 std::string signature_str = "src=\"";
 size_t signature_size = signature_str.size();

 while (i < limit)
       {
        size_t pos_start = source.find (signature_str, i);
        if (pos_start == std::string::npos)
           break;

        size_t pos_end = source.find ("\"", pos_start + signature_size);
        if (pos_end == std::string::npos)
           break;

        //else

        std::string url = source.substr (pos_start + signature_size, pos_end - (pos_start + signature_size));

        //find "#" if any
        //remove after # to the end of string

        size_t pos_part = url.find ("#");
        if (pos_end != std::string::npos)
            url = url.substr (0, pos_part);

        if (ends_with (url, "html") || ends_with (url, "xhtml"))
           {
            std::string url_to_add = prefix + url;
            result.push_back (url_to_add);
           }

        i = pos_end + 1;
       }

   return result;
}


void print_lines (std::vector <std::string> lines)
{
  for (auto s: lines)
    std::cout << s << '\n';

}

/*
//ok
char16_t cp1251_to_utf16[256] = {
    // 0x00 - 0x7F: Соответствуют символам ASCII
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
    0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,
    0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017,
    0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F,
    0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
    0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
    0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
    0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
    0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
    0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F,
    0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
    0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
    0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F,
    // 0x80 - 0xFF: Символы CP1251
    0x0402, 0x0403, 0x201A, 0x0453, 0x201E, 0x2026, 0x2020, 0x2021,
    0x20AC, 0x2030, 0x0409, 0x2039, 0x040A, 0x040C, 0x040B, 0x040F,
    0x0452, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
    0x0020, 0x2122, 0x0459, 0x203A, 0x045A, 0x045C, 0x045B, 0x045F,
    0x00A0, 0x040E, 0x045E, 0x0408, 0x00A4, 0x0490, 0x00A6, 0x00A7,
    0x0401, 0x00A9, 0x0404, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x0407,
    0x00B0, 0x00B1, 0x0406, 0x0456, 0x0491, 0x00B5, 0x00B6, 0x00B7,
    0x0451, 0x2116, 0x0454, 0x00BB, 0x0458, 0x0405, 0x0455, 0x0457,
    0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
    0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
    0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
    0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
    0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
    0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
    0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
    0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F
};

*/

/*
#include <string>
#include <cstdint>

char16_t cp1251_to_utf16[256] = {
    // 0x00 - 0x7F: Соответствуют символам ASCII
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
    0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,
    0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017,
    0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F,
    0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
    0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
    0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
    0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
    0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
    0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F,
    0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
    0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
    0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F,
    // 0x80 - 0xFF: Символы CP1251
    0x0402, 0x0403, 0x201A, 0x0453, 0x201E, 0x2026, 0x2020, 0x2021,
    0x20AC, 0x2030, 0x0409, 0x2039, 0x040A, 0x040C, 0x040B, 0x040F,
    0x0452, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
    0x0020, 0x2122, 0x0459, 0x203A, 0x045A, 0x045C, 0x045B, 0x045F,
    0x00A0, 0x040E, 0x045E, 0x0408, 0x00A4, 0x0490, 0x00A6, 0x00A7,
    0x0401, 0x00A9, 0x0404, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x0407,
    0x00B0, 0x00B1, 0x0406, 0x0456, 0x0491, 0x00B5, 0x00B6, 0x00B7,
    0x0451, 0x2116, 0x0454, 0x00BB, 0x0458, 0x0405, 0x0455, 0x0457,
    0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
    0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
    0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
    0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
    0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
    0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
    0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
    0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F
};

std::string convert_cp1251_to_utf8(const char* text);
{

    std::string utf8_text;

    // Iterate through each character in the CP1251 text
    for (const char* cp1251_char = text; *cp1251_char != '\0'; ++cp1251_char) {
        unsigned char cp1251_code = static_cast<unsigned char>(*cp1251_char);

        // Check if the character falls within the CP1251 range
        if (cp1251_code < 0x80) {
            // For ASCII characters, simply append them to the UTF-8 string
            utf8_text += *cp1251_char;
        } else {
            // For CP1251 characters, look up their UTF-16 representation in the array
            char16_t utf16_code = cp1251_to_utf16[cp1251_code - 0x80];

            // Encode the UTF-16 character as UTF-8
            if (utf16_code < 0x80) {
                utf8_text += static_cast<char>(utf16_code);
            } else if (utf16_code < 0x800) {
                utf8_text += static_cast<char>((utf16_code >> 6) | 0xC0);
                utf8_text += static_cast<char>((utf16_code & 0x3F) | 0x80);
            } else {
                utf8_text += static_cast<char>((utf16_code >> 12) | 0xE0);
                utf8_text += static_cast<char>(((utf16_code >> 6) & 0x3F) | 0x80);
                utf8_text += static_cast<char>((utf16_code & 0x3F) | 0x80);
            }
        }
    }

    return utf8_text;
}
*/

std::string string_between(const std::string &source,
                           const std::string &sep1,
                           const std::string &sep2)
{
    std::string result;
    size_t pos1 = source.find(sep1);
    if (pos1 == std::string::npos)
        return result;

    size_t pos2 = source.find(sep2, pos1 + sep1.size());
    if (pos2 == std::string::npos)
        return result;

    pos1 += sep1.size();

    result = source.substr(pos1, pos2 - pos1);
    return result;
}

/*
 *
 #include <iconv.h>
#include <string>

std::string cp1251_to_utf8(char* str) {
    std::string outstr;
    if (str == nullptr) return outstr;

    iconv_t conv = iconv_open("UTF-8", "CP1251");
    if (conv == (iconv_t)-1) {
        perror("iconv_open");
        return outstr;
    }

    size_t inbytesleft = strlen(str);
    size_t outbytesleft = inbytesleft * 2;
    outstr.resize(outbytesleft);

    char* inptr = str;
    char* outptr = &outstr[0];

    if (iconv(conv, &inptr, &inbytesleft, &outptr, &outbytesleft) == (size_t)-1) {
        perror("iconv");
        outstr.clear();
    } else {
        outstr.resize(outstr.size() - outbytesleft);
    }

    iconv_close(conv);
    return outstr;
}

Обратите внимание, что для работы этого кода вам потребуется библиотека iconv. Если вы используете Linux, установите ее с помощью команды sudo apt-get install libiconv-hook-dev. Если вы используете Windows, вам потребуется скачать и установить библиотеку libiconv отдельно. Пожалуйста, убедитесь, что вы правильно подключили эту библиотеку в ваш проект.



*/


//ok
char16_t cp1251_to_utf16[256] = {
    // 0x00 - 0x7F: Соответствуют символам ASCII
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
    0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,
    0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017,
    0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F,
    0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
    0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
    0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
    0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
    0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
    0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F,
    0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
    0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
    0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F,
    // 0x80 - 0xFF: Символы CP1251
    0x0402, 0x0403, 0x201A, 0x0453, 0x201E, 0x2026, 0x2020, 0x2021,
    0x20AC, 0x2030, 0x0409, 0x2039, 0x040A, 0x040C, 0x040B, 0x040F,
    0x0452, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
    0x0020, 0x2122, 0x0459, 0x203A, 0x045A, 0x045C, 0x045B, 0x045F,
    0x00A0, 0x040E, 0x045E, 0x0408, 0x00A4, 0x0490, 0x00A6, 0x00A7,
    0x0401, 0x00A9, 0x0404, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x0407,
    0x00B0, 0x00B1, 0x0406, 0x0456, 0x0491, 0x00B5, 0x00B6, 0x00B7,
    0x0451, 0x2116, 0x0454, 0x00BB, 0x0458, 0x0405, 0x0455, 0x0457,
    0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
    0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
    0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
    0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
    0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
    0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
    0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
    0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F
};


//ok
char16_t* ConvertFromCP1251ToUTF16(const char* cp1251Text)
{
  size_t len = strlen (cp1251Text);
//  UTF16TEXT* utf16Text = new UTF16TEXT[2 * len + 1]; // Максимальная длина UTF-16 символа - 2 байта, учитываем нулевой символ
  char16_t* utf16Text = new char16_t[len + 1]; // Максимальная длина UTF-16 символа - 2 байта, учитываем нулевой символ

  char16_t* p = utf16Text;

  while (*cp1251Text)
        {
         unsigned char c = *cp1251Text++;
         *p++ = cp1251_to_utf16[c]; // Преобразование всех символов
        }

//  *p = u'\0';
  *p = '\0';

  return utf16Text;
}



std::string ConvertUTF16ToUTF8(const char16_t* utf16_text)
{
    std::string utf8_text;

    // Итерируемся по массиву UTF-16 до тех пор, пока не достигнем нулевого символа
    for (int i = 0; utf16_text[i] != u'\0'; ++i) {
        char16_t unicode_value = utf16_text[i];

        // Если символ меньше или равен 0x7F, он в кодировке ASCII и представляется в UTF-8 одним байтом
        if (unicode_value <= 0x7F) {
            utf8_text.push_back(static_cast<char>(unicode_value));
        } else if (unicode_value <= 0x7FF) {
            // Если символ в диапазоне 0x80 - 0x7FF, он представляется в UTF-8 двумя байтами
            utf8_text.push_back(static_cast<char>(0xC0 | (unicode_value >> 6))); // Первый байт
            utf8_text.push_back(static_cast<char>(0x80 | (unicode_value & 0x3F))); // Второй байт
        } else {
            // В противном случае символ представляется в UTF-8 тремя байтами
            utf8_text.push_back(static_cast<char>(0xE0 | (unicode_value >> 12))); // Первый байт
            utf8_text.push_back(static_cast<char>(0x80 | ((unicode_value >> 6) & 0x3F))); // Второй байт
            utf8_text.push_back(static_cast<char>(0x80 | (unicode_value & 0x3F))); // Третий байт
        }
    }

    return utf8_text;
}
