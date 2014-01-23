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
#include "fConfig.h"
#include "Engine.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Globalization;


namespace nsRandomPhotoScreensaver {

	/// <summary>
	/// Summary for fCalendar
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class fCalendar : public System::Windows::Forms::Form {
		private: System::Windows::Forms::Panel^  pCalendar;
		private: int id;
		private: int month;
		private: array<System::Windows::Forms::Label^> ^days;
		private: System::Windows::Forms::Label^  lMonthAndYear;
		private: System::Windows::Forms::Panel^  panel;
		private: int nrRows, nrCols;
		private: bool resizing;
						 fConfig^ config;

	public:
		fCalendar(int id, int month) {
			config = gConfig;
			this->id = id;
			this->month = month;
			days = gcnew array<System::Windows::Forms::Label^>(50);
			this->Bounds = config->calendarForms[this->id];

			InitializeComponent();
			createCalendar(pCalendar, month);
			this->resizing = false;
		  this->Bounds = config->calendarForms[this->id];

			//this->Bounds = config->calendarForms[this->id];
			//this->Enabled = false;
			
			//
			//TODO: Add the constructor code here
			//
		}
		// Returns number of day 0 (Sun) ... 6 (Sat)
		public: int DayOfWeekToInt(DayOfWeek dow) {
			int i = 0;
			switch(dow) {
				case DayOfWeek::Sunday:   i = 0; break;
				case DayOfWeek::Monday:	  i = 1; break;
				case DayOfWeek::Tuesday:  i = 2; break;
				case DayOfWeek::Wednesday:i = 3; break;
				case DayOfWeek::Thursday: i = 4; break;
				case DayOfWeek::Friday:   i = 5; break;
				case DayOfWeek::Saturday: i = 6; break;			
			}
			return i;
		}

		// Returns number of day of week based on given first day of week
		public: int DayOfWeekToInt(DayOfWeek dow, DayOfWeek firstDOW) {
			int day = DayOfWeekToInt(dow);
			int first = DayOfWeekToInt(firstDOW);
			return abs((day - first) % 7);
		}

		Drawing::Font^ setFontSize(System::Drawing::Font^ %font, int size) {
			if (size <= 0) size = 1;
			if (size != font->Size) font = gcnew System::Drawing::Font(font->FontFamily, size, font->Style, font->Unit, font->GdiCharSet, font->GdiVerticalFont);
			return font;
		}

		void shrinkToFit(System::Windows::Forms::Control^ item) {
			System::Drawing::Size textSize(item->Width+1, item->Height+1);
			item->Font = setFontSize(item->Font, item->Height);
			while(((textSize.Width > item->Width) || (textSize.Height > item->Height)) && (item->Font->Size > 1)) {
				item->Font = setFontSize(item->Font, item->Font->Size-1);
				textSize = TextRenderer::MeasureText(item->Text, item->Font);
			}
		}

		public: void resizeCalendar() {

			int smallestFont = panel->Height;
			int rowHeight = panel->Height / (this->nrRows+1);
			int colWidth = panel->Width / this->nrCols;
			this->lMonthAndYear->Location = System::Drawing::Point(0, 0);
			this->lMonthAndYear->Size = System::Drawing::Size(panel->Width, rowHeight);
			shrinkToFit(this->lMonthAndYear);
			//this->lMonthAndYear->Font = setFontSize(this->lMonthAndYear->Font, this->lMonthAndYear->Height);

			// Resize days of week
			for(int i = 0; i < 7; i++) {
				this->days[i]->Location = System::Drawing::Point(colWidth * int(i % this->nrCols), rowHeight+ rowHeight * int(i / this->nrCols));
				this->days[i]->Size = System::Drawing::Size(colWidth, rowHeight);
				shrinkToFit(this->days[i]);
				if (smallestFont > this->days[i]->Font->Size) {
					smallestFont = this->days[i]->Font->Size;
				}
			}
			for(int j = 0; j < 7; j++) {
				if (this->days[j]->Font->Size != smallestFont) 
					this->days[j]->Font = setFontSize(this->days[j]->Font, smallestFont);
			}
			for(int i = 7; i < this->nrRows*7; i++) {
				this->days[i]->Location = System::Drawing::Point(colWidth * int(i % this->nrCols), rowHeight+ rowHeight * int(i / this->nrCols));
				this->days[i]->Size = System::Drawing::Size(colWidth, rowHeight);
				if (i == 7) {
					String^ temp = this->days[i]->Text;
					this->days[i]->Text = "30";
					shrinkToFit(this->days[i]);
					this->days[i]->Text=temp;
				}
				this->days[i]->Font = setFontSize(this->days[i]->Font, this->days[7]->Font->Size);
			}
		}

//		 = CultureInfo::CurrentCulture->DateTimeFormat->FirstDayOfWeek;

		public: void createCalendar() {
			createCalendar(pCalendar, this->month);
		}

		public: void createCalendar(System::Windows::Forms::Panel^ panel) {
			createCalendar(panel, 0);
		}

		public: void createCalendar(System::Windows::Forms::Panel^ panel, int monthDif) {
			Calendar^ calendar = CultureInfo::CurrentCulture->Calendar;

			CalendarWeekRule myCWR = CultureInfo::CurrentCulture->DateTimeFormat->CalendarWeekRule;
			DateTimeFormatInfo^ myDTFI = CultureInfo::CurrentCulture->DateTimeFormat;
			array<String^>^ dayNames;
			dayNames = myDTFI->AbbreviatedDayNames;

			int year=DateTime::Today.Year;
			int month=DateTime::Today.Month;
			if ((month+monthDif) > 12) {
				year = year+int((double(month+monthDif) / 12));
				month= (month+monthDif) % 12;
			} else {
				month = (month+monthDif) % 12;
				year = DateTime::Today.Year;
			}

			DateTime dtFirstOfMonth = DateTime(year, month, 1);
			DateTime dtStart = dtFirstOfMonth.AddDays(-1* DayOfWeekToInt(dtFirstOfMonth.DayOfWeek, CultureInfo::CurrentCulture->DateTimeFormat->FirstDayOfWeek));

			int DaysOnCalendar = DateTime::DaysInMonth(DateTime::Today.Year, DateTime::Today.Month);
			if	(dtStart.Month != dtFirstOfMonth.Month) DaysOnCalendar += DateTime::DaysInMonth(dtStart.Year, dtStart.Month) - dtStart.Day;

			this->nrRows = int(System::Math::Ceiling(double(DaysOnCalendar) / 7))+1;
			this->nrCols = 7;
			this->panel = panel;

			int rowHeight = panel->Height / (this->nrRows+1);
			int colWidth = panel->Width / this->nrCols;

			
			this->lMonthAndYear = (gcnew System::Windows::Forms::Label());
			this->lMonthAndYear->AutoSize = false;

			this->lMonthAndYear->Font = config->btnCalendarFont->Font;
			this->lMonthAndYear->ForeColor = config->btnCalTitleFront->BackColor;
			this->lMonthAndYear->BackColor = config->btnCalTitleBack->BackColor;
			this->lMonthAndYear->Text = dtFirstOfMonth.ToString("MMMM yy");
			this->lMonthAndYear->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->panel->Controls->Add(this->lMonthAndYear);

			this->Text = dtFirstOfMonth.ToString("MMMM yy") + " Calendar RPS";

			for(int i = 0; i < this->nrRows*7; i++) {
				this->days[i] = (gcnew System::Windows::Forms::Label());
				this->days[i]->AutoSize = false;
				this->days[i]->Font = config->btnCalendarFont->Font;
				if (dtStart.Month != dtFirstOfMonth.Month) {
					this->days[i]->ForeColor = config->btnCalDaysOtherFront->BackColor;
					this->days[i]->BackColor = config->btnCalDaysOtherBack->BackColor;
				} else {
					if (DateTime::Compare(dtStart, DateTime::Today) == 0) {
						this->days[i]->ForeColor = config->btnCalTodayFront->BackColor;
						this->days[i]->BackColor = config->btnCalTodayBack->BackColor;
					} else {
						this->days[i]->ForeColor = config->btnCalDayFront->BackColor;
						this->days[i]->BackColor = config->btnCalDayBack->BackColor;
					}
				}
				//this->days[i]->Name = L"days[i]";
				if (i < 7) {
					this->days[i]->ForeColor = config->btnDayNamesFront->BackColor;
					this->days[i]->BackColor = config->btnDayNamesBack->BackColor;
					//switch(i) {
						//this->days[i]->Text = dayNames[i];
					this->days[i]->Text = dayNames[DayOfWeekToInt(dtStart.AddDays(i).DayOfWeek)];
					//} 
				} else {
						this->days[i]->Text = int(dtStart.Day).ToString();
						dtStart = dtStart.AddDays(1);
				}
				this->days[i]->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
				this->panel->Controls->Add(this->days[i]);

			}
			resizeCalendar();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~fCalendar()
		{
			if (components)
			{
				delete components;
			}
		}
	protected: 

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
			this->pCalendar = (gcnew System::Windows::Forms::Panel());
			this->SuspendLayout();
			// 
			// pCalendar
			// 
			this->pCalendar->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->pCalendar->Location = System::Drawing::Point(0, 0);
			this->pCalendar->Name = L"pCalendar";
			this->pCalendar->Size = System::Drawing::Size(485, 414);
			this->pCalendar->TabIndex = 2;
			// 
			// fCalendar
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::White;
			this->ClientSize = System::Drawing::Size(484, 413);
			this->Controls->Add(this->pCalendar);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"fCalendar";
			this->Opacity = 0.5;
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->Text = L"Calendar Random Photo Screensaver";
			this->ResizeBegin += gcnew System::EventHandler(this, &fCalendar::pCalendar_ResizeBegin);
			this->Activated += gcnew System::EventHandler(this, &fCalendar::pCalendar_Activated);
			this->VisibleChanged += gcnew System::EventHandler(this, &fCalendar::fCalendar_VisibleChanged);
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &fCalendar::pCalendar_FormClosing);
			this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &fCalendar::fCalendar_KeyDown);
			this->ResizeEnd += gcnew System::EventHandler(this, &fCalendar::pCalendar_ResizeEnd);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void pCalendar_Activated(System::Object^  sender, System::EventArgs^  e) {
	  if (this->FormBorderStyle == ::FormBorderStyle::None) this->Owner->Activate();
	}
	private: System::Void pCalendar_ResizeBegin(System::Object^  sender, System::EventArgs^  e) {
	  this->resizing = true;
	}
	private: System::Void pCalendar_ResizeEnd(System::Object^  sender, System::EventArgs^  e) {
		resizeCalendar();
		this->resizing = false;	  
	}
	private: System::Void pCalendar_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
	  if (config->getNumberCalendars() > id) 
			config->calendarForms[this->id] = this->Bounds;
		this->FormBorderStyle = ::FormBorderStyle::None;
	  if (e->CloseReason == ::CloseReason::UserClosing) {
			this->Owner->Activate();
			e->Cancel = true;
	  }
	}

	private: System::Void fCalendar_VisibleChanged(System::Object^  sender, System::EventArgs^  e) {
		if (this->Visible) {
		  this->Bounds = config->calendarForms[this->id];
			resizeCalendar();
		}
		//else config->calendarForms[this->id] = this->Bounds;
	}
	private: System::Void fCalendar_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
	  //(Engine^)this->Owner->showMessageAll("Test");
	  this->Owner->Focus();
		e->Handled = false;

	}
};
}
