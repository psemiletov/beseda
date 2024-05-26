/***********************************************************
 *   this code by Peter Semiletov is Public Domain         *
 **********************************************************/

#ifndef BOOKMARKS_H
#define BOOKMARKS_H

#include <string>
#include <vector>

#include "pairfile.h"


class CBookmarks
{
public:

   CPairFile pf;
   int current_index;
   void load (const std::string &filename);
};

#endif
