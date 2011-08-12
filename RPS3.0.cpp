/*****************************************************************
* Copyright (C) 2005-2011 Marijn Kampf <marijn (at) abscreensavers (dot) com>
* See http://www.abscreensavers.com for more information.
* This program is part of Random Photo Screensaver
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
	Changes:
	20090507
		Changed: Added Screensaver on / off
		Updated: int main(array<System::String ^> ^args)
		By:			 MK.
*/

#include "stdafx.h"
#include "Engine.h"
#include "fConfig.h"
#include "vcclr.h"

std::vector<TMultiMonitor*> multiMonitors;
using namespace System::Diagnostics;
using namespace nsRandomPhotoScreensaver;

bool singleProcess() {
	Process^ currentProcess = Process::GetCurrentProcess();
	String^ filename = currentProcess->MainModule->FileName->ToLower();
	array<Process^>^ processes = Process::GetProcessesByName(currentProcess->ProcessName);
	for (int i = 0; i < processes->Length; i++) {
		String^ t = processes[i]->MainModule->FileName->ToLower();
		if ((String::Compare(processes[i]->MainModule->FileName->ToLower(), filename)==0)) {
			if (processes[i]->Id != currentProcess->Id) {
				if (processes[i]->MainWindowHandle == currentProcess->MainWindowHandle) {
					if (!processes[i]->CloseMainWindow()) {
						processes[i]->Kill();
					}
				}
			}
		}
	}
	return true;
}

[STAThreadAttribute]
int main(array<System::String ^> ^args) {
	TScreensaverAction action;

	singleProcess();
	if ((args->Length > 0) && (args[0]->Length >= 2)) {
		switch (Char::ToLower(args[0][1])) {
			case 's':
				action=saRun;
			break;
			case 'c':
				action=saConfig;
			break;
			case 'w': // Randomize wallpaper only
				action=saWallpaper;
			break;			
			case 'p':
				action=saPreview;
			break;
			case 'o':
				bool update = false;
				String^ setting;
				String^ value = args[0]->ToLower()->Trim(String("-/\\").ToCharArray());
				if (String::Compare(value, "off")==0) {
					setting = "0";		
					update = true;
				} else if (String::Compare(value, "on")==0) {
					setting = "1";
					update = true;
				}
				if (update) {
					RegistryKey^ rk = Registry::CurrentUser->CreateSubKey("Control Panel\\Desktop");
					String^ oldValue = "on";
					if (Convert::ToString(rk->GetValue("ScreenSaveActive")) == "0") oldValue = "off";
					rk->SetValue("ScreenSaveActive", setting);
					MessageBox::Show("Your screensaver has been truned " + value + "." + Environment::NewLine + "(It was " + oldValue + ")", "Screensaver turned " + value + ".", MessageBoxButtons::OK, MessageBoxIcon::Information );
					Application::Exit();
					return 0;
				}
			break;
		}
	}


	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Create hwnd array!
	array<IntPtr>^ hwnd;
	hwnd = gcnew array<IntPtr>(args->Length);

	if (action == saPreview) {
		for(int i = 0; (i+1) < args->Length; i++) {
			hwnd[i] = IntPtr(Convert::ToInt64(args[i+1]));
		}
	}

	// Initialise global object config
	config = gcnew fConfig(action, hwnd);
//	engine = gcnew Engine(hwnd);

	switch (config->action) {
		Engine^ engine;
		case saWallpaper: case saRun:
			engine = gcnew Engine(hwnd);
			Application::Run(engine);
		break;
		case saPreview:
			if (args->Length > 1) {
				engine = gcnew Engine(hwnd);
				Application::Run(engine);
			} else {
				MessageBox::Show("Argument missing!\nExpected parameters:\n\n /p <hwnd>", "Preview window", MessageBoxButtons::OK, MessageBoxIcon::Exclamation );
				Application::Exit();	
			}
		break;
		default:
			Application::Run(config);
		break;
	} 
	return 0;

};
