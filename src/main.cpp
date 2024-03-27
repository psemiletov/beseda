#include <iostream>
#include <string>

#include <signal.h>

#include "speech.h"
#include "textbuffer.h"

#define BOOK1 "/home/rox/devel/test-books/Dracula by Bram Stoker.txt"
#define BOOK2 "/home/rox/devel/test-books/frankenstein.txt"
#define BOOK3 "/home/rox/devel/test-books/Pride and Prejudice by Jane Austen.txt"


using namespace std;



/*

  std::signal (SIGINT, f_signal_handler);


 while (g_signal != SIGINT)

 */


int main (int argc, char *argv[])
{
  CSpeech sp;
  sp.init ("beseda");


  signal (SIGINT, f_signal_handler);

  sp.say ("hello");


  cout << "type:";
  
  std::string s;

  cin >> s;


  sp.say (s.c_str());

  return 0;
}
 
