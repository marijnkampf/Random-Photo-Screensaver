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

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace nsRandomPhotoScreensaver {

	/// <summary>
	/// Summary for fMetadata
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class fMetadata : public System::Windows::Forms::Form
	{
	private: 
 	  int id;
		TMetaTemplate^ metaTemplate;
	  int monitor;
	private: System::Windows::Forms::Panel^  pSettings;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::ComboBox^  cbStylesheet;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::ComboBox^  cbTemplate;
	private: System::Windows::Forms::Label^  label1;

	private: System::Windows::Forms::ComboBox^  cbMonitor;
	private: System::Windows::Forms::Button^  btnUpdate;
	public: System::Windows::Forms::WebBrowser^  wbMetadata;

	public:
		fMetadata(int id, String^ mdTemplate, String^ mdStylesheet, int monitor) {
			this->id = id;
			this->monitor = monitor;
			InitializeComponent();
			//this->cbStylesheet->Items->AddRange(Directory::GetFiles(config->appDataFolder + METADATASUBFOLDER, "*.css"));
			//this->cbTemplate->Items->AddRange(Directory::GetFiles(config->appDataFolder + METADATASUBFOLDER, "*.htm?"));
			this->cbStylesheet->Items->AddRange(this->GetFilesOnly(config->appDataFolder + METADATASUBFOLDER, "*.css"));
			this->cbTemplate->Items->AddRange(this->GetFilesOnly(config->appDataFolder + METADATASUBFOLDER, "*.htm?"));
			String^ monitors;
			for(int i = 0; i < multiMonitors.size(); i++) monitors = monitors + "Monitor " + (i+1) + ";";
			monitors = monitors->Trim(';');
			this->cbMonitor->Items->AddRange(monitors->Split(';'));
			if (cbMonitor->Items->Count > monitor) cbMonitor->SelectedIndex = monitor;
			else if (cbMonitor->Items->Count > 0) cbMonitor->SelectedIndex = 0;
			cbStylesheet->Text = mdStylesheet;
			cbTemplate->Text = mdTemplate;
			readTemplate();
		}

		array<String^>^ GetFilesOnly(String^ folder, String^ searchPattern) {
			if (Directory::Exists(folder)) {
				DirectoryInfo^ di = gcnew DirectoryInfo(folder);

				array<FileInfo^>^ files = di->GetFiles(searchPattern);
				array<String^>^ ret = gcnew array<String^>(files->Length);
				for(int i =0; i < files->Length; i++) {
					ret[i] = files[i]->Name;
				}
				return ret;
			} else {
				array<String^>^ ret = gcnew array<String^>(0);
				return ret;
			}
		}

		void viewSettings(bool visibility) {
			this->pSettings->Location = Point(0, 0);
			this->pSettings->Visible = visibility;
		}

		void readTemplate() {
			this->metaTemplate = gcnew TMetaTemplate(config->appDataFolder + METADATASUBFOLDER + "\\" + this->cbTemplate->Text, config->appDataFolder + METADATASUBFOLDER + "\\" + this->cbStylesheet->Text);
		}

		void showMetadata(TMetadata^ metadata) {
			this->wbMetadata->DocumentText = this->metaTemplate->processMetadata(metadata);			
		}

		int getMonitorNr() {
			return this->monitor;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~fMetadata() {
			if (components) {
				delete components;
			}
		}
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void) {
			this->wbMetadata = (gcnew System::Windows::Forms::WebBrowser());
			this->pSettings = (gcnew System::Windows::Forms::Panel());
			this->btnUpdate = (gcnew System::Windows::Forms::Button());
			this->cbMonitor = (gcnew System::Windows::Forms::ComboBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->cbStylesheet = (gcnew System::Windows::Forms::ComboBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->cbTemplate = (gcnew System::Windows::Forms::ComboBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->pSettings->SuspendLayout();
			this->SuspendLayout();
			// 
			// wbMetadata
			// 
			this->wbMetadata->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->wbMetadata->Location = System::Drawing::Point(0, 0);
			this->wbMetadata->MinimumSize = System::Drawing::Size(20, 20);
			this->wbMetadata->Name = L"wbMetadata";
			this->wbMetadata->ScrollBarsEnabled = false;
			this->wbMetadata->Size = System::Drawing::Size(377, 332);
			this->wbMetadata->TabIndex = 0;
			// 
			// pSettings
			// 
			this->pSettings->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->pSettings->Controls->Add(this->btnUpdate);
			this->pSettings->Controls->Add(this->cbMonitor);
			this->pSettings->Controls->Add(this->label3);
			this->pSettings->Controls->Add(this->cbStylesheet);
			this->pSettings->Controls->Add(this->label2);
			this->pSettings->Controls->Add(this->cbTemplate);
			this->pSettings->Controls->Add(this->label1);
			this->pSettings->Location = System::Drawing::Point(0, 0);
			this->pSettings->Name = L"pSettings";
			this->pSettings->Size = System::Drawing::Size(377, 153);
			this->pSettings->TabIndex = 3;
			this->pSettings->Visible = false;
			// 
			// btnUpdate
			// 
			this->btnUpdate->Location = System::Drawing::Point(3, 125);
			this->btnUpdate->Name = L"btnUpdate";
			this->btnUpdate->Size = System::Drawing::Size(117, 23);
			this->btnUpdate->TabIndex = 20;
			this->btnUpdate->Text = L"Apply to next photo";
			this->btnUpdate->UseVisualStyleBackColor = true;
			this->btnUpdate->Click += gcnew System::EventHandler(this, &fMetadata::btnUpdate_Click_2);
			// 
			// cbMonitor
			// 
			this->cbMonitor->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->cbMonitor->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cbMonitor->FormattingEnabled = true;
			this->cbMonitor->Location = System::Drawing::Point(3, 98);
			this->cbMonitor->Name = L"cbMonitor";
			this->cbMonitor->Size = System::Drawing::Size(371, 21);
			this->cbMonitor->TabIndex = 19;
			this->cbMonitor->SelectedIndexChanged += gcnew System::EventHandler(this, &fMetadata::cbMonitor_SelectedIndexChanged);
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(3, 82);
			this->label3->Margin = System::Windows::Forms::Padding(3, 5, 3, 0);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(59, 13);
			this->label3->TabIndex = 18;
			this->label3->Text = L"Monitor:";
			// 
			// cbStylesheet
			// 
			this->cbStylesheet->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->cbStylesheet->FormattingEnabled = true;
			this->cbStylesheet->Location = System::Drawing::Point(3, 59);
			this->cbStylesheet->Name = L"cbStylesheet";
			this->cbStylesheet->Size = System::Drawing::Size(371, 21);
			this->cbStylesheet->TabIndex = 17;
			this->cbStylesheet->SelectedIndexChanged += gcnew System::EventHandler(this, &fMetadata::cbStylesheet_SelectedIndexChanged);
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(3, 42);
			this->label2->Margin = System::Windows::Forms::Padding(3, 5, 3, 0);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(59, 13);
			this->label2->TabIndex = 16;
			this->label2->Text = L"Stylesheet:";
			// 
			// cbTemplate
			// 
			this->cbTemplate->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->cbTemplate->FormattingEnabled = true;
			this->cbTemplate->Location = System::Drawing::Point(3, 20);
			this->cbTemplate->Name = L"cbTemplate";
			this->cbTemplate->Size = System::Drawing::Size(371, 21);
			this->cbTemplate->TabIndex = 15;
			this->cbTemplate->SelectedIndexChanged += gcnew System::EventHandler(this, &fMetadata::cbTemplate_SelectedIndexChanged);
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(3, 4);
			this->label1->Margin = System::Windows::Forms::Padding(3, 5, 3, 0);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(59, 13);
			this->label1->TabIndex = 13;
			this->label1->Text = L"Template:";
			// 
			// fMetadata
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(377, 331);
			this->Controls->Add(this->pSettings);
			this->Controls->Add(this->wbMetadata);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->KeyPreview = true;
			this->Name = L"fMetadata";
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->Text = L"fMetadata";
			this->Activated += gcnew System::EventHandler(this, &fMetadata::fMetadata_Activated);
			this->VisibleChanged += gcnew System::EventHandler(this, &fMetadata::fMetadata_VisibleChanged);
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &fMetadata::fMetadata_FormClosing);
			this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &fMetadata::fMetadata_KeyDown);
			this->pSettings->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void btnUpdate_Click(System::Object^  sender, System::EventArgs^  e) {
	}
	private: System::Void fMetadata_Activated(System::Object^  sender, System::EventArgs^  e) {
 	  if (this->FormBorderStyle == ::FormBorderStyle::None) this->Owner->Activate();
	}
	private: System::Void fMetadata_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
		if (config->nupMetadata->Value > id) {
			config->metadataForms[this->id] = this->Bounds;
			this->FormBorderStyle = ::FormBorderStyle::None;
			viewSettings(false);
			if (e->CloseReason == ::CloseReason::UserClosing) {
				this->Owner->Activate();
				e->Cancel = true;
			}
		}
	}
	private: System::Void fMetadata_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
	  this->Owner->Focus();
		e->Handled = false;
	}
	private: System::Void fMetadata_VisibleChanged(System::Object^  sender, System::EventArgs^  e) {
		if (this->Visible) {
		  this->Bounds = config->metadataForms[this->id];
		} else {
			config->metadataForms[this->id] = this->Bounds;
		}
	}
	private: System::Void flowLayoutPanel1_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {
	}
	private: System::Void cbMonitor_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
	  this->monitor = cbMonitor->SelectedIndex;
		config->mdMonitor[id] = this->monitor;
		config->changed = true;
	}
	private: System::Void cbStylesheet_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
	  config->mdStylesheet[id] = cbStylesheet->Text;
		config->changed = true;
	}
	private: System::Void cbTemplate_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
		config->mdTemplate[id] = cbTemplate->Text;
		config->changed = true;
	}
	private: System::Void btnUpdate_Click_1(System::Object^  sender, System::EventArgs^  e) {
	}
	private: System::Void btnUpdate_Click_2(System::Object^  sender, System::EventArgs^  e) {
		this->readTemplate();
		config->changed = true;
	}
};
}
