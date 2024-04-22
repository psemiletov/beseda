/***********************************************************
 *   this code by Peter Semiletov is Public Domain         *
 **********************************************************/

#ifndef UTL_H
#define UTL_H

#include <string>
#include <vector>

//using namespace std;

//File utils

bool is_dir (char *path);
std::string get_file_name (std::string const& path);

size_t get_file_size (const std::string &fname);
std::string get_file_ext (const std::string &fname);
std::string replace_file_ext (const std::string &fname, const std::string &ext);
std::string get_file_path (const std::string &path);
std::string get_home_dir();
std::string current_path();
size_t get_free_space (const std::string &path);
bool file_exists (const std::string &name);
bool is_program_exists (const std::string &appname);
bool is_path_abs (const std::string &path);
bool path_exists (const std::string &fname);
std::string get_tmp_dir();

std::vector <std::string> files_get_list (const std::string &path);
std::vector <std::string> files_get_list (const std::string &path, const std::string &ext); //ext with dot: ".txt"
std::vector <std::string> files_get_list (const std::string &path,
                                         std::vector <std::string> exts);



//String utis

std::string bytes_to_file_size3 (size_t val);
std::string bytes_to_file_size (size_t val);
std::string str_replace (std::string &source, const std::string &text_to_find, const std::string &replace_with);
std::string string_replace_all (const std::string &s, const std::string &from, const std::string &to);
std::vector <std::string> split_string_to_vector (const std::string& s, const std::string& delimeter, const bool keep_empty = true);
std::string string_to_lower (const std::string &s);
size_t string_to_file_size (const std::string &val);
std::string string_file_load (const std::string &fname);
std::vector <std::string> vector_file_load (const std::string &fname);
std::string get_macro_name (const std::string &value);
void string_save_to_file (const std::string &fname, const std::string &s);
//bool hasEnding (std::string const &fullString, std::string const &ending);
bool ends_with (std::string const & value, std::string const & ending);
std::string format3 (size_t n);
std::string format3 (const std::string &s);
std::string html_strip (const std::string &source);
std::vector <std::string> extract_hrefs (const std::string &source, const std::string &prefix);
std::vector <std::string> extract_src_from_toc (const std::string &source, const std::string &prefix);
void print_lines (std::vector <std::string> lines);


#endif
