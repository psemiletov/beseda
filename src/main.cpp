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
#include "filelist.h"

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
#define BOOKHTML "/home/rox/devel/test-books/aorta.html"
#define BOOKEPUB "/home/rox/devel/test-books/pg1513.epub"
#define BOOKEPUB3 "/home/rox/devel/test-books/pg1513-images-3.epub"



/*

  std::signal (SIGINT, f_signal_handler);


 while (g_signal != SIGINT)

 */


extern int g_position;
extern int g_state;

int saved_pos;


int main (int argc, char *argv[])
{

  CFileList filelist;

  g_position = 0;

  std::string program_name = "Beseda " + std::string (VERSION_NUMBER) + " by Peter Semiletov, https://psemiletov.github.io/beseda/";

  std::string config_dir = get_home_dir() + "/.config";
  mkdir (config_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  config_dir += "/beseda";
  mkdir (config_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

  std::string message;
  CBookmarks bookmarks;

  CPairFile settings;

  std::string fname_bookmarks = get_home_dir() + "/.config/beseda/bookmarks.conf";
  std::string fname_config = get_home_dir() + "/.config/beseda/config.conf";

  bookmarks.load (fname_bookmarks);
  settings.load (fname_config);

  std::string filename = "no file is loaded";
  int locale_only = settings.get_int ("locale_only", 1);


#ifdef DEBUGFIO
   if (argc == 1)
      filename = BOOKEPUB3;
#endif

   if (argc == 2)
     { //take from argv[1]
       if (! is_dir (argv[1]))
          filename = argv[1];
      //else filename is empty

     }

  //std::cout << "filename:::" << filename << std::endl;
  //std::cout << setlocale(LC_ALL, NULL) << std::endl;

  char *loc = setlocale (LC_ALL, "");

/*
  std::string locale_string;

  if (loc)
     locale_string = loc;
  else
      locale_string = "en";
*/

//SPEECH INIT

  CSpeech sp;
  sp.init ("beseda");
  sp.get_voices (locale_only);

  sp.current_voice_index = settings.get_int ("voice", 0);
  if (sp.current_voice_index > sp.voices.size() - 1)
     {
      sp.current_voice_index = 0;
      settings.set_int ("voice", sp.current_voice_index);
     }

  sp.set_voice_by_index (sp.current_voice_index);

 // std::cout << "VOICE IS: " << sp.voices[sp.current_voice_index].name << std::endl;

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


  if (text_buffer.loaded)
     {
      filelist.update_for_file (filename);
      filelist.current_index = filelist.get_list_index_from_fname (filename);
     }
  else
     {
      if (argc == 2) //directory supplied
        {
         //TESTED
         std::string dirpath = argv[1];
         filelist.update_for_directory (dirpath);
        }
       else //NOT TESTED
           filelist.update_for_directory (current_path());
    }

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
  std::string current_voice;

  saved_pos = -1;

  sp.say ("Hello!");


  /* Event loop */
  while (running)
        {

         erase();

         printw ("%s\n", program_name.c_str());

         printw ("message: %s\n", message.c_str());

           //NOT LOADED!!!!

  //std::cout << "VOICE IS: " << sp.voices[sp.current_voice_index].name << std::endl;

         if (! sp.output_module_name.empty())
            printw ("synth module: %s\n", sp.output_module_name.c_str());

         if (sp.voices.size() > 0)
            {
             current_voice = sp.voices[sp.current_voice_index].name;
             printw ("voice: %s\n", current_voice.c_str());
            }

         printw ("slot: %d\n", bookmarks.current_index);
         printw ("filename: %s\n", filename.c_str());

         if (text_buffer.loaded)
            printw ("lines total: %s\n", std::to_string (text_buffer.lines.size()).c_str());

         std::string str_counter =  std::to_string(g_position);
         printw ("current line: %s\n", str_counter.c_str());


         //ПЕРЕПИСАТЬ, ОБНОВЛЯТЬ ЛИШЬ ПРИ ИЗМЕНЕНИИ ПАРАМЕТРОВ!

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


         if (ch == KEY_LEFT)
            {
             if (filelist.current_index == -1 || filelist.current_index == 0)
                {
                 refresh();
                 continue;
                }
             //lushinp();

             filelist.left();

             if (filelist.current_index != -1)
                {
                 //if (text_buffer.loaded)
             //       sp.cancel();

                 std::string temp_filename = filelist.files[filelist.current_index];

                 message = get_file_name (temp_filename);

                 sp.say (message.c_str());

                 g_position = 0;
                 g_state = SPCH_STATE_STOPPED;
//                 continue;

                }
            }


        if (ch == KEY_RIGHT)
            {
             //плохо, переписать проверки!

             //flushinp();

             if (filelist.current_index == filelist.files.size() - 1)
               {
                refresh();
                continue;
               }

             filelist.right();

             if (filelist.current_index != -1)
                if (filelist.current_index < filelist.files.size())
                   {
                    std::string temp_filename = filelist.files[filelist.current_index];

                    message = get_file_name (temp_filename);
                    sp.say (message.c_str());

                    g_position = 0;
                    g_state = SPCH_STATE_STOPPED;
                  }
            }


        if (ch == 10/*KEY_ENTER*/)
            {
             if (text_buffer.loaded)
                 sp.cancel();

             if (filelist.current_index != -1)
                {
                 filename = filelist.files[filelist.current_index];

               //add here say filename
                 text_buffer.load (filename);

                 saved_pos = -1;
                 g_position = 0;
                 g_state = SPCH_STATE_SAYING;
                }
            }

         //select voice

        if (ch == '[')
          {
           if (sp.current_voice_index != 0)
             {
              sp.current_voice_index--;
              sp.set_voice_by_index (sp.current_voice_index);
              settings.set_int ("voice", sp.current_voice_index);
             }
          }

        if (ch == ']')
          {
           if (sp.current_voice_index < sp.voices.size() - 1)
              {
               sp.current_voice_index++;
               sp.set_voice_by_index (sp.current_voice_index);
               settings.set_int ("voice", sp.current_voice_index);
             }
          }


       if (ch == 'z')
          {
           if (sp.current_voice_index < sp.voices.size() - 1)
              {
               if (locale_only == 0)
                   locale_only = 1;
               else
                   locale_only = 0;

               sp.get_voices (locale_only);
               sp.set_voice_by_index (sp.current_voice_index);
             }
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

            message = "Boomark for " + filename + " is saved";

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

            if (text_buffer.loaded)
               {
                message = "Boomark for " + filename + " is loaded";

                filelist.update_for_file (filename);
                filelist.current_index = filelist.get_list_index_from_fname (filename);

                g_position = bookmarks.pf.get_int (bmarkprefix + "position");
                g_state = SPCH_STATE_SAYING;
               }
           }


       if (! text_buffer.loaded)
           {
            g_state = SPCH_STATE_STOPPED;

            refresh();
            continue; //просто крутимся
           }


        //PAUSE/RESUME
       if (ch == ' ' && text_buffer.loaded) //space
          {
           if (g_state != SPCH_STATE_PAUSED)
              {
               message = "PAUSED";
               sp.pause();
              }
           else
               if (g_state == SPCH_STATE_PAUSED)
                  {
                   message = "RESUMED";
                   sp.resume();
                  }
            }


           //EOF - ВЫХОДИМ ЛИ?
       if (g_position != 0 && g_position > text_buffer.lines.size())
          {
           running = false;
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

        refresh();
       }


  endwin();

  sp.stop();

  settings.set_int ("locale_only", locale_only);
  settings.set_int ("voice", sp.current_voice_index);

  settings.save();

  return 0;
}
 
