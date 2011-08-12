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

#include "image.hpp"
#include "exif.hpp"
#include "string"

using namespace System;
using namespace System::Data;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::IO;
using namespace System::Text::RegularExpressions;
using namespace System::Diagnostics;


namespace nsRandomPhotoScreensaver {

	struct Exiv2Image {
		Exiv2::Image::AutoPtr image;
	};

	public ref class TMetadataItem {	
		public: String^ key;
		public: String^ value;
		public: String^ plainValue;
		public: TMetadataItem() {};
		public: TMetadataItem(String^ key, String ^value, String^ plainValue) {
			this->key = key;
			this->value = value;
			this->plainValue = plainValue;
		}
	};

	public ref class TMetadata {
	private:
		bool success;
		String^ filename;
		Exiv2Image *exiv2Image;
		Regex^ reKeyFilter;

		void MarshalString ( String ^ s, std::string& os ) {
			 using namespace Runtime::InteropServices;
			 const char* chars = 
					(const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
			 os = chars;
			 Marshal::FreeHGlobal(IntPtr((void*)chars));
		}

		void MarshalString ( String ^ s, std::wstring& os ) {
			 using namespace Runtime::InteropServices;
			 const wchar_t* chars = 
					(const wchar_t*)(Marshal::StringToHGlobalUni(s)).ToPointer();
			 os = chars;
			 Marshal::FreeHGlobal(IntPtr((void*)chars));
		}

	public:
		TMetadata(String^ _filename) : filename(_filename) {
			this->success = false;
			if (filename->Length > 0) {
				std::string fn;
				MarshalString(filename, fn);
				exiv2Image = new Exiv2Image;
				try {
					exiv2Image->image = Exiv2::ImageFactory::open(fn);
					exiv2Image->image->readMetadata();
					this->success = true;
				} catch(const Exiv2::AnyError&) {
					delete exiv2Image;
				}
			}
		}

	protected:
		~TMetadata() {
			if (this->success) delete exiv2Image;
		}

	public:
		bool match(String^ key, String^ filter) {
			return match(key, filter, false);
		}
		bool match(String^ key, String^ filter, bool regexp) {
			if (regexp) {
				return this->reKeyFilter->IsMatch(key);
			} else return ((key != String::Empty) && (String::Compare(key, filter, StringComparison::InvariantCultureIgnoreCase) == 0));
		}
		ArrayList^ find(String^ keyFilter) {
			bool regexp;
			ArrayList^ match = gcnew ArrayList;
			if ((keyFilter->Length > 0) && (keyFilter[0] == '[') && (keyFilter[keyFilter->Length-1] == ']')) {
				keyFilter = keyFilter->Substring(1, keyFilter->Length-2);
				regexp = true;			
				try {
					this->reKeyFilter = gcnew Regex(keyFilter, RegexOptions::IgnoreCase|RegexOptions::Compiled);
				} catch (Exception^ ex) {
					if (config->cbDebug->Checked) {
						MessageBox::Show(ex->Message + Environment::NewLine +
							keyFilter, 
							"Debug info: Invallid Regular Expression", MessageBoxButtons::OK,
							 MessageBoxIcon::Warning);
					}
					regexp = false;	
				}
			} else regexp = false;
			try {
				if (this->success) {
					Exiv2::ExifData &exifData = exiv2Image->image->exifData();
					Exiv2::ExifData::const_iterator exivEnd = exifData.end();
					for (Exiv2::ExifData::const_iterator i = exifData.begin(); i != exivEnd; ++i) {
							String^ key = gcnew String(i->key().c_str());
							String^ value = gcnew String(i->print().c_str());
							if (this->match(key, keyFilter, regexp)) {	
								match->Add(gcnew TMetadataItem(key, value, gcnew String(i->toString().c_str())));
							}
					}

					Exiv2::IptcData &iptcData = exiv2Image->image->iptcData();
					Exiv2::IptcData::const_iterator iptcEnd = iptcData.end();
					for (Exiv2::IptcData::const_iterator i = iptcData.begin(); i != iptcEnd; ++i) {
						String^ key = gcnew String(i->key().c_str());
						String^ value = gcnew String(i->print().c_str());
						if (this->match(key, keyFilter, regexp)) {	
							match->Add(gcnew TMetadataItem(key, value, gcnew String(i->toString().c_str())));
						}
					}

					Exiv2::XmpData &xmpData = exiv2Image->image->xmpData();
					Exiv2::XmpData::const_iterator xmpEnd = xmpData.end();
					for (Exiv2::XmpData::const_iterator i = xmpData.begin(); i != xmpEnd; ++i) {
						String^ key = gcnew String(i->key().c_str());
						String^ value = gcnew String(i->print().c_str());
						if (this->match(key, keyFilter, regexp)) {	
							match->Add(gcnew TMetadataItem(key, value, gcnew String(i->toString().c_str())));
						}
					}
				}
			} catch(Exception^ ex) {

			}
			return match;
		}

		String^ readMetadata() {
			String^ metadata;
			if (this->success) {
				Exiv2::ExifData &exifData = exiv2Image->image->exifData();
				Exiv2::ExifData::const_iterator end = exifData.end();
				for (Exiv2::ExifData::const_iterator i = exifData.begin(); i != end; ++i) {
					// Add results to textbox
					metadata = metadata + gcnew String(i->key().c_str())
							+ "\t" + gcnew String(i->print().c_str())
							+ Environment::NewLine;
				}
				return metadata;
			}
			return "";
		}
	};

	public ref class TMetaTemplate {
	private:
		String^ metaTemplate;
		Regex^ rxGroups;
		Regex^ rxTags;

	public:
		void init() {
			rxGroups = gcnew Regex("<#[^#]*#>", static_cast<RegexOptions>(RegexOptions::Singleline));
			rxTags = gcnew Regex("{[^}]*}", static_cast<RegexOptions>(RegexOptions::Singleline));
		}

		TMetaTemplate(String^ metaTemplate) {
			this->metaTemplate = metaTemplate;
			this->init();
		}

		TMetaTemplate(String^ templateFile, String^ styleFile) {
			if (File::Exists(templateFile)) {
				this->metaTemplate = File::ReadAllText(templateFile);
				if (styleFile != String::Empty) {
					this->metaTemplate = this->metaTemplate->Replace("<head>", "<head>" + Environment::NewLine + "<link type='text/css' rel='stylesheet' href='" + styleFile + "'>" + Environment::NewLine);
				}
			} else {
				MessageBox::Show("Can't find: " + Environment::NewLine + "'" + templateFile + "'",
					 "File not found", MessageBoxButtons::OK,
					 MessageBoxIcon::Warning);
				this->metaTemplate = String::Empty;
			}
			this->init();
		}

		property String^ loadTemplate {
			void set(String^ templateFilename) { 
				this->metaTemplate = File::ReadAllText(templateFilename);
			}
		}

		String^ processMetadata(TMetadata^ metadata) {
			String^ output = "";
			MatchCollection^ matchesGroups = this->rxGroups->Matches(this->metaTemplate);
			// Report on each match.
			int pos = 0;
			
			for each (Match^ groupMatch in matchesGroups) {
				String^ tagCode = "";
				String^ MetaKey = "";
				output += metaTemplate->Substring(pos, groupMatch->Index - pos);
				MatchCollection^ matchesTags = this->rxTags->Matches(groupMatch->Value);	
				// Start at position 2 to strip <#
				int tagPos = 2;
				bool prettyName = false;
				for each (Match^ tagMatch in matchesTags) {
					tagCode += groupMatch->Value->Substring(tagPos, tagMatch->Index - tagPos);
					if ((tagMatch->Value->Length > 2) && (tagMatch->Value[1] == '@')) {
						switch(tagMatch->Value->ToLower()[2]) {
							case 'p': // pretty name
								prettyName = true;
							break;
							case 'n': // name
							break;
							default:
								MessageBox::Show("Unknown metadata attribute '" + tagMatch->Value + "'",
									 "RPS Metadata", MessageBoxButtons::OKCancel,
									 MessageBoxIcon::Warning);
							break;
						}
						tagCode += "{@"+tagMatch->Value->ToLower()[2]+"}";
					} else {
						// SELECTED EXIF/IPTC KEY
						MetaKey = tagMatch->Value->ToLower()->Trim();
						tagCode += MetaKey;
					}
					tagPos = tagMatch->Index + tagMatch->Length;
				}
				tagCode += groupMatch->Value->Substring(tagPos, groupMatch->Value->Length-tagPos-2);
				ArrayList^ matches = metadata->find(MetaKey->Trim(String("{}").ToCharArray()));
				String^ tags = "";
				for(int i = 0; i < matches->Count; i++) {
					String^ temp = tagCode;
					temp = temp->Replace("{@n}", safe_cast<TMetadataItem^>(matches[i])->key)->Replace(MetaKey, safe_cast<TMetadataItem^>(matches[i])->value);
					if (prettyName) {
						temp = temp->Replace("{@p}", this->prettyName(safe_cast<TMetadataItem^>(matches[i])->key));
					}
					tags += temp;
				}
				String^ t = groupMatch->Value->Substring(tagPos, groupMatch->Value->Length - tagPos);
				output += tags;// + t;
				pos = groupMatch->Index + groupMatch->Length;
			}
			output += metaTemplate->Substring(pos);
			return output;
		}

		String^ prettyName(String^ s) {
			return Regex::Replace(s->Substring(s->LastIndexOf(".")+1),"([A-Z])", " $1");
		}
	};
};
