# Beseda: ebook speech reader

This Public Domain console program speaks the ebooks and is controlled via the keyboard to make it handy for the vision accessibility. Fast and simple. 

Supported formats: TXT, FB2, EPUB, [X]HTML, ODT, DOCX, KWD, ABW.

Please read the [FINE MANUAL](https://psemiletov.github.io/beseda/)

## Run:

```beseda book_filename```

or (no starting file, load from bookmarks later)


```beseda```


## Keyboard controls:

q - exit

up/down - scroll the current text by the paragraph

left/right - go to previous/next supported file at the current directory

enter - load and play the selected (by left/right) file

space - when playing, play or pause

0..9 - choose the current bookmark slot

F2 - save the current file name and posion as the bookmark at the current bookmark slot

F3 - load the file and the position to the current slot, if it has been saved previously

[ and ] - select the voice from the available at the current sound synths. The list of the voices is depened on the current language, or for all supported languages (press Z to choose the mode, from locale langiage to all languages).


## How to build and install:


### Dependencies:

[Speech Dispatcher](https://freebsoft.org/speechd) - need to be configured separately. Use Speech Dispatcher and some voice synths, such as [RHVoice](https://github.com/RHVoice/RHVoice) or [Festival, Flite](http://festvox.org).

[ncurses](https://invisible-island.net/ncurses/)

### Bundled libraries:

[pugixml](https://pugixml.org)

[zip](https://github.com/kuba--/zip)


### To build Beseda run under sudo or root:

```
mkdir b
cd b
cmake ..
make
make install
```

## Working example configuration:

1. Install Speech Dispatcher

2. Install RHVoice and voices for it
Copy the ```/etc/speech-dispatcher``` directory to ```$HOME/.config```

3. There, edit ```speechd.conf```, where uncomment the line with ```AddModule "rhVoice"``` and add the line: ```DefaultModule rhvoice```

4. To avoid crackles when run other audio software (such as Reaper or Ardour), if you use Pipewire or Pulse, uncomment ```AudioOutputMethod "pulse"```


## Donate

If you like this software, you can donate to me, Peter Semiletov, via Paypal: peter.semiletov@gmail.com

