Random Photo Screensaver 4
==========================

Random Photo Screensaver 4 (RPS4) is a photo slideshow screensaver written in Visual Studio C#.

More information including executable download for Windows XP, Vista, 7 & 8/8.1 can be found at [abScreensavers.com](http://www.abscreensavers.com/random-photo-screensaver).

Features & Benefits
--------
- Media
    - [x] Photo formats
    - [ ] RAW files
    - [x] Animated gifs
    - [x] Videos
- Image database (instant access to all file once indexed)
    - [ ] Filters on filename / metadata
    - [x] Hide system attribute hidden files
    - [ ] Better/easier selection of folders to include / exclude
- Multi monitor support
    - [x] Synchorised / non-synchronised changing of images
- Show
    - [x] Clock (none, current time, running time)
    - [x] Filenames
    - [x] Fit images to screen
    - [ ] Don't scale up small images
- Slideshow
    - Ordering
        - [x] Sequential
        - [.] Random
            - [ ] View sequential previous photo in folder (up/down keys)
    - [x] Navigate through previous photos (left/right keys)
- Keyboard shortcuts
    - [x] Select monitor (1...9 / 0: all)
    - [x] Delete current file(s) (to recycling bin)
    - [x] Open current file(s) in explorer
    - [ ] Open current file(s) in custom editor
    - [ ] Rotate (Exif save)
- Wallpapers
    - [x] Set wallpaper (never, daily, every run)
    - [ ] Set wallpaper from shortcut (W key)
- Miscellaneous
    - [ ] Load website
    - [ ] Kiosk mode (require password/pin for all options but browsing)
    - [ ] Record last x photos shown

Structure
---------
RPS consists of two programs. A [launcher]("ToDo: Make github project and insert link") that resides in the users' Windows folder and the actual program that resides in the installation folder including all required libraries. 

I've chosen for this option as it avoids cluttering the Windows folder with loads of files, makes installation of library files easier and avoids creating conflicts with existing library files.

Installation
------------
If you only want to use the screensaver download the latest executable from [abScreensavers.com](http://www.abscreensavers.com/random-photo-screensaver). 

It requires .Net 4.0 and Internet Explorer 8 (the latest version of IE is recommended though).

Compiling from source
---------------------
The following instruction are how to compile RPS from source in Microsoft Visual Studio C# 2013. The Express Edition can be downloaded for free from: http://www.visualstudio.com/downloads/download-visual-studio-vs
- Download the source files from https://github.com/marijnkampf/Random-Photo-Screensaver/tree/RPS4
- Open RPS 4.sln
- Compile
 
History
-------
- 2005: RPS 1 & 2 written in Delphi
- 2008: RPS 3 written in Visual Studio C++
- 2014: RPS 4 written in Visual Studio C#
    - 2014/10/17: Beta release 1

Design rationale
----------------
This is the second complete rewrite of RPS in almost 10 years. When I choose for C++ in 2008 I had performance in mind most of all. I however found that there are far more code examples for C# and that the performance between C++ and C# doesn't differ that much for a screensaver application.

I'm a [web developer](http://www.exadium.com) by day and software developer at night, basing the display on browser technology should make it easier for 3rd parties to develop plugins for things as transitions and other features. How the photos are shown should be completely customised in the final RPS 4 version.

Structure
---------
- Launcher
- RPS 4
    - data
        - Settings
            - html, js, css
        - Monitor
        - Vendor
    - database (C:\Users\[username]\AppData\Local\Random Photo Screensaver)
        - settings.sqlite
        - store.sqlite
        - meta.sqlite

Credits
-------
ToDo update all external libraries
The following (free) libraries and programs have been used in/ship with RPS4:
- [Microsoft Visual Studio 2013 C# Express Edition](http://www.visualstudio.com/downloads/download-visual-studio-vs)
    - [ExifTool by Phil Harvey](http://www.sno.phy.queensu.ca/~phil/exiftool/)
    - [SQLite](http://www.sqlite.org/)
- JavaScript 
    - [jQuery](http://jquery.com/)
    - [jQuery UI](http://jqueryui.com/)
    - [Fancytree](https://github.com/mar10/fancytree)
    - [Highlight text](http://johannburkard.de/blog/programming/javascript/highlight-javascript-text-higlighting-jquery-plugin.html)

License
-------
To be decided


ToDo
----
- Update GitHub URL after beta release becomes release version