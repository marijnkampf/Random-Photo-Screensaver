Random Photo Screensaver 4 (Beta)
==========================

Random Photo Screensaver 4 (RPS4) is a photo slideshow screensaver written in Visual Studio C#.

More information including executable download for Windows XP, Vista, 7 & 8/8.1 can be found at [abScreensavers.com](http://www.abscreensavers.com/random-photo-screensaver).

Random Photo Screensaver 3
--------------------------
The source for the previous version RPS3 written in Visual Studio C++, can be found at https://github.com/marijnkampf/Random-Photo-Screensaver/tree/RPS3. It will only be updated with bug fixes no new features are added.

Structure
---------
RPS consists of two programs. A [launcher]("ToDo: Make github project and insert link") that resides in the users' Windows folder and the actual program that resides in the installation folder including all required libraries. 

I've chosen for this option as it avoids cluttering the Windows folder with loads of files, makes installation of library files easier and avoids creating conflicts with existing library files.

Installation
------------
If you only want to use the screensaver download the latest executable from [abScreensavers.com](http://www.abscreensavers.com/random-photo-screensaver). 

RPS requires .Net 4.0 and Internet Explorer 8 (the latest version of IE is recommended though).

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
    - [UFRaw](http://ufraw.sourceforge.net/)
    - [SQLite](http://www.sqlite.org/)
    - [Virtual Machine used for testing](https://www.virtualbox.org/)
    	- [Windows XP/Vista](https://www.modern.ie/en-us/virtualization-tools#downloads)
    	- [Windows 7](http://www.microsoft.com/en-us/evalcenter/evaluate-windows-7-enterprise)
    	- [Windows 8](http://www.microsoft.com/en-us/evalcenter/evaluate-windows-8-1-enterprise)
    	- [Windows 8.1](http://www.microsoft.com/en-us/evalcenter/evaluate-windows-8-enterprise)
- JavaScript 
    - [jQuery](http://jquery.com/)
    - [jQuery UI](http://jqueryui.com/)
    - [Fancytree](https://github.com/mar10/fancytree)
    - [Highlight text](http://johannburkard.de/blog/programming/javascript/highlight-javascript-text-higlighting-jquery-plugin.html)
    - <del>[Object fit by Steve Workman/Simon Schmid](https://github.com/steveworkman/jquery-object-fit)</del>
    - [Colour picker](http://bgrins.github.io/spectrum/)
- CSS
		- [Pure CSS speech bubbles](nicolasgallagher.com/pure-css-speech-bubbles/)
- Miscellaneous
		- [Waiting image spinners](http://preloaders.net/en/circular)
		- [Windows icons converted with iconverticons](http://iconverticons.com/online/)
- [NSIS installer](http://nsis.sourceforge.net/Main_Page)
	- [StdUtils NSIS](http://muldersoft.com/docs/stdutils_readme.html#e5717960)

License
-------
Copyright (C) 2005-2014 Marijn Kampf <marijn (at) abscreensavers (dot) com>

Random Photo Screensaver is free software (http://www.gnu.org/philosophy/free-sw.html); you can redistribute it and/or modify it under the terms of the GNU General Public License (http://www.gnu.org/licenses/gpl.html) as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

You may use this software on a trial basis free of charge for up to thirty (30) days in order to ascertain its usefulness for your purposes. Beyond that time, you are required to either remit payment (http://www.abscreensavers.com/rps/donation) for its use, or remove it from your computer. 

Alternatively, Random Photo Screensaver is also available with a commercial license, which allows it to be used in closed-source projects. Contact me (http://www.abscreensavers.com/contact) for more information.

Random Photo Screensaver is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See http://www.abscreensavers.com for more information.

ToDo
----
- Update GitHub URL after beta release becomes release version
