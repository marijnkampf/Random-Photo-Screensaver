
/*****************************************************************
* Copyright (C) 2005-2011 Marijn Kampf <mkampf (at) abscreensavers (dot) com>
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

#include "stdafx.h" 
#include "vcclr.h"
//#include "Engine.h"
#include "autoRegistry.h"
#include "multiMonitor.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Xml::Serialization;
using namespace System::IO;

extern std::vector<TMultiMonitor*> multiMonitors;

#define PAYPAL_URL "http://www.abscreensavers.com/rps/paypal"
#define METADATASUBFOLDER "metadata"

namespace nsRandomPhotoScreensaver {
	enum TClockType { ctNone, ctCurrent, ctRunning };
	enum TWallpaperFrequency { wfNone, wfDaily, wfRun };
	enum TRawCacheDimensions { rcdMonitor, rcdOriginal };	
	enum TScreensaverAction { saRun, saConfig, saWallpaper, saPreview };
	enum TCalendarsVisibility { cvNone, cvLow, cvHigh };
	enum TConfigAction { caNone, caFoldersChanged };

	/// <summary>
	/// Summary for fConfig
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>

	// Global version of configuration
	//ref class fConfig;
	//gcroot<fConfig^> config;

	ref class fConfig;
//	ref class Engine;
	gcroot<fConfig^> config;

	public ref class fConfig : public System::Windows::Forms::Form {
	private:
		AutoRegistry^ pRegistry;
		int oldClockMonitorIndex;
	public:
		System::Windows::Forms::Form^ engine;
		//Engine^ engine;

		TConfigAction configAction;

		int screenMaxDimension; // Used to resize RAW files
		System::Drawing::Rectangle desktop;
		bool calendarsNotDefined;
		bool metadataNotDefined;
		TCalendarsVisibility calendarsVisible;
		TCalendarsVisibility metadataVisible;
		array<String^> ^mdTemplate;
		array<String^> ^mdStylesheet;
		array<int> ^mdMonitor;
		bool changed;
		array<Drawing::Rectangle> ^calendarForms;
		array<Drawing::Rectangle> ^metadataForms;
		array<TClockType> ^clockType;
		array<Drawing::Font^> ^clockFont;
		array<Drawing::Color> ^clockColor;
		array<String^> ^lastFile;
		Drawing::Font^ filenameFont;
		String^ appDataFolder;
		bool engineRunning;
		TScreensaverAction action;
		DateTime dtStart;
		String^ _tbFolderValue;

private: System::Windows::Forms::Button^  btnCancel;
private: System::Windows::Forms::Button^  btnCopyMonInfo;
private: System::Windows::Forms::Button^  btnDonate;
private: System::Windows::Forms::Button^  btnFolder;
private: System::Windows::Forms::Button^  btnOk;

private: System::Windows::Forms::Button^  btnWallpaperFolder;
private: System::Windows::Forms::Button^  button1;
	public: System::Windows::Forms::CheckBox^  cbEnableRaw;


private: System::Windows::Forms::ComboBox^  cbClockMonitor;
private: System::Windows::Forms::ComboBox^  cbQuickSimpleMetadata;
private: System::Windows::Forms::FlowLayoutPanel^  flowLayoutPanel1;
private: System::Windows::Forms::FlowLayoutPanel^  flowLayoutPanel2;
private: System::Windows::Forms::FlowLayoutPanel^  flowLayoutPanel3;
private: System::Windows::Forms::FlowLayoutPanel^  flowLayoutPanel4;
private: System::Windows::Forms::FlowLayoutPanel^  flowLayoutPanel5;
private: System::Windows::Forms::GroupBox^  groupBox1;
private: System::Windows::Forms::GroupBox^  groupBox2;
private: System::Windows::Forms::GroupBox^  groupBox3;
private: System::Windows::Forms::GroupBox^  groupBox4;
private: System::Windows::Forms::GroupBox^  groupBox5;
private: System::Windows::Forms::GroupBox^  groupBox6;
	private: System::Windows::Forms::GroupBox^  gbRawAdvanced;


private: System::Windows::Forms::Label^  lCalendarInfo;
private: System::Windows::Forms::Label^  lMonitors;
private: System::Windows::Forms::Label^  label10;
private: System::Windows::Forms::Label^  label11;
private: System::Windows::Forms::Label^  lVersionInfo;
private: System::Windows::Forms::Label^  label13;
private: System::Windows::Forms::Label^  label14;
private: System::Windows::Forms::Label^  label15;
private: System::Windows::Forms::Label^  label16;
private: System::Windows::Forms::Label^  label17;
private: System::Windows::Forms::Label^  label18;
private: System::Windows::Forms::Label^  label19;
private: System::Windows::Forms::Label^  label1;
private: System::Windows::Forms::Label^  label20;
private: System::Windows::Forms::Label^  label21;
private: System::Windows::Forms::Label^  label22;
private: System::Windows::Forms::Label^  label23;
private: System::Windows::Forms::Label^  label24;
private: System::Windows::Forms::Label^  label25;
private: System::Windows::Forms::Label^  label26;
private: System::Windows::Forms::Label^  label27;
private: System::Windows::Forms::Label^  label28;
private: System::Windows::Forms::Label^  label29;
private: System::Windows::Forms::Label^  label2;
private: System::Windows::Forms::Label^  label30;	
private: System::Windows::Forms::Label^  label31;
private: System::Windows::Forms::Label^  label32;
private: System::Windows::Forms::Label^  label33;
private: System::Windows::Forms::Label^  label34;
private: System::Windows::Forms::Label^  label35;
private: System::Windows::Forms::Label^  label36;
private: System::Windows::Forms::Label^  label37;
private: System::Windows::Forms::Label^  label38;
private: System::Windows::Forms::Label^  label39;
private: System::Windows::Forms::Label^  label3;
private: System::Windows::Forms::Label^  label40;
private: System::Windows::Forms::Label^  label41;
private: System::Windows::Forms::Label^  label42;
private: System::Windows::Forms::Label^  label43;
private: System::Windows::Forms::Label^  label44;
private: System::Windows::Forms::Label^  label45;
private: System::Windows::Forms::Label^  label46;
private: System::Windows::Forms::Label^  label47;
private: System::Windows::Forms::Label^  label48;
private: System::Windows::Forms::Label^  label49;
private: System::Windows::Forms::Label^  label4;


private: System::Windows::Forms::Label^  label51;
private: System::Windows::Forms::Label^  label5;
private: System::Windows::Forms::Label^  label6;
private: System::Windows::Forms::Label^  label7;
private: System::Windows::Forms::Label^  label8;
private: System::Windows::Forms::Label^  label9;
private: System::Windows::Forms::Panel^  panel1;
private: System::Windows::Forms::Panel^  panel2;
private: System::Windows::Forms::Panel^  panel3;
private: System::Windows::Forms::Panel^  panel4;
private: System::Windows::Forms::PictureBox^  pictureBox1;
private: System::Windows::Forms::RadioButton^  rbClockNo;
private: System::Windows::Forms::RadioButton^  rbClockRun;
private: System::Windows::Forms::RadioButton^  rbClockTime;



private: System::Windows::Forms::RadioButton^  rbWallpaperDaily;
private: System::Windows::Forms::RadioButton^  rbWallpaperNone;
private: System::Windows::Forms::RadioButton^  rbWallpaperRun;
private: System::Windows::Forms::RichTextBox^  rtbDonate;
private: System::Windows::Forms::RichTextBox^  rtbDonation;
private: System::Windows::Forms::RichTextBox^  rtbShortcutKeys;
private: System::Windows::Forms::SplitContainer^  splitContainer1;
private: System::Windows::Forms::TabControl^  tcTabs;
private: System::Windows::Forms::TabPage^  tpAdvanced;
private: System::Windows::Forms::TabPage^  tpCalendar;
private: System::Windows::Forms::TabPage^  tpClock;
private: System::Windows::Forms::TabPage^  tpGeneral;
private: System::Windows::Forms::TabPage^  tpMetadata;
private: System::Windows::Forms::TabPage^  tpMonitors;
private: System::Windows::Forms::TabPage^  tpRaw;
private: System::Windows::Forms::TabPage^  tpShortcuts;
private: System::Windows::Forms::TabPage^  tpSupport;
private: System::Windows::Forms::TabPage^  tpWallpaper;
private: System::Windows::Forms::TextBox^  tbInfo;
public: System::Windows::Forms::Button^  btnAllBackgrounds;
public: System::Windows::Forms::Button^  btnBackgroundColor;
public: System::Windows::Forms::Button^  btnCalDayBack;
public: System::Windows::Forms::Button^  btnCalDayFront;
public: System::Windows::Forms::Button^  btnCalDayOtherBack;
public: System::Windows::Forms::Button^  btnCalDayOtherFront;
public: System::Windows::Forms::Button^  btnCalDaysOtherBack;
public: System::Windows::Forms::Button^  btnCalDaysOtherFront;
public: System::Windows::Forms::Button^  btnCalTitleBack;
public: System::Windows::Forms::Button^  btnCalTitleFront;
public: System::Windows::Forms::Button^  btnCalTodayBack;
public: System::Windows::Forms::Button^  btnCalTodayFront;
public: System::Windows::Forms::Button^  btnCalendarFont;
public: System::Windows::Forms::Button^  btnClockFont;
public: System::Windows::Forms::Button^  btnDayNamesBack;
public: System::Windows::Forms::Button^  btnDayNamesFront;
public: System::Windows::Forms::Button^  btnFilenamefont;
public: System::Windows::Forms::Button^  btnMetadataFont;
public: System::Windows::Forms::Button^  btnWallpaperBackgroundColor;
public: System::Windows::Forms::Button^  btnWallpaperFilenameFont;
public: System::Windows::Forms::Button^  button3;
public: System::Windows::Forms::Button^  button4;
public: System::Windows::Forms::Button^  button6;
public: System::Windows::Forms::CheckBox^  cbAnimatedTransitions;
public: System::Windows::Forms::CheckBox^  cbCloseAfterImageLocate;
public: System::Windows::Forms::CheckBox^  cbCurrentMonthCalendar;
public: System::Windows::Forms::CheckBox^  cbDebug;
public: System::Windows::Forms::CheckBox^  cbDisplayFilenames;
public: System::Windows::Forms::CheckBox^  cbExifRotate;
public: System::Windows::Forms::CheckBox^  cbFolderInfo;
public: System::Windows::Forms::CheckBox^  cbHideHidden;
public: System::Windows::Forms::CheckBox^  cbHideTopLevelFolders;
public: System::Windows::Forms::CheckBox^  cbMetadata;
public: System::Windows::Forms::CheckBox^  cbMonitorSequence;
public: System::Windows::Forms::CheckBox^  cbOrdered;
public: System::Windows::Forms::CheckBox^  cbPanoramaStretch;
public: System::Windows::Forms::CheckBox^  cbRandomPositions;
public: System::Windows::Forms::CheckBox^  cbSameImage;
public: System::Windows::Forms::CheckBox^  cbStretchSmall;
public: System::Windows::Forms::CheckBox^  cbWallpaperFilenames;
public: System::Windows::Forms::CheckBox^  cbWallpaperFolder;
public: System::Windows::Forms::CheckBox^  cbWallpaperStretch;
public: System::Windows::Forms::ComboBox^  cbLocation;
public: System::Windows::Forms::ComboBox^  cbMouseSensitivity;
public: System::Windows::Forms::NumericUpDown^  nupFolderInfoTimeout;
public: System::Windows::Forms::NumericUpDown^  nupMetadata;
public: System::Windows::Forms::NumericUpDown^  nupNrFolCalendars;
public: System::Windows::Forms::NumericUpDown^  nupNrPrevCalendars;
public: System::Windows::Forms::NumericUpDown^  nupTimeout;
public: System::Windows::Forms::TextBox^  tbClockMask;
public: System::Windows::Forms::TextBox^  tbExcludeRegEx;
public: System::Windows::Forms::TextBox^  tbExcludeSubfolders;
public: System::Windows::Forms::TextBox^  tbFolder;

public: System::Windows::Forms::TextBox^  tbSimpleMetadata;
public: System::Windows::Forms::TextBox^  tbWallpaperFolder;
public: System::Windows::Forms::TrackBar^  tbHigh;
public: System::Windows::Forms::TrackBar^  tbLow;
public: System::Windows::Forms::TrackBar^  tbMDHigh;
public: System::Windows::Forms::TrackBar^  tbMDLow;

public: 


public: 
private: 

private: System::Windows::Forms::Label^  label53;


private: 

public: 

public: 


public: TWallpaperFrequency wallpaperFrequency;
private: System::Windows::Forms::Label^  label55;
public: 
public: System::Windows::Forms::TextBox^  tbRawParameters;
private: 
private: System::Windows::Forms::Label^  label54;
public: 
private: System::Windows::Forms::Label^  label52;
public: System::Windows::Forms::TextBox^  tbRawConverter;
private: 
private: System::Windows::Forms::Button^  btnRawConverter;
public: 
public: System::Windows::Forms::CheckBox^  cbRawCacheInFolder;
private: 
private: System::Windows::Forms::GroupBox^  groupBox8;
public: 
private: System::Windows::Forms::RadioButton^  rbRawCacheDimensionsOriginal;
public: System::Windows::Forms::RadioButton^  rbRawCacheDimensionsMonitor;
private: 
private: System::Windows::Forms::Label^  lSeparateFolder;
public: 
public: System::Windows::Forms::TextBox^  tbRawCacheFolder;
private: 
private: System::Windows::Forms::Button^  btnRawCacheFolder;
public: 
public: System::Windows::Forms::TextBox^  tbRawExtensions;
public: System::Windows::Forms::CheckBox^  cbOnlyEscapeExits;
public: System::Windows::Forms::CheckBox^  cbDeleteKey;

public: System::Windows::Forms::CheckBox^  cbConfirmDelete;
private: 
public: TRawCacheDimensions rawCacheDimensions;

	public:
		fConfig(TScreensaverAction action, array<IntPtr>^ hwnd)	{
			engine = nullptr;
			configAction = caNone;
			calendarsNotDefined = true;
			metadataNotDefined = true;
			calendarsVisible = cvNone;
			metadataVisible = cvNone;
			this->action = action;
			engineRunning = false;
			changed = false;
			this->dtStart = DateTime::Now;
			InitializeComponent();

			desktop = initMonitors(action==saPreview, hwnd);
			clockType = gcnew array<TClockType>(multiMonitors.size());
			clockFont = gcnew array<Drawing::Font^>(multiMonitors.size());
			clockColor = gcnew array<Drawing::Color>(multiMonitors.size());
			lastFile = gcnew array<String^>(multiMonitors.size());

			screenMaxDimension = 0;
			/* Init clock monitor control box */
			for(unsigned int i = 0; i < multiMonitors.size(); i++) {
				screenMaxDimension = Math::Max(screenMaxDimension, Math::Max(multiMonitors[i]->width, multiMonitors[i]->height));
				cbClockMonitor->Items->Add("Monitor " + (i+1));
				this->clockType[i] = ctNone;
				this->clockFont[i] = btnClockFont->Font;				
				this->clockColor[i] = btnClockFont->ForeColor;
				lastFile[i] = "";
			}
			cbClockMonitor->SelectedIndex = 0;
			oldClockMonitorIndex = cbClockMonitor->SelectedIndex;
			if (multiMonitors.size() == 1) {
				lMonitors->Visible = false;
				cbClockMonitor->Visible = false;
				tcTabs->Controls->RemoveByKey("tpMonitors");
			}

			rtbDonation->Rtf = rtbDonation->Text;
			rtbShortcutKeys->Rtf = rtbShortcutKeys->Text;
			rtbDonate->Rtf = rtbDonate->Text;

			// Test if old registry key exists
			// If it does and there is no new one yet, open old and safe it to the new one.
			String^ oldRegKey = "Software\\abScreensavers.com\\rps3.0";
			String^ newRegKey = "Software\\abScreensavers.com\\Random Photo Screensaver";
			RegistryKey^ rk = Registry::CurrentUser->OpenSubKey(oldRegKey);
			bool oldRegKeyExists;
			bool newRegKeyExists;
			if (rk) oldRegKeyExists = true;
			else oldRegKeyExists = false;
			rk = Registry::CurrentUser->OpenSubKey(newRegKey);
			if (rk) newRegKeyExists = true;
			else newRegKeyExists = false;

			if (oldRegKeyExists && !newRegKeyExists) {
				pRegistry = gcnew AutoRegistry(oldRegKey);
				this->TransferPersistent(false);
				this->Transfer(false);
				pRegistry->SetRegKey(newRegKey);
				this->initAppDataFolder();
				this->TransferPersistent(true);
				this->Transfer(true);
			} else {
				pRegistry = gcnew AutoRegistry(newRegKey);
				this->initAppDataFolder();
				this->TransferPersistent(false);
				this->Transfer(false);
			}

			this->updateHistoryValues();

			const String^ cSimpleMetadataFile = "simpleMetadata.txt";
			if (File::Exists(addTrailingSlash(appDataFolder) + cSimpleMetadataFile)) {
				array<String^>^ lines = File::ReadAllLines(addTrailingSlash(appDataFolder) + cSimpleMetadataFile);
				for(int i = 0; i < lines->Length; i++) {
					cbQuickSimpleMetadata->Items->Add(lines[i]);
				}				
				cbQuickSimpleMetadata->SelectedIndex = 0;
			} else cbQuickSimpleMetadata->Visible = false;
		}

		public: void setEngine(System::Windows::Forms::Form^ engine) {
//			safe_cast<nsRandomPhotoScreensaver::Engine>(engine)->test();
			this->engine = engine;
			this->engine->Text = this->lVersionInfo->Text;
		}

		String^ addTrailingSlash(String^ s) {
			if (s->Length > 0) {
				if (s[s->Length-1] != '\\') s += "\\";
			} else {
				s += "\\";
			}
			return s;
		}

		void updateHistoryValues() {
			this->_tbFolderValue = this->tbFolder->Text;
		}

		void initAppDataFolder() {
			this->appDataFolder = addTrailingSlash((String^)pRegistry->Add(false, appDataFolder, "appDataFolder", System::Environment::ExpandEnvironmentVariables("%AppData%") + "\\abScreensavers.com\\Random Photo Screensaver"));
		}

		void resizeNrCalendars() {
			if ((calendarsNotDefined) || (this->getNumberCalendars() != calendarForms->Length)) {
				if (calendarsNotDefined) calendarForms = gcnew array<Drawing::Rectangle> (int(this->getNumberCalendars()));
				else Array::Resize(calendarForms, int(this->getNumberCalendars()));
				calendarsNotDefined = false;
			}
		}

		void resizeNrMetadata() {
			resizeNrMetadata(false);
		}

		void resizeNrMetadata(bool save) {
			int start = 0;
			if ((metadataNotDefined) || (this->nupMetadata->Value != metadataForms->Length)) {
				if (metadataNotDefined) {
					metadataForms = gcnew array<Drawing::Rectangle> (int(this->nupMetadata->Value));
				} else Array::Resize(metadataForms, int(this->nupMetadata->Value));
			}
			if ((metadataNotDefined) || (this->nupMetadata->Value != mdTemplate->Length)) {
				if (metadataNotDefined) mdTemplate = gcnew array<String^>((int)nupMetadata->Value);
				else Array::Resize(mdTemplate, (int)nupMetadata->Value);
			}
			if ((metadataNotDefined) || (this->nupMetadata->Value != mdStylesheet->Length)) {
				if (metadataNotDefined) mdStylesheet = gcnew array<String^>((int)nupMetadata->Value);
				else Array::Resize(mdStylesheet, (int)nupMetadata->Value);
			}
			if ((metadataNotDefined) || (this->nupMetadata->Value != mdMonitor->Length)) {
				if (metadataNotDefined) mdMonitor = gcnew array<int>((int)nupMetadata->Value);
				else Array::Resize(mdMonitor, (int)nupMetadata->Value);
				for(int i = start; i < nupMetadata->Value; i++) mdMonitor[i] = i % multiMonitors.size();

			}
			TransferBoundsArray(save, metadataForms, "metadataForm");
			if (save) start = 0;
			for(int i = start; i < nupMetadata->Value; i++) {
				mdTemplate[i] = (String^)pRegistry->Add(save, mdTemplate[i], "metadataTemplate"+(i+1), "default.html");
				mdStylesheet[i] = (String^)pRegistry->Add(save, mdStylesheet[i], "metadataStylesheet"+(i+1), "system.css");
				mdMonitor[i] = pRegistry->Add(save, mdMonitor[i], "metadataMonitorNr"+(i+1), mdMonitor[i]);
			}
			metadataNotDefined = false;
		}

		void updateWallpaperFolderSelection() {
			tbWallpaperFolder->Enabled = cbWallpaperFolder->Checked;
			btnWallpaperFolder->Enabled = cbWallpaperFolder->Checked;
		}


		void setClockSettingsToBtn(int m) {
			btnClockFont->ForeColor = this->clockColor[m];
			btnClockFont->BackColor = adjustToBrightness(btnClockFont->ForeColor);
			btnClockFont->Font = this->clockFont[m];

			/* ClockType */
			switch(clockType[m]) {
				case ctNone: rbClockNo->Checked = true; break;
				case ctRunning: rbClockRun->Checked = true; break;
				default: /*ctCurrent*/ rbClockTime->Checked = true; break;
			}
		}

		void getClockSettingsFromBtn(int m) {
			this->clockColor[m] = btnClockFont->ForeColor;
			this->clockFont[m] = btnClockFont->Font;

			/* ClockType */
			if (rbClockNo->Checked) clockType[m] = ctNone;
			else if (rbClockRun->Checked) clockType[m] = ctRunning;
			else clockType[m] = ctCurrent;
		}

		void TransferBoundsArray(bool save, array<Drawing::Rectangle> ^bounds, String^ registryName) {
			TransferBoundsArray(save, bounds, registryName, 25, 250);
		}

		void TransferBoundsArray(bool save, array<Drawing::Rectangle>^ bounds, String^ registryName, int margin, int size) {
			for(int i = 0; i < bounds->Length; i++) {
				if ((bounds[i].Width == 0) && (bounds[i].Height == 0) && 
					(bounds[i].Top == 0) && (bounds[i].Left == 0)) {
					int x = margin+i*(size+margin);
					int y = multiMonitors[0]->height/2;

					if ((x > this->desktop.Width)) {
						y = multiMonitors[0]->height/2 + int(double(x / this->desktop.Width)) * margin;
						x = x % this->desktop.Width;
					}
					bounds[i] = System::Drawing::Rectangle(x, y, size, size);
				}
				bounds[i] = (System::Drawing::Rectangle)pRegistry->Add(save, bounds[i], registryName + (i+1), bounds[i]);				
			}
		}

		void TransferForms(bool save) {
			resizeNrCalendars();
			resizeNrMetadata(save);
			calendarsVisible = (TCalendarsVisibility)pRegistry->Add(save, (int)this->calendarsVisible, "calendarsVisible", (int)this->calendarsVisible);
			metadataVisible = (TCalendarsVisibility)pRegistry->Add(save, (int)this->metadataVisible, "metadataVisible", (int)this->metadataVisible);

			if (calendarForms->Length > 0) TransferBoundsArray(save, calendarForms, "calendarForm");
			if (metadataForms->Length > 0) TransferBoundsArray(save, metadataForms, "metadataForm");
		}

		void TransferPersistent(bool save) {
			// Save last file settings for ordered display
			for(unsigned int i = 0; i < multiMonitors.size(); i++) {
				this->lastFile[i] = (String^)pRegistry->Add(save, lastFile[i], "lastFile"+(i+1), lastFile[i]);
			}
			TransferForms(save);
		}

		int getNumberCalendars() {
			int i = 0;
			if (this->cbCurrentMonthCalendar->Checked) i++;
			i = i + int(nupNrPrevCalendars->Value);
			i = i + int(nupNrFolCalendars->Value);
			return i;
		}

		void Transfer(bool save) {
			/* General */
			tbFolder->Text = (String^)pRegistry->Add(save, tbFolder->Text, "folder", Environment::GetFolderPath(Environment::SpecialFolder::MyPictures));
			nupTimeout->Value = (int)pRegistry->Add(save, (int)nupTimeout->Value, "timeout", (int)nupTimeout->Value);
			cbStretchSmall->Checked = pRegistry->Add(save, cbStretchSmall->Checked, "stretchSmallImages", cbStretchSmall->Checked);
			cbRandomPositions->Checked = pRegistry->Add(save, cbRandomPositions->Checked, "randomPositionsSmallImages", cbRandomPositions->Checked);			
			cbExifRotate->Checked = pRegistry->Add(save, cbExifRotate->Checked, "exifRotate", cbExifRotate->Checked);			
			cbAnimatedTransitions->Checked = pRegistry->Add(save, cbAnimatedTransitions->Checked, "animatedTransitions", cbAnimatedTransitions->Checked);
			btnBackgroundColor->BackColor = System::Drawing::Color::FromArgb((int)pRegistry->Add(save, btnBackgroundColor->BackColor.ToArgb(), "backgroundColor", btnBackgroundColor->BackColor.ToArgb()));
			btnBackgroundColor->ForeColor = adjustToBrightness(btnBackgroundColor->BackColor);
			cbDisplayFilenames->Checked = pRegistry->Add(save, cbDisplayFilenames->Checked, "displayFilenames", cbDisplayFilenames->Checked);
			cbHideTopLevelFolders->Checked = pRegistry->Add(save, cbHideTopLevelFolders->Checked, "hideTopLevelFolders", cbHideTopLevelFolders->Checked);
			btnFilenamefont->ForeColor = System::Drawing::Color::FromArgb((int)pRegistry->Add(save, btnFilenamefont->ForeColor.ToArgb(), "filenameFontColor", btnFilenamefont->ForeColor.ToArgb()));
			btnFilenamefont->BackColor = adjustToBrightness(btnFilenamefont->ForeColor);
			filenameFont = btnFilenamefont->Font;
			btnFilenamefont->Font = pRegistry->Add(save, filenameFont, "filenameFont", filenameFont);
			filenameFont = btnFilenamefont->Font;			

			/* Advanced */
			tbExcludeSubfolders->Text = (String^)pRegistry->Add(save, tbExcludeSubfolders->Text, "excludedSubfolders", tbExcludeSubfolders->Text);
			tbExcludeRegEx->Text = (String^)pRegistry->Add(save, tbExcludeRegEx->Text, "excludeRegEx", tbExcludeRegEx->Text);
			cbHideHidden->Checked = pRegistry->Add(save, cbHideHidden->Checked, "hideHidden", cbHideHidden->Checked);
			cbOrdered->Checked = pRegistry->Add(save, cbOrdered->Checked, "ordered", cbOrdered->Checked);
			cbMouseSensitivity->Text = (String^)pRegistry->Add(save, cbMouseSensitivity->SelectedItem, "mouseSensitivity", "Normal");
			cbOnlyEscapeExits->Checked = pRegistry->Add(save, cbOnlyEscapeExits->Checked, "onlyEscapeExits", cbOnlyEscapeExits->Checked);			

			cbFolderInfo->Checked = pRegistry->Add(save, cbFolderInfo->Checked, "folderInfo", cbFolderInfo->Checked);			
			nupFolderInfoTimeout->Value = (int)pRegistry->Add(save, (int)nupFolderInfoTimeout->Value, "folderInfoTimeout", (int)nupFolderInfoTimeout->Value);
			cbDebug->Checked = pRegistry->Add(save, cbDebug->Checked, "debug", cbDebug->Checked);			
			cbCloseAfterImageLocate->Checked = pRegistry->Add(save, cbCloseAfterImageLocate->Checked, "closeAfterImageLocate", cbCloseAfterImageLocate->Checked);			

			cbConfirmDelete->Checked = pRegistry->Add(save, cbConfirmDelete->Checked, "confirmDelete", cbConfirmDelete->Checked);			
			cbDeleteKey->Checked = pRegistry->Add(save, cbDeleteKey->Checked, "deleteKey", cbDeleteKey->Checked);			

			/* Clock */
			//if (!save) getClockSettingsFromBtn(cbClockMonitor->SelectedIndex);
			for(unsigned int i = 0; i < multiMonitors.size(); i++) {
				this->clockColor[i] = System::Drawing::Color::FromArgb((int)pRegistry->Add(save, this->clockColor[i].ToArgb(), "clockFontColor" + (i+1), this->clockColor[i].ToArgb()));
				this->clockFont[i] = pRegistry->Add(save, this->clockFont[i], "clockFont"+(i+1), this->clockFont[i]);
				this->clockType[i] = (TClockType)pRegistry->Add(save, (int)this->clockType[i], "clockType" + (i+1), (int)this->clockType[i]);
			}
			//getClockSettingsFromBtn(cbClockMonitor->SelectedIndex);
			setClockSettingsToBtn(cbClockMonitor->SelectedIndex);
			tbClockMask->Text = (String^)pRegistry->Add(save, tbClockMask->Text, "clockMask", tbClockMask->Text);

			/* Wallpaper */
			if (rbWallpaperRun->Checked) wallpaperFrequency = wfRun;
			else if (rbWallpaperDaily->Checked) wallpaperFrequency = wfDaily;
			else wallpaperFrequency = wfNone;
			wallpaperFrequency = (TWallpaperFrequency)pRegistry->Add(save, (int)wallpaperFrequency, "wallpaperFrequency", (int)wallpaperFrequency);
			switch(wallpaperFrequency) {
				case wfRun: rbWallpaperRun->Checked = true; break;
				case wfDaily: rbWallpaperDaily->Checked = true; break;
				default: rbWallpaperNone->Checked = true; break;
			}

			cbWallpaperFilenames->Checked = pRegistry->Add(save, cbWallpaperFilenames->Checked, "wallpaperShowFilenames", cbWallpaperFilenames->Checked);
			cbWallpaperStretch->Checked = pRegistry->Add(save, cbWallpaperStretch->Checked, "wallpaperStretchSmallImages", cbWallpaperStretch->Checked);
			
			btnWallpaperFilenameFont->ForeColor = System::Drawing::Color::FromArgb((int)pRegistry->Add(save, btnWallpaperFilenameFont->ForeColor.ToArgb(), "wallpaperFilenameFontColor", btnWallpaperFilenameFont->ForeColor.ToArgb()));
			btnWallpaperFilenameFont->BackColor = adjustToBrightness(btnWallpaperFilenameFont->ForeColor);
			btnWallpaperFilenameFont->Font = pRegistry->Add(save, btnWallpaperFilenameFont->Font, "wallpaperFilenameFont", btnWallpaperFilenameFont->Font);

			btnWallpaperBackgroundColor->BackColor = System::Drawing::Color::FromArgb((int)pRegistry->Add(save, btnWallpaperBackgroundColor->BackColor.ToArgb(), "wallpaperBackgroundColor", btnWallpaperBackgroundColor->BackColor.ToArgb()));
			btnWallpaperBackgroundColor->ForeColor = adjustToBrightness(btnWallpaperBackgroundColor->BackColor);

			cbWallpaperFolder->Checked = pRegistry->Add(save, cbWallpaperFolder->Checked, "separateWallpaperFolder", cbWallpaperFolder->Checked);
			tbWallpaperFolder->Text = addTrailingSlash((String^)pRegistry->Add(save, tbWallpaperFolder->Text, "wallpaperFolder", Environment::GetFolderPath(Environment::SpecialFolder::MyPictures)));
			updateWallpaperFolderSelection();

			/* Calendars */
			cbCurrentMonthCalendar->Checked = pRegistry->Add(save, cbCurrentMonthCalendar->Checked, "currentMonthCalendar", cbCurrentMonthCalendar->Checked);
			nupNrPrevCalendars->Value = (int)pRegistry->Add(save, (int)nupNrPrevCalendars->Value, "nrPreviousCalendars", (int)nupNrPrevCalendars->Value);
			nupNrFolCalendars->Value = (int)pRegistry->Add(save, (int)nupNrFolCalendars->Value, "nrFollowingCalendars", (int)nupNrFolCalendars->Value);

			tbLow->Value = (int)pRegistry->Add(save, (int)tbLow->Value, "calendarOpacityLow", (int)tbLow->Value);
			tbHigh->Value = (int)pRegistry->Add(save, (int)tbHigh->Value, "calendarOpacityHigh", (int)tbHigh->Value);

			btnCalendarFont->Font = pRegistry->Add(save, btnCalendarFont->Font, "calendarFont", btnCalendarFont->Font);
			btnCalDayBack->BackColor = System::Drawing::Color::FromArgb((int)pRegistry->Add(save, btnCalDayBack->BackColor.ToArgb(), "calendarDaysBack", btnCalDayBack->BackColor.ToArgb()));
			btnCalDayBack->ForeColor = adjustToBrightness(btnCalDayBack->BackColor);
			btnCalDayFront->BackColor = System::Drawing::Color::FromArgb((int)pRegistry->Add(save, btnCalDayFront->BackColor.ToArgb(), "calendarDaysFront", btnCalDayFront->BackColor.ToArgb()));
			btnCalDayFront->ForeColor = adjustToBrightness(btnCalDayFront->BackColor);
			btnCalDaysOtherBack->BackColor = System::Drawing::Color::FromArgb((int)pRegistry->Add(save, btnCalDaysOtherBack->BackColor.ToArgb(), "calendarDaysOtherBack", btnCalDaysOtherBack->BackColor.ToArgb()));
			btnCalDaysOtherBack->ForeColor = adjustToBrightness(btnCalDaysOtherBack->BackColor);
			btnCalDaysOtherFront->BackColor = System::Drawing::Color::FromArgb((int)pRegistry->Add(save, btnCalDaysOtherFront->BackColor.ToArgb(), "calendarDaysOtherFront", btnCalDaysOtherFront->BackColor.ToArgb()));
			btnCalDaysOtherFront->ForeColor = adjustToBrightness(btnCalDaysOtherFront->BackColor);
			btnCalTitleBack->BackColor = System::Drawing::Color::FromArgb((int)pRegistry->Add(save, btnCalTitleBack->BackColor.ToArgb(), "calendarTitleBack", btnCalTitleBack->BackColor.ToArgb()));
			btnCalTitleBack->ForeColor = adjustToBrightness(btnCalTitleBack->BackColor);
			btnCalTitleFront->BackColor = System::Drawing::Color::FromArgb((int)pRegistry->Add(save, btnCalTitleFront->BackColor.ToArgb(), "calendarTitleFront", btnCalTitleFront->BackColor.ToArgb()));
			btnCalTitleFront->ForeColor = adjustToBrightness(btnCalTitleFront->BackColor);
			btnCalTodayBack->BackColor = System::Drawing::Color::FromArgb((int)pRegistry->Add(save, btnCalTodayBack->BackColor.ToArgb(), "calendarTodayBack", btnCalTodayBack->BackColor.ToArgb()));
			btnCalTodayBack->ForeColor = adjustToBrightness(btnCalTodayBack->BackColor);
			btnCalTodayFront->BackColor = System::Drawing::Color::FromArgb((int)pRegistry->Add(save, btnCalTodayFront->BackColor.ToArgb(), "calendarTodayFront", btnCalTodayFront->BackColor.ToArgb()));
			btnCalTodayFront->ForeColor = adjustToBrightness(btnCalTodayFront->BackColor);
			btnDayNamesBack->BackColor = System::Drawing::Color::FromArgb((int)pRegistry->Add(save, btnDayNamesBack->BackColor.ToArgb(), "calendarDayNamesBack", btnDayNamesBack->BackColor.ToArgb()));
			btnDayNamesBack->ForeColor = adjustToBrightness(btnDayNamesBack->BackColor);
			btnDayNamesFront->BackColor = System::Drawing::Color::FromArgb((int)pRegistry->Add(save, btnDayNamesFront->BackColor.ToArgb(), "calendarDayNamesFront", btnDayNamesFront->BackColor.ToArgb()));
			btnDayNamesFront->ForeColor = adjustToBrightness(btnDayNamesFront->BackColor);

			/* Multi Monitor */
			if (multiMonitors.size() == 1) {
				cbPanoramaStretch->Checked = false; 
				tpMonitors->Visible = false;
			}
			cbPanoramaStretch->Checked = pRegistry->Add(save, cbPanoramaStretch->Checked, "panoramaStretch", cbPanoramaStretch->Checked);			
			cbSameImage->Checked = pRegistry->Add(save, cbSameImage->Checked, "sameImage", cbSameImage->Checked);				
			cbMonitorSequence->Checked = pRegistry->Add(save, cbMonitorSequence->Checked, "monitorSequence", cbMonitorSequence->Checked);				

			/* RAW */
			cbEnableRaw->Checked = pRegistry->Add(save, cbEnableRaw->Checked, "enableRaw", cbEnableRaw->Checked);
			cbRawCacheInFolder->Checked = pRegistry->Add(save, cbRawCacheInFolder->Checked, "rawFolderCache", cbRawCacheInFolder->Checked);

			tbRawCacheFolder->Text = (String^)pRegistry->Add(save, tbRawCacheFolder->Text, "rawCacheFolder", this->appDataFolder + "RAWCache\\");

			if (rbRawCacheDimensionsMonitor->Checked) rawCacheDimensions = rcdMonitor;
			else rawCacheDimensions = rcdOriginal;
			rawCacheDimensions = (TRawCacheDimensions)pRegistry->Add(save, (int)rawCacheDimensions, "rawCacheDimensions", (int)rawCacheDimensions);
			switch(rawCacheDimensions) {
				case rcdMonitor: rbRawCacheDimensionsMonitor->Checked = true; break;
				default: rbRawCacheDimensionsOriginal->Checked = true; break;
			}

			tbRawConverter->Text = (String^)pRegistry->Add(save, tbRawConverter->Text, "rawConverter", Path::GetDirectoryName(System::Reflection::Assembly::GetEntryAssembly()->Location) + "\\ufraw\\ufraw-batch.exe");
			tbRawParameters->Text = (String^)pRegistry->Add(save, tbRawParameters->Text, "rawParameters", "--silent --exposure=auto --out-type=jpg --output=\"#JPG#\" \"#RAW#\"");
			tbRawExtensions->Text = (String^)pRegistry->Add(save, tbRawExtensions->Text, "rawExtensions", ".bay .bmq .cr2 .crw .cs1 .dc2 .dcr .fff .k25 .kdc .mos .mrw .nef .orf .pef .raf .rdc .srf .x3f");

			/* Metadata */
			nupMetadata->Value = (int)pRegistry->Add(save, (int)nupMetadata->Value, "nrMetadata", (int)nupMetadata->Value);
			resizeNrMetadata(save);

			tbMDLow->Value = (int)pRegistry->Add(save, (int)tbMDLow->Value, "metadataOpacityLow", (int)tbMDLow->Value);
			tbMDHigh->Value = (int)pRegistry->Add(save, (int)tbMDHigh->Value, "metadataOpacityHigh", (int)tbMDHigh->Value);

			/* Simple Metadata */	
			cbMetadata->Checked = pRegistry->Add(save, cbMetadata->Checked, "metadata", cbMetadata->Checked);			
			cbLocation->Text = (String^)pRegistry->Add(save, cbLocation->SelectedItem, "simple Metadata Location", "Bottom Right");
			tbSimpleMetadata->Text = (String^)pRegistry->Add(save, tbSimpleMetadata->Text, "simple Metadata", "<#{Exif.Photo.ExposureTime} |#><# {Exif.Photo.FNumber} |#><# {Exif.Photo.ISOSpeedRatings} |#> <# {[Exif\\.Photo\\.FocalLength.*]}#>");

			btnMetadataFont->ForeColor = System::Drawing::Color::FromArgb((int)pRegistry->Add(save, btnMetadataFont->ForeColor.ToArgb(), "metadataFontColor", btnMetadataFont->ForeColor.ToArgb()));
			btnMetadataFont->BackColor = adjustToBrightness(btnMetadataFont->ForeColor);
			btnMetadataFont->Font = pRegistry->Add(save, btnMetadataFont->Font, "metadataFont", btnMetadataFont->Font);

			// Forms
			TransferForms(save);
		}

		Color adjustToBrightness(Color^ to) {
			if (to->GetBrightness() < 0.5) return Color::FromName("White");
			else return Color::FromName("Black");
		}
	public: 
		void configChanged() {
			if (config->changed) {
			//if (System::Windows::Forms::MessageBox::Show ("Save changes?", "Configuration settings changed", MessageBoxButtons::YesNo, MessageBoxIcon::Question) == ::DialogResult::Yes) {
				Transfer(true);
			//}
			}
		}

	protected:		~fConfig() {
			TransferPersistent(true);
			if (components)	{
				delete components;
			}
		}


	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(fConfig::typeid));
			this->tcTabs = (gcnew System::Windows::Forms::TabControl());
			this->tpGeneral = (gcnew System::Windows::Forms::TabPage());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->cbHideTopLevelFolders = (gcnew System::Windows::Forms::CheckBox());
			this->btnFilenamefont = (gcnew System::Windows::Forms::Button());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->cbDisplayFilenames = (gcnew System::Windows::Forms::CheckBox());
			this->flowLayoutPanel1 = (gcnew System::Windows::Forms::FlowLayoutPanel());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->tbFolder = (gcnew System::Windows::Forms::TextBox());
			this->btnFolder = (gcnew System::Windows::Forms::Button());
			this->label16 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->nupTimeout = (gcnew System::Windows::Forms::NumericUpDown());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->btnBackgroundColor = (gcnew System::Windows::Forms::Button());
			this->cbAnimatedTransitions = (gcnew System::Windows::Forms::CheckBox());
			this->label17 = (gcnew System::Windows::Forms::Label());
			this->cbStretchSmall = (gcnew System::Windows::Forms::CheckBox());
			this->cbRandomPositions = (gcnew System::Windows::Forms::CheckBox());
			this->cbExifRotate = (gcnew System::Windows::Forms::CheckBox());
			this->tpAdvanced = (gcnew System::Windows::Forms::TabPage());
			this->flowLayoutPanel4 = (gcnew System::Windows::Forms::FlowLayoutPanel());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->tbExcludeSubfolders = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label15 = (gcnew System::Windows::Forms::Label());
			this->label43 = (gcnew System::Windows::Forms::Label());
			this->tbExcludeRegEx = (gcnew System::Windows::Forms::TextBox());
			this->label47 = (gcnew System::Windows::Forms::Label());
			this->label48 = (gcnew System::Windows::Forms::Label());
			this->cbFolderInfo = (gcnew System::Windows::Forms::CheckBox());
			this->nupFolderInfoTimeout = (gcnew System::Windows::Forms::NumericUpDown());
			this->label18 = (gcnew System::Windows::Forms::Label());
			this->cbHideHidden = (gcnew System::Windows::Forms::CheckBox());
			this->cbOrdered = (gcnew System::Windows::Forms::CheckBox());
			this->label13 = (gcnew System::Windows::Forms::Label());
			this->cbMouseSensitivity = (gcnew System::Windows::Forms::ComboBox());
			this->cbOnlyEscapeExits = (gcnew System::Windows::Forms::CheckBox());
			this->cbDebug = (gcnew System::Windows::Forms::CheckBox());
			this->cbCloseAfterImageLocate = (gcnew System::Windows::Forms::CheckBox());
			this->cbDeleteKey = (gcnew System::Windows::Forms::CheckBox());
			this->cbConfirmDelete = (gcnew System::Windows::Forms::CheckBox());
			this->tpClock = (gcnew System::Windows::Forms::TabPage());
			this->flowLayoutPanel2 = (gcnew System::Windows::Forms::FlowLayoutPanel());
			this->lMonitors = (gcnew System::Windows::Forms::Label());
			this->cbClockMonitor = (gcnew System::Windows::Forms::ComboBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->rbClockRun = (gcnew System::Windows::Forms::RadioButton());
			this->rbClockTime = (gcnew System::Windows::Forms::RadioButton());
			this->rbClockNo = (gcnew System::Windows::Forms::RadioButton());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->btnClockFont = (gcnew System::Windows::Forms::Button());
			this->label49 = (gcnew System::Windows::Forms::Label());
			this->tbClockMask = (gcnew System::Windows::Forms::TextBox());
			this->tpCalendar = (gcnew System::Windows::Forms::TabPage());
			this->lCalendarInfo = (gcnew System::Windows::Forms::Label());
			this->flowLayoutPanel5 = (gcnew System::Windows::Forms::FlowLayoutPanel());
			this->splitContainer1 = (gcnew System::Windows::Forms::SplitContainer());
			this->label22 = (gcnew System::Windows::Forms::Label());
			this->cbCurrentMonthCalendar = (gcnew System::Windows::Forms::CheckBox());
			this->label21 = (gcnew System::Windows::Forms::Label());
			this->label20 = (gcnew System::Windows::Forms::Label());
			this->nupNrFolCalendars = (gcnew System::Windows::Forms::NumericUpDown());
			this->nupNrPrevCalendars = (gcnew System::Windows::Forms::NumericUpDown());
			this->label19 = (gcnew System::Windows::Forms::Label());
			this->label30 = (gcnew System::Windows::Forms::Label());
			this->label29 = (gcnew System::Windows::Forms::Label());
			this->label24 = (gcnew System::Windows::Forms::Label());
			this->tbHigh = (gcnew System::Windows::Forms::TrackBar());
			this->tbLow = (gcnew System::Windows::Forms::TrackBar());
			this->btnCalendarFont = (gcnew System::Windows::Forms::Button());
			this->label23 = (gcnew System::Windows::Forms::Label());
			this->btnAllBackgrounds = (gcnew System::Windows::Forms::Button());
			this->label36 = (gcnew System::Windows::Forms::Label());
			this->label35 = (gcnew System::Windows::Forms::Label());
			this->label34 = (gcnew System::Windows::Forms::Label());
			this->label31 = (gcnew System::Windows::Forms::Label());
			this->label32 = (gcnew System::Windows::Forms::Label());
			this->label33 = (gcnew System::Windows::Forms::Label());
			this->label25 = (gcnew System::Windows::Forms::Label());
			this->btnCalTitleFront = (gcnew System::Windows::Forms::Button());
			this->btnCalTitleBack = (gcnew System::Windows::Forms::Button());
			this->label26 = (gcnew System::Windows::Forms::Label());
			this->btnDayNamesFront = (gcnew System::Windows::Forms::Button());
			this->btnDayNamesBack = (gcnew System::Windows::Forms::Button());
			this->label27 = (gcnew System::Windows::Forms::Label());
			this->btnCalDayFront = (gcnew System::Windows::Forms::Button());
			this->btnCalDayBack = (gcnew System::Windows::Forms::Button());
			this->label28 = (gcnew System::Windows::Forms::Label());
			this->btnCalTodayFront = (gcnew System::Windows::Forms::Button());
			this->btnCalTodayBack = (gcnew System::Windows::Forms::Button());
			this->label37 = (gcnew System::Windows::Forms::Label());
			this->btnCalDaysOtherFront = (gcnew System::Windows::Forms::Button());
			this->btnCalDaysOtherBack = (gcnew System::Windows::Forms::Button());
			this->tpWallpaper = (gcnew System::Windows::Forms::TabPage());
			this->groupBox5 = (gcnew System::Windows::Forms::GroupBox());
			this->flowLayoutPanel3 = (gcnew System::Windows::Forms::FlowLayoutPanel());
			this->cbWallpaperFolder = (gcnew System::Windows::Forms::CheckBox());
			this->tbWallpaperFolder = (gcnew System::Windows::Forms::TextBox());
			this->btnWallpaperFolder = (gcnew System::Windows::Forms::Button());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->btnWallpaperBackgroundColor = (gcnew System::Windows::Forms::Button());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->groupBox4 = (gcnew System::Windows::Forms::GroupBox());
			this->btnWallpaperFilenameFont = (gcnew System::Windows::Forms::Button());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->cbWallpaperStretch = (gcnew System::Windows::Forms::CheckBox());
			this->cbWallpaperFilenames = (gcnew System::Windows::Forms::CheckBox());
			this->groupBox3 = (gcnew System::Windows::Forms::GroupBox());
			this->rbWallpaperNone = (gcnew System::Windows::Forms::RadioButton());
			this->rbWallpaperDaily = (gcnew System::Windows::Forms::RadioButton());
			this->rbWallpaperRun = (gcnew System::Windows::Forms::RadioButton());
			this->tpMonitors = (gcnew System::Windows::Forms::TabPage());
			this->groupBox6 = (gcnew System::Windows::Forms::GroupBox());
			this->label38 = (gcnew System::Windows::Forms::Label());
			this->cbMonitorSequence = (gcnew System::Windows::Forms::CheckBox());
			this->cbSameImage = (gcnew System::Windows::Forms::CheckBox());
			this->tbInfo = (gcnew System::Windows::Forms::TextBox());
			this->btnCopyMonInfo = (gcnew System::Windows::Forms::Button());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->cbPanoramaStretch = (gcnew System::Windows::Forms::CheckBox());
			this->tpRaw = (gcnew System::Windows::Forms::TabPage());
			this->label51 = (gcnew System::Windows::Forms::Label());
			this->cbEnableRaw = (gcnew System::Windows::Forms::CheckBox());
			this->gbRawAdvanced = (gcnew System::Windows::Forms::GroupBox());
			this->label55 = (gcnew System::Windows::Forms::Label());
			this->tbRawParameters = (gcnew System::Windows::Forms::TextBox());
			this->label54 = (gcnew System::Windows::Forms::Label());
			this->label52 = (gcnew System::Windows::Forms::Label());
			this->tbRawConverter = (gcnew System::Windows::Forms::TextBox());
			this->btnRawConverter = (gcnew System::Windows::Forms::Button());
			this->cbRawCacheInFolder = (gcnew System::Windows::Forms::CheckBox());
			this->groupBox8 = (gcnew System::Windows::Forms::GroupBox());
			this->rbRawCacheDimensionsOriginal = (gcnew System::Windows::Forms::RadioButton());
			this->rbRawCacheDimensionsMonitor = (gcnew System::Windows::Forms::RadioButton());
			this->lSeparateFolder = (gcnew System::Windows::Forms::Label());
			this->tbRawCacheFolder = (gcnew System::Windows::Forms::TextBox());
			this->btnRawCacheFolder = (gcnew System::Windows::Forms::Button());
			this->tbRawExtensions = (gcnew System::Windows::Forms::TextBox());
			this->tpShortcuts = (gcnew System::Windows::Forms::TabPage());
			this->rtbShortcutKeys = (gcnew System::Windows::Forms::RichTextBox());
			this->tpMetadata = (gcnew System::Windows::Forms::TabPage());
			this->label42 = (gcnew System::Windows::Forms::Label());
			this->nupMetadata = (gcnew System::Windows::Forms::NumericUpDown());
			this->label41 = (gcnew System::Windows::Forms::Label());
			this->panel4 = (gcnew System::Windows::Forms::Panel());
			this->label44 = (gcnew System::Windows::Forms::Label());
			this->label45 = (gcnew System::Windows::Forms::Label());
			this->label46 = (gcnew System::Windows::Forms::Label());
			this->tbMDHigh = (gcnew System::Windows::Forms::TrackBar());
			this->tbMDLow = (gcnew System::Windows::Forms::TrackBar());
			this->label39 = (gcnew System::Windows::Forms::Label());
			this->label40 = (gcnew System::Windows::Forms::Label());
			this->panel2 = (gcnew System::Windows::Forms::Panel());
			this->btnMetadataFont = (gcnew System::Windows::Forms::Button());
			this->cbQuickSimpleMetadata = (gcnew System::Windows::Forms::ComboBox());
			this->cbLocation = (gcnew System::Windows::Forms::ComboBox());
			this->tbSimpleMetadata = (gcnew System::Windows::Forms::TextBox());
			this->cbMetadata = (gcnew System::Windows::Forms::CheckBox());
			this->tpSupport = (gcnew System::Windows::Forms::TabPage());
			this->rtbDonate = (gcnew System::Windows::Forms::RichTextBox());
			this->btnOk = (gcnew System::Windows::Forms::Button());
			this->btnCancel = (gcnew System::Windows::Forms::Button());
			this->lVersionInfo = (gcnew System::Windows::Forms::Label());
			this->rtbDonation = (gcnew System::Windows::Forms::RichTextBox());
			this->btnDonate = (gcnew System::Windows::Forms::Button());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->label14 = (gcnew System::Windows::Forms::Label());
			this->tcTabs->SuspendLayout();
			this->tpGeneral->SuspendLayout();
			this->groupBox1->SuspendLayout();
			this->flowLayoutPanel1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nupTimeout))->BeginInit();
			this->tpAdvanced->SuspendLayout();
			this->flowLayoutPanel4->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nupFolderInfoTimeout))->BeginInit();
			this->tpClock->SuspendLayout();
			this->flowLayoutPanel2->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->tpCalendar->SuspendLayout();
			this->flowLayoutPanel5->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->splitContainer1))->BeginInit();
			this->splitContainer1->Panel1->SuspendLayout();
			this->splitContainer1->Panel2->SuspendLayout();
			this->splitContainer1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nupNrFolCalendars))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nupNrPrevCalendars))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->tbHigh))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->tbLow))->BeginInit();
			this->tpWallpaper->SuspendLayout();
			this->groupBox5->SuspendLayout();
			this->flowLayoutPanel3->SuspendLayout();
			this->panel1->SuspendLayout();
			this->groupBox4->SuspendLayout();
			this->groupBox3->SuspendLayout();
			this->tpMonitors->SuspendLayout();
			this->groupBox6->SuspendLayout();
			this->tpRaw->SuspendLayout();
			this->gbRawAdvanced->SuspendLayout();
			this->groupBox8->SuspendLayout();
			this->tpShortcuts->SuspendLayout();
			this->tpMetadata->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nupMetadata))->BeginInit();
			this->panel4->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->tbMDHigh))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->tbMDLow))->BeginInit();
			this->panel2->SuspendLayout();
			this->tpSupport->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->BeginInit();
			this->SuspendLayout();
			this->tcTabs->Controls->Add(this->tpGeneral);
			this->tcTabs->Controls->Add(this->tpAdvanced);
			this->tcTabs->Controls->Add(this->tpClock);
			this->tcTabs->Controls->Add(this->tpCalendar);
			this->tcTabs->Controls->Add(this->tpWallpaper);
			this->tcTabs->Controls->Add(this->tpMonitors);
			this->tcTabs->Controls->Add(this->tpRaw);
			this->tcTabs->Controls->Add(this->tpShortcuts);
			this->tcTabs->Controls->Add(this->tpMetadata);
			this->tcTabs->Controls->Add(this->tpSupport);
			this->tcTabs->ItemSize = System::Drawing::Size(55, 18);
			this->tcTabs->Location = System::Drawing::Point(8, 93);
			this->tcTabs->Multiline = true;
			this->tcTabs->Name = L"tcTabs";
			this->tcTabs->SelectedIndex = 0;
			this->tcTabs->Size = System::Drawing::Size(356, 336);
			this->tcTabs->TabIndex = 0;
			this->tpGeneral->Controls->Add(this->groupBox1);
			this->tpGeneral->Controls->Add(this->flowLayoutPanel1);
			this->tpGeneral->Location = System::Drawing::Point(4, 40);
			this->tpGeneral->Name = L"tpGeneral";
			this->tpGeneral->Padding = System::Windows::Forms::Padding(3);
			this->tpGeneral->Size = System::Drawing::Size(348, 292);
			this->tpGeneral->TabIndex = 0;
			this->tpGeneral->Text = L"General";
			this->tpGeneral->UseVisualStyleBackColor = true;
			this->groupBox1->Controls->Add(this->cbHideTopLevelFolders);
			this->groupBox1->Controls->Add(this->btnFilenamefont);
			this->groupBox1->Controls->Add(this->label7);
			this->groupBox1->Controls->Add(this->cbDisplayFilenames);
			this->groupBox1->Dock = System::Windows::Forms::DockStyle::Top;
			this->groupBox1->Location = System::Drawing::Point(3, 169);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(342, 68);
			this->groupBox1->TabIndex = 1;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Filenames";
			this->cbHideTopLevelFolders->AutoSize = true;
			this->cbHideTopLevelFolders->Location = System::Drawing::Point(120, 14);
			this->cbHideTopLevelFolders->Name = L"cbHideTopLevelFolders";
			this->cbHideTopLevelFolders->Size = System::Drawing::Size(219, 17);
			this->cbHideTopLevelFolders->TabIndex = 15;
			this->cbHideTopLevelFolders->Text = L"Remove top level folder(s) from filenames";
			this->cbHideTopLevelFolders->UseVisualStyleBackColor = true;
			this->btnFilenamefont->Font = (gcnew System::Drawing::Font(L"Arial", 8.25, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0), true));
			this->btnFilenamefont->ForeColor = System::Drawing::Color::White;
			this->btnFilenamefont->Location = System::Drawing::Point(83, 30);
			this->btnFilenamefont->Name = L"btnFilenamefont";
			this->btnFilenamefont->Size = System::Drawing::Size(222, 30);
			this->btnFilenamefont->TabIndex = 14;
			this->btnFilenamefont->Text = L"Select Font";
			this->btnFilenamefont->UseVisualStyleBackColor = true;
			this->btnFilenamefont->Click += gcnew System::EventHandler(this, &fConfig::btnFilenamefont_Click);
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(4, 42);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(73, 13);
			this->label7->TabIndex = 13;
			this->label7->Text = L"Filename font:";
			this->cbDisplayFilenames->AutoSize = true;
			this->cbDisplayFilenames->Checked = true;
			this->cbDisplayFilenames->CheckState = System::Windows::Forms::CheckState::Checked;
			this->cbDisplayFilenames->Location = System::Drawing::Point(7, 14);
			this->cbDisplayFilenames->Margin = System::Windows::Forms::Padding(7, 3, 3, 3);
			this->cbDisplayFilenames->Name = L"cbDisplayFilenames";
			this->cbDisplayFilenames->Size = System::Drawing::Size(113, 17);
			this->cbDisplayFilenames->TabIndex = 12;
			this->cbDisplayFilenames->Text = L"Display filename(s)";
			this->cbDisplayFilenames->UseVisualStyleBackColor = true;
			this->flowLayoutPanel1->AutoSize = true;
			this->flowLayoutPanel1->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->flowLayoutPanel1->Controls->Add(this->label1);
			this->flowLayoutPanel1->Controls->Add(this->tbFolder);
			this->flowLayoutPanel1->Controls->Add(this->btnFolder);
			this->flowLayoutPanel1->Controls->Add(this->label16);
			this->flowLayoutPanel1->Controls->Add(this->label4);
			this->flowLayoutPanel1->Controls->Add(this->nupTimeout);
			this->flowLayoutPanel1->Controls->Add(this->label5);
			this->flowLayoutPanel1->Controls->Add(this->label6);
			this->flowLayoutPanel1->Controls->Add(this->btnBackgroundColor);
			this->flowLayoutPanel1->Controls->Add(this->cbAnimatedTransitions);
			this->flowLayoutPanel1->Controls->Add(this->label17);
			this->flowLayoutPanel1->Controls->Add(this->cbStretchSmall);
			this->flowLayoutPanel1->Controls->Add(this->cbRandomPositions);
			this->flowLayoutPanel1->Controls->Add(this->cbExifRotate);
			this->flowLayoutPanel1->Dock = System::Windows::Forms::DockStyle::Top;
			this->flowLayoutPanel1->Location = System::Drawing::Point(3, 3);
			this->flowLayoutPanel1->Margin = System::Windows::Forms::Padding(0);
			this->flowLayoutPanel1->Name = L"flowLayoutPanel1";
			this->flowLayoutPanel1->Size = System::Drawing::Size(342, 166);
			this->flowLayoutPanel1->TabIndex = 0;
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(3, 7);
			this->label1->Margin = System::Windows::Forms::Padding(3, 7, 3, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(72, 13);
			this->label1->TabIndex = 1;
			this->label1->Text = L"Photos folder:";
			this->tbFolder->Location = System::Drawing::Point(81, 3);
			this->tbFolder->Name = L"tbFolder";
			this->tbFolder->Size = System::Drawing::Size(159, 20);
			this->tbFolder->TabIndex = 0;
			this->flowLayoutPanel1->SetFlowBreak(this->btnFolder, true);
			this->btnFolder->Location = System::Drawing::Point(246, 3);
			this->btnFolder->Name = L"btnFolder";
			this->btnFolder->Size = System::Drawing::Size(75, 23);
			this->btnFolder->TabIndex = 2;
			this->btnFolder->Text = L"&Browse";
			this->btnFolder->UseVisualStyleBackColor = true;
			this->btnFolder->Click += gcnew System::EventHandler(this, &fConfig::btnFolder_Click);
			this->label16->AutoSize = true;
			this->flowLayoutPanel1->SetFlowBreak(this->label16, true);
			this->label16->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25, System::Drawing::FontStyle::Italic, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label16->Location = System::Drawing::Point(3, 29);
			this->label16->Name = L"label16";
			this->label16->Size = System::Drawing::Size(298, 13);
			this->label16->TabIndex = 20;
			this->label16->Text = L"Multiple folders allowed, separate with ; E.g. c:\\pics;e:\\photos";
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(3, 49);
			this->label4->Margin = System::Windows::Forms::Padding(3, 7, 3, 0);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(113, 13);
			this->label4->TabIndex = 6;
			this->label4->Text = L"Display each photo for";
			this->nupTimeout->Location = System::Drawing::Point(122, 45);
			this->nupTimeout->Name = L"nupTimeout";
			this->nupTimeout->Size = System::Drawing::Size(120, 20);
			this->nupTimeout->TabIndex = 7;
			this->nupTimeout->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
			this->label5->AutoSize = true;
			this->flowLayoutPanel1->SetFlowBreak(this->label5, true);
			this->label5->Location = System::Drawing::Point(248, 49);
			this->label5->Margin = System::Windows::Forms::Padding(3, 7, 3, 0);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(47, 13);
			this->label5->TabIndex = 8;
			this->label5->Text = L"seconds";
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(3, 75);
			this->label6->Margin = System::Windows::Forms::Padding(3, 7, 3, 0);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(94, 13);
			this->label6->TabIndex = 9;
			this->label6->Text = L"Background color:";
			this->btnBackgroundColor->BackColor = System::Drawing::Color::SteelBlue;
			this->flowLayoutPanel1->SetFlowBreak(this->btnBackgroundColor, true);
			this->btnBackgroundColor->ForeColor = System::Drawing::Color::White;
			this->btnBackgroundColor->Location = System::Drawing::Point(103, 71);
			this->btnBackgroundColor->Name = L"btnBackgroundColor";
			this->btnBackgroundColor->Size = System::Drawing::Size(192, 23);
			this->btnBackgroundColor->TabIndex = 10;
			this->btnBackgroundColor->Text = L"Select Color";
			this->btnBackgroundColor->UseVisualStyleBackColor = false;
			this->btnBackgroundColor->Click += gcnew System::EventHandler(this, &fConfig::btnBackgroundColor_Click);
			this->cbAnimatedTransitions->AutoSize = true;
			this->cbAnimatedTransitions->Checked = true;
			this->cbAnimatedTransitions->CheckState = System::Windows::Forms::CheckState::Checked;
			this->flowLayoutPanel1->SetFlowBreak(this->cbAnimatedTransitions, true);
			this->cbAnimatedTransitions->Location = System::Drawing::Point(7, 100);
			this->cbAnimatedTransitions->Margin = System::Windows::Forms::Padding(7, 3, 3, 3);
			this->cbAnimatedTransitions->Name = L"cbAnimatedTransitions";
			this->cbAnimatedTransitions->Size = System::Drawing::Size(141, 17);
			this->cbAnimatedTransitions->TabIndex = 15;
			this->cbAnimatedTransitions->Text = L"Use animated transitions";
			this->cbAnimatedTransitions->UseVisualStyleBackColor = true;
			this->label17->AutoSize = true;
			this->label17->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label17->Location = System::Drawing::Point(3, 123);
			this->label17->Margin = System::Windows::Forms::Padding(3, 3, 0, 0);
			this->label17->Name = L"label17";
			this->label17->Size = System::Drawing::Size(70, 13);
			this->label17->TabIndex = 19;
			this->label17->Text = L"Small photos:";
			this->cbStretchSmall->AutoSize = true;
			this->cbStretchSmall->Checked = true;
			this->cbStretchSmall->CheckState = System::Windows::Forms::CheckState::Checked;
			this->cbStretchSmall->Location = System::Drawing::Point(73, 123);
			this->cbStretchSmall->Margin = System::Windows::Forms::Padding(0, 3, 3, 3);
			this->cbStretchSmall->Name = L"cbStretchSmall";
			this->cbStretchSmall->Size = System::Drawing::Size(123, 17);
			this->cbStretchSmall->TabIndex = 11;
			this->cbStretchSmall->Text = L"Stretch to full screen";
			this->cbStretchSmall->UseVisualStyleBackColor = true;
			this->cbRandomPositions->AutoSize = true;
			this->cbRandomPositions->Checked = true;
			this->cbRandomPositions->CheckState = System::Windows::Forms::CheckState::Checked;
			this->flowLayoutPanel1->SetFlowBreak(this->cbRandomPositions, true);
			this->cbRandomPositions->Location = System::Drawing::Point(199, 123);
			this->cbRandomPositions->Margin = System::Windows::Forms::Padding(0, 3, 3, 3);
			this->cbRandomPositions->Name = L"cbRandomPositions";
			this->cbRandomPositions->Size = System::Drawing::Size(118, 17);
			this->cbRandomPositions->TabIndex = 18;
			this->cbRandomPositions->Text = L"Randomise position";
			this->cbRandomPositions->UseVisualStyleBackColor = true;
			this->cbExifRotate->AutoSize = true;
			this->cbExifRotate->Checked = true;
			this->cbExifRotate->CheckState = System::Windows::Forms::CheckState::Checked;
			this->cbExifRotate->Location = System::Drawing::Point(3, 146);
			this->cbExifRotate->Name = L"cbExifRotate";
			this->cbExifRotate->Size = System::Drawing::Size(273, 17);
			this->cbExifRotate->TabIndex = 21;
			this->cbExifRotate->Text = L"Rotate image based on EXIF orientation (if available)";
			this->cbExifRotate->UseVisualStyleBackColor = true;
			this->tpAdvanced->Controls->Add(this->flowLayoutPanel4);
			this->tpAdvanced->Location = System::Drawing::Point(4, 40);
			this->tpAdvanced->Name = L"tpAdvanced";
			this->tpAdvanced->Padding = System::Windows::Forms::Padding(3);
			this->tpAdvanced->Size = System::Drawing::Size(348, 292);
			this->tpAdvanced->TabIndex = 6;
			this->tpAdvanced->Text = L"Advanced";
			this->tpAdvanced->UseVisualStyleBackColor = true;
			this->flowLayoutPanel4->AutoSize = true;
			this->flowLayoutPanel4->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->flowLayoutPanel4->Controls->Add(this->label2);
			this->flowLayoutPanel4->Controls->Add(this->tbExcludeSubfolders);
			this->flowLayoutPanel4->Controls->Add(this->label3);
			this->flowLayoutPanel4->Controls->Add(this->label15);
			this->flowLayoutPanel4->Controls->Add(this->label43);
			this->flowLayoutPanel4->Controls->Add(this->tbExcludeRegEx);
			this->flowLayoutPanel4->Controls->Add(this->label47);
			this->flowLayoutPanel4->Controls->Add(this->label48);
			this->flowLayoutPanel4->Controls->Add(this->cbFolderInfo);
			this->flowLayoutPanel4->Controls->Add(this->nupFolderInfoTimeout);
			this->flowLayoutPanel4->Controls->Add(this->label18);
			this->flowLayoutPanel4->Controls->Add(this->cbHideHidden);
			this->flowLayoutPanel4->Controls->Add(this->cbOrdered);
			this->flowLayoutPanel4->Controls->Add(this->label13);
			this->flowLayoutPanel4->Controls->Add(this->cbMouseSensitivity);
			this->flowLayoutPanel4->Controls->Add(this->cbOnlyEscapeExits);
			this->flowLayoutPanel4->Controls->Add(this->cbDebug);
			this->flowLayoutPanel4->Controls->Add(this->cbCloseAfterImageLocate);
			this->flowLayoutPanel4->Controls->Add(this->cbDeleteKey);
			this->flowLayoutPanel4->Controls->Add(this->cbConfirmDelete);
			this->flowLayoutPanel4->Dock = System::Windows::Forms::DockStyle::Top;
			this->flowLayoutPanel4->Location = System::Drawing::Point(3, 3);
			this->flowLayoutPanel4->Margin = System::Windows::Forms::Padding(0);
			this->flowLayoutPanel4->Name = L"flowLayoutPanel4";
			this->flowLayoutPanel4->Size = System::Drawing::Size(342, 266);
			this->flowLayoutPanel4->TabIndex = 1;
			this->label2->Location = System::Drawing::Point(3, 7);
			this->label2->Margin = System::Windows::Forms::Padding(3, 7, 3, 0);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(110, 13);
			this->label2->TabIndex = 28;
			this->label2->Text = L"Exclude subfolders:";
			this->flowLayoutPanel4->SetFlowBreak(this->tbExcludeSubfolders, true);
			this->tbExcludeSubfolders->Location = System::Drawing::Point(119, 3);
			this->tbExcludeSubfolders->Name = L"tbExcludeSubfolders";
			this->tbExcludeSubfolders->Size = System::Drawing::Size(205, 20);
			this->tbExcludeSubfolders->TabIndex = 29;
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25, System::Drawing::FontStyle::Italic, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label3->Location = System::Drawing::Point(0, 26);
			this->label3->Margin = System::Windows::Forms::Padding(0);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(235, 13);
			this->label3->TabIndex = 30;
			this->label3->Text = L"Separate excluded subfolders with; For example:";
			this->label15->AutoSize = true;
			this->flowLayoutPanel4->SetFlowBreak(this->label15, true);
			this->label15->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25, static_cast<System::Drawing::FontStyle>((System::Drawing::FontStyle::Bold | System::Drawing::FontStyle::Italic)), 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->label15->Location = System::Drawing::Point(235, 26);
			this->label15->Margin = System::Windows::Forms::Padding(0);
			this->label15->Name = L"label15";
			this->label15->Size = System::Drawing::Size(99, 13);
			this->label15->TabIndex = 31;
			this->label15->Text = L"thumbs;800x600";
			this->label43->AutoSize = true;
			this->label43->Location = System::Drawing::Point(3, 46);
			this->label43->Margin = System::Windows::Forms::Padding(3, 7, 3, 0);
			this->label43->Name = L"label43";
			this->label43->Size = System::Drawing::Size(110, 13);
			this->label43->TabIndex = 35;
			this->label43->Text = L"Exclude files (RegEx):";
			this->flowLayoutPanel4->SetFlowBreak(this->tbExcludeRegEx, true);
			this->tbExcludeRegEx->Location = System::Drawing::Point(119, 42);
			this->tbExcludeRegEx->Name = L"tbExcludeRegEx";
			this->tbExcludeRegEx->Size = System::Drawing::Size(205, 20);
			this->tbExcludeRegEx->TabIndex = 36;
			this->label47->AutoSize = true;
			this->label47->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25, System::Drawing::FontStyle::Italic, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label47->Location = System::Drawing::Point(0, 65);
			this->label47->Margin = System::Windows::Forms::Padding(0);
			this->label47->Name = L"label47";
			this->label47->Size = System::Drawing::Size(236, 13);
			this->label47->TabIndex = 37;
			this->label47->Text = L"Regular expression to exclude files; For example:";
			this->label48->AutoSize = true;
			this->flowLayoutPanel4->SetFlowBreak(this->label48, true);
			this->label48->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25, static_cast<System::Drawing::FontStyle>((System::Drawing::FontStyle::Bold | System::Drawing::FontStyle::Italic)), 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->label48->Location = System::Drawing::Point(236, 65);
			this->label48->Margin = System::Windows::Forms::Padding(0);
			this->label48->Name = L"label48";
			this->label48->Size = System::Drawing::Size(105, 13);
			this->label48->TabIndex = 38;
			this->label48->Text = L".*\\.gif$|xl.*\\.pcx$";
			this->cbFolderInfo->AutoSize = true;
			this->cbFolderInfo->Location = System::Drawing::Point(7, 83);
			this->cbFolderInfo->Margin = System::Windows::Forms::Padding(7, 5, 0, 0);
			this->cbFolderInfo->Name = L"cbFolderInfo";
			this->cbFolderInfo->Size = System::Drawing::Size(121, 17);
			this->cbFolderInfo->TabIndex = 23;
			this->cbFolderInfo->Text = L"Hide folder info after";
			this->cbFolderInfo->UseVisualStyleBackColor = true;
			this->nupFolderInfoTimeout->AccessibleDescription = L"";
			this->nupFolderInfoTimeout->Location = System::Drawing::Point(131, 81);
			this->nupFolderInfoTimeout->Name = L"nupFolderInfoTimeout";
			this->nupFolderInfoTimeout->Size = System::Drawing::Size(39, 20);
			this->nupFolderInfoTimeout->TabIndex = 24;
			this->nupFolderInfoTimeout->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) {10, 0, 0, 0});
			this->label18->AutoSize = true;
			this->flowLayoutPanel4->SetFlowBreak(this->label18, true);
			this->label18->Location = System::Drawing::Point(173, 83);
			this->label18->Margin = System::Windows::Forms::Padding(0, 5, 0, 0);
			this->label18->Name = L"label18";
			this->label18->Size = System::Drawing::Size(47, 13);
			this->label18->TabIndex = 25;
			this->label18->Text = L"seconds";
			this->cbHideHidden->AutoSize = true;
			this->cbHideHidden->Checked = true;
			this->cbHideHidden->CheckState = System::Windows::Forms::CheckState::Checked;
			this->flowLayoutPanel4->SetFlowBreak(this->cbHideHidden, true);
			this->cbHideHidden->Location = System::Drawing::Point(7, 107);
			this->cbHideHidden->Margin = System::Windows::Forms::Padding(7, 3, 3, 3);
			this->cbHideHidden->Name = L"cbHideHidden";
			this->cbHideHidden->Size = System::Drawing::Size(239, 17);
			this->cbHideHidden->TabIndex = 26;
			this->cbHideHidden->Text = L"Hide folders and files with hidden attribute set";
			this->cbHideHidden->UseVisualStyleBackColor = true;
			this->cbOrdered->AutoSize = true;
			this->cbOrdered->Location = System::Drawing::Point(7, 130);
			this->cbOrdered->Margin = System::Windows::Forms::Padding(7, 3, 3, 3);
			this->cbOrdered->Name = L"cbOrdered";
			this->cbOrdered->Size = System::Drawing::Size(275, 17);
			this->cbOrdered->TabIndex = 27;
			this->cbOrdered->Text = L"Show photos in sequential order (Disable randomizer)";
			this->cbOrdered->UseVisualStyleBackColor = true;
			this->label13->AutoSize = true;
			this->label13->Location = System::Drawing::Point(3, 154);
			this->label13->Margin = System::Windows::Forms::Padding(3, 4, 3, 0);
			this->label13->Name = L"label13";
			this->label13->Size = System::Drawing::Size(90, 13);
			this->label13->TabIndex = 32;
			this->label13->Text = L"Mouse sensitivity:";
			this->cbMouseSensitivity->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->flowLayoutPanel4->SetFlowBreak(this->cbMouseSensitivity, true);
			this->cbMouseSensitivity->FormattingEnabled = true;
			this->cbMouseSensitivity->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"Sensitive", L"Normal", L"Insensitive", L"Ignore movement"});
			this->cbMouseSensitivity->Location = System::Drawing::Point(99, 150);
			this->cbMouseSensitivity->Margin = System::Windows::Forms::Padding(3, 0, 3, 3);
			this->cbMouseSensitivity->Name = L"cbMouseSensitivity";
			this->cbMouseSensitivity->Size = System::Drawing::Size(121, 21);
			this->cbMouseSensitivity->TabIndex = 33;
			this->cbOnlyEscapeExits->AutoSize = true;
			this->cbOnlyEscapeExits->Location = System::Drawing::Point(7, 177);
			this->cbOnlyEscapeExits->Margin = System::Windows::Forms::Padding(7, 3, 3, 3);
			this->cbOnlyEscapeExits->Name = L"cbOnlyEscapeExits";
			this->cbOnlyEscapeExits->Size = System::Drawing::Size(258, 17);
			this->cbOnlyEscapeExits->TabIndex = 40;
			this->cbOnlyEscapeExits->Text = L"On keyboard only Escape key exists screensaver";
			this->cbOnlyEscapeExits->UseVisualStyleBackColor = true;
			this->cbDebug->AutoSize = true;
			this->cbDebug->Location = System::Drawing::Point(7, 200);
			this->cbDebug->Margin = System::Windows::Forms::Padding(7, 3, 3, 3);
			this->cbDebug->Name = L"cbDebug";
			this->cbDebug->Size = System::Drawing::Size(172, 17);
			this->cbDebug->TabIndex = 34;
			this->cbDebug->Text = L"Enabled debugging information";
			this->cbDebug->UseVisualStyleBackColor = true;
			this->cbCloseAfterImageLocate->AutoSize = true;
			this->cbCloseAfterImageLocate->Checked = true;
			this->cbCloseAfterImageLocate->CheckState = System::Windows::Forms::CheckState::Checked;
			this->cbCloseAfterImageLocate->Location = System::Drawing::Point(7, 223);
			this->cbCloseAfterImageLocate->Margin = System::Windows::Forms::Padding(7, 3, 3, 3);
			this->cbCloseAfterImageLocate->Name = L"cbCloseAfterImageLocate";
			this->cbCloseAfterImageLocate->Size = System::Drawing::Size(318, 17);
			this->cbCloseAfterImageLocate->TabIndex = 39;
			this->cbCloseAfterImageLocate->Text = L"Close screensaver after locating image(s) in Windows Explorer";
			this->cbCloseAfterImageLocate->UseVisualStyleBackColor = true;
			this->cbDeleteKey->AutoSize = true;
			this->cbDeleteKey->Checked = true;
			this->cbDeleteKey->CheckState = System::Windows::Forms::CheckState::Checked;
			this->cbDeleteKey->Location = System::Drawing::Point(7, 246);
			this->cbDeleteKey->Margin = System::Windows::Forms::Padding(7, 3, 3, 3);
			this->cbDeleteKey->Name = L"cbDeleteKey";
			this->cbDeleteKey->Size = System::Drawing::Size(111, 17);
			this->cbDeleteKey->TabIndex = 41;
			this->cbDeleteKey->Text = L"Enable delete key";
			this->cbDeleteKey->UseVisualStyleBackColor = true;
			this->cbDeleteKey->CheckedChanged += gcnew System::EventHandler(this, &fConfig::cbDeleteKey_CheckedChanged);
			this->cbConfirmDelete->AutoSize = true;
			this->cbConfirmDelete->Checked = true;
			this->cbConfirmDelete->CheckState = System::Windows::Forms::CheckState::Checked;
			this->cbConfirmDelete->Location = System::Drawing::Point(128, 246);
			this->cbConfirmDelete->Margin = System::Windows::Forms::Padding(7, 3, 3, 3);
			this->cbConfirmDelete->Name = L"cbConfirmDelete";
			this->cbConfirmDelete->Size = System::Drawing::Size(153, 17);
			this->cbConfirmDelete->TabIndex = 42;
			this->cbConfirmDelete->Text = L"Show confirm delete dialog";
			this->cbConfirmDelete->UseVisualStyleBackColor = true;
			this->tpClock->Controls->Add(this->flowLayoutPanel2);
			this->tpClock->Location = System::Drawing::Point(4, 40);
			this->tpClock->Name = L"tpClock";
			this->tpClock->Padding = System::Windows::Forms::Padding(3);
			this->tpClock->Size = System::Drawing::Size(348, 292);
			this->tpClock->TabIndex = 1;
			this->tpClock->Text = L"Clock";
			this->tpClock->UseVisualStyleBackColor = true;
			this->flowLayoutPanel2->Controls->Add(this->lMonitors);
			this->flowLayoutPanel2->Controls->Add(this->cbClockMonitor);
			this->flowLayoutPanel2->Controls->Add(this->groupBox2);
			this->flowLayoutPanel2->Controls->Add(this->label9);
			this->flowLayoutPanel2->Controls->Add(this->btnClockFont);
			this->flowLayoutPanel2->Controls->Add(this->label49);
			this->flowLayoutPanel2->Controls->Add(this->tbClockMask);
			this->flowLayoutPanel2->Location = System::Drawing::Point(3, 6);
			this->flowLayoutPanel2->Name = L"flowLayoutPanel2";
			this->flowLayoutPanel2->Size = System::Drawing::Size(313, 300);
			this->flowLayoutPanel2->TabIndex = 0;
			this->lMonitors->AutoSize = true;
			this->lMonitors->Location = System::Drawing::Point(3, 0);
			this->lMonitors->Name = L"lMonitors";
			this->lMonitors->Padding = System::Windows::Forms::Padding(0, 7, 0, 0);
			this->lMonitors->Size = System::Drawing::Size(78, 20);
			this->lMonitors->TabIndex = 0;
			this->lMonitors->Text = L"Select Monitor:";
			this->cbClockMonitor->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->flowLayoutPanel2->SetFlowBreak(this->cbClockMonitor, true);
			this->cbClockMonitor->FormattingEnabled = true;
			this->cbClockMonitor->Location = System::Drawing::Point(87, 3);
			this->cbClockMonitor->Name = L"cbClockMonitor";
			this->cbClockMonitor->Size = System::Drawing::Size(211, 21);
			this->cbClockMonitor->TabIndex = 1;
			this->cbClockMonitor->SelectedIndexChanged += gcnew System::EventHandler(this, &fConfig::cbClockMonitor_SelectedIndexChanged);
			this->groupBox2->Controls->Add(this->rbClockRun);
			this->groupBox2->Controls->Add(this->rbClockTime);
			this->groupBox2->Controls->Add(this->rbClockNo);
			this->groupBox2->Dock = System::Windows::Forms::DockStyle::Top;
			this->flowLayoutPanel2->SetFlowBreak(this->groupBox2, true);
			this->groupBox2->Location = System::Drawing::Point(3, 30);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(295, 100);
			this->groupBox2->TabIndex = 2;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Type";
			this->rbClockRun->AutoSize = true;
			this->rbClockRun->Location = System::Drawing::Point(6, 65);
			this->rbClockRun->Name = L"rbClockRun";
			this->rbClockRun->Size = System::Drawing::Size(145, 17);
			this->rbClockRun->TabIndex = 2;
			this->rbClockRun->TabStop = true;
			this->rbClockRun->Text = L"Screensaver running time";
			this->rbClockRun->UseVisualStyleBackColor = true;
			this->rbClockRun->CheckedChanged += gcnew System::EventHandler(this, &fConfig::rbClockRun_CheckedChanged);
			this->rbClockTime->AutoSize = true;
			this->rbClockTime->Location = System::Drawing::Point(6, 42);
			this->rbClockTime->Name = L"rbClockTime";
			this->rbClockTime->Size = System::Drawing::Size(81, 17);
			this->rbClockTime->TabIndex = 1;
			this->rbClockTime->TabStop = true;
			this->rbClockTime->Text = L"Current time";
			this->rbClockTime->UseVisualStyleBackColor = true;
			this->rbClockTime->CheckedChanged += gcnew System::EventHandler(this, &fConfig::rbClockTime_CheckedChanged);
			this->rbClockNo->AutoSize = true;
			this->rbClockNo->Location = System::Drawing::Point(6, 19);
			this->rbClockNo->Name = L"rbClockNo";
			this->rbClockNo->Size = System::Drawing::Size(68, 17);
			this->rbClockNo->TabIndex = 0;
			this->rbClockNo->TabStop = true;
			this->rbClockNo->Text = L"No clock";
			this->rbClockNo->UseVisualStyleBackColor = true;
			this->rbClockNo->CheckedChanged += gcnew System::EventHandler(this, &fConfig::rbClockNo_CheckedChanged);
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(3, 133);
			this->label9->Name = L"label9";
			this->label9->Padding = System::Windows::Forms::Padding(0, 60, 0, 0);
			this->label9->Size = System::Drawing::Size(58, 73);
			this->label9->TabIndex = 3;
			this->label9->Text = L"Clock font:";
			this->btnClockFont->Font = (gcnew System::Drawing::Font(L"Arial", 63.75, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnClockFont->ForeColor = System::Drawing::Color::White;
			this->btnClockFont->Location = System::Drawing::Point(67, 136);
			this->btnClockFont->Name = L"btnClockFont";
			this->btnClockFont->Size = System::Drawing::Size(231, 123);
			this->btnClockFont->TabIndex = 4;
			this->btnClockFont->Text = L"14:23:01";
			this->btnClockFont->UseVisualStyleBackColor = true;
			this->btnClockFont->Click += gcnew System::EventHandler(this, &fConfig::btnClockFont_Click);
			this->label49->AutoSize = true;
			this->label49->Location = System::Drawing::Point(3, 267);
			this->label49->Margin = System::Windows::Forms::Padding(3, 5, 3, 0);
			this->label49->Name = L"label49";
			this->label49->Size = System::Drawing::Size(70, 13);
			this->label49->TabIndex = 5;
			this->label49->Text = L"Format mask:";
			this->tbClockMask->Location = System::Drawing::Point(79, 265);
			this->tbClockMask->Name = L"tbClockMask";
			this->tbClockMask->Size = System::Drawing::Size(224, 20);
			this->tbClockMask->TabIndex = 6;
			this->tbClockMask->Text = L"HH:mm:ss";
			this->tpCalendar->Controls->Add(this->lCalendarInfo);
			this->tpCalendar->Controls->Add(this->flowLayoutPanel5);
			this->tpCalendar->Location = System::Drawing::Point(4, 40);
			this->tpCalendar->Name = L"tpCalendar";
			this->tpCalendar->Size = System::Drawing::Size(348, 292);
			this->tpCalendar->TabIndex = 7;
			this->tpCalendar->Text = L"Calendar";
			this->tpCalendar->UseVisualStyleBackColor = true;
			this->lCalendarInfo->BackColor = System::Drawing::SystemColors::Info;
			this->lCalendarInfo->Location = System::Drawing::Point(3, 248);
			this->lCalendarInfo->Name = L"lCalendarInfo";
			this->lCalendarInfo->Padding = System::Windows::Forms::Padding(0, 3, 0, 0);
			this->lCalendarInfo->Size = System::Drawing::Size(342, 46);
			this->lCalendarInfo->TabIndex = 1;
			this->lCalendarInfo->Text = L"When screensaver is running: press \'C\' to select calendar opacity and press \'Ctrl" 
				L"+C\'  to reposition and resize.";
			this->flowLayoutPanel5->Controls->Add(this->splitContainer1);
			this->flowLayoutPanel5->Controls->Add(this->btnCalendarFont);
			this->flowLayoutPanel5->Controls->Add(this->label23);
			this->flowLayoutPanel5->Controls->Add(this->btnAllBackgrounds);
			this->flowLayoutPanel5->Controls->Add(this->label36);
			this->flowLayoutPanel5->Controls->Add(this->label35);
			this->flowLayoutPanel5->Controls->Add(this->label34);
			this->flowLayoutPanel5->Controls->Add(this->label31);
			this->flowLayoutPanel5->Controls->Add(this->label32);
			this->flowLayoutPanel5->Controls->Add(this->label33);
			this->flowLayoutPanel5->Controls->Add(this->label25);
			this->flowLayoutPanel5->Controls->Add(this->btnCalTitleFront);
			this->flowLayoutPanel5->Controls->Add(this->btnCalTitleBack);
			this->flowLayoutPanel5->Controls->Add(this->label26);
			this->flowLayoutPanel5->Controls->Add(this->btnDayNamesFront);
			this->flowLayoutPanel5->Controls->Add(this->btnDayNamesBack);
			this->flowLayoutPanel5->Controls->Add(this->label27);
			this->flowLayoutPanel5->Controls->Add(this->btnCalDayFront);
			this->flowLayoutPanel5->Controls->Add(this->btnCalDayBack);
			this->flowLayoutPanel5->Controls->Add(this->label28);
			this->flowLayoutPanel5->Controls->Add(this->btnCalTodayFront);
			this->flowLayoutPanel5->Controls->Add(this->btnCalTodayBack);
			this->flowLayoutPanel5->Controls->Add(this->label37);
			this->flowLayoutPanel5->Controls->Add(this->btnCalDaysOtherFront);
			this->flowLayoutPanel5->Controls->Add(this->btnCalDaysOtherBack);
			this->flowLayoutPanel5->Location = System::Drawing::Point(0, 3);
			this->flowLayoutPanel5->Name = L"flowLayoutPanel5";
			this->flowLayoutPanel5->Size = System::Drawing::Size(345, 265);
			this->flowLayoutPanel5->TabIndex = 0;
			this->splitContainer1->Location = System::Drawing::Point(0, 0);
			this->splitContainer1->Margin = System::Windows::Forms::Padding(0);
			this->splitContainer1->Name = L"splitContainer1";
			this->splitContainer1->Panel1->Controls->Add(this->label22);
			this->splitContainer1->Panel1->Controls->Add(this->cbCurrentMonthCalendar);
			this->splitContainer1->Panel1->Controls->Add(this->label21);
			this->splitContainer1->Panel1->Controls->Add(this->label20);
			this->splitContainer1->Panel1->Controls->Add(this->nupNrFolCalendars);
			this->splitContainer1->Panel1->Controls->Add(this->nupNrPrevCalendars);
			this->splitContainer1->Panel1->Controls->Add(this->label19);
			this->splitContainer1->Panel2->Controls->Add(this->label30);
			this->splitContainer1->Panel2->Controls->Add(this->label29);
			this->splitContainer1->Panel2->Controls->Add(this->label24);
			this->splitContainer1->Panel2->Controls->Add(this->tbHigh);
			this->splitContainer1->Panel2->Controls->Add(this->tbLow);
			this->splitContainer1->Size = System::Drawing::Size(339, 100);
			this->splitContainer1->SplitterDistance = 190;
			this->splitContainer1->TabIndex = 31;
			this->label22->AutoSize = true;
			this->label22->Location = System::Drawing::Point(98, 60);
			this->label22->Name = L"label22";
			this->label22->Padding = System::Windows::Forms::Padding(0, 4, 0, 0);
			this->label22->Size = System::Drawing::Size(91, 17);
			this->label22->TabIndex = 6;
			this->label22->Text = L"month(s) following";
			this->cbCurrentMonthCalendar->AutoSize = true;
			this->cbCurrentMonthCalendar->Location = System::Drawing::Point(3, 6);
			this->cbCurrentMonthCalendar->Name = L"cbCurrentMonthCalendar";
			this->cbCurrentMonthCalendar->Size = System::Drawing::Size(180, 17);
			this->cbCurrentMonthCalendar->TabIndex = 1;
			this->cbCurrentMonthCalendar->Text = L"Show calendar for current month";
			this->cbCurrentMonthCalendar->UseVisualStyleBackColor = true;
			this->label21->AutoSize = true;
			this->label21->Location = System::Drawing::Point(0, 55);
			this->label21->Name = L"label21";
			this->label21->Padding = System::Windows::Forms::Padding(0, 4, 0, 0);
			this->label21->Size = System::Drawing::Size(37, 17);
			this->label21->TabIndex = 5;
			this->label21->Text = L"Show ";
			this->label20->AutoSize = true;
			this->label20->Location = System::Drawing::Point(98, 29);
			this->label20->Name = L"label20";
			this->label20->Padding = System::Windows::Forms::Padding(0, 4, 0, 0);
			this->label20->Size = System::Drawing::Size(90, 17);
			this->label20->TabIndex = 3;
			this->label20->Text = L"month(s) previous";
			this->nupNrFolCalendars->Location = System::Drawing::Point(43, 58);
			this->nupNrFolCalendars->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {12, 0, 0, 0});
			this->nupNrFolCalendars->Name = L"nupNrFolCalendars";
			this->nupNrFolCalendars->Size = System::Drawing::Size(49, 20);
			this->nupNrFolCalendars->TabIndex = 4;
			this->nupNrPrevCalendars->Location = System::Drawing::Point(43, 32);
			this->nupNrPrevCalendars->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {12, 0, 0, 0});
			this->nupNrPrevCalendars->Name = L"nupNrPrevCalendars";
			this->nupNrPrevCalendars->Size = System::Drawing::Size(49, 20);
			this->nupNrPrevCalendars->TabIndex = 0;
			this->label19->AutoSize = true;
			this->label19->Location = System::Drawing::Point(0, 29);
			this->label19->Name = L"label19";
			this->label19->Padding = System::Windows::Forms::Padding(0, 4, 0, 0);
			this->label19->Size = System::Drawing::Size(37, 17);
			this->label19->TabIndex = 2;
			this->label19->Text = L"Show ";
			this->label30->AutoSize = true;
			this->label30->Location = System::Drawing::Point(4, 7);
			this->label30->Name = L"label30";
			this->label30->Size = System::Drawing::Size(88, 13);
			this->label30->TabIndex = 33;
			this->label30->Text = L"Calendar Opacity";
			this->label29->AutoSize = true;
			this->label29->Location = System::Drawing::Point(4, 64);
			this->label29->Name = L"label29";
			this->label29->Size = System::Drawing::Size(25, 13);
			this->label29->TabIndex = 32;
			this->label29->Text = L"2nd";
			this->label24->AutoSize = true;
			this->label24->Location = System::Drawing::Point(4, 32);
			this->label24->Name = L"label24";
			this->label24->Size = System::Drawing::Size(21, 13);
			this->label24->TabIndex = 31;
			this->label24->Text = L"1st";
			this->tbHigh->AutoSize = false;
			this->tbHigh->BackColor = System::Drawing::SystemColors::ControlLightLight;
			this->tbHigh->LargeChange = 10;
			this->tbHigh->Location = System::Drawing::Point(33, 58);
			this->tbHigh->Maximum = 100;
			this->tbHigh->Name = L"tbHigh";
			this->tbHigh->Size = System::Drawing::Size(104, 20);
			this->tbHigh->TabIndex = 30;
			this->tbHigh->TickFrequency = 10;
			this->tbHigh->Value = 65;
			this->tbLow->AutoSize = false;
			this->tbLow->BackColor = System::Drawing::SystemColors::ControlLightLight;
			this->tbLow->LargeChange = 10;
			this->tbLow->Location = System::Drawing::Point(33, 30);
			this->tbLow->Maximum = 100;
			this->tbLow->Name = L"tbLow";
			this->tbLow->Size = System::Drawing::Size(104, 20);
			this->tbLow->TabIndex = 29;
			this->tbLow->TickFrequency = 10;
			this->tbLow->Value = 30;
			this->btnCalendarFont->Font = (gcnew System::Drawing::Font(L"Trebuchet MS", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnCalendarFont->ForeColor = System::Drawing::Color::White;
			this->btnCalendarFont->Location = System::Drawing::Point(3, 103);
			this->btnCalendarFont->Name = L"btnCalendarFont";
			this->btnCalendarFont->Size = System::Drawing::Size(127, 30);
			this->btnCalendarFont->TabIndex = 16;
			this->btnCalendarFont->Text = L"Calendar Font";
			this->btnCalendarFont->UseVisualStyleBackColor = true;
			this->btnCalendarFont->Click += gcnew System::EventHandler(this, &fConfig::btnCalendarFont_Click);
			this->label23->AutoSize = true;
			this->label23->Location = System::Drawing::Point(136, 100);
			this->label23->Name = L"label23";
			this->label23->Padding = System::Windows::Forms::Padding(0, 7, 0, 0);
			this->label23->Size = System::Drawing::Size(101, 20);
			this->label23->TabIndex = 12;
			this->label23->Text = L"Set all backgrounds";
			this->btnAllBackgrounds->BackColor = System::Drawing::Color::White;
			this->flowLayoutPanel5->SetFlowBreak(this->btnAllBackgrounds, true);
			this->btnAllBackgrounds->ForeColor = System::Drawing::Color::White;
			this->btnAllBackgrounds->Location = System::Drawing::Point(243, 103);
			this->btnAllBackgrounds->Name = L"btnAllBackgrounds";
			this->btnAllBackgrounds->Size = System::Drawing::Size(25, 23);
			this->btnAllBackgrounds->TabIndex = 11;
			this->btnAllBackgrounds->Text = L"Background Color";
			this->btnAllBackgrounds->UseVisualStyleBackColor = false;
			this->btnAllBackgrounds->Click += gcnew System::EventHandler(this, &fConfig::btnAllBackgrounds_Click);
			this->label36->Location = System::Drawing::Point(3, 136);
			this->label36->Name = L"label36";
			this->label36->Size = System::Drawing::Size(99, 13);
			this->label36->TabIndex = 37;
			this->label36->Text = L"Pick colors";
			this->label36->TextAlign = System::Drawing::ContentAlignment::BottomLeft;
			this->label35->Location = System::Drawing::Point(105, 136);
			this->label35->Margin = System::Windows::Forms::Padding(0);
			this->label35->Name = L"label35";
			this->label35->Size = System::Drawing::Size(32, 13);
			this->label35->TabIndex = 36;
			this->label35->Text = L"Front";
			this->label35->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			this->label34->Location = System::Drawing::Point(137, 136);
			this->label34->Margin = System::Windows::Forms::Padding(0);
			this->label34->Name = L"label34";
			this->label34->Size = System::Drawing::Size(32, 13);
			this->label34->TabIndex = 35;
			this->label34->Text = L"Back";
			this->label34->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			this->label31->Location = System::Drawing::Point(172, 136);
			this->label31->Name = L"label31";
			this->label31->Size = System::Drawing::Size(99, 13);
			this->label31->TabIndex = 40;
			this->label31->Text = L"Pick colors";
			this->label31->TextAlign = System::Drawing::ContentAlignment::BottomLeft;
			this->label32->Location = System::Drawing::Point(274, 136);
			this->label32->Margin = System::Windows::Forms::Padding(0);
			this->label32->Name = L"label32";
			this->label32->Size = System::Drawing::Size(32, 13);
			this->label32->TabIndex = 39;
			this->label32->Text = L"Front";
			this->label32->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			this->label33->Location = System::Drawing::Point(306, 136);
			this->label33->Margin = System::Windows::Forms::Padding(0);
			this->label33->Name = L"label33";
			this->label33->Size = System::Drawing::Size(32, 13);
			this->label33->TabIndex = 38;
			this->label33->Text = L"Back";
			this->label33->TextAlign = System::Drawing::ContentAlignment::BottomCenter;
			this->label25->Location = System::Drawing::Point(3, 149);
			this->label25->Name = L"label25";
			this->label25->Padding = System::Windows::Forms::Padding(0, 7, 0, 0);
			this->label25->Size = System::Drawing::Size(100, 20);
			this->label25->TabIndex = 18;
			this->label25->Text = L"Title (month/year)";
			this->btnCalTitleFront->BackColor = System::Drawing::Color::MidnightBlue;
			this->btnCalTitleFront->ForeColor = System::Drawing::Color::White;
			this->btnCalTitleFront->Location = System::Drawing::Point(109, 152);
			this->btnCalTitleFront->Name = L"btnCalTitleFront";
			this->btnCalTitleFront->Size = System::Drawing::Size(25, 23);
			this->btnCalTitleFront->TabIndex = 17;
			this->btnCalTitleFront->Text = L"Foreground";
			this->btnCalTitleFront->UseVisualStyleBackColor = false;
			this->btnCalTitleFront->Click += gcnew System::EventHandler(this, &fConfig::btnCalTitleFront_Click);
			this->btnCalTitleBack->BackColor = System::Drawing::Color::White;
			this->btnCalTitleBack->ForeColor = System::Drawing::Color::White;
			this->btnCalTitleBack->Location = System::Drawing::Point(140, 152);
			this->btnCalTitleBack->Name = L"btnCalTitleBack";
			this->btnCalTitleBack->Size = System::Drawing::Size(25, 23);
			this->btnCalTitleBack->TabIndex = 19;
			this->btnCalTitleBack->Text = L"Background";
			this->btnCalTitleBack->UseVisualStyleBackColor = false;
			this->btnCalTitleBack->Click += gcnew System::EventHandler(this, &fConfig::btnCalTitleBack_Click);
			this->label26->Location = System::Drawing::Point(171, 149);
			this->label26->Name = L"label26";
			this->label26->Padding = System::Windows::Forms::Padding(0, 7, 0, 0);
			this->label26->Size = System::Drawing::Size(100, 20);
			this->label26->TabIndex = 21;
			this->label26->Text = L"Days Mon-Sun";
			this->btnDayNamesFront->BackColor = System::Drawing::Color::Red;
			this->btnDayNamesFront->ForeColor = System::Drawing::Color::White;
			this->btnDayNamesFront->Location = System::Drawing::Point(277, 152);
			this->btnDayNamesFront->Name = L"btnDayNamesFront";
			this->btnDayNamesFront->Size = System::Drawing::Size(25, 23);
			this->btnDayNamesFront->TabIndex = 20;
			this->btnDayNamesFront->Text = L"Foreground";
			this->btnDayNamesFront->UseVisualStyleBackColor = false;
			this->btnDayNamesFront->Click += gcnew System::EventHandler(this, &fConfig::btnDayNamesFront_Click);
			this->btnDayNamesBack->BackColor = System::Drawing::Color::White;
			this->btnDayNamesBack->ForeColor = System::Drawing::Color::White;
			this->btnDayNamesBack->Location = System::Drawing::Point(308, 152);
			this->btnDayNamesBack->Name = L"btnDayNamesBack";
			this->btnDayNamesBack->Size = System::Drawing::Size(25, 23);
			this->btnDayNamesBack->TabIndex = 22;
			this->btnDayNamesBack->Text = L"Background";
			this->btnDayNamesBack->UseVisualStyleBackColor = false;
			this->btnDayNamesBack->Click += gcnew System::EventHandler(this, &fConfig::btnDayNamesBack_Click);
			this->label27->Location = System::Drawing::Point(3, 178);
			this->label27->Name = L"label27";
			this->label27->Padding = System::Windows::Forms::Padding(0, 7, 0, 0);
			this->label27->Size = System::Drawing::Size(100, 20);
			this->label27->TabIndex = 24;
			this->label27->Text = L"Days current month";
			this->btnCalDayFront->BackColor = System::Drawing::Color::Black;
			this->btnCalDayFront->ForeColor = System::Drawing::Color::White;
			this->btnCalDayFront->Location = System::Drawing::Point(109, 181);
			this->btnCalDayFront->Name = L"btnCalDayFront";
			this->btnCalDayFront->Size = System::Drawing::Size(25, 23);
			this->btnCalDayFront->TabIndex = 23;
			this->btnCalDayFront->Text = L"Foreground";
			this->btnCalDayFront->UseVisualStyleBackColor = false;
			this->btnCalDayFront->Click += gcnew System::EventHandler(this, &fConfig::btnCalDayFront_Click);
			this->btnCalDayBack->BackColor = System::Drawing::Color::White;
			this->btnCalDayBack->ForeColor = System::Drawing::Color::White;
			this->btnCalDayBack->Location = System::Drawing::Point(140, 181);
			this->btnCalDayBack->Name = L"btnCalDayBack";
			this->btnCalDayBack->Size = System::Drawing::Size(25, 23);
			this->btnCalDayBack->TabIndex = 25;
			this->btnCalDayBack->Text = L"Background";
			this->btnCalDayBack->UseVisualStyleBackColor = false;
			this->btnCalDayBack->Click += gcnew System::EventHandler(this, &fConfig::btnCalDayBack_Click);
			this->label28->Location = System::Drawing::Point(171, 178);
			this->label28->Name = L"label28";
			this->label28->Padding = System::Windows::Forms::Padding(0, 7, 0, 0);
			this->label28->Size = System::Drawing::Size(100, 20);
			this->label28->TabIndex = 27;
			this->label28->Text = L"Today";
			this->btnCalTodayFront->BackColor = System::Drawing::Color::White;
			this->btnCalTodayFront->ForeColor = System::Drawing::Color::White;
			this->btnCalTodayFront->Location = System::Drawing::Point(277, 181);
			this->btnCalTodayFront->Name = L"btnCalTodayFront";
			this->btnCalTodayFront->Size = System::Drawing::Size(25, 23);
			this->btnCalTodayFront->TabIndex = 26;
			this->btnCalTodayFront->Text = L"Foreground";
			this->btnCalTodayFront->UseVisualStyleBackColor = false;
			this->btnCalTodayFront->Click += gcnew System::EventHandler(this, &fConfig::btnCalTodayFront_Click);
			this->btnCalTodayBack->BackColor = System::Drawing::Color::Red;
			this->btnCalTodayBack->ForeColor = System::Drawing::Color::White;
			this->btnCalTodayBack->Location = System::Drawing::Point(308, 181);
			this->btnCalTodayBack->Name = L"btnCalTodayBack";
			this->btnCalTodayBack->Size = System::Drawing::Size(25, 23);
			this->btnCalTodayBack->TabIndex = 28;
			this->btnCalTodayBack->Text = L"Background";
			this->btnCalTodayBack->UseVisualStyleBackColor = false;
			this->btnCalTodayBack->Click += gcnew System::EventHandler(this, &fConfig::btnCalTodayBack_Click);
			this->label37->Location = System::Drawing::Point(3, 207);
			this->label37->Name = L"label37";
			this->label37->Padding = System::Windows::Forms::Padding(0, 7, 0, 0);
			this->label37->Size = System::Drawing::Size(100, 20);
			this->label37->TabIndex = 42;
			this->label37->Text = L"Days other months";
			this->btnCalDaysOtherFront->BackColor = System::Drawing::Color::DimGray;
			this->btnCalDaysOtherFront->ForeColor = System::Drawing::Color::White;
			this->btnCalDaysOtherFront->Location = System::Drawing::Point(109, 210);
			this->btnCalDaysOtherFront->Name = L"btnCalDaysOtherFront";
			this->btnCalDaysOtherFront->Size = System::Drawing::Size(25, 23);
			this->btnCalDaysOtherFront->TabIndex = 41;
			this->btnCalDaysOtherFront->Text = L"Foreground";
			this->btnCalDaysOtherFront->UseVisualStyleBackColor = false;
			this->btnCalDaysOtherFront->Click += gcnew System::EventHandler(this, &fConfig::btnCalDaysOtherFront_Click);
			this->btnCalDaysOtherBack->BackColor = System::Drawing::Color::White;
			this->btnCalDaysOtherBack->ForeColor = System::Drawing::Color::White;
			this->btnCalDaysOtherBack->Location = System::Drawing::Point(140, 210);
			this->btnCalDaysOtherBack->Name = L"btnCalDaysOtherBack";
			this->btnCalDaysOtherBack->Size = System::Drawing::Size(25, 23);
			this->btnCalDaysOtherBack->TabIndex = 43;
			this->btnCalDaysOtherBack->Text = L"Background";
			this->btnCalDaysOtherBack->UseVisualStyleBackColor = false;
			this->btnCalDaysOtherBack->Click += gcnew System::EventHandler(this, &fConfig::btnCalDaysOtherBack_Click);
			this->tpWallpaper->Controls->Add(this->groupBox5);
			this->tpWallpaper->Controls->Add(this->panel1);
			this->tpWallpaper->Controls->Add(this->groupBox4);
			this->tpWallpaper->Controls->Add(this->groupBox3);
			this->tpWallpaper->Location = System::Drawing::Point(4, 40);
			this->tpWallpaper->Name = L"tpWallpaper";
			this->tpWallpaper->Padding = System::Windows::Forms::Padding(3);
			this->tpWallpaper->Size = System::Drawing::Size(348, 292);
			this->tpWallpaper->TabIndex = 2;
			this->tpWallpaper->Text = L"Wallpaper";
			this->tpWallpaper->UseVisualStyleBackColor = true;
			this->groupBox5->Controls->Add(this->flowLayoutPanel3);
			this->groupBox5->Dock = System::Windows::Forms::DockStyle::Top;
			this->groupBox5->Location = System::Drawing::Point(3, 220);
			this->groupBox5->Name = L"groupBox5";
			this->groupBox5->Size = System::Drawing::Size(342, 68);
			this->groupBox5->TabIndex = 7;
			this->groupBox5->TabStop = false;
			this->groupBox5->Text = L"Wallpaper folder";
			this->flowLayoutPanel3->Controls->Add(this->cbWallpaperFolder);
			this->flowLayoutPanel3->Controls->Add(this->tbWallpaperFolder);
			this->flowLayoutPanel3->Controls->Add(this->btnWallpaperFolder);
			this->flowLayoutPanel3->Dock = System::Windows::Forms::DockStyle::Fill;
			this->flowLayoutPanel3->Location = System::Drawing::Point(3, 16);
			this->flowLayoutPanel3->Margin = System::Windows::Forms::Padding(0);
			this->flowLayoutPanel3->Name = L"flowLayoutPanel3";
			this->flowLayoutPanel3->Size = System::Drawing::Size(336, 49);
			this->flowLayoutPanel3->TabIndex = 1;
			this->cbWallpaperFolder->AutoSize = true;
			this->flowLayoutPanel3->SetFlowBreak(this->cbWallpaperFolder, true);
			this->cbWallpaperFolder->Location = System::Drawing::Point(3, 3);
			this->cbWallpaperFolder->Margin = System::Windows::Forms::Padding(3, 3, 3, 0);
			this->cbWallpaperFolder->Name = L"cbWallpaperFolder";
			this->cbWallpaperFolder->Size = System::Drawing::Size(166, 17);
			this->cbWallpaperFolder->TabIndex = 1;
			this->cbWallpaperFolder->Text = L"Use separate wallpaper folder";
			this->cbWallpaperFolder->UseVisualStyleBackColor = true;
			this->cbWallpaperFolder->CheckedChanged += gcnew System::EventHandler(this, &fConfig::cbWallpaperFolder_CheckedChanged);
			this->tbWallpaperFolder->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tbWallpaperFolder->Location = System::Drawing::Point(3, 23);
			this->tbWallpaperFolder->Name = L"tbWallpaperFolder";
			this->tbWallpaperFolder->Size = System::Drawing::Size(220, 20);
			this->tbWallpaperFolder->TabIndex = 2;
			this->btnWallpaperFolder->Location = System::Drawing::Point(229, 23);
			this->btnWallpaperFolder->Name = L"btnWallpaperFolder";
			this->btnWallpaperFolder->Size = System::Drawing::Size(75, 23);
			this->btnWallpaperFolder->TabIndex = 3;
			this->btnWallpaperFolder->Text = L"&Browse";
			this->btnWallpaperFolder->UseVisualStyleBackColor = true;
			this->btnWallpaperFolder->Click += gcnew System::EventHandler(this, &fConfig::btnWallpaperFolder_Click);
			this->panel1->Controls->Add(this->btnWallpaperBackgroundColor);
			this->panel1->Controls->Add(this->label10);
			this->panel1->Dock = System::Windows::Forms::DockStyle::Top;
			this->panel1->Location = System::Drawing::Point(3, 186);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(342, 34);
			this->panel1->TabIndex = 6;
			this->btnWallpaperBackgroundColor->BackColor = System::Drawing::Color::SteelBlue;
			this->btnWallpaperBackgroundColor->ForeColor = System::Drawing::Color::White;
			this->btnWallpaperBackgroundColor->Location = System::Drawing::Point(106, 5);
			this->btnWallpaperBackgroundColor->Name = L"btnWallpaperBackgroundColor";
			this->btnWallpaperBackgroundColor->Size = System::Drawing::Size(204, 23);
			this->btnWallpaperBackgroundColor->TabIndex = 7;
			this->btnWallpaperBackgroundColor->Text = L"Select color";
			this->btnWallpaperBackgroundColor->UseVisualStyleBackColor = false;
			this->btnWallpaperBackgroundColor->Click += gcnew System::EventHandler(this, &fConfig::btnWallpaperBackgroundColor_Click);
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(6, 10);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(94, 13);
			this->label10->TabIndex = 6;
			this->label10->Text = L"Background color:";
			this->groupBox4->Controls->Add(this->btnWallpaperFilenameFont);
			this->groupBox4->Controls->Add(this->label8);
			this->groupBox4->Controls->Add(this->cbWallpaperStretch);
			this->groupBox4->Controls->Add(this->cbWallpaperFilenames);
			this->groupBox4->Dock = System::Windows::Forms::DockStyle::Top;
			this->groupBox4->Location = System::Drawing::Point(3, 92);
			this->groupBox4->Name = L"groupBox4";
			this->groupBox4->Size = System::Drawing::Size(342, 94);
			this->groupBox4->TabIndex = 1;
			this->groupBox4->TabStop = false;
			this->groupBox4->Text = L"Filenames";
			this->btnWallpaperFilenameFont->Font = (gcnew System::Drawing::Font(L"Arial", 8.25, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnWallpaperFilenameFont->ForeColor = System::Drawing::Color::White;
			this->btnWallpaperFilenameFont->Location = System::Drawing::Point(85, 66);
			this->btnWallpaperFilenameFont->Margin = System::Windows::Forms::Padding(3, 0, 3, 3);
			this->btnWallpaperFilenameFont->Name = L"btnWallpaperFilenameFont";
			this->btnWallpaperFilenameFont->Size = System::Drawing::Size(225, 23);
			this->btnWallpaperFilenameFont->TabIndex = 3;
			this->btnWallpaperFilenameFont->Text = L"Select font";
			this->btnWallpaperFilenameFont->UseVisualStyleBackColor = true;
			this->btnWallpaperFilenameFont->Click += gcnew System::EventHandler(this, &fConfig::btnWallpaperFilenameFont_Click);
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(6, 71);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(73, 13);
			this->label8->TabIndex = 2;
			this->label8->Text = L"Filename font:";
			this->cbWallpaperStretch->AutoSize = true;
			this->cbWallpaperStretch->Checked = true;
			this->cbWallpaperStretch->CheckState = System::Windows::Forms::CheckState::Checked;
			this->cbWallpaperStretch->Location = System::Drawing::Point(7, 42);
			this->cbWallpaperStretch->Margin = System::Windows::Forms::Padding(3, 3, 3, 0);
			this->cbWallpaperStretch->Name = L"cbWallpaperStretch";
			this->cbWallpaperStretch->Size = System::Drawing::Size(122, 17);
			this->cbWallpaperStretch->TabIndex = 1;
			this->cbWallpaperStretch->Text = L"Stretch small images";
			this->cbWallpaperStretch->UseVisualStyleBackColor = true;
			this->cbWallpaperFilenames->AutoSize = true;
			this->cbWallpaperFilenames->Checked = true;
			this->cbWallpaperFilenames->CheckState = System::Windows::Forms::CheckState::Checked;
			this->cbWallpaperFilenames->Location = System::Drawing::Point(7, 19);
			this->cbWallpaperFilenames->Name = L"cbWallpaperFilenames";
			this->cbWallpaperFilenames->Size = System::Drawing::Size(176, 17);
			this->cbWallpaperFilenames->TabIndex = 0;
			this->cbWallpaperFilenames->Text = L"Display filename(s) on wallpaper";
			this->cbWallpaperFilenames->UseVisualStyleBackColor = true;
			this->groupBox3->Controls->Add(this->rbWallpaperNone);
			this->groupBox3->Controls->Add(this->rbWallpaperDaily);
			this->groupBox3->Controls->Add(this->rbWallpaperRun);
			this->groupBox3->Dock = System::Windows::Forms::DockStyle::Top;
			this->groupBox3->Location = System::Drawing::Point(3, 3);
			this->groupBox3->Name = L"groupBox3";
			this->groupBox3->Size = System::Drawing::Size(342, 89);
			this->groupBox3->TabIndex = 0;
			this->groupBox3->TabStop = false;
			this->groupBox3->Text = L"Change wallpaper";
			this->rbWallpaperNone->AutoSize = true;
			this->rbWallpaperNone->Checked = true;
			this->rbWallpaperNone->Location = System::Drawing::Point(7, 68);
			this->rbWallpaperNone->Name = L"rbWallpaperNone";
			this->rbWallpaperNone->Size = System::Drawing::Size(182, 17);
			this->rbWallpaperNone->TabIndex = 2;
			this->rbWallpaperNone->TabStop = true;
			this->rbWallpaperNone->Text = L"Never (Do not change wallpaper)";
			this->rbWallpaperNone->UseVisualStyleBackColor = true;
			this->rbWallpaperDaily->AutoSize = true;
			this->rbWallpaperDaily->Location = System::Drawing::Point(7, 43);
			this->rbWallpaperDaily->Name = L"rbWallpaperDaily";
			this->rbWallpaperDaily->Size = System::Drawing::Size(48, 17);
			this->rbWallpaperDaily->TabIndex = 1;
			this->rbWallpaperDaily->Text = L"Daily";
			this->rbWallpaperDaily->UseVisualStyleBackColor = true;
			this->rbWallpaperRun->AutoSize = true;
			this->rbWallpaperRun->Location = System::Drawing::Point(7, 20);
			this->rbWallpaperRun->Name = L"rbWallpaperRun";
			this->rbWallpaperRun->Size = System::Drawing::Size(153, 17);
			this->rbWallpaperRun->TabIndex = 0;
			this->rbWallpaperRun->Text = L"After each screensaver run";
			this->rbWallpaperRun->UseVisualStyleBackColor = true;
			this->tpMonitors->Controls->Add(this->groupBox6);
			this->tpMonitors->Location = System::Drawing::Point(4, 40);
			this->tpMonitors->Name = L"tpMonitors";
			this->tpMonitors->Padding = System::Windows::Forms::Padding(3);
			this->tpMonitors->Size = System::Drawing::Size(348, 292);
			this->tpMonitors->TabIndex = 4;
			this->tpMonitors->Text = L"Monitors";
			this->tpMonitors->UseVisualStyleBackColor = true;
			this->groupBox6->Controls->Add(this->label38);
			this->groupBox6->Controls->Add(this->cbMonitorSequence);
			this->groupBox6->Controls->Add(this->cbSameImage);
			this->groupBox6->Controls->Add(this->tbInfo);
			this->groupBox6->Controls->Add(this->btnCopyMonInfo);
			this->groupBox6->Controls->Add(this->label11);
			this->groupBox6->Controls->Add(this->cbPanoramaStretch);
			this->groupBox6->Dock = System::Windows::Forms::DockStyle::Top;
			this->groupBox6->Location = System::Drawing::Point(3, 3);
			this->groupBox6->Name = L"groupBox6";
			this->groupBox6->Size = System::Drawing::Size(342, 340);
			this->groupBox6->TabIndex = 0;
			this->groupBox6->TabStop = false;
			this->groupBox6->Text = L"Multi monitors settings";
			this->label38->BackColor = System::Drawing::SystemColors::Info;
			this->label38->Dock = System::Windows::Forms::DockStyle::Top;
			this->label38->Location = System::Drawing::Point(3, 96);
			this->label38->Name = L"label38";
			this->label38->Padding = System::Windows::Forms::Padding(17, 0, 0, 0);
			this->label38->Size = System::Drawing::Size(336, 16);
			this->label38->TabIndex = 7;
			this->label38->Text = L"Only for sequential disoplay, not with randomised photos.";
			this->cbMonitorSequence->AutoSize = true;
			this->cbMonitorSequence->Dock = System::Windows::Forms::DockStyle::Top;
			this->cbMonitorSequence->Location = System::Drawing::Point(3, 79);
			this->cbMonitorSequence->Name = L"cbMonitorSequence";
			this->cbMonitorSequence->Size = System::Drawing::Size(336, 17);
			this->cbMonitorSequence->TabIndex = 6;
			this->cbMonitorSequence->Text = L"Move images from monitor to monitor in sequence";
			this->cbMonitorSequence->UseVisualStyleBackColor = true;
			this->cbSameImage->AutoSize = true;
			this->cbSameImage->Dock = System::Windows::Forms::DockStyle::Top;
			this->cbSameImage->Location = System::Drawing::Point(3, 62);
			this->cbSameImage->Name = L"cbSameImage";
			this->cbSameImage->Size = System::Drawing::Size(336, 17);
			this->cbSameImage->TabIndex = 10;
			this->cbSameImage->Text = L"Show same image on all monitors";
			this->cbSameImage->UseVisualStyleBackColor = true;
			this->tbInfo->Location = System::Drawing::Point(188, 118);
			this->tbInfo->Multiline = true;
			this->tbInfo->Name = L"tbInfo";
			this->tbInfo->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->tbInfo->Size = System::Drawing::Size(134, 53);
			this->tbInfo->TabIndex = 9;
			this->tbInfo->Visible = false;
			this->btnCopyMonInfo->AccessibleDescription = L"The information copied can be requested by the developer if you have issues with " 
				L"your multi monitor setup.";
			this->btnCopyMonInfo->AccessibleRole = System::Windows::Forms::AccessibleRole::HelpBalloon;
			this->btnCopyMonInfo->Location = System::Drawing::Point(22, 118);
			this->btnCopyMonInfo->Name = L"btnCopyMonInfo";
			this->btnCopyMonInfo->Size = System::Drawing::Size(160, 23);
			this->btnCopyMonInfo->TabIndex = 8;
			this->btnCopyMonInfo->Text = L"Copy monitor info to clipboard";
			this->btnCopyMonInfo->UseVisualStyleBackColor = true;
			this->btnCopyMonInfo->Click += gcnew System::EventHandler(this, &fConfig::btnCopyMonInfo_Click);
			this->label11->BackColor = System::Drawing::SystemColors::Info;
			this->label11->Dock = System::Windows::Forms::DockStyle::Top;
			this->label11->Location = System::Drawing::Point(3, 33);
			this->label11->Name = L"label11";
			this->label11->Padding = System::Windows::Forms::Padding(17, 0, 0, 0);
			this->label11->Size = System::Drawing::Size(336, 29);
			this->label11->TabIndex = 5;
			this->label11->Text = L"If a panorama photo is to be displayed on the main monitor it will be automatical" 
				L"ly stretched over all available monitors.";
			this->cbPanoramaStretch->AutoSize = true;
			this->cbPanoramaStretch->Checked = true;
			this->cbPanoramaStretch->CheckState = System::Windows::Forms::CheckState::Checked;
			this->cbPanoramaStretch->Dock = System::Windows::Forms::DockStyle::Top;
			this->cbPanoramaStretch->Location = System::Drawing::Point(3, 16);
			this->cbPanoramaStretch->Name = L"cbPanoramaStretch";
			this->cbPanoramaStretch->Size = System::Drawing::Size(336, 17);
			this->cbPanoramaStretch->TabIndex = 4;
			this->cbPanoramaStretch->Text = L"Smart stretch panorama photos";
			this->cbPanoramaStretch->UseVisualStyleBackColor = true;
			this->tpRaw->Controls->Add(this->label51);
			this->tpRaw->Controls->Add(this->cbEnableRaw);
			this->tpRaw->Controls->Add(this->gbRawAdvanced);
			this->tpRaw->Location = System::Drawing::Point(4, 40);
			this->tpRaw->Name = L"tpRaw";
			this->tpRaw->Padding = System::Windows::Forms::Padding(3);
			this->tpRaw->Size = System::Drawing::Size(348, 292);
			this->tpRaw->TabIndex = 9;
			this->tpRaw->Text = L"RAW";
			this->tpRaw->UseVisualStyleBackColor = true;
			this->label51->Anchor = System::Windows::Forms::AnchorStyles::None;
			this->label51->BackColor = System::Drawing::SystemColors::Info;
			this->label51->Location = System::Drawing::Point(3, 3);
			this->label51->Name = L"label51";
			this->label51->Padding = System::Windows::Forms::Padding(8, 0, 0, 0);
			this->label51->Size = System::Drawing::Size(342, 29);
			this->label51->TabIndex = 9;
			this->label51->Text = L"To display RAW files JPG versions are created and stored.\r\nPlease note screensave" 
				L"r will run slow whilst an image is converted.";
			this->cbEnableRaw->AutoSize = true;
			this->cbEnableRaw->Location = System::Drawing::Point(14, 35);
			this->cbEnableRaw->Name = L"cbEnableRaw";
			this->cbEnableRaw->Size = System::Drawing::Size(186, 17);
			this->cbEnableRaw->TabIndex = 10;
			this->cbEnableRaw->Text = L"Show RAW files (Cached as JPG)";
			this->cbEnableRaw->UseVisualStyleBackColor = true;
			this->gbRawAdvanced->Controls->Add(this->label55);
			this->gbRawAdvanced->Controls->Add(this->tbRawParameters);
			this->gbRawAdvanced->Controls->Add(this->label54);
			this->gbRawAdvanced->Controls->Add(this->label52);
			this->gbRawAdvanced->Controls->Add(this->tbRawConverter);
			this->gbRawAdvanced->Controls->Add(this->btnRawConverter);
			this->gbRawAdvanced->Controls->Add(this->cbRawCacheInFolder);
			this->gbRawAdvanced->Controls->Add(this->groupBox8);
			this->gbRawAdvanced->Controls->Add(this->lSeparateFolder);
			this->gbRawAdvanced->Controls->Add(this->tbRawCacheFolder);
			this->gbRawAdvanced->Controls->Add(this->btnRawCacheFolder);
			this->gbRawAdvanced->Controls->Add(this->tbRawExtensions);
			this->gbRawAdvanced->Location = System::Drawing::Point(2, 58);
			this->gbRawAdvanced->Name = L"gbRawAdvanced";
			this->gbRawAdvanced->Size = System::Drawing::Size(339, 228);
			this->gbRawAdvanced->TabIndex = 8;
			this->gbRawAdvanced->TabStop = false;
			this->gbRawAdvanced->Text = L"Advanced settings";
			this->label55->AutoSize = true;
			this->label55->Location = System::Drawing::Point(4, 189);
			this->label55->Margin = System::Windows::Forms::Padding(3, 7, 3, 0);
			this->label55->Name = L"label55";
			this->label55->Size = System::Drawing::Size(89, 13);
			this->label55->TabIndex = 14;
			this->label55->Text = L"RAW extensions:";
			this->tbRawParameters->Location = System::Drawing::Point(7, 160);
			this->tbRawParameters->Name = L"tbRawParameters";
			this->tbRawParameters->Size = System::Drawing::Size(326, 20);
			this->tbRawParameters->TabIndex = 12;
			this->label54->BackColor = System::Drawing::SystemColors::Info;
			this->label54->Location = System::Drawing::Point(4, 139);
			this->label54->Name = L"label54";
			this->label54->Size = System::Drawing::Size(342, 18);
			this->label54->TabIndex = 11;
			this->label54->Text = L"Convertor parameters: (Use #RAW# #JPG# placeholders)";
			this->label52->AutoSize = true;
			this->label52->Location = System::Drawing::Point(6, 120);
			this->label52->Margin = System::Windows::Forms::Padding(3, 7, 3, 0);
			this->label52->Name = L"label52";
			this->label52->Size = System::Drawing::Size(56, 13);
			this->label52->TabIndex = 9;
			this->label52->Text = L"Converter:";
			this->tbRawConverter->Location = System::Drawing::Point(90, 117);
			this->tbRawConverter->Name = L"tbRawConverter";
			this->tbRawConverter->Size = System::Drawing::Size(159, 20);
			this->tbRawConverter->TabIndex = 8;
			this->btnRawConverter->Location = System::Drawing::Point(257, 115);
			this->btnRawConverter->Name = L"btnRawConverter";
			this->btnRawConverter->Size = System::Drawing::Size(75, 23);
			this->btnRawConverter->TabIndex = 10;
			this->btnRawConverter->Text = L"&Browse";
			this->btnRawConverter->UseVisualStyleBackColor = true;
			this->btnRawConverter->Click += gcnew System::EventHandler(this, &fConfig::btnRawConverter_Click);
			this->cbRawCacheInFolder->AutoSize = true;
			this->cbRawCacheInFolder->Location = System::Drawing::Point(12, 19);
			this->cbRawCacheInFolder->Name = L"cbRawCacheInFolder";
			this->cbRawCacheInFolder->Size = System::Drawing::Size(230, 17);
			this->cbRawCacheInFolder->TabIndex = 7;
			this->cbRawCacheInFolder->Text = L"Cache JPEG file in same folder as RAW file";
			this->cbRawCacheInFolder->UseVisualStyleBackColor = true;
			this->cbRawCacheInFolder->CheckStateChanged += gcnew System::EventHandler(this, &fConfig::cbRawCacheInFolder_CheckStateChanged);
			this->groupBox8->Controls->Add(this->rbRawCacheDimensionsOriginal);
			this->groupBox8->Controls->Add(this->rbRawCacheDimensionsMonitor);
			this->groupBox8->Location = System::Drawing::Point(6, 66);
			this->groupBox8->Name = L"groupBox8";
			this->groupBox8->Size = System::Drawing::Size(326, 43);
			this->groupBox8->TabIndex = 6;
			this->groupBox8->TabStop = false;
			this->groupBox8->Text = L"Cached image size";
			this->rbRawCacheDimensionsOriginal->AutoSize = true;
			this->rbRawCacheDimensionsOriginal->Location = System::Drawing::Point(185, 19);
			this->rbRawCacheDimensionsOriginal->Name = L"rbRawCacheDimensionsOriginal";
			this->rbRawCacheDimensionsOriginal->Size = System::Drawing::Size(107, 17);
			this->rbRawCacheDimensionsOriginal->TabIndex = 2;
			this->rbRawCacheDimensionsOriginal->Text = L"Keep original size";
			this->rbRawCacheDimensionsOriginal->UseVisualStyleBackColor = true;
			this->rbRawCacheDimensionsMonitor->AutoSize = true;
			this->rbRawCacheDimensionsMonitor->Checked = true;
			this->rbRawCacheDimensionsMonitor->Location = System::Drawing::Point(6, 19);
			this->rbRawCacheDimensionsMonitor->Name = L"rbRawCacheDimensionsMonitor";
			this->rbRawCacheDimensionsMonitor->Size = System::Drawing::Size(173, 17);
			this->rbRawCacheDimensionsMonitor->TabIndex = 0;
			this->rbRawCacheDimensionsMonitor->TabStop = true;
			this->rbRawCacheDimensionsMonitor->Text = L"Biggest monitor (recommended)";
			this->rbRawCacheDimensionsMonitor->UseVisualStyleBackColor = true;
			this->lSeparateFolder->AutoSize = true;
			this->lSeparateFolder->ForeColor = System::Drawing::SystemColors::ControlText;
			this->lSeparateFolder->Location = System::Drawing::Point(7, 42);
			this->lSeparateFolder->Margin = System::Windows::Forms::Padding(3, 7, 3, 0);
			this->lSeparateFolder->Name = L"lSeparateFolder";
			this->lSeparateFolder->Size = System::Drawing::Size(82, 13);
			this->lSeparateFolder->TabIndex = 4;
			this->lSeparateFolder->Text = L"Separate folder:";
			this->tbRawCacheFolder->Location = System::Drawing::Point(91, 39);
			this->tbRawCacheFolder->Name = L"tbRawCacheFolder";
			this->tbRawCacheFolder->Size = System::Drawing::Size(159, 20);
			this->tbRawCacheFolder->TabIndex = 3;
			this->btnRawCacheFolder->Location = System::Drawing::Point(258, 37);
			this->btnRawCacheFolder->Name = L"btnRawCacheFolder";
			this->btnRawCacheFolder->Size = System::Drawing::Size(75, 23);
			this->btnRawCacheFolder->TabIndex = 5;
			this->btnRawCacheFolder->Text = L"&Browse";
			this->btnRawCacheFolder->UseVisualStyleBackColor = true;
			this->btnRawCacheFolder->Click += gcnew System::EventHandler(this, &fConfig::btnRawCacheFolder_Click);
			this->tbRawExtensions->Location = System::Drawing::Point(93, 186);
			this->tbRawExtensions->Name = L"tbRawExtensions";
			this->tbRawExtensions->Size = System::Drawing::Size(239, 20);
			this->tbRawExtensions->TabIndex = 13;
			this->tpShortcuts->Controls->Add(this->rtbShortcutKeys);
			this->tpShortcuts->Location = System::Drawing::Point(4, 40);
			this->tpShortcuts->Name = L"tpShortcuts";
			this->tpShortcuts->Padding = System::Windows::Forms::Padding(3);
			this->tpShortcuts->Size = System::Drawing::Size(348, 292);
			this->tpShortcuts->TabIndex = 3;
			this->tpShortcuts->Text = L"Keys";
			this->tpShortcuts->UseVisualStyleBackColor = true;
			this->rtbShortcutKeys->BackColor = System::Drawing::SystemColors::ControlLightLight;
			this->rtbShortcutKeys->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->rtbShortcutKeys->Dock = System::Windows::Forms::DockStyle::Fill;
			this->rtbShortcutKeys->Location = System::Drawing::Point(3, 3);
			this->rtbShortcutKeys->Name = L"rtbShortcutKeys";
			this->rtbShortcutKeys->ReadOnly = true;
			this->rtbShortcutKeys->Size = System::Drawing::Size(342, 286);
			this->rtbShortcutKeys->TabIndex = 0;
			this->rtbShortcutKeys->Text = resources->GetString(L"rtbShortcutKeys.Text");
			this->rtbShortcutKeys->LinkClicked += gcnew System::Windows::Forms::LinkClickedEventHandler(this, &fConfig::rtbShortcutKeys_LinkClicked);
			this->tpMetadata->Controls->Add(this->label42);
			this->tpMetadata->Controls->Add(this->nupMetadata);
			this->tpMetadata->Controls->Add(this->label41);
			this->tpMetadata->Controls->Add(this->panel4);
			this->tpMetadata->Controls->Add(this->label39);
			this->tpMetadata->Controls->Add(this->label40);
			this->tpMetadata->Controls->Add(this->panel2);
			this->tpMetadata->Controls->Add(this->cbMetadata);
			this->tpMetadata->Location = System::Drawing::Point(4, 40);
			this->tpMetadata->Name = L"tpMetadata";
			this->tpMetadata->Size = System::Drawing::Size(348, 292);
			this->tpMetadata->TabIndex = 8;
			this->tpMetadata->Text = L"Metadata";
			this->tpMetadata->UseVisualStyleBackColor = true;
			this->label42->AutoSize = true;
			this->label42->Location = System::Drawing::Point(89, 138);
			this->label42->Name = L"label42";
			this->label42->Size = System::Drawing::Size(132, 13);
			this->label42->TabIndex = 15;
			this->label42->Text = L"floating metadata windows";
			this->nupMetadata->Location = System::Drawing::Point(43, 136);
			this->nupMetadata->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {25, 0, 0, 0});
			this->nupMetadata->Name = L"nupMetadata";
			this->nupMetadata->Size = System::Drawing::Size(40, 20);
			this->nupMetadata->TabIndex = 14;
			this->label41->AutoSize = true;
			this->label41->Location = System::Drawing::Point(3, 138);
			this->label41->Name = L"label41";
			this->label41->Size = System::Drawing::Size(34, 13);
			this->label41->TabIndex = 13;
			this->label41->Text = L"Show";
			this->panel4->Controls->Add(this->label44);
			this->panel4->Controls->Add(this->label45);
			this->panel4->Controls->Add(this->label46);
			this->panel4->Controls->Add(this->tbMDHigh);
			this->panel4->Controls->Add(this->tbMDLow);
			this->panel4->Location = System::Drawing::Point(6, 162);
			this->panel4->Name = L"panel4";
			this->panel4->Size = System::Drawing::Size(142, 82);
			this->panel4->TabIndex = 12;
			this->label44->AutoSize = true;
			this->label44->Location = System::Drawing::Point(3, 0);
			this->label44->Name = L"label44";
			this->label44->Size = System::Drawing::Size(91, 13);
			this->label44->TabIndex = 38;
			this->label44->Text = L"Metadata Opacity";
			this->label45->AutoSize = true;
			this->label45->Location = System::Drawing::Point(3, 57);
			this->label45->Name = L"label45";
			this->label45->Size = System::Drawing::Size(25, 13);
			this->label45->TabIndex = 37;
			this->label45->Text = L"2nd";
			this->label46->AutoSize = true;
			this->label46->Location = System::Drawing::Point(3, 25);
			this->label46->Name = L"label46";
			this->label46->Size = System::Drawing::Size(21, 13);
			this->label46->TabIndex = 36;
			this->label46->Text = L"1st";
			this->tbMDHigh->AutoSize = false;
			this->tbMDHigh->BackColor = System::Drawing::SystemColors::ControlLightLight;
			this->tbMDHigh->LargeChange = 10;
			this->tbMDHigh->Location = System::Drawing::Point(32, 51);
			this->tbMDHigh->Maximum = 100;
			this->tbMDHigh->Name = L"tbMDHigh";
			this->tbMDHigh->Size = System::Drawing::Size(104, 20);
			this->tbMDHigh->TabIndex = 35;
			this->tbMDHigh->TickFrequency = 10;
			this->tbMDHigh->Value = 90;
			this->tbMDLow->AutoSize = false;
			this->tbMDLow->BackColor = System::Drawing::SystemColors::ControlLightLight;
			this->tbMDLow->LargeChange = 10;
			this->tbMDLow->Location = System::Drawing::Point(32, 23);
			this->tbMDLow->Maximum = 100;
			this->tbMDLow->Name = L"tbMDLow";
			this->tbMDLow->Size = System::Drawing::Size(104, 20);
			this->tbMDLow->TabIndex = 34;
			this->tbMDLow->TickFrequency = 10;
			this->tbMDLow->Value = 50;
			this->label39->AutoSize = true;
			this->label39->Location = System::Drawing::Point(0, 117);
			this->label39->Name = L"label39";
			this->label39->Size = System::Drawing::Size(133, 13);
			this->label39->TabIndex = 4;
			this->label39->Text = L"Floating metadata settings:";
			this->label40->BackColor = System::Drawing::SystemColors::Info;
			this->label40->Location = System::Drawing::Point(0, 269);
			this->label40->Name = L"label40";
			this->label40->Padding = System::Windows::Forms::Padding(0, 3, 0, 0);
			this->label40->Size = System::Drawing::Size(348, 23);
			this->label40->TabIndex = 2;
			this->label40->Text = L"Press Ctrl+M when screensaver is running to reposition and resize.";
			this->panel2->Controls->Add(this->btnMetadataFont);
			this->panel2->Controls->Add(this->cbQuickSimpleMetadata);
			this->panel2->Controls->Add(this->cbLocation);
			this->panel2->Controls->Add(this->tbSimpleMetadata);
			this->panel2->Location = System::Drawing::Point(3, 26);
			this->panel2->Name = L"panel2";
			this->panel2->Size = System::Drawing::Size(339, 86);
			this->panel2->TabIndex = 1;
			this->btnMetadataFont->Font = (gcnew System::Drawing::Font(L"Arial", 8.25, System::Drawing::FontStyle::Bold));
			this->btnMetadataFont->ForeColor = System::Drawing::Color::White;
			this->btnMetadataFont->Location = System::Drawing::Point(250, 58);
			this->btnMetadataFont->Name = L"btnMetadataFont";
			this->btnMetadataFont->Size = System::Drawing::Size(86, 25);
			this->btnMetadataFont->TabIndex = 4;
			this->btnMetadataFont->Text = L"Metadata Font";
			this->btnMetadataFont->UseVisualStyleBackColor = true;
			this->btnMetadataFont->Click += gcnew System::EventHandler(this, &fConfig::btnMetadataFont_Click);
			this->cbQuickSimpleMetadata->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cbQuickSimpleMetadata->FormattingEnabled = true;
			this->cbQuickSimpleMetadata->Items->AddRange(gcnew cli::array< System::Object^  >(1) {L"Quick select metadata"});
			this->cbQuickSimpleMetadata->Location = System::Drawing::Point(104, 60);
			this->cbQuickSimpleMetadata->Name = L"cbQuickSimpleMetadata";
			this->cbQuickSimpleMetadata->Size = System::Drawing::Size(140, 21);
			this->cbQuickSimpleMetadata->TabIndex = 3;
			this->cbQuickSimpleMetadata->SelectedIndexChanged += gcnew System::EventHandler(this, &fConfig::cbQuickSimpleMetadata_SelectedIndexChanged);
			this->cbLocation->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cbLocation->FormattingEnabled = true;
			this->cbLocation->Items->AddRange(gcnew cli::array< System::Object^  >(9) {L"Top Left", L"Top Middle", L"Top Right", L"Middle Left", 
				L"Middle Middle", L"Middle Right", L"Bottom Left", L"Bottom Middle", L"Bottom Right"});
			this->cbLocation->Location = System::Drawing::Point(3, 60);
			this->cbLocation->Name = L"cbLocation";
			this->cbLocation->Size = System::Drawing::Size(95, 21);
			this->cbLocation->TabIndex = 1;
			this->tbSimpleMetadata->Location = System::Drawing::Point(3, 3);
			this->tbSimpleMetadata->Multiline = true;
			this->tbSimpleMetadata->Name = L"tbSimpleMetadata";
			this->tbSimpleMetadata->ScrollBars = System::Windows::Forms::ScrollBars::Both;
			this->tbSimpleMetadata->Size = System::Drawing::Size(333, 54);
			this->tbSimpleMetadata->TabIndex = 0;
			this->tbSimpleMetadata->WordWrap = false;
			this->cbMetadata->AutoSize = true;
			this->cbMetadata->Checked = true;
			this->cbMetadata->CheckState = System::Windows::Forms::CheckState::Checked;
			this->cbMetadata->Location = System::Drawing::Point(3, 3);
			this->cbMetadata->Name = L"cbMetadata";
			this->cbMetadata->Size = System::Drawing::Size(189, 17);
			this->cbMetadata->TabIndex = 0;
			this->cbMetadata->Text = L"Show metadata (EXIF/IPTC/XMP)";
			this->cbMetadata->UseVisualStyleBackColor = true;
			this->tpSupport->Controls->Add(this->rtbDonate);
			this->tpSupport->Location = System::Drawing::Point(4, 40);
			this->tpSupport->Name = L"tpSupport";
			this->tpSupport->Padding = System::Windows::Forms::Padding(3);
			this->tpSupport->Size = System::Drawing::Size(348, 292);
			this->tpSupport->TabIndex = 5;
			this->tpSupport->Text = L"Donate";
			this->tpSupport->UseVisualStyleBackColor = true;
			this->rtbDonate->BackColor = System::Drawing::SystemColors::ControlLightLight;
			this->rtbDonate->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->rtbDonate->Dock = System::Windows::Forms::DockStyle::Fill;
			this->rtbDonate->Location = System::Drawing::Point(3, 3);
			this->rtbDonate->Name = L"rtbDonate";
			this->rtbDonate->ReadOnly = true;
			this->rtbDonate->Size = System::Drawing::Size(342, 286);
			this->rtbDonate->TabIndex = 1;
			this->rtbDonate->Text = resources->GetString(L"rtbDonate.Text");
			this->rtbDonate->LinkClicked += gcnew System::Windows::Forms::LinkClickedEventHandler(this, &fConfig::rtbShortcutKeys_LinkClicked);
			this->btnOk->Location = System::Drawing::Point(266, 435);
			this->btnOk->Name = L"btnOk";
			this->btnOk->Size = System::Drawing::Size(96, 23);
			this->btnOk->TabIndex = 1;
			this->btnOk->Text = L"&Ok";
			this->btnOk->UseVisualStyleBackColor = true;
			this->btnOk->Click += gcnew System::EventHandler(this, &fConfig::btnOk_Click);
			this->btnCancel->Location = System::Drawing::Point(265, 462);
			this->btnCancel->Name = L"btnCancel";
			this->btnCancel->Size = System::Drawing::Size(97, 23);
			this->btnCancel->TabIndex = 2;
			this->btnCancel->Text = L"&Cancel";
			this->btnCancel->UseVisualStyleBackColor = true;
			this->btnCancel->Click += gcnew System::EventHandler(this, &fConfig::btnCancel_Click);
			this->lVersionInfo->AutoSize = true;
			this->lVersionInfo->Font = (gcnew System::Drawing::Font(L"Arial", 16, System::Drawing::FontStyle::Bold));
			this->lVersionInfo->ForeColor = System::Drawing::SystemColors::HotTrack;
			this->lVersionInfo->Location = System::Drawing::Point(6, 61);
			this->lVersionInfo->Name = L"lVersionInfo";
			this->lVersionInfo->Size = System::Drawing::Size(354, 26);
			this->lVersionInfo->TabIndex = 3;
			this->lVersionInfo->Text = L"Random Photo Screensaver 3.4.6";
			this->rtbDonation->BackColor = System::Drawing::SystemColors::Control;
			this->rtbDonation->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->rtbDonation->Cursor = System::Windows::Forms::Cursors::Hand;
			this->rtbDonation->Location = System::Drawing::Point(11, 435);
			this->rtbDonation->Name = L"rtbDonation";
			this->rtbDonation->ReadOnly = true;
			this->rtbDonation->Size = System::Drawing::Size(245, 88);
			this->rtbDonation->TabIndex = 4;
			this->rtbDonation->Text = resources->GetString(L"rtbDonation.Text");
			this->rtbDonation->Click += gcnew System::EventHandler(this, &fConfig::rtbDonation_Click);
			this->btnDonate->BackColor = System::Drawing::Color::Gold;
			this->btnDonate->Cursor = System::Windows::Forms::Cursors::Hand;
			this->btnDonate->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btnDonate->Location = System::Drawing::Point(190, 491);
			this->btnDonate->Name = L"btnDonate";
			this->btnDonate->Size = System::Drawing::Size(172, 32);
			this->btnDonate->TabIndex = 5;
			this->btnDonate->Text = L"Donate";
			this->btnDonate->UseVisualStyleBackColor = false;
			this->btnDonate->Click += gcnew System::EventHandler(this, &fConfig::btnDonate_Click);
			this->pictureBox1->Cursor = System::Windows::Forms::Cursors::Hand;
			this->pictureBox1->Image = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pictureBox1.Image")));
			this->pictureBox1->InitialImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"pictureBox1.InitialImage")));
			this->pictureBox1->Location = System::Drawing::Point(0, -7);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(370, 65);
			this->pictureBox1->TabIndex = 6;
			this->pictureBox1->TabStop = false;
			this->pictureBox1->Click += gcnew System::EventHandler(this, &fConfig::pictureBox1_Click);
			this->label14->AutoSize = true;
			this->label14->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)), static_cast<System::Int32>(static_cast<System::Byte>(204)), 
				static_cast<System::Int32>(static_cast<System::Byte>(0)));
			this->label14->Font = (gcnew System::Drawing::Font(L"Arial", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label14->ForeColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(0)), static_cast<System::Int32>(static_cast<System::Byte>(102)), 
				static_cast<System::Int32>(static_cast<System::Byte>(255)));
			this->label14->Location = System::Drawing::Point(312, 47);
			this->label14->Name = L"label14";
			this->label14->Size = System::Drawing::Size(42, 19);
			this->label14->TabIndex = 7;
			this->label14->Text = L"beta";
			this->label14->Visible = false;
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(369, 530);
			this->Controls->Add(this->label14);
			this->Controls->Add(this->pictureBox1);
			this->Controls->Add(this->btnDonate);
			this->Controls->Add(this->lVersionInfo);
			this->Controls->Add(this->btnCancel);
			this->Controls->Add(this->btnOk);
			this->Controls->Add(this->tcTabs);
			this->Controls->Add(this->rtbDonation);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->Name = L"fConfig";
			this->Text = L"Configuration Random Photo Screensaver";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &fConfig::fConfig_FormClosing);
			this->tcTabs->ResumeLayout(false);
			this->tpGeneral->ResumeLayout(false);
			this->tpGeneral->PerformLayout();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->flowLayoutPanel1->ResumeLayout(false);
			this->flowLayoutPanel1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nupTimeout))->EndInit();
			this->tpAdvanced->ResumeLayout(false);
			this->tpAdvanced->PerformLayout();
			this->flowLayoutPanel4->ResumeLayout(false);
			this->flowLayoutPanel4->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nupFolderInfoTimeout))->EndInit();
			this->tpClock->ResumeLayout(false);
			this->flowLayoutPanel2->ResumeLayout(false);
			this->flowLayoutPanel2->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			this->tpCalendar->ResumeLayout(false);
			this->flowLayoutPanel5->ResumeLayout(false);
			this->flowLayoutPanel5->PerformLayout();
			this->splitContainer1->Panel1->ResumeLayout(false);
			this->splitContainer1->Panel1->PerformLayout();
			this->splitContainer1->Panel2->ResumeLayout(false);
			this->splitContainer1->Panel2->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->splitContainer1))->EndInit();
			this->splitContainer1->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nupNrFolCalendars))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nupNrPrevCalendars))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->tbHigh))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->tbLow))->EndInit();
			this->tpWallpaper->ResumeLayout(false);
			this->groupBox5->ResumeLayout(false);
			this->flowLayoutPanel3->ResumeLayout(false);
			this->flowLayoutPanel3->PerformLayout();
			this->panel1->ResumeLayout(false);
			this->panel1->PerformLayout();
			this->groupBox4->ResumeLayout(false);
			this->groupBox4->PerformLayout();
			this->groupBox3->ResumeLayout(false);
			this->groupBox3->PerformLayout();
			this->tpMonitors->ResumeLayout(false);
			this->groupBox6->ResumeLayout(false);
			this->groupBox6->PerformLayout();
			this->tpRaw->ResumeLayout(false);
			this->tpRaw->PerformLayout();
			this->gbRawAdvanced->ResumeLayout(false);
			this->gbRawAdvanced->PerformLayout();
			this->groupBox8->ResumeLayout(false);
			this->groupBox8->PerformLayout();
			this->tpShortcuts->ResumeLayout(false);
			this->tpMetadata->ResumeLayout(false);
			this->tpMetadata->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->nupMetadata))->EndInit();
			this->panel4->ResumeLayout(false);
			this->panel4->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->tbMDHigh))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->tbMDLow))->EndInit();
			this->panel2->ResumeLayout(false);
			this->panel2->PerformLayout();
			this->tpSupport->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void FontDialogButtonClick(Button^ button) {
	  FontDialog^ fdFilename;
		fdFilename = gcnew System::Windows::Forms::FontDialog;
		fdFilename->Font = button->Font;
		fdFilename->Color = button->ForeColor;
		fdFilename->FontMustExist = true;
		fdFilename->ShowHelp = true;
		fdFilename->ShowColor = true;
		fdFilename->ShowEffects = true;
		System::Windows::Forms::DialogResult result = fdFilename->ShowDialog();
    if ( result == ::DialogResult::OK ) {
			button->ForeColor = fdFilename->Color;
			button->Font = fdFilename->Font;
    }
		button->BackColor = adjustToBrightness(button->ForeColor);
	}

	private: System::Void ColorDialogButtonClick(Button^ button) {
	  ColorDialog^ cdBackground;
		cdBackground = gcnew System::Windows::Forms::ColorDialog;
		cdBackground->ShowHelp = true;
		cdBackground->FullOpen = true;
		cdBackground->Color = button->BackColor;
		System::Windows::Forms::DialogResult result = cdBackground->ShowDialog();
    if ( result == ::DialogResult::OK ) {
       button->BackColor = cdBackground->Color;
    }
		button->ForeColor = adjustToBrightness(button->BackColor);
	}

	private: System::Void FolderDialogButtonClick(TextBox^ folder) {
		this->FolderDialogButtonClick(folder, "Select the directory that contains your images.");
	}

	private: System::Void FolderDialogButtonClick(TextBox^ folder, String^ description) {
	  FolderBrowserDialog^ fbdFolder;
		fbdFolder = gcnew System::Windows::Forms::FolderBrowserDialog;
		fbdFolder->Description = description;
		fbdFolder->ShowNewFolderButton = false;
		fbdFolder->SelectedPath = folder->Text;
		System::Windows::Forms::DialogResult result = fbdFolder->ShowDialog();
    if ( result == ::DialogResult::OK ) {
      folder->Text = fbdFolder->SelectedPath;
    }
	}

	private: System::Void FileDialogButtonClick(TextBox^ file) {
	  OpenFileDialog^ fbdFile;
		fbdFile = gcnew System::Windows::Forms::OpenFileDialog;
		fbdFile->FileName = file->Text;
		System::Windows::Forms::DialogResult result = fbdFile->ShowDialog();
    if ( result == ::DialogResult::OK ) {
      file->Text = fbdFile->FileName;
    }
	}

	private: System::Void btnOk_Click(System::Object^  sender, System::EventArgs^  e) {
	  Transfer(true);
		if (this->_tbFolderValue != this->tbFolder->Text) {

			// !!Biggest hack in screensaver!!
			// Can't get proper communication from fConfig form to Engine form. fConfig will change Form Text when attention is required by Engine form.
			if (this->engine != nullptr) {
				this->configAction = caFoldersChanged;
				this->engine->Text = this->lVersionInfo->Text + " ##";
			}

			// Selected folder(s) changed
			//System::Windows::Forms::MessageBox::Show (this->_tbFolderValue + " != " + this->tbFolder->Text) ;
		}
		if (!engineRunning) Application::Exit();
		else {
			this->Hide();
		}
	}

	public: void resetEngineText() {
		this->engine->Text = this->lVersionInfo->Text;
	}

	private: System::Void btnCancel_Click(System::Object^ sender, System::EventArgs^  e) {
		if (!engineRunning) Application::Exit();
		else {
			Transfer(false);
			this->Hide();
		}
	}

	private: System::Void btnFolder_Click(System::Object^  sender, System::EventArgs^  e) {
	  FolderDialogButtonClick(tbFolder);
	}

	public: System::Void btnFolder_Click() {
	  FolderDialogButtonClick(tbFolder);
	}

	private: System::Void btnWallpaperFolder_Click(System::Object^  sender, System::EventArgs^  e) {
	  FolderDialogButtonClick(tbWallpaperFolder);
	}

	private: System::Void btnRawCacheFolder_Click(System::Object^  sender, System::EventArgs^  e) {
	  FolderDialogButtonClick(tbRawCacheFolder);
	}

	private: System::Void btnRawConverter_Click(System::Object^  sender, System::EventArgs^  e) {
	  FileDialogButtonClick(tbRawConverter);
	}

	private: System::Void cbEnableRaw_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		gbRawAdvanced->Enabled = cbEnableRaw->Checked;
	}

	private: System::Void btnBackgroundColor_Click(System::Object^  sender, System::EventArgs^  e) {
		ColorDialogButtonClick(btnBackgroundColor);
	}

	private: System::Void btnWallpaperBackgroundColor_Click(System::Object^  sender, System::EventArgs^  e) {
		ColorDialogButtonClick(btnWallpaperBackgroundColor);
	}

	private: System::Void btnWallpaperFilenameFont_Click(System::Object^  sender, System::EventArgs^  e) {
		FontDialogButtonClick(btnWallpaperFilenameFont);
	}

	private: System::Void btnFilenamefont_Click(System::Object^  sender, System::EventArgs^  e) {
		FontDialogButtonClick(btnFilenamefont);
	}

	private: System::Void btnMetadataFont_Click(System::Object^  sender, System::EventArgs^  e) {
		FontDialogButtonClick(btnMetadataFont);
	}

	private: System::Void btnClockFont_Click(System::Object^  sender, System::EventArgs^  e) {
		FontDialogButtonClick(btnClockFont);
		getClockSettingsFromBtn(cbClockMonitor->SelectedIndex);
	}

	private: System::Void rbClockNo_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		this->clockType[cbClockMonitor->SelectedIndex] = ctNone;
	}

	private: System::Void rbClockTime_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		this->clockType[cbClockMonitor->SelectedIndex] = ctCurrent;
	}

	private: System::Void rbClockRun_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		this->clockType[cbClockMonitor->SelectedIndex] = ctRunning;
	}

	private: System::Void cbClockMonitor_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
		getClockSettingsFromBtn(oldClockMonitorIndex);
		oldClockMonitorIndex = cbClockMonitor->SelectedIndex;
		setClockSettingsToBtn(cbClockMonitor->SelectedIndex);
	}

	private: System::Void rtbShortcutKeys_LinkClicked(System::Object^  sender, System::Windows::Forms::LinkClickedEventArgs^  e) {
	  System::Diagnostics::Process::Start( e->LinkText );
	}

	private: System::Void cbWallpaperFolder_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
	  updateWallpaperFolderSelection();
	}

	private: System::Void rtbDonation_Click(System::Object^  sender, System::EventArgs^  e) {
	  System::Diagnostics::Process::Start(PAYPAL_URL);
	}

	private: System::Void btnDonate_Click(System::Object^  sender, System::EventArgs^  e) {
	  System::Diagnostics::Process::Start(PAYPAL_URL);
	}

	private: System::Void pictureBox1_Click(System::Object^  sender, System::EventArgs^  e) {
	  System::Diagnostics::Process::Start(PAYPAL_URL);
	}

	private: System::Void fConfig_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
		// Check if screensaver running if so close otherwise hide window
	 if ((action == saRun) && (e->CloseReason == System::Windows::Forms::CloseReason::UserClosing)) {
			this->Hide();
			resizeNrCalendars();
			resizeNrMetadata();
			::Cursor::Hide();
			e->Cancel = true;
		}
	}

	private: System::Void btnCalTitleFront_Click(System::Object^  sender, System::EventArgs^  e) {
		ColorDialogButtonClick(btnCalTitleFront);
	}

	private: System::Void btnCalDayFront_Click(System::Object^  sender, System::EventArgs^  e) {
		ColorDialogButtonClick(btnCalDayFront);
	}	

	private: System::Void btnCalTitleBack_Click(System::Object^  sender, System::EventArgs^  e) {
		ColorDialogButtonClick(btnCalTitleBack);
	}	

	private: System::Void btnDayNamesBack_Click(System::Object^  sender, System::EventArgs^  e) {
		ColorDialogButtonClick(btnDayNamesBack);
	}	

	private: System::Void btnDayNamesFront_Click(System::Object^  sender, System::EventArgs^  e) {
		ColorDialogButtonClick(btnDayNamesFront);
	}	

	private: System::Void btnCalDaysOtherBack_Click(System::Object^  sender, System::EventArgs^  e) {
		ColorDialogButtonClick(btnCalDaysOtherBack);
	}	

	private: System::Void btnCalDaysOtherFront_Click(System::Object^  sender, System::EventArgs^  e) {
		ColorDialogButtonClick(btnCalDaysOtherFront);
	}	

	private: System::Void btnCalDayBack_Click(System::Object^  sender, System::EventArgs^  e) {
		ColorDialogButtonClick(btnCalDayBack);
	}	

	private: System::Void btnCalTodayFront_Click(System::Object^  sender, System::EventArgs^  e) {
		ColorDialogButtonClick(btnCalTodayFront);
	}	

	private: System::Void btnCalTodayBack_Click(System::Object^  sender, System::EventArgs^  e) {
		ColorDialogButtonClick(btnCalTodayBack);
	}	

	private: System::Void btnCalendarFont_Click(System::Object^  sender, System::EventArgs^  e) {
		FontDialogButtonClick(btnCalendarFont);
	}

	private: System::Void btnAllBackgrounds_Click(System::Object^  sender, System::EventArgs^  e) {
		ColorDialogButtonClick(btnAllBackgrounds);
		btnCalDayBack->BackColor = btnAllBackgrounds->BackColor;
		btnCalDaysOtherBack->BackColor = btnAllBackgrounds->BackColor;
		btnCalTitleBack->BackColor = btnAllBackgrounds->BackColor;
		btnCalTodayBack->BackColor = btnAllBackgrounds->BackColor;
		btnDayNamesBack->BackColor = btnAllBackgrounds->BackColor;
	}

	private: System::Void cbQuickSimpleMetadata_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
//		if (System::Windows::Forms::MessageBox::Show ("Overwrite changes?", "This will override any changes to the simple metadata above\n\nDo you want to continue?", MessageBoxButtons::YesNo, MessageBoxIcon::Question) == ::DialogResult::Yes) {
	  if (cbQuickSimpleMetadata->SelectedIndex > 0) tbSimpleMetadata->Text = cbQuickSimpleMetadata->Text->Substring(cbQuickSimpleMetadata->Text->IndexOf(':')+1)->Replace("\\n", Environment::NewLine);
	//	}
  }
	private: System::Void btnCopyMonInfo_Click(System::Object^  sender, System::EventArgs^  e) {
	  array<Screen^>^ screens;
//		Screen^ screen;
		screens = Screen::AllScreens;
		tbInfo->Visible = true;
		tbInfo->Text = "Found " + screens->Length + " screens";	
		tbInfo->Text = tbInfo->Text + Environment::NewLine;
		for(int i = 0; i < screens->Length; i++) {
			tbInfo->Text = String::Concat(tbInfo->Text, screens[i]->DeviceName);	
			tbInfo->Text = tbInfo->Text + Environment::NewLine;
			if (screens[i]->Primary) {
				tbInfo->Text = tbInfo->Text + "Primary Screen";	
				tbInfo->Text = tbInfo->Text + Environment::NewLine;
			}
			tbInfo->Text = tbInfo->Text + "Bounds: " + screens[i]->Bounds.ToString();	
			tbInfo->Text = tbInfo->Text + Environment::NewLine;
			tbInfo->Text = tbInfo->Text + "WorkingArea: " + screens[i]->WorkingArea.ToString();	
			tbInfo->Text = tbInfo->Text + Environment::NewLine;
			tbInfo->Text = tbInfo->Text + "BitsPerPixel : " + screens[i]->BitsPerPixel.ToString();	
			tbInfo->Text = tbInfo->Text + Environment::NewLine;
			tbInfo->Text = tbInfo->Text + Environment::NewLine;
		}
		Clipboard::SetText(tbInfo->Text);
		tbInfo->Visible = false;
	}
	private: System::Void cbRawCacheInFolder_CheckStateChanged(System::Object^  sender, System::EventArgs^  e) {
		tbRawCacheFolder->Enabled = !cbRawCacheInFolder->Checked;
		btnRawCacheFolder->Enabled = !cbRawCacheInFolder->Checked;
		if (cbRawCacheInFolder->Checked) {
			lSeparateFolder->ForeColor = System::Drawing::SystemColors::ControlDark;
		} else {
			lSeparateFolder->ForeColor = System::Drawing::SystemColors::ControlText;
		}
	}
	private: System::Void cbDeleteKey_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
		cbConfirmDelete->Enabled = !cbDeleteKey->Checked;
	}
};
}
