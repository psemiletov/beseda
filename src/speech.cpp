#include "speech.h"

#include <iostream>

CSpeech::CSpeech()
{
  initialized = false;
  spd_connection = 0;

}


CSpeech::~CSpeech()
{
  if (initialized)
     if (spd_connection)
         spd_close (spd_connection);

}


void CSpeech::init (const char* client_name)
{
   spd_connection = spd_open (client_name,
                              "main",
                              NULL, //username
                              SPD_MODE_THREADED);


   if (spd_connection)
      initialized = true;

}


void CSpeech::say (const char* text)
{
   int result = spd_say (spd_connection, SPD_TEXT, text);

   if (result == -1)
      std::cout << "say error!" << std::endl;


}