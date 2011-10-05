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
//#include "TAnimator.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Globalization;
using namespace System::Text::RegularExpressions;


// Add Interop.IWshRuntimeLibrary.dll to Resource Files in Solution Explorer
//#using <Interop.IWshRuntimeLibrary.dll>
//namespace Win32 { #include <shlobj.h> };


#define reEXT "bmp|gif|jpeg|jpg|png"

#define HISTORY_SIZE 1000
#define NO_IMAGE -1
#define NOT_FOUND -1
namespace nsRandomPhotoScreensaver {
enum TDirection {dCurrent, dNext, dPrev};
enum TImageType { itNormal, itWallpaper };


public ref class TConductor: public System::ComponentModel::Component {

	private:
		int nrMonitors;
		ArrayList^ images;
		ArrayList^ wallpaperImages;
		//ArrayList^ sortOrder;
		//array<String^> ^images;
		//array<int> ^sortOrder;

		Regex^ rxExt;
		Regex^ rxDecodableExt;
		Regex^ rxExcludeFiles;
		bool bExcludeFilesRegEx;
		int fileCount, dirCount;

		array<int> ^sequentialId;
		array<int,2> ^history;
		array<int> ^historyPointer;
		array<int> ^historyOffset;
		array<int> ^initialImageFound;

		//array<System::IO::FileSystemWatcher^> ^fileSystemWatchers;
		ArrayList^ fileSystemWatchers;

		private: FileSystemWatcher^ watcher;

		//TAnimator^ animator;

	public:
		Bitmap^ panoramaBuffer;
		bool panorama;
		String^ rawExtensions;
		TConductor(int m) {
			nrMonitors = m;
			images = gcnew ArrayList;
			wallpaperImages = gcnew ArrayList;
			fileSystemWatchers = gcnew ArrayList;
			//sortOrder = gcnew ArrayList;

			rxDecodableExt = gcnew Regex(reEXT);

			String^ extensionFilters = "";
			if (config->cbEnableRaw->Checked) {
				// Add additional user defined (RAW) file extensions to filter
				// Remove everthing but A-Za-z0-9.|
				Regex^ rgx = gcnew Regex("[^A-Z0-9]",static_cast<RegexOptions>(RegexOptions::Compiled | RegexOptions::IgnoreCase));
				extensionFilters = rgx->Replace(config->tbRawExtensions->Text, "|");
				rgx = gcnew Regex("\\|+",static_cast<RegexOptions>(RegexOptions::Compiled));
				extensionFilters = rgx->Replace(extensionFilters, "|");
				// Remove trailing |
				extensionFilters = extensionFilters->Trim('|');
				if (extensionFilters->Length > 0) extensionFilters = reEXT + "|" + extensionFilters;
				else extensionFilters = reEXT;
			}

			rxExt = gcnew Regex(extensionFilters);
			bExcludeFilesRegEx = (config->tbExcludeRegEx->Text->Length > 0);
			if (bExcludeFilesRegEx) rxExcludeFiles = gcnew Regex(config->tbExcludeRegEx->Text);
			this->fileCount = 0;
			this->dirCount = 0;

			//*history = gcnew int[m, HISTORY_SIZE];
			history = gcnew array<int, 2>(m,HISTORY_SIZE);
			historyPointer = gcnew array<int>(m);
			historyOffset = gcnew array<int>(m);
			initialImageFound = gcnew array<int>(m);
			sequentialId = gcnew array<int>(m);
			for(int i = 0; i < nrMonitors; i++) {
				historyPointer[i] = 1;
				historyOffset[i] = 1;
				sequentialId[i] = NOT_FOUND;
				//initialImageFound[i] = NOT_FOUND;
				for(int j = 0; j < HISTORY_SIZE; j++) {
					history[i,j] = NO_IMAGE;
				}
			}

			// Create panorama buffer regardless of panorama setting. To avoid error on
			// non pano startup and enabling pano when running.
			// 2Do investigate check if panoBuffer initialised and only init when needed to save memory.
			//if (config->cbPanoramaStretch->Checked) 
			panoramaBuffer = gcnew Bitmap(config->desktop.Width, config->desktop.Height);
		}

		void setInitialImage() {
			if (config->lastFile[0] != "") {
				sequentialId[0] = this->getImageId(config->lastFile[0]);
				if (sequentialId[0] < 0) sequentialId[0] = 0;
				for(int i=1; i < nrMonitors; i++) {
					if (this->images->Count > 0) sequentialId[i] = (sequentialId[0] + i) % this->images->Count;
					else sequentialId[i] = (sequentialId[0] + i);
				}
			}
		}

		void removeImageFromList(String^ filename) {
			int id = getImageId(filename);
			if (id >= 0) this->images->RemoveAt(id);
			if (this->fileCount > 0) this->fileCount--;			
		}

		int getImageId(String^ filename) {
			return images->IndexOf(filename);
		}

		int getImageCount() {
			return fileCount;
		}
		
		int getWallpaperImageCount() {
			return wallpaperImages->Count;
		}

		int getDirCount() {
			return dirCount;
		}

		int getImgId(int m) {
			return getImgId(m, dCurrent, true, 0);
		}

		int getImgId(int m, TDirection direction, bool random, int stepSize) {
			if ((config->cbSameImage->Checked) && (m != 0)) {
			  return getImgId(0);
			}
			int directionStep=0;
			switch(direction) {
				case dNext: {
					directionStep = 1;
				} break;
				case dPrev: {
					directionStep = -1;
				} break;
			}
			if (!config->cbOrdered->Checked) {
				switch(direction) {
					case dNext: {
						if (random) historyPointer[m]++;
						else historyOffset[m]++;
					} break;
					case dPrev: {
						if (random) historyPointer[m]--;
						else historyOffset[m]--;
					} break;
				}
				historyPointer[m]%=HISTORY_SIZE;
				if (historyPointer[m] < 0) historyPointer[m] = HISTORY_SIZE-1;
				if (history[m, historyPointer[m]] == NO_IMAGE) {
					Random^ rnd = gcnew Random();
					history[m, historyPointer[m]] = rnd->Next(this->images->Count);

					int dirOffset = historyPointer[m]+directionStep;
					dirOffset %= HISTORY_SIZE;
					if (dirOffset < 0) dirOffset = HISTORY_SIZE - 1;
					history[m, dirOffset] = NO_IMAGE;
				}
				int r = 0;
				if (this->images->Count > 0) {
					r = (history[m, historyPointer[m]]+historyOffset[m])%this->images->Count;
					if (r < 0) r = this->images->Count + ((history[m, historyPointer[m]]+historyOffset[m])%this->images->Count-2);
				}
				return r;
			} else {
				if (sequentialId[m] == NOT_FOUND) {
					sequentialId[m] = (m % this->images->Count);
					return sequentialId[m];
				} else {
					if (abs(stepSize) == 1) {
						if (config->cbMonitorSequence->Checked) stepSize = 1;
						else stepSize = nrMonitors;
					}
					sequentialId[m] = (sequentialId[m] + directionStep*stepSize) % this->images->Count;
					if (sequentialId[m] < 0) sequentialId[m] = this->images->Count + sequentialId[m];
					return sequentialId[m];
				}
			}
		}

		void adjustNextImg(TDirection direction) {
			if (config->cbOrdered->Checked) {
/*				int adjustStep;
				if (direction == dPrev) adjustStep = step*-1;
				else adjustStep = step;*/
				if (direction == dNext) sequentialId[0] -= 1;
				//if (direction == dPrev) sequentialId[0] += 1;
					for(int i = 1; i < nrMonitors; i++) {
						sequentialId[i] = sequentialId[0] + i;
					}
				//}
			}			
		}

		String^ getImage(int m, TDirection direction, bool random, int step) {
			if (this->images->Count > 0) {
				if (config->cbOrdered->Checked && (sequentialId[m] == NOT_FOUND)) {
					if (config->lastFile[m]->Length > 0) return config->lastFile[m];
				} 
				int id = getImgId(m, direction, random, step);
				return safe_cast<String^>(this->images[id]);
			}
			return "";
		}

		bool cacheRawImage(String^ raw, String^ jpg) {
			if (!config->cbEnableRaw->Checked) return false;
			String^ size = "";
			if (config->rbRawCacheDimensionsMonitor->Checked) {
				size="--size=" + config->screenMaxDimension;
			}

			System::Diagnostics::Process^ p = gcnew System::Diagnostics::Process;
			p->StartInfo->UseShellExecute = false;
      p->StartInfo->WindowStyle = System::Diagnostics::ProcessWindowStyle::Hidden;
      p->StartInfo->CreateNoWindow = true;
			p->StartInfo->FileName = config->tbRawConverter->Text;
			p->StartInfo->Arguments = size + " " + config->tbRawParameters->Text->Replace("#RAW#", raw)->Replace("#JPG#", jpg);
			if (!File::Exists(p->StartInfo->FileName)) {
				config->cbEnableRaw->Checked = false;
				System::Windows::Forms::MessageBox::Show ("RAW Converter not found:" + Environment::NewLine + p->StartInfo->FileName + Environment::NewLine + "Further RAW conversion disabled for this screensaver run", "RAW conversions temporarily disabled", MessageBoxButtons::OK, MessageBoxIcon::Exclamation);
				return false;
			} else {
				p->Start();
				p->WaitForExit();
				return File::Exists(jpg);
			}
			return true;
		}

		// Returns standard (jpg/bmp/etc) decodable image for (RAW) filename
		String^ checkImageCache(String^ filename) {
			String^ ext;
			if (rxDecodableExt->IsMatch(ext = Path::GetExtension(filename)->ToLower())) {
				return filename;
			} else {
				String^ newFilename;
				if (config->cbRawCacheInFolder->Checked) {
					newFilename = Path::ChangeExtension(filename, ".jpg");
				} else {
					newFilename = Path::ChangeExtension(config->addTrailingSlash(config->tbRawCacheFolder->Text) + filename->Replace(":", ""), ".jpg");
					if (!Directory::Exists(Path::GetDirectoryName(newFilename))) {
						Directory::CreateDirectory(Path::GetDirectoryName(newFilename));
					}
				}

				if (File::Exists(newFilename)) {
					return newFilename;
				} else {
					if (this->cacheRawImage(filename, newFilename)) return newFilename;
				}
				//MessageBox::Show(ext, "Info", MessageBoxButtons::OK, MessageBoxIcon::Exclamation );
			}
			return "";
		}

		int getStepSize(System::Windows::Forms::KeyEventArgs^ e) {
			if (e->Shift) return 5;
			if (e->Control) return 25;
			if (e->Alt) return 100;
			return 1;
		}

		void sortFilenames() {
			this->images->Sort();
		}

		String^ getRandomWallpaper() {
			Random^ rnd = gcnew Random();
			if (config->cbWallpaperFolder->Checked) {
				if (this->wallpaperImages->Count > 0) {
					return this->checkImageCache(safe_cast<String^>(this->wallpaperImages[rnd->Next(this->wallpaperImages->Count)]));
				}
			} else {
				if (this->images->Count > 0) {
					return this->checkImageCache(safe_cast<String^>(this->images[rnd->Next(this->images->Count)]));
				}
			}
			return "";
		}

		String^ getRandomImage() {
			Random^ rnd = gcnew Random();
			return safe_cast<String^>(this->images[rnd->Next(this->images->Count)]);
		}

		public: System::Void OnCreated(System::IO::FileSystemEventArgs^ e) {
			if (Directory::Exists( e->FullPath ) ) {
				this->readImageFolder(e->FullPath, itNormal, true);
			} else {
				this->addImage(e->FullPath, itNormal, true);
			}
		}

		public: System::Void OnDeleted(System::IO::FileSystemEventArgs^ e) {
			// Try to determine whether it was a folder (fails for folders with . in)
			int i = this->images->IndexOf(e->FullPath);
			if (i >= 0) {
				this->images->RemoveAt(i);
				this->fileCount--;
			} else {
				// No exact result found; either file not indexed yet or folder (presumed)
				String^ deletedFolder = config->addTrailingSlash(e->FullPath);
			  IEnumerator^ image = this->images->GetEnumerator();
				while ( image->MoveNext() ) {
					String^ filename = safe_cast<String^>(image->Current);
					if (filename->StartsWith(deletedFolder)) {
						this->images->Remove(image);
						this->fileCount--;
					}
				}
				this->dirCount--;
			}
		}

		public: System::Void ClearImages() {
			this->images->Clear();
			this->wallpaperImages->Clear();
			this->dirCount = 0;
			this->fileCount = 0;
		}

		public: System::Void OnRenamed(System::IO::RenamedEventArgs^  e) {
			if (Directory::Exists( e->FullPath ) ) {
				String^ oldFolder = config->addTrailingSlash(e->OldFullPath);
				String^ newFolder = config->addTrailingSlash(e->FullPath);
				for(int i =0; i < this->images->Count; i++) {
					if (safe_cast<String^>(this->images[i])->StartsWith(oldFolder)) {
						this->images[i] = safe_cast<String^>(this->images[i])->Replace(oldFolder, newFolder);
					}					
				}
			} else {
				int i = this->images->IndexOf(e->OldFullPath);
				this->images[i] = e->FullPath;
			}
		}

		public: System::Void addImage(String^ filename, TImageType imageType) {
			this->addImage(filename, imageType, false);
		}

		// checkDuplicated is required with FileSystemWatcher as both files and its folders are added
		public: System::Void addImage(String^ filename, TImageType imageType, bool checkDuplicates) {
			if ((!config->cbHideHidden->Checked) || ((config->cbHideHidden->Checked) && (File::GetAttributes(filename) & FileAttributes::Hidden) != FileAttributes::Hidden)) {
				// Check extention
				if (rxExt->IsMatch(Path::GetExtension(filename)->ToLower())) {
					if ((!bExcludeFilesRegEx) || (!rxExcludeFiles->IsMatch(filename))) {
						if (imageType == itNormal) {
							if (!checkDuplicates || !this->images->Contains(filename)) {
								this->fileCount++;
								this->images->Add(filename);
							}
						} else {
							if (!checkDuplicates || !this->wallpaperImages->Contains(filename)) {
								this->wallpaperImages->Add(filename);
							}
						}
					}
				}
			}
		}

/*		public: String^ readShortcut(String^ filename) {
			IShellLink *pShellLink;
			::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_ALL,
												 IID_IShellLink, (void**)&pShellLink);
			pShellLink->SetPath(szPath);  // Path to the object we are referring to

			//IWshRuntimeLibrary::WshShell^ wshShell = gcnew IWshRuntimeLibrary::WshShell;
			//IWshRuntimeLibrary::IWshShortcut^ shortcut = (IWshRuntimeLibrary::IWshShortcut^)wshShell->CreateShortcut(filename);
			return shortcut->TargetPath;
		}*/

		public: int readImageFolder(String^ full_path, TImageType imageType) {
			return this->readImageFolder(full_path, imageType, false);
		}

		public: int readImageFolder(String^ full_path, TImageType imageType, bool checkDuplicates) {
			String^ ext;
			//unsigned int extpos;

			// Cut short amount of images read when in preview
			if ((config->action==saPreview) && (this->fileCount > 100)) return -1;

			if ( !Directory::Exists( full_path ) ) {
				System::Windows::Forms::MessageBox::Show ("\nPhoto Folder Not found: " + full_path);
				return 1;
			} else {
				if (imageType == itNormal) this->dirCount++;
				// Process files in this directory
				array<String^>^fileEntries = Directory::GetFiles( full_path );
				Array::Sort(fileEntries);
				IEnumerator^ files = fileEntries->GetEnumerator();
				while ( files->MoveNext() )	{
					String^ filename = safe_cast<String^>(files->Current);
					String^ ext = Path::GetExtension(filename)->ToLower();
/*					if (Path::GetExtension(filename)->ToLower() == ".lnk") {
						filename = this->readShortcut(filename);
					}*/
					// Check for hidden files
					this->addImage(filename, imageType, checkDuplicates);

					// Cut short amount of images read when in preview
					if ((config->action==saPreview) && (this->fileCount > 100)) return -1;
				}

				// Recurse into subdirectories of this directory.
				array<String^>^subdirectoryEntries = Directory::GetDirectories( full_path );
				Array::Sort(subdirectoryEntries);
				IEnumerator^ dirs = subdirectoryEntries->GetEnumerator();
				while (dirs->MoveNext()) {
					String^ subdirectory = safe_cast<String^>(dirs->Current);
					int pos = subdirectory->LastIndexOf("\\");
					String^ subdirname = subdirectory->Substring(pos+1, subdirectory->Length-pos-1);

					if ((!config->cbHideHidden->Checked) || ((config->cbHideHidden->Checked) && (File::GetAttributes(subdirectory) & FileAttributes::Hidden) != FileAttributes::Hidden)) {
						if (config->tbExcludeSubfolders->Text->IndexOf(subdirname) == -1) {
							readImageFolder(subdirectory, imageType, checkDuplicates);
							
							// Cut short amount of images read when in preview
							if ((config->action==saPreview) && (this->fileCount > 100)) return -1;
						}
					}
				}
			} 
			return 0;
		}
};
}
