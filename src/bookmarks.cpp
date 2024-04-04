/***********************************************************
 *   this code by Peter Semiletov is Public Domain         *
 **********************************************************/

#include "bookmarks.h"
#include "pairfile.h"


void CBookmarks::load (const std::string &filename)
{
  current_index = 0;
  pf.load (filename);
}


