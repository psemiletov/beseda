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

#define LINES_PER_PAGE 7


//#define DEBUGFIO 1

#define BOOK0 "/home/rox/devel/test-books/test.txt"
#define BOOK1 "/home/rox/devel/test-books/Dracula by Bram Stoker.txt"
#define BOOK2 "/home/rox/devel/test-books/frankenstein.txt"
#define BOOK3 "/home/rox/devel/test-books/Pride and Prejudice by Jane Austen.txt"
#define BOOK4 "/home/rox/devel/test-books/чтоделать.txt"
#define BOOK5 "/home/rox/devel/test-books/human_hist.abw"
#define BOOKDOCX "/home/rox/devel/test-books/01.docx"
#define BOOKODT "/home/rox/devel/test-books/01.odt"
#define BOOKKWD "/home/rox/devel/test-books/dark_rituals.kwd"
#define BOOKFB2 "/home/rox/devel/test-books/dar.fb2"
#define BOOKFB2ZIP "/home/rox/devel/test-books/Dante_Aligeri__Bozhestvennaja_komedija.fb2.zip"
#define BOOKFBZ "/home/rox/devel/test-books/ud-tom-01Z.fbz"



using namespace std;

//

/*

  std::signal (SIGINT, f_signal_handler);


 while (g_signal != SIGINT)

 */


extern int g_position;
extern int g_state;

int saved_pos;

int main (int argc, char *argv[])
{
  g_position = 0;

  std::string config_dir = get_home_dir() + "/.config";
  mkdir (config_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  config_dir += "/beseda";
  mkdir (config_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

  std::string message;
  CBookmarks bookmarks;

  std::string fname_bookmarks = get_home_dir() + "/.config/beseda/bookmarks.conf";

  bookmarks.load (fname_bookmarks);

  std::string filename = "no file loaded";


#ifdef DEBUGFIO
   if (argc == 1)
      filename = BOOKFBZ;
#endif

   if (argc == 2)
       //take from argv[1]
       filename = argv[1];

  //std::cout << "filename:::" << filename << std::endl;

  //std::cout << setlocale(LC_ALL, NULL) << std::endl;
  setlocale (LC_ALL, "");

  //SPEECH INIT
  CSpeech sp;
  sp.init ("beseda");

  //Do we need it?
  signal (SIGINT, f_signal_handler);

  CTextBuffer text_buffer;

  if (file_exists (filename))
     {
      text_buffer.load (filename);
      g_state = SPCH_STATE_SAYING;
     }
  else
      g_state = SPCH_STATE_STOPPED;


#ifdef DEBUGFIO

  std::cout << "text_buffer.lines.size():" << text_buffer.lines.size() << std::endl;
  return 0;

#endif


  bool running = true;


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

         erase();

         printw ("message: %s\n", bookmarks.pf.file_name.c_str());
         printw ("slot: %d\n", bookmarks.current_index);
         printw ("filename: %s\n", filename.c_str());

         if (text_buffer.loaded)
            printw ("lines total: %s\n", std::to_string (text_buffer.lines.size()).c_str());

         std::string str_counter =  std::to_string(g_position);
         printw ("current line: %s\n", str_counter.c_str());

         refresh(); //ncurses

     /*   if (g_signal == SIGINT)
           {
            running = false;
            g_state = SPCH_STATE_STOPPED;
            break;
           }
*/
         ch = getch();

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

         //PgUp
         if (ch == KEY_PPAGE  && text_buffer.loaded)
            {
             sp.cancel();
             g_state = SPCH_STATE_SAYING;

             g_position -= LINES_PER_PAGE;
             if (g_position < 0)
                g_position = 0;
            }


         //PgDn
         if (ch == KEY_NPAGE  && text_buffer.loaded)
            {
             sp.cancel();
             g_state = SPCH_STATE_SAYING;

             g_position += LINES_PER_PAGE;
             if (g_position > (text_buffer.lines.size() - 1))
                g_position = text_buffer.lines.size() - 1;
            }



         //OK
         if (ch == KEY_HOME && text_buffer.loaded)
            {
             sp.cancel();
             g_state = SPCH_STATE_SAYING;
             g_position = 0;
            }


         //ok
         if (ch == KEY_END && text_buffer.loaded)
            {
             sp.cancel();
             g_state = SPCH_STATE_SAYING;
             g_position = text_buffer.lines.size() - 1;
            }

         //ok
         if (ch == KEY_UP && text_buffer.loaded)
            {
             sp.cancel();
             g_state = SPCH_STATE_SAYING;

             if (g_position > 0)
                g_position--;
            }

         //ok
         if (ch == KEY_DOWN && text_buffer.loaded)
            {
             sp.cancel();
             g_state = SPCH_STATE_SAYING;

             if (g_position < text_buffer.lines.size() - 1)
                g_position++;
            }


         //slots

          if (ch == '0')
             bookmarks.current_index = 0;


          if (ch == '1')
             bookmarks.current_index = 1;

          if (ch == '2')
             bookmarks.current_index = 2;

          if (ch == '3')
             bookmarks.current_index = 3;

          if (ch == '4')
             bookmarks.current_index = 4;

          if (ch == '5')
             bookmarks.current_index = 5;

          if (ch == '6')
             bookmarks.current_index = 6;

          if (ch == '7')
             bookmarks.current_index = 7;

          if (ch == '7')
             bookmarks.current_index = 7;

          if (ch == '8')
             bookmarks.current_index = 8;

          if (ch == '9')
             bookmarks.current_index = 9;

            //save bookmark

          if (ch == KEY_F(2) && text_buffer.loaded)
            {
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

             g_position = bookmarks.pf.get_int (bmarkprefix + "position");
             g_state = SPCH_STATE_SAYING;
            }

         if (ch == ' ' && text_buffer.loaded) //space
            {
             if (g_state != SPCH_STATE_PAUSED)
                {
                 message = "PAUSED";
                 sp.pause();
                 continue;
                }
             else
                {
                 message = "RESUMED";
                 sp.resume();
                 continue;
                }
             }



        if (! text_buffer.loaded)
            {
             g_state = SPCH_STATE_STOPPED;
             continue;
            }


           //EOF - ВЫХОДИМ ЛИ?
          if (g_position !=0 && g_position > text_buffer.lines.size())
             {
              running = false;
              //state = SPCH_STATE_STOPPED;
              sp.stop();
              break;
             }

          //Say here:
          if (text_buffer.loaded)
          if (g_state == SPCH_STATE_SAYING && saved_pos != g_position)
             {
              sp.say (text_buffer.lines[g_position].c_str());
              saved_pos = g_position;
             }
      }


  endwin();

  sp.stop();

  bookmarks.pf.save();


  //  std::cout << "111111111111 - B" << std::endl;

  //bookmarks.save();

  return 0;
}
 
