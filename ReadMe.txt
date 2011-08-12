**Random Photo Screensaver**
Random Photo Screensaver (RPS) is a photo slide show screensaver written in Visual Studio C++. 

More information can be found at http://www.abscreensavers.com/random-photo-screensaver

**Structure**
RPS consists of two programs. A launcher that resides in the users' Windows folder and the actual program that resides in the installation folder including all required libraries. I've chosen for this option to make installation of the library files easier and avoid any conflicts with existing library files.

** Installation **
I've written and compiled RPS using Microsoft Visual C++ 2010 Express Edition (MSVC 2010), this can be downloaded for free from http://www.microsoft.com/express/Downloads/#2010-Visual-CPP.

It is strongly recommended to start with the complete archive which contains all dependencies, you can download this from:
http://www.abscreensavers.com/abdownloads/rps/rps3.4full.zip
For updates, you can simply update the RPS source files, which will be much much smaller. The updates are published to 	https://github.com/marijnkampf/Visual-C---Random-Photo-Screensaver
A zipfile of the latest version can downloaded from https://github.com/marijnkampf/Visual-C---Random-Photo-Screensaver/zipball/master

It should be a matter of unzipping the files, opening rps\RPS3.0.sln in Visual C++ 2010 and compiling for the screensaver.

and "RPS Launcher\Random Photo Screensaver Launcher.sln" for the launcher.

**Credits**
RPS using the following components/external programs: 
EXIF and IPTC data: Exiv2 by Andreas Huggel (http://www.exiv2.org)
RAW files: UFRaw by Udi Fuchs (http://ufraw.sourceforge.net) which is using dcraw code by David J. Coffin (http://www.cybercom.net/~dcoffin/dcraw/)
Other components used Expat (http://expat.sourceforge.net/) by James Clark and Zlib (http://zlib.net/) by Greg Roelofs/Mark Adler.

**SDK**
RPS required Windows Software Development Kit (SDK) in the old MSVC 2005 version, not sure whether it requires it for the MSVC 2010 version.

**Manual Installation**
RPS requires the Exiv2 library which in turn requires a number of other libraries. If you're not starting with the rps3.4full.zip try the following:

exiv2    http://www.exiv2.org/
zlib     http://zlib.net/
expat    http://expat.sourceforge.net/

Download, install and compile the above libraries.

If you haven't used the full download archive (or using a different folder structure) you may need to adjust the required library paths. You will need to alter:

Project > Properties
	Configuration Properties > Linker > Input > Additional Dependencies
		Change the folders to the locations of your libraries:
			..\exiv2-0.21.1\msvc\bin\DebugDLL\exiv2d.lib;..\Expat-2.0.1\win32\bin\debug\libexpat.lib;%(AdditionalDependencies)			
	Build Events > Post-Build Event > Command Line
		Change the folders to the locations of your libraries:	
			copy "..\exiv2-0.21.1\msvc\bin\DebugDLL\*.dll" "$(OutDir)"
	Configuration Properties > VC++ Directories > Include Directories
		..\exiv2-0.21.1\msvc\include\exiv2;..\exiv2-0.21.1\msvc\include;$(IncludePath)

Cross your fingers and compile :)


**License**
Copyright (C) 2005-2011 Marijn Kampf <marijn (at) abscreensavers (dot) com>
See http://www.abscreensavers.com for more information.

Random Photo Screensaver is free software (http://www.gnu.org/philosophy/free-sw.html); you can redistribute it and/or modify it under the terms of the GNU General Public License (http://www.gnu.org/licenses/gpl.html) as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

You may use this software on a trial basis free of charge for up to thirty (30) days in order to ascertain its usefulness for your purposes. Beyond that time, you are required to either remit payment (http://www.abscreensavers.com/rps/donation) for its use, or remove it from your computer. 

Alternatively, Random Photo Screensaver is also available with a commercial license, which allows it to be used in closed-source projects. Contact me (http://www.abscreensavers.com/contact) for more information. A separate license for Exiv2 may be required. Please see http://www.exiv2.org/.

Random Photo Screensaver is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
