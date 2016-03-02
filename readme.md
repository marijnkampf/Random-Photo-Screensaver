Random Photo Screensaver(tm) 4
==========================

Random Photo Screensaver 4 (RPS4) is a photo slideshow screensaver written in Visual Studio C#.

More information including executable download for Windows XP, Vista, 7 & 8/8.1 can be found at [abScreensavers.com](http://www.abscreensavers.com/random-photo-screensaver).

Random Photo Screensaver 3
--------------------------
The source for the previous version RPS3 written in Visual Studio C++, can be found at https://github.com/marijnkampf/Random-Photo-Screensaver/tree/RPS3. It will only be updated with bug fixes no new features are added.

Structure
---------
RPS consists of two programs. A [launcher](https://github.com/marijnkampf/Random-Photo-Screensaver/tree/master/RPS%20Launcher) that resides in the users' Windows folder and the actual [Random Photo Screensaver](https://github.com/marijnkampf/Random-Photo-Screensaver/tree/master/RPS%204) program that resides in the installation folder including all required libraries. 

I've chosen for this option as it avoids cluttering the Windows folder with loads of files, makes installation of library files easier and avoids creating conflicts with existing library files.

Installation
------------
If you only want to use the screensaver download the latest executable from [abScreensavers.com](http://www.abscreensavers.com/random-photo-screensaver). 

RPS requires .Net 4.0 and Internet Explorer 8 (the latest version of IE is recommended though).

Compiling from source
---------------------
The following instruction are how to compile RPS from source in Microsoft Visual Studio C# 2015. The Express Edition can be downloaded for free from: http://www.visualstudio.com/downloads/download-visual-studio-vs
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
    - data (%programdata%\Random Photo Screensaver)
    	- config.html
    	- monitor.html
    		- css
    		- images
    		- js
    		- vendor
    - database (%programdata%\Random Photo Screensaver)
        - settings.sqlite
        - store.sqlite
        - meta.sqlite
    	Alternative location for personalised settings files is "%localappdata%\Random Photo Screensaver"

Credits
-------
See http://www.abscreensavers.com/random-photo-screensaver/configuration/?tab=credits for a full list of credits of software, libraries and other code used in the making of RPS.

License
-------
Copyright (C) 2005-2015 Marijn Kampf <marijn (at) abscreensavers (dot) com>

Random Photo Screensaver(tm) is free software (http://www.gnu.org/philosophy/free-sw.html); you can redistribute it and/or modify it under the terms of the GNU General Public License (http://www.gnu.org/licenses/gpl.html) as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

The Random Photo Screensaver, abScreensavers.com names and logos are trademarks and may not be used in third party releases without written permission. See [list of trademarks](http://www.abscreensavers.com/random-photo-screensaver/open-source/trademarks). In short, if you release a separate version you have to change the name and logos of your screensaver.

Alternatively, Random Photo Screensaver is also available with a commercial license, which allows it to be used in closed-source projects. Contact me (http://www.abscreensavers.com/contact) for more information.

Random Photo Screensaver is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See http://www.abscreensavers.com for more information.
