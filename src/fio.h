/*
written at 2024 by Peter Semiletov
this code is the public domain
*/

#ifndef FIO_H
#define FIO_H

#include <string>
#include <vector>

class CFIO
{
public:

  virtual std::vector <std::string> load (const std::string &fname) = 0;
  virtual bool understand (const std::string &fname) = 0;
  virtual ~CFIO(){};

};


class CFIOPlainText: public CFIO
{
public:

  std::vector <std::string> load (const std::string &fname);
  bool understand (const std::string &fname);
};



class CFIOABW: public CFIO
{
public:

  std::vector <std::string> load (const std::string &fname);
  bool understand (const std::string &fname);
};


class CFIOHTML: public CFIO
{
public:

  std::vector <std::string> load (const std::string &fname);
  bool understand (const std::string &fname);
};


class CFIOXMLZipped: public CFIO
{
public:

  std::vector <std::string> load (const std::string &fname);
  bool understand (const std::string &fname);
};


class CFIOFB2: public CFIO
{
public:

  std::vector <std::string> load (const std::string &fname);
  bool understand (const std::string &fname);
};


class CFIOEPUB: public CFIO
{
public:

  std::vector <std::string> load (const std::string &fname);
  bool understand (const std::string &fname);
};


class CFIOList
{
public:

  std::vector <CFIO*> loaders;

  CFIO* get_loader_for_file (const std::string &fname);

  CFIOList();
  ~CFIOList();
};

#endif

