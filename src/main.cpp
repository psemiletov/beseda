#include <iostream>
#include <string>

#include <signal.h>

#include <ncurses.h>

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

  initscr();

    addstr("How are you?");

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
  while(running) {

        addstr("|");


        ch = getch();


//     refresh();

    if (ch == 'q') {


      /* This will cause the while loop to end the */
      /* next time the while loop checks the */
      /* value of 'running' */
      running = false;
    }


   if (g_position > text_buffer.lines.size())
       running = false;

      sp.say (text_buffer.lines[g_position].c_str());


   // addstr(g_position);



     // sp.say (text_buffer.lines[0].c_str());

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

  return 0;
}
 
