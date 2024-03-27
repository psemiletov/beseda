#ifndef SPEECH_H
#define SPEECH_H

/*

 usr/include/speech-dispatcher/libspeechd.h
usr/include/speech-dispatcher/libspeechd_version.h
usr/include/speech-dispatcher/spd_audio_plugin.h
usr/include/speech-dispatcher/speechd_defines.h
usr/include/speech-dispatcher/speechd_types.h


 */


#include <libspeechd.h>
#include <semaphore.h>
#include <signal.h>

class CSpeech
{
  public:

  SPDConnection *spd_connection;

  bool initialized;

  CSpeech();
  ~CSpeech();

  void init (const char* client_name);

  void say (const char* text);


};


//sig_atomic_t g_signal;

//

namespace
{
//  volatile std::sig_atomic_t g_signal;
    volatile sig_atomic_t g_signal;

//  volatile int sockfd;
}


void f_signal_handler (int signal);

#endif
