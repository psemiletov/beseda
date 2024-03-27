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


int main (int argc, char *argv[])
{
  // Initialise ncurses

  g_position = 0;




  std::cout << setlocale(LC_ALL, NULL);

  initscr();

    //addstr("How are you?");



// Give NCurses control of input
  keypad(stdscr, TRUE);

  CSpeech sp;
  sp.init ("beseda");


  signal (SIGINT, f_signal_handler);


  CTextBuffer text_buffer;
  text_buffer.load (BOOK1);


 bool running;

   halfdelay(1);

   int ch;

  /* Event loop */
  while (running)
        {
         if (g_signal == SIGINT)
             break;

         addstr("|");

         std::string str_counter =  std::to_string(g_position);

         addstr(str_counter.c_str());

         ch = getch();

           //     refresh();

         if (ch == 'q')
             {
              running = false;
              break;
             }


         if (ch == KEY_HOME)
            {
             g_position = 0;
            }

         if (ch == KEY_END)
            {
             g_position = text_buffer.lines.size() - 2;
            }


        if (ch == KEY_UP)
            {
             if (g_position > 0)
                g_position--;
            }

        if (ch == KEY_DOWN)
            {
             if (g_position < text_buffer.lines.size() - 2)
                g_position++;
            }


         if (ch == 's')//stop
            {
             addstr("ENTER");
             sp.stop();
             continue;
            }

         if (ch == ' ') //space
            {

             sp.paused = ! sp.paused;

             if (sp.paused)
                {
                 addstr("PAUSE");
                 sp.pause();
                 continue;
                }
              else
                 {
                  addstr("UNPAUSE");
                  sp.resume();
                  continue;

                 }


             // g_position = text_buffer.lines.size() - 1;
             }


          if (g_position > text_buffer.lines.size())
             {
              running = false;
              break;

             }

          if (! sp.paused)
              sp.say (text_buffer.lines[g_position].c_str());

/*
           if (g_signal == SIGINT)
             {
              running = false;
              break;

            }
*/

  }

 // sp.say (text_buffer.lines[0].c_str());

//  sp.say ("hello");
/*
  cout << "type:";
  std::string s;
   cin >> s;


  sp.say (s.c_str());
*/

  endwin();

  sp.stop();

  return 0;
}
 
