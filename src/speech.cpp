#include "speech.h"

#include <iostream>
#include <ncurses.h>


//sem_t g_semaphore;


//bool saying;


int g_state;
int g_position;



void f_signal_handler (int signal)
{
  std::cout <<  "f_signal_handler " << std::endl;

  g_signal = signal;

//  shutdown (sockfd, 2);
  //close (sockfd);

  std::cout << "Exiting by the signal" << std::endl;
}


/* Callback for Speech Dispatcher notifications */
void cbk_end_of_speech (size_t msg_id, size_t client_id, SPDNotificationType type)
{
   /* We don't check msg_id here since we will only send one
       message. */

   /* Callbacks are running in a separate thread, so let the
       (sleeping) main thread know about the event and wake it up. */

 // addstr("*cbk_end_of_speech*");

  if (type == SPD_EVENT_END)
      {
       addstr("*SPD_EVENT_END*");
       //g_state = SPCH_STATE_SAYING;
       g_position++;
      }
/*
  if (type == SPD_EVENT_CANCEL)
     {
      addstr("*SPD_CANCEL*");
     }
*/
  addstr(std::to_string (type).c_str());



//  sem_post(&g_semaphore);

}


void cbk_cancel_of_speech (size_t msg_id, size_t client_id, SPDNotificationType type)
{
//  addstr("*cbk_cancel_of_speech*");



  if (type == SPD_EVENT_CANCEL)
     {
      addstr("*SPD_EVENT_CANCEL*");
     }

   /* We don't check msg_id here since we will only send one
       message. */

   /* Callbacks are running in a separate thread, so let the
       (sleeping) main thread know about the event and wake it up. */

  //saying = false;
  //sem_post(&g_semaphore);

 //   std::cout << "say ok" << std::endl;

}


CSpeech::CSpeech()
{
  initialized = false;
  spd_connection = 0;

  g_state = SPCH_STATE_STOPPED;

  //sem_init (&g_semaphore, 0, 0);

}


CSpeech::~CSpeech()
{
  if (initialized)
     if (spd_connection)
        {
         spd_close (spd_connection);
         //sem_close (&g_semaphore);
         //sem_destroy (&g_semaphore);
        }

}


void CSpeech::init (const char* client_name)
{
  spd_connection = spd_open (client_name,
                             "main",
                             NULL, //username
                             SPD_MODE_THREADED);

  if (spd_connection)
     {
      initialized = true;

      /* Set callback handler for 'end' and 'cancel' events. */
      spd_connection->callback_end = cbk_end_of_speech;
     // spd_connection->callback_cancel = cbk_cancel_of_speech;


         /* Ask Speech Dispatcher to notify us about these events. */
      spd_set_notification_on(spd_connection, SPD_END);
//      spd_set_notification_on(spd_connection, SPD_CANCEL);
     }
}


void CSpeech::say (const char* text)
{
  if (! initialized)
      return;

  g_state = SPCH_STATE_SAYING;


  int result = spd_say (spd_connection, SPD_TEXT, text);

  if (result == -1)
     std::cout << "say error!" << std::endl;

   //А ЕСЛИ НЕ ЖДАТЬ?
  //sem_wait (&g_semaphore);
}


void CSpeech::stop()
{
  if (! initialized)
      return;

   spd_stop (spd_connection);
   g_position = 0;
   g_state = SPCH_STATE_STOPPED;

}


void CSpeech::pause()
{
  if (! initialized)
     return;

  spd_pause (spd_connection);
  g_state = SPCH_STATE_PAUSED;
}


void CSpeech::play()
{
   if (! initialized)
      return;

  g_state = SPCH_STATE_SAYING;

//  paused = false;
//  saying = true;
  g_position = 0;
}



void CSpeech::resume()
{
  if (! initialized)
     return;

   g_state = SPCH_STATE_SAYING;
   spd_resume (spd_connection);
}



void CSpeech::cancel()
{
  if (! initialized)
      return;

  spd_cancel (spd_connection);
  g_state = SPCH_STATE_STOPPED;

}
