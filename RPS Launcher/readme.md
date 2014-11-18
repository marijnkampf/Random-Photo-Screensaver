Random Photo Screensaver Launcher
=================================

To avoid installing loads of random files in the Windows folder Random Photo 
Screensaver 4 (RPS4) uses this launcher. It gets the installation folder from 
the registry entry:

HKEY_CURRENT_USER\Software\abScreensavers.com\Random Photo Screensaver\installFolder

and runs the most up to date version of RPS it finds (sorted by Creation Time).

Installation
------------
If you only want to use the screensaver download the latest executable from [abScreensavers.com](http://www.abscreensavers.com/random-photo-screensaver). 

RPS requires .Net 4.0 and Internet Explorer 8 (the latest version of IE is recommended though).

Compiling from source
---------------------
The following instruction are how to compile RPS from source in Microsoft Visual Studio C# 2013. The Express Edition can be downloaded for free from: http://www.visualstudio.com/downloads/download-visual-studio-vs
- Download the source files from https://github.com/marijnkampf/Random-Photo-Screensaver/tree/RPS4
- Open RPS Launcher\RPS Launcher.sln
- Compile


For additional information see: 
https://github.com/marijnkampf/Random-Photo-Screensaver/blob/master/readme.md