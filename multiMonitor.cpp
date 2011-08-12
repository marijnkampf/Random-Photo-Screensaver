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
#include "stdafx.h"
#include "multiMonitor.h"
#include <windows.h>
#define COMPILE_MULTIMON_STUBS
#pragma managed(push,off)
#include "multimon.h"
#pragma managed(pop)

TMultiMonitor::TMultiMonitor(int _id, RECT _rcMonitor) : id(_id) {
	this->panoX = 0;
	this->x = _rcMonitor.left;
	this->y = _rcMonitor.top;
	this->width = _rcMonitor.right - _rcMonitor.left;
	this->height = _rcMonitor.bottom - _rcMonitor.top;
}

// Initialises multiple monitors structure and returns size of entire desktop in RectangleF
System::Drawing::Rectangle initMonitors() { array<IntPtr>^ hwnd; return initMonitors(false, hwnd); }
System::Drawing::Rectangle initMonitors(bool screensaverPreview, array<IntPtr>^ hwnd) {
	if (!multiMonitors.empty()) {
		multiMonitors.erase(multiMonitors.begin(), multiMonitors.end());
	}
	if (screensaverPreview) {
		int width=0, height=0;
		for(int i=0; i < hwnd->Length-1; i++) {
			WINDOWINFO wi;
			wi.cbSize = sizeof(wi);
			GetWindowInfo((HWND)hwnd[i].ToInt32(), &wi);

			TMultiMonitor *monitor;
			RECT preview;

			// Add rc monitor and mirror settings in multimonitorSize
			preview.top = 0;
			preview.left = 0;
			preview.right = wi.rcClient.right-wi.rcClient.left;//+width;
			preview.bottom = wi.rcClient.bottom-wi.rcClient.top;

			monitor = new TMultiMonitor(1, preview);
			monitor->x = 0;
			monitor->panoX = width;
			monitor->id = i;
			multiMonitors.push_back(monitor);

			if (preview.bottom > height) height=preview.bottom;
			width+=(wi.rcClient.right-wi.rcClient.left);//-preview.left;
		}
		return System::Drawing::Rectangle(0,0,width,height);
	} else {
		System::Drawing::Rectangle desktop;
		array<System::Windows::Forms::Screen^>^ screens;
		screens = System::Windows::Forms::Screen::AllScreens;
		for(int i = 0; i < screens->Length; i++) {
			TMultiMonitor *monitor;
			RECT bounds;
			bounds.top = screens[i]->Bounds.Top;
			bounds.left = screens[i]->Bounds.Left;
			bounds.right = screens[i]->Bounds.Left+screens[i]->Bounds.Width;
			bounds.bottom = screens[i]->Bounds.Top+screens[i]->Bounds.Height;

			desktop = System::Drawing::Rectangle::Union(desktop, screens[i]->Bounds);

			monitor = new TMultiMonitor(i, bounds);
			multiMonitors.push_back(monitor);
			multiMonitors[i]->panoX = bounds.left;
		}
		// Correct origin of panorama 
		for(int i = 0; i < screens->Length; i++) {
			multiMonitors[i]->panoX = multiMonitors[i]->panoX - desktop.Left;
		}
		return desktop;
	}
}

