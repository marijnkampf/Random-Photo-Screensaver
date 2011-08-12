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
#pragma once

using namespace Microsoft::Win32;
using namespace System;
using namespace System::Collections;
using namespace System::Drawing;

public ref class AutoRegistry {
public:
	void SetRegKey(String^ key);	// Sets registry key to keep settings in
	AutoRegistry(String^ key);	// Constructor that sets registry key to keep settings in
	virtual ~AutoRegistry();	// Destructor (empty)

	bool Add(bool save, bool value, String^ name, bool defaultValue);
	int Add(bool save, int %value, String^ name, int defaultValue);

	Object^ Add(bool save, Object^ %value, String^ name, Object^ defaultValue); 
	Font^ Add(bool save, Font^ %value, String^ name, Font^ defaultValue); 
	System::Drawing::Rectangle Add(bool save, System::Drawing::Rectangle %value, String^ name, System::Drawing::Rectangle defaultValue);

	ArrayList^ registryKeys;
	ArrayList^ registryValues;

	void Test(Object^ %value) {
		value = "Test";
	}

protected:
	String^ key;	// Our key full name
	RegistryKey^ rk;

private:
};
