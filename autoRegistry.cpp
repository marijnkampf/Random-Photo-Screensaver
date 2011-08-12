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

#include "autoRegistry.h"

using namespace Microsoft::Win32;
using namespace System;
using namespace System::Text::RegularExpressions;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AutoRegistry::AutoRegistry(String^ key)
{
	registryKeys = gcnew ArrayList;
	registryValues = gcnew ArrayList;
	SetRegKey(key);
}

AutoRegistry::~AutoRegistry()
{
}

void AutoRegistry::SetRegKey(String^ key)
{
	this->key=key;
	this->rk = nullptr;
	this->rk = Registry::CurrentUser->CreateSubKey(key);
	this->rk->Close();
}

bool AutoRegistry::Add(bool save, bool value, String^ name, bool defaultValue) {
	this->rk = nullptr;
	this->rk = Registry::CurrentUser->OpenSubKey(this->key, true);
	if (this->rk) { 
		if (save) {
			this->rk->SetValue(name, value);
		} else {
			// Read from registry
			value = Convert::ToBoolean(this->rk->GetValue(name, defaultValue));
			this->registryKeys->Add(name);
			this->registryValues->Add(value);
		}
	}
	this->rk->Close();
	return value;
}

int AutoRegistry::Add(bool save, int %value, String^ name, int defaultValue) {
	this->rk = nullptr;
	this->rk = Registry::CurrentUser->OpenSubKey(this->key, true);
	if (this->rk) { 
		if (save) {
			this->rk->SetValue(name, value);
		} else {
			// Read from registry
			value = Convert::ToInt32(this->rk->GetValue(name, defaultValue));
			this->registryKeys->Add(name);
			this->registryValues->Add(value);
		}
	}
	this->rk->Close();
	return value;
}

String^ getValue(String^ name, String^ string, String^ defaultValue) {
	int p = string->IndexOf(name);
	if (p != -1) {
		int start = string->IndexOf("=", p);
		int end = string->IndexOf(",", start);
		if (end == -1) end = string->Length;
		String^ s = (String^)string->Substring(start+1, end - start-1);
		return s;
	}
	return defaultValue;
}

float getValue(String^ name, String^ string, float defaultValue) {
	int p = string->IndexOf(name);
	if (p != -1) {
		int start = string->IndexOf("=", p);
		int end = string->IndexOf(",", start);
		if (end == -1) end = string->Length;
		float s = (float)Convert::ToDouble(string->Substring(start+1, end - start-1));
		return s;
	}
	return defaultValue;
}

#define SEPARATOR + ", " +

System::Drawing::Rectangle AutoRegistry::Add(bool save, System::Drawing::Rectangle %value, String^ name, System::Drawing::Rectangle defaultValue) {
	this->rk = nullptr;
	this->rk = Registry::CurrentUser->OpenSubKey(this->key, true);
	if (this->rk) { 
		if (save) {
		   System::ComponentModel::TypeConverter^ converter =
			     System::ComponentModel::TypeDescriptor::GetConverter( System::Drawing::Rectangle::typeid );
			this->rk->SetValue(name, converter->ConvertToString(value));
		} else {
		   System::ComponentModel::TypeConverter^ converter =
			     System::ComponentModel::TypeDescriptor::GetConverter( System::Drawing::Rectangle::typeid );
			
			String^ s = (String^)this->rk->GetValue(name, "");
			if (s->Length > 0) value = static_cast<System::Drawing::Rectangle>(converter->ConvertFromString( s ));
		}
	}
	this->rk->Close();
	return value;
}

Font^ AutoRegistry::Add(bool save, Font^ %value, String^ name, Font^ defaultValue) {
	this->rk = nullptr;
	this->rk = Registry::CurrentUser->OpenSubKey(this->key, true);
	if (this->rk) { 
		if (save) {
		   System::ComponentModel::TypeConverter^ converter =
			     System::ComponentModel::TypeDescriptor::GetConverter( System::Drawing::Font::typeid );
			this->rk->SetValue(name, converter->ConvertToString(value));
		} else {
		   System::ComponentModel::TypeConverter^ converter =
			     System::ComponentModel::TypeDescriptor::GetConverter( System::Drawing::Font::typeid );
			
			String^ s = (String^)this->rk->GetValue(name, "");
			if (s->Length > 0) value = dynamic_cast<System::Drawing::Font^>(converter->ConvertFromString( s ));
		}
	}
	this->rk->Close();
	return value;
}

Object^ AutoRegistry::Add(bool save, Object^ %value, String^ name, Object^ defaultValue) {
	this->rk = nullptr;
	this->rk = Registry::CurrentUser->OpenSubKey(this->key, true);
	if (this->rk) { 
		if (save) {
			try {
				this->rk->SetValue(name, value);
			} catch (Exception^ ex) {
			}
		} else {
			// Read from registry
			value = (Object^)this->rk->GetValue(name, defaultValue);
			this->registryKeys->Add(name);
			this->registryValues->Add(value);
		}
	}
	this->rk->Close();
	return value;
}
