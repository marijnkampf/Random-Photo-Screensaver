/*
	Based on code from: http://jaimersamples.members.winisp.net/samples/healthcare.zip
	License unkown
	17 Feb 2009 Converted from c# to c++ by Marijn Kampf
*/
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

#include "Windows.h"

#define WM_POWERBROADCAST 0x0218

#define PBT_APMQUERYSUSPEND 0x0000
#define PBT_APMQUERYSTANDBY 0x0001
#define PBT_APMQUERYSUSPENDFAILED 0x0002
#define PBT_APMQUERYSTANDBYFAILED 0x0003
#define PBT_APMSUSPEND 0x0004
#define PBT_APMSTANDBY 0x0005
#define PBT_APMRESUMECRITICAL 0x0006
#define PBT_APMRESUMESUSPEND 0x0007
#define PBT_APMRESUMESTANDBY 0x0008
#define PBT_APMBATTERYLOW 0x0009
#define PBT_APMPOWERSTATUSCHANGE 0x000A
#define PBT_APMOEMEVENT 0x000B
#define PBT_APMRESUMEAUTOMATIC 0x0012
#define PBT_POWERSETTINGCHANGE 0x8013
#define DEVICE_NOTIFY_WINDOW_HANDLE 0x00000000
#define DEVICE_NOTIFY_SERVICE_HANDLE 0x00000001

namespace nsRandomPhotoScreensaver {
	using namespace System;
	using namespace System::Collections::Generic;
	using namespace System::Text;
	using namespace System::Windows;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Media;
	using namespace System::ComponentModel;
	using namespace System::Data;
	using namespace System::Security::Permissions;

	using namespace System::Runtime::InteropServices;
	using namespace Microsoft::Win32;

	// This structure is sent when the PBT_POWERSETTINGSCHANGE message is sent.
	// It describes the power setting that has changed and contains data about the change
	[StructLayout(LayoutKind::Sequential, Pack = 4)]
	private value class POWERBROADCAST_SETTING 	{
		public:
			Guid PowerSetting;
			Int32 DataLength;
	};

	enum PowerPlan {
				PPAutomatic,
				PPHighPerformance,
				PPPowerSaver,
	};

	public ref class PowerChecker: public System::Windows::Forms::Form	{

		public:
			delegate void PowerChanged();

			// Event that this class raises
			// when the power information changes.
			virtual event PowerChanged ^PowerChangedEvent;

		public:
			System::String ^report;

		private:
			IntPtr hBattCapacity;
			IntPtr hMonitorOn;
			IntPtr hPowerScheme;
			IntPtr hPowerSrc;
			bool runningOnDC;
			bool monitorOn;
			bool machineInSuspend;
			bool infoAvailable;
			PowerPlan powerPlan;

		public: PowerChecker() {
			report = System::String::Empty;
//			report = "none";
//			this->Text = "RPS Power Noticfications";
		}

		void Close() {
			// Clean up by unregistering any notifications we have registered for.
			try {
				UnregisterForPowerNotifications();
			}
			catch(...) {
					//API not supported
			}
		}

		#pragma region WndProc
    [SecurityPermission(SecurityAction::Demand, Flags=SecurityPermissionFlag::UnmanagedCode)]
		virtual void WndProc( Message% m ) override {
//			this->report = this->report + "\n" + DateTime::Now.ToLongTimeString() + " " + m.Msg.ToString() + " " + m.WParam.ToInt32() + " " + m.LParam.ToInt32();
			//Clipboard::SetText(this->report);
			switch (m.Msg) {
				case WM_POWERBROADCAST:
					//this->report = this->report + "\n" + m.Msg.ToString() + " " + m.WParam.ToInt32() + " " + m.LParam.ToInt32();
				  //this->report = m.Msg.ToString() + " " + m.WParam.ToInt32() + " " + m.LParam.ToInt32();
					//this->report = "WM_POWERBROADCAST";

					OnPowerBroadcast(m.WParam, m.LParam);
				break;
				case HCBT_SYSCOMMAND:
					//this->report = m.Msg.ToString() + " " + m.WParam.ToInt32() + " " + m.LParam.ToInt32();
					if ((m.WParam.ToInt32() & 0xFFF0 )== SC_MONITORPOWER) {
						//this->report = "SC_MONITORPOWER";
						Console::Beep();
					}
				break;
			}
			Form::WndProc(m);
		}
		#pragma endregion

		#pragma region RegisterForPowerNotifications
		public: void RegisterForPowerNotifications(IntPtr handle) {
//				this->Show();
//				this->Hide();
		  //this->report = "RegisterForPowerNotifications" + handle.ToString();
			//IntPtr handle = //gcnew //WindowInteropHelper(Application::Current->Windows[0])->Handle;
			try {
				hPowerSrc = RegisterPowerSettingNotification(handle, GUID_ACDC_POWER_SOURCE, DEVICE_NOTIFY_WINDOW_HANDLE);
				hBattCapacity = RegisterPowerSettingNotification(handle, GUID_BATTERY_PERCENTAGE_REMAINING, DEVICE_NOTIFY_WINDOW_HANDLE);
				hMonitorOn = RegisterPowerSettingNotification(handle, GUID_MONITOR_POWER_ON, DEVICE_NOTIFY_WINDOW_HANDLE);
				hPowerScheme = RegisterPowerSettingNotification(handle, GUID_POWERSCHEME_PERSONALITY, DEVICE_NOTIFY_WINDOW_HANDLE);
				this->infoAvailable = true;
			  //this->report = "InfoAvailable";
				//System::Windows::Forms::DialogResult r = System::Windows::Forms::MessageBox::Show ("Test", "Test", MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
			} catch (Exception ^ex) {
			  //this->report = "Error";
				this->infoAvailable = false;
				// Not Windows Vista
			}
		}
		#pragma endregion

		#pragma region UnregisterForPowerNotifications
		private: void UnregisterForPowerNotifications() {
			UnregisterPowerSettingNotification(hBattCapacity);
			UnregisterPowerSettingNotification(hMonitorOn);
			UnregisterPowerSettingNotification(hPowerScheme);
			UnregisterPowerSettingNotification(hPowerSrc);
		}
		#pragma endregion

		#pragma region Properties
		public: property bool RunningOnBattery {
			bool get() {
				return runningOnDC;
			}
		}

		property bool MonitorOn {
			bool get() {
				return monitorOn;
			}
		}

		property bool InfoAvailable {
			bool get() {
				return infoAvailable;
			}
		}

		property bool InSuspend {
			bool get() {
				return machineInSuspend;
			}
		}

		property PowerPlan ActivePowerPlan {
			PowerPlan get() {
				return powerPlan;
			}
		}
		#pragma endregion

		#pragma region Power changed message information

		private: void OnPowerBroadcast(IntPtr wParam, IntPtr lParam) {
//			this->report = this->report + "\n" + DateTime::Now.ToLongTimeString() + " " + wParam.ToInt32() + " " + lParam.ToInt32();
//			Clipboard::SetText(this->report);
		  //this->report = "!" + wParam.ToInt32() + " " + lParam.ToInt32();
			//System::Windows::Forms::DialogResult r = System::Windows::Forms::MessageBox::Show ("Test", "OnPowerBroadcast", MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
			//this->report = "OnPowerBroadcast";
//							 Console::Beep();
			// Report the change that has occurred.
			ReportPowerChange(safe_cast<int>(wParam));
/*			if (lParam == GUID_MONITOR_POWER_ON) {
				this->report = "!!!!!!";
			}
			if (safe_cast<int>(wParam) == GUID_MONITOR_POWER_ON) {
				this->report = "??????";
			}*/
//			Console::Beep();
//			this->report = this->report + "\n" + DateTime::Now.ToLongTimeString() + " " + wParam.ToInt32() + " " + lParam.ToInt32() + " " + PBT_POWERSETTINGCHANGE;
//			Clipboard::SetText(this->report);

			if (safe_cast<int>(wParam) == PBT_POWERSETTINGCHANGE) {
//				this->report = "!!!";
//				Clipboard::SetText(this->report);
				// Respond to a change in the power settings.
				PowerSettingChange(lParam);
			}
			
			try {
				// Raise the event to notify any listeners of a change.
				//if (PowerChangedEvent != nullptr) {
					PowerChangedEvent();
				//}
			} catch (Exception ^ex) {
					// An exception possibly thrown by the client of our event
			}
		}

		protected: void ReportPowerChange(int reason) {
//				System::Windows::Forms::DialogResult r = System::Windows::Forms::MessageBox::Show ("Test", "1", MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
//			System::String ^report = System::String::Empty;
			switch (reason) {
				case PBT_APMSUSPEND:
					report = "System is suspending operation ";
					machineInSuspend = true;
				break;
				case PBT_APMSTANDBY:
					report = "System is standing by ";
					machineInSuspend = true;
				break;
				case PBT_APMRESUMECRITICAL:
					report = "Operation resuming after critical suspension.";
					machineInSuspend = false;
				break;
				case PBT_APMRESUMESUSPEND:
					report = "Operation resuming after suspension.";
					machineInSuspend = false;
				break;
				case PBT_APMRESUMESTANDBY:
					report = "Operation resuming after stand by.";
					machineInSuspend = false;
				break;
				case PBT_APMBATTERYLOW:
					report = "Battery power is low.";
				break;
				case PBT_APMPOWERSTATUSCHANGE:
					//report = "Power status has changed.";
				break;
				case PBT_APMOEMEVENT:
					report = "OEM-defined event occurred.";
				break;
				case PBT_APMRESUMEAUTOMATIC:
					report = "Operation resuming automatically after event.";
				break;
				case PBT_POWERSETTINGCHANGE:
					// report = "Power setting has changed";
				break;
				default:
					report = "Unknown reason:" + reason.ToString();
				break;
			}
			if (System::String::Empty != report) {
					///Use this in case the kind of power change need to be displayed.
					///eventsListBox.Items.Add(report);
					///
			}
		}

		private: void PowerSettingChange(IntPtr lParam) {
//			System::Windows::Forms::DialogResult r = System::Windows::Forms::MessageBox::Show ("Test", "2", MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
//		  this->report = "PowerSettingChange";

		  POWERBROADCAST_SETTING ^ps = safe_cast<POWERBROADCAST_SETTING^>(Marshal::PtrToStructure(lParam, POWERBROADCAST_SETTING::typeid));

			IntPtr pData = safe_cast<IntPtr>(safe_cast<int>(lParam) + Marshal::SizeOf(ps));

			// Handle a change to the power plan.
			if (ps->PowerSetting == GUID_POWERSCHEME_PERSONALITY) {
				SetPowerPlan(ps, pData);
			} else if (ps->DataLength == Marshal::SizeOf(Int32::typeid)) {
				Int32 iData = safe_cast<Int32>(Marshal::PtrToStructure(pData, Int32::typeid));
				if (ps->PowerSetting == GUID_BATTERY_PERCENTAGE_REMAINING) {
						SetBatteryLevel(iData);
				} else if (ps->PowerSetting == GUID_MONITOR_POWER_ON) {
						SetMonitorState(iData);
				} else if (ps->PowerSetting == GUID_ACDC_POWER_SOURCE) {
						SetPowerSource(iData);
				}
			}
		}

		void SetBatteryLevel(Int32 iData) {
			System::String ^eventText = "Battery percentage remaining chenged";
				//iData is the level of battery charge remaining
		}

		private: void SetPowerSource(Int32 iData) {
			String^ powerSrc;
			switch (iData) {
				case 0:
					runningOnDC = false;
					powerSrc = "AC";
				break;
				case 1:
					runningOnDC = true;
					powerSrc = "DC";
				break;
				case 2:
					runningOnDC = true;
					powerSrc = "Short term DC (e.g. UPS)";
				break;
				default:
					powerSrc = "Unknown";
				break;
			}
		}

		private: void SetMonitorState(Int32 iData) {
			String^ eventText;
			switch (iData) {
				case 0:
					monitorOn = false;
					eventText = "Monitor powered down";
				break;
				case 1:
					monitorOn = true;
					eventText = "Monitor powered up";
				break;
				default:
					eventText = "Unknown monitor event";
				break;
			}
		}

		private: void SetPowerPlan(POWERBROADCAST_SETTING^ ps, IntPtr pData) {
			String^ eventText;
			if (ps->DataLength == Marshal::SizeOf(Guid::typeid)) {
				Guid newPersonality = (Guid)Marshal::PtrToStructure(pData, Guid::typeid);
				if (newPersonality == GUID_MAX_POWER_SAVINGS) {
					powerPlan = PPPowerSaver;
					eventText = "Switched to Max Power savings";
				} else if (newPersonality == GUID_MIN_POWER_SAVINGS) {
					powerPlan = PPHighPerformance;
					eventText = "Switched to Min Power savings";
				} else if (newPersonality == GUID_TYPICAL_POWER_SAVINGS) {
					powerPlan = PPAutomatic;
					eventText = "Switched to Automatic Power savings";
				} else {
					eventText = "switched to unknown Power savings";
				}
			}
		}
	
		#pragma region Vista Platform SDK APIs and declarations
		// These Guids can be found in WinNT.h in the Vista Platform SDK

		private:
		static Guid GUID_BATTERY_PERCENTAGE_REMAINING = Guid("A7AD8041-B45A-4CAE-87A3-EECBB468A9E1");
		static Guid GUID_MONITOR_POWER_ON = Guid(0x02731015, 0x4510, 0x4526, 0x99, 0xE6, 0xE5, 0xA1, 0x7E, 0xBD, 0x1A, 0xEA);
		static Guid GUID_ACDC_POWER_SOURCE = Guid(0x5D3E9A59, 0xE9D5, 0x4B00, 0xA6, 0xBD, 0xFF, 0x34, 0xFF, 0x51, 0x65, 0x48);
		static Guid GUID_POWERSCHEME_PERSONALITY = Guid(0x245D8541, 0x3943, 0x4422, 0xB0, 0x25, 0x13, 0xA7, 0x84, 0xF6, 0x79, 0xB7);

		// POWER PERSONALITIES

		// Maximum Power Savings - Very aggressive power savings
		// measures are used to help stretch battery life.
		static Guid GUID_MAX_POWER_SAVINGS = Guid(0xA1841308, 0x3541, 0x4FAB, 0xBC, 0x81, 0xF7, 0x15, 0x56, 0xF2, 0x0B, 0x4A);
		// No Power Savings - Almost no power savings measures are used.
		static Guid GUID_MIN_POWER_SAVINGS = Guid(0x8C5E7FDA, 0xE8BF, 0x4A96, 0x9A, 0x85, 0xA6, 0xE2, 0x3A, 0x8C, 0x63, 0x5C);
		// Typical Power Savings - Fairly aggressive power savings measures are used.
		static Guid GUID_TYPICAL_POWER_SAVINGS = Guid(0x381B4222, 0xF694, 0x41F0, 0x96, 0x85, 0xFF, 0x5B, 0xB2, 0x60, 0xDF, 0x2E);

		static int jx = 1;
		// Win32 decls and defs
		//
/*		int WM_POWERBROADCAST = 0x0218;

		literal int PBT_APMQUERYSUSPEND = 0x0000;
		literal int PBT_APMQUERYSTANDBY = 0x0001;
		literal int PBT_APMQUERYSUSPENDFAILED = 0x0002;
		literal int PBT_APMQUERYSTANDBYFAILED = 0x0003;
		literal int PBT_APMSUSPEND = 0x0004;
		literal int PBT_APMSTANDBY = 0x0005;
		literal int PBT_APMRESUMECRITICAL = 0x0006;
		literal int PBT_APMRESUMESUSPEND = 0x0007;
		literal int PBT_APMRESUMESTANDBY = 0x0008;
		literal int PBT_APMBATTERYLOW = 0x0009;
		literal int PBT_APMPOWERSTATUSCHANGE = 0x000A; // power status
		literal int PBT_APMOEMEVENT = 0x000B;
		literal int PBT_APMRESUMEAUTOMATIC = 0x0012;
		literal int PBT_POWERSETTINGCHANGE = 0x8013; // DPPE


		literal int DEVICE_NOTIFY_WINDOW_HANDLE = 0x00000000;
		literal int DEVICE_NOTIFY_SERVICE_HANDLE = 0x00000001;
		*/

		private:
		[DllImport("User32", EntryPoint = "RegisterPowerSettingNotification", CallingConvention = CallingConvention::StdCall)]
		static IntPtr RegisterPowerSettingNotification(IntPtr hRecipient, Guid %PowerSettingGuid, Int32 Flags);

		[DllImport("User32", EntryPoint = "UnregisterPowerSettingNotification", CallingConvention = CallingConvention::StdCall)]
		static bool UnregisterPowerSettingNotification(IntPtr handle);

		public:
		[DllImport("Kernel32.DLL", CharSet = CharSet::Auto, SetLastError = true)]
		static EXECUTION_STATE SetThreadExecutionState(EXECUTION_STATE state);
		#pragma endregion
	};
};