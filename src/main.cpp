#include <iostream>
#include <string>

#include <signal.h>

#include <ncurses.h>
#include <locale.h>

#include "speech.h"
#include "textbuffer.h"

#define BOOK1 "/home/rox/devel/test-books/Dracula by Bram Stoker.txt"
#define BOOK2 "/home/rox/devel/test-books/frankenstein.txt"
#define BOOK3 "/home/rox/devel/test-books/Pride and Prejudice by Jane Austen.txt"


using namespace std;



//

/*

  std::signal (SIGINT, f_signal_handler);


 while (g_signal != SIGINT)

 */


extern int g_position;

//extern bool saying;

extern int g_state;

int saved_pos;

int main (int argc, char *argv[])
{
  // Initialise ncurses

  g_position = 0;




  std::cout << setlocale(LC_ALL, NULL);

  initscr();

    //addstr("How are you?");



// Give NCurses control of input
  keypad(stdscr, TRUE);
  cbreak();
  noecho();

  CSpeech sp;
  sp.init ("beseda");


  signal (SIGINT, f_signal_handler);


  CTextBuffer text_buffer;
  text_buffer.load (BOOK1);

  g_state = SPCH_STATE_SAYING;

  bool running = true;

  halfdelay(1);

  int ch;

  saved_pos = -1;

  /* Event loop */
  while (running)
        {

         if (g_signal == SIGINT)
           {
            running = false;
            break;
           }

         addstr("|");

         std::string str_counter =  std::to_string(g_position);
         addstr(str_counter.c_str());

         //flushinp();

         ch = getch();

           //     refresh();
         if (ch == 'q')
            {
             running = false;
             break;
            }

            /*
         if (ch == 'n')
            {
             sp.cancel();
             continue;
            }


         if (ch == KEY_HOME)
            {
             sp.cancel();
             g_position = 0;
            }

         if (ch == KEY_END)
            {
             sp.cancel();
             g_position = text_buffer.lines.size() - 2;
            }


         if (ch == KEY_UP)
            {
             sp.cancel();
             if (g_position > 1)
                g_position -= 2;
            }

         if (ch == KEY_DOWN)
            {
             sp.cancel();

             if (g_position < text_buffer.lines.size() - 2)
                 g_position++;

             g_state = SPCH_STATE_SAYING;

            }


        if (ch == 'c')//stop
            {
             addstr ("CANCEL");
             sp.cancel();
             continue;
            }


         if (ch == 's')//stop
            {
             addstr ("STOP");
             sp.stop();
             continue;
            }

           */

         if (ch == ' ') //space
            {
             if (g_state != SPCH_STATE_PAUSED)
                {
                 addstr("PAUSE");
                 sp.pause();
                 continue;
                }
             else
                {
                 addstr("RESUME");
                 sp.resume();
                 continue;
                }
             }


           //EOF - ВЫХОДИМ ЛИ?
          if (g_position > text_buffer.lines.size())
             {
              running = false;
              //state = SPCH_STATE_STOPPED;
              sp.stop();
              break;
             }

          if (g_state == SPCH_STATE_SAYING && saved_pos != g_position)
              {
               sp.say (text_buffer.lines[g_position].c_str());
               saved_pos = g_position;
              }


/*
           if (g_signal == SIGINT)
             {
              running = false;
              break;

            }
*/

  }


  endwin();

  sp.stop();

  return 0;
}
 
