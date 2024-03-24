#include <iostream>
#include <string>

#include "speech.h"


using namespace std;

int main (int argc, char *argv[])
{
  CSpeech sp;
  sp.init ("beseda");

  sp.say ("hello");


  cout << "type:";
  
  std::string s;

  cin >> s;


  sp.say (s.c_str());

  return 0;
}
 
