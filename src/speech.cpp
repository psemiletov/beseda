#include "speech.h"

#include <iostream>


sem_t semaphore;

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
void cbk_end_of_speech(size_t msg_id, size_t client_id, SPDNotificationType type)
{
   /* We don't check msg_id here since we will only send one
       message. */

   /* Callbacks are running in a separate thread, so let the
       (sleeping) main thread know about the event and wake it up. */


   sem_post(&semaphore);

   g_position++;
//   std::cout << "say ok" << std::endl;

}


CSpeech::CSpeech()
{
  initialized = false;
  spd_connection = 0;

  sem_init (&semaphore, 0, 0);

}


CSpeech::~CSpeech()
{
  if (initialized)
     if (spd_connection)
       {
        spd_close (spd_connection);
        sem_close (&semaphore);
        sem_destroy (&semaphore);

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
      spd_connection->callback_end = cbk_end_of_speech;


         /* Ask Speech Dispatcher to notify us about these events. */
     spd_set_notification_on(spd_connection, SPD_END);
     spd_set_notification_on(spd_connection, SPD_CANCEL);

     }

}


void CSpeech::say (const char* text)
{
   if (! initialized)
      return;

   int result = spd_say (spd_connection, SPD_TEXT, text);

   if (result == -1)
      std::cout << "say error!" << std::endl;

   sem_wait(&semaphore);


}