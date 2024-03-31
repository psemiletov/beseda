#include <iostream>
#include <string>

#include <signal.h>

#include <ncurses.h>
#include <locale.h>

#include <sys/stat.h>  //for mkdir


#include "utl.h"
#include "pairfile.h"
#include "speech.h"
#include "textbuffer.h"
#include "bookmarks.h"

#define DEBUGFIO 1

#define BOOK0 "/home/rox/devel/test-books/test.txt"
#define BOOK1 "/home/rox/devel/test-books/Dracula by Bram Stoker.txt"
#define BOOK2 "/home/rox/devel/test-books/frankenstein.txt"
#define BOOK3 "/home/rox/devel/test-books/Pride and Prejudice by Jane Austen.txt"
#define BOOK4 "/home/rox/devel/test-books/чтоделать.txt"
#define BOOK5 "/home/rox/devel/test-books/human_hist.abw"
#define BOOKDOCX "/home/rox/devel/test-books/01.docx"
#define BOOKODT "/home/rox/devel/test-books/01.odt"


#define BOOKKWD "/home/rox/devel/test-books/dark_rituals.kwd"


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

   std::string config_dir = get_home_dir() + "/.config";

   mkdir (config_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

   config_dir += "/beseda";
   mkdir (config_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);


  std::string message;

  CBookmarks bookmarks;

  std::string fname_bookmarks = get_home_dir() + "/.config/beseda/bookmarks.conf";

  bookmarks.load (fname_bookmarks);

   std::string filename;

   if (argc == 1)
      filename = BOOKKWD;

   if (argc == 2)
      {
       //take from argv[1]
       filename = argv[1];

      }




  // Initialise ncurses

  g_position = 0;

  std::cout << setlocale(LC_ALL, NULL) << std::endl;
  setlocale(LC_ALL, "");



  //SPEECH INIT
  CSpeech sp;
  sp.init ("beseda");


  //Do we need it?
  signal (SIGINT, f_signal_handler);


  CTextBuffer text_buffer;
  text_buffer.load (filename);

#ifdef DEBUGFIO

   std::cout << "text_buffer.lines.size():" << text_buffer.lines.size() << std::endl;
  return 0;

#endif




  g_state = SPCH_STATE_SAYING;

  bool running = true;


   //std::string ttt;

   //std::cin >> ttt;

  //NCURSES INIT
  initscr();
  keypad(stdscr, TRUE);
  cbreak();
  noecho();
  clear();


  halfdelay(1);
  //use nodelay instead?

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


         erase();

        // move (0, 0);
         //addstr (filename.c_str());

         //printw ("message: %s\n", message.c_str());
         printw ("message: %s\n", bookmarks.pf.file_name.c_str());
         printw ("slot: %d\n", bookmarks.current_index);
         printw ("filename: %s\n", filename.c_str());

         printw ("lines total: %s\n", std::to_string (text_buffer.lines.size()).c_str());
         std::string str_counter =  std::to_string(g_position);
         printw ("current line: %s\n", str_counter.c_str());

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
*/

         //OK
         if (ch == KEY_HOME)
            {
             sp.cancel();
             g_state = SPCH_STATE_SAYING;
             g_position = 0;
            }


         //ok
         if (ch == KEY_END)
            {
             sp.cancel();
             g_state = SPCH_STATE_SAYING;
             g_position = text_buffer.lines.size() - 1;
            }

         //ok
         if (ch == KEY_UP)
            {
             sp.cancel();
             g_state = SPCH_STATE_SAYING;

             if (g_position > 0)
                g_position--;
            }

         //ok
         if (ch == KEY_DOWN)
            {
             sp.cancel();
             g_state = SPCH_STATE_SAYING;

             if (g_position < text_buffer.lines.size() - 1)
                g_position++;

            }


          if (ch == '0')
             bookmarks.current_index = 0;


          if (ch == '1')
             bookmarks.current_index = 1;

          if (ch == '2')
             bookmarks.current_index = 2;

            //save bookmark

          if (ch == KEY_F(2))
//          if (ch == 's')

            {
             //message = "BMX SAVED\n";

             std::string bmarkprefix = "bm" + std::to_string (bookmarks.current_index) + "_";

             message = bmarkprefix;

             bookmarks.pf.set_string (bmarkprefix + "filename", filename);
             bookmarks.pf.set_int (bmarkprefix + "position", g_position);

             bookmarks.pf.save();
            }



            //load bookmark
         if (ch == KEY_F(3))
            {
             //load bookmark
             std::string bmarkprefix = "bm" + std::to_string (bookmarks.current_index) + "_";

             filename = bookmarks.pf.get_string (bmarkprefix + "filename");
             text_buffer.load (filename);

             g_position = bookmarks.pf.get_int ("bm00_position");
             g_state = SPCH_STATE_SAYING;
            }


/*

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

         //WORK
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

           refresh(); //ncurses
  }


  endwin();

  sp.stop();

  bookmarks.pf.save();


  //  std::cout << "111111111111 - B" << std::endl;

  //bookmarks.save();

  return 0;
}
 
