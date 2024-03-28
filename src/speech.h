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


#define SPCH_STATE_STOPPED 0
#define SPCH_STATE_SAYING 1
#define SPCH_STATE_PAUSED 2
#define SPCH_STATE_NEXT 3


class CSpeech
{
  public:

  SPDConnection *spd_connection;

  bool initialized;

  CSpeech();
  ~CSpeech();



  //bool paused;

  void init (const char* client_name);

  void say (const char* text);

  void stop();
  void pause();
  void play();
  void resume();
  void cancel();


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
