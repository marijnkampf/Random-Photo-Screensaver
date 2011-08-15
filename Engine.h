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

#include "multiMonitor.h"
#include "saverMonitor.h"
#include "TConductor.h"
#include "fConfig.h"
#include "TAnimator.h"
#include "fCalendar.h"
#include "fMetadata.h"
#include "WinUser.h"
#include "PowerChecker.h"
#using <Microsoft.VisualBasic.dll>
//extern Engine engine;

extern std::vector<TMultiMonitor*> multiMonitors;

namespace nsRandomPhotoScreensaver {
//	#define SVCNAME TEXT("SvcName")
	#define CM_ALL -1
	#define IMAGE_ERROR_RETRIES 10
	#define GUID_MONITOR_POWER_ON "02731015-4510-4526-99e6-e5a17ebd1aea"

/*	SERVICE_STATUS          gSvcStatus; 
	SERVICE_STATUS_HANDLE   gSvcStatusHandle; 
	HANDLE                  ghSvcStopEvent;*/

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Text::RegularExpressions;
	using namespace System::IO;
	using namespace System::Threading;
	using namespace System::Media;
	using namespace Microsoft::Win32;
	using namespace System::Security::Permissions;

	/// <summary>
	/// Summary for Engine
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	//ref class Engine;
//	ref class Engine;
//	gcroot<Engine^> engine;

	public ref class MonitorIDFilename : public System::Object {
	public:
		int id;
		String^ filename;
		MonitorIDFilename(int id, String^ filename) {
			this->id = id;
			this->filename = filename;
		}

	};

	public ref class Engine : public System::Windows::Forms::Form {
	private:
		array<IntPtr>^ hdcPreview;
		array<fCalendar^>^ calendars;
		array<fMetadata^>^ metadata;
		bool calendarsNotDefined;
		bool metadataNotDefined;
		int mouseX, mouseY;
		bool firstTick;
		bool monitorOffTimesOut;
		int monitorOffTimeOut;
		DateTime dtLastUserInteraction;
//		PowerChecker^ powerChecker;
	//BufferedGraphicsContext^ context;
    //BufferedGraphics^ grafx;

		//System::Drawing::Rectangle desktop; // Size of entire desktop
		int currentMonitor;
		
		TConductor^ conductor;
		TAnimator^ animator;

		bool imgNr;
		bool initialised;
		ArrayList^ fileSystemWatchers;

		private: FileSystemWatcher^ watcher;
		private: array<FileSystemWatcher^>^ watchers;

	private: System::Windows::Forms::Timer^  tAnimations;
					 array<TSaverMonitor^>^saverMonitors;

	public: System::ComponentModel::BackgroundWorker^  bgwImageFolder;
	//public: System::ComponentModel::BackgroundWorker^  deleteFile;
	//private: System::ComponentModel::BackgroundWorker^ bwLoadImagesToMonitor;

	public: void reReadImageFolder() {
		//conductor->clearAllImages();
		bgwImageFolder->RunWorkerAsync();
	}

	private: System::Void OnChanged(System::Object^  sender, System::IO::FileSystemEventArgs^  e) {
		//this->textBox2->Text += 
		System::Windows::Forms::MessageBox::Show ("CHANGED: " + e->FullPath + System::Environment::NewLine);
	}

	private: System::Void OnCreated(System::Object^  sender, System::IO::FileSystemEventArgs^  e) {
		// Check folder > Add files (Restore folder)
	  conductor->OnCreated(e);
		//System::Windows::Forms::MessageBox::Show ("CREATED: " + e->FullPath + System::Environment::NewLine);
	}

	private: System::Void OnDeleted(System::Object^  sender, System::IO::FileSystemEventArgs^  e) {
		// check folder? > Delete sub folders and files
		//System::Windows::Forms::MessageBox::Show ("DELETED: " + e->FullPath + System::Environment::NewLine);
		//this->watchers = gcnew array<FileSystemWatcher ^>(folders->Length);
		conductor->OnDeleted(e);
	}

	private: System::Void OnRenamed(System::Object^  sender, System::IO::RenamedEventArgs^  e) {
		conductor->OnRenamed(e);
		//System::Windows::Forms::MessageBox::Show ("RENAMED: " + e->OldFullPath + " TO " + e->FullPath + System::Environment::NewLine);	  
	}

	public: int watchFolder(String^ full_path, FileSystemWatcher^ watcher) {
		watcher = gcnew FileSystemWatcher;
		watcher->SynchronizingObject = this;
		watcher->NotifyFilter = static_cast<NotifyFilters>(NotifyFilters::FileName | NotifyFilters::DirectoryName);
		//watcher->Changed += gcnew System::IO::FileSystemEventHandler( this, &Engine::OnChanged );
		watcher->Created += gcnew System::IO::FileSystemEventHandler( this, &Engine::OnCreated );
		watcher->Deleted += gcnew System::IO::FileSystemEventHandler( this, &Engine::OnDeleted );
		watcher->Renamed += gcnew System::IO::RenamedEventHandler( this, &Engine::OnRenamed );
		watcher->Path = full_path;
		watcher->EnableRaisingEvents = true;
		watcher->IncludeSubdirectories = true;
		return 0;
	}

	private: System::Void DoWorkImageFolder(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e) {
		//ReadDirectorychangesW(

		if ((config->action != saWallpaper) || (!config->cbWallpaperFolder->Checked)) {
			array<String^> ^folders = config->tbFolder->Text->Split(';');
//			this->watchers = Array::CreateInstance(FileSystemWatcher::typeid, folders->Length);
			this->watchers = gcnew array<FileSystemWatcher ^>(folders->Length);

			int i = 0;
			for each(String^ folder in folders) {
				if (folder->Length > 2) {
					if (bgwImageFolder->CancellationPending) return;
					this->watchFolder(folder, this->watchers[i]);
					conductor->readImageFolder(folder, itNormal);
				}
				// Start monitoring folder
				i++;
			}
			if (conductor->getImageCount() == 0) {
				setBackgroundText("No images found in folder(s):\n" + config->tbFolder->Text->Replace(";", Environment::NewLine) + "\n\nPress 'S' to open Settings configuration menu.");
			}
			conductor->sortFilenames();
			if (config->action != saWallpaper) conductor->setInitialImage();
		}
		//int i = 1;	
		if (config->action != saPreview) this->randomizeWallpaper();
		if (config->action == saWallpaper) Application::Exit();
	}

	private: System::Void bgwDeleteFile_DoWork(System::Object^ sender, System::ComponentModel::DoWorkEventArgs^ e) {
		int i = 0;
		MonitorIDFilename^ monitorIDFilename = (MonitorIDFilename^)e->Argument;
		int monitor = monitorIDFilename->id;
		String^ file = monitorIDFilename->filename;
		//String^ file = saverMonitors[monitor]->filename;
		while(File::Exists(file) && (i < 100)) {
			i++;
			try {
				Microsoft::VisualBasic::FileIO::FileSystem::DeleteFile(file,
					Microsoft::VisualBasic::FileIO::UIOption::OnlyErrorDialogs,
					Microsoft::VisualBasic::FileIO::RecycleOption::SendToRecycleBin,
					Microsoft::VisualBasic::FileIO::UICancelOption::DoNothing);
				if (saverMonitors[monitor]->filename == file) saverMonitors[monitor]->showMessage("Image deleted\n" + Path::GetFileName(file));
			} catch(ArgumentNullException ^ex) {
				saverMonitors[monitor]->showMessage("Nothing to delete");
				i = 100;
			} catch(Exception ^ex) {
				if (saverMonitors[monitor]->filename == file) saverMonitors[monitor]->showMessage("Deleting\n"+Path::GetFileName(file));
				Sleep(1000);
			} 			
		}
	}

	public: void getPowerOffSettings() {
		this->dtLastUserInteraction = DateTime::Now;
		int ilp, ipo;
		bool blp, bpo;
   	SystemParametersInfo(SPI_GETLOWPOWERTIMEOUT, 0, &ilp, 0);
   	SystemParametersInfo(SPI_GETLOWPOWERACTIVE, 0, &blp, 0);
   	SystemParametersInfo(SPI_GETPOWEROFFTIMEOUT, 0, &ipo, 0);
   	SystemParametersInfo(SPI_GETPOWEROFFACTIVE, 0, &bpo, 0);
		
		monitorOffTimesOut = (bpo || ipo);
		monitorOffTimeOut = 0;
		monitorOffTimeOut = Math::Max(bpo, ipo);
	}

	public: bool checkInitMetadata() {
		int start = 0;
		if ((config->action == saWallpaper) || (config->action == saPreview)) return false;
		if ((metadataNotDefined) || (config->nupMetadata->Value != metadata->Length)) {
			if (!metadataNotDefined) start = metadata->Length;
			//config->resizeNrMetadata();
			if (metadataNotDefined) metadata = gcnew array<fMetadata^>(int(config->nupMetadata->Value));
			else {
				if (metadata->Length > config->nupMetadata->Value) {
					for(int i=(int)config->nupMetadata->Value; i < metadata->Length; i++) {
						metadata[i]->Close();
					}
				}
				Array::Resize(metadata, int(config->nupMetadata->Value));
			}
			config->resizeNrMetadata();
			for(int i=start; i < config->nupMetadata->Value; i++) {
				metadata[i] = gcnew fMetadata(i, config->mdTemplate[i], config->mdStylesheet[i], config->mdMonitor[i]);
				AddOwnedForm(metadata[i]);
				metadata[i]->Bounds = config->metadataForms[i];
				setOpacity((Form^)metadata[i], config->metadataVisible, config->tbMDLow->Value, config->tbMDHigh->Value);
			}
			metadataNotDefined = false;
			return true;
		}
		if (config->nupMetadata->Value > 0) return true;
		return false;
	}

	public: bool checkInitCalendars() {
		if ((config->action == saWallpaper) || (config->action == saPreview)) return false;
		if ((calendarsNotDefined) || (config->getNumberCalendars() != calendars->Length)) {
			int start = 0;
			if (calendarsNotDefined) calendars = gcnew array<fCalendar^>(int(config->getNumberCalendars()));
			else {
				start = calendars->Length;
				Array::Resize(calendars, int(config->getNumberCalendars()));
			}
			int month = 0;
			month = (int)config->nupNrPrevCalendars->Value * -1;
			
			for(int i= start; i < config->getNumberCalendars(); i++) {
				if ((month == 0) && (config->cbCurrentMonthCalendar->Checked == false)) month++; 
				calendars[i] = gcnew fCalendar(i, month);
				AddOwnedForm(calendars[i]);
				setOpacity((Form^)calendars[i], config->calendarsVisible, config->tbLow->Value, config->tbHigh->Value);
				//if (config->calendarsVisible != cvNone) calendars[i]->Show();
				month++;
			}
			calendarsNotDefined = false;
			return true;
		}
		if (config->getNumberCalendars() > 0) return true;
		return false;
	}


	public:
		Engine(array<IntPtr>^ hdcPreview) {
			getPowerOffSettings();
			firstTick = true;
			calendarsNotDefined = true;
			metadataNotDefined = true;
			this->hdcPreview = hdcPreview;
			this->SetStyle( static_cast<ControlStyles>(ControlStyles::AllPaintingInWmPaint | ControlStyles::UserPaint), true );
//			context = BufferedGraphicsManager::Current;
			config->engineRunning = true;
			//config->engine = this;

			InitializeComponent();

			saverMonitors = gcnew array<TSaverMonitor^>(multiMonitors.size());
			//System::Windows::Forms::MessageBox::Show (multiMonitors.size().ToString()) ;
			for(int i=0; i < saverMonitors->Length; i++) {
				IntPtr hwnd;
				if (hdcPreview->Length > (i+1)) hwnd = hdcPreview[i];
				saverMonitors[i] = gcnew TSaverMonitor(this->CreateGraphics(), System::Drawing::Rectangle(multiMonitors[i]->x, multiMonitors[i]->y, multiMonitors[i]->width, multiMonitors[i]->height), i, this->Width, this->Height, multiMonitors[i]->panoX, hwnd);				
			}

			config->desktop = initMonitors(config->action==saPreview, hdcPreview);
			conductor = gcnew TConductor(multiMonitors.size());

			bgwImageFolder->WorkerSupportsCancellation = true;
			bgwImageFolder->RunWorkerAsync();

			if (config->action != saWallpaper) {
				this->mouseX = -1;
				this->mouseY = -1;

				this->currentMonitor = CM_ALL;
				this->ClientSize = config->desktop.Size;
				this->Location = Point(config->desktop.Location.X, config->desktop.Location.Y);//config->desktop.Location;
				this->DoubleBuffered = true;

				this->tImage->Interval = (int)config->nupTimeout->Value*1000;

				this->tClock->Enabled = true;
				this->initialised = true;
				Thread::Sleep(100); // Sleep for a bit to read a bigger range of images
				this->getImage(dNext, true, 1, config->cbAnimatedTransitions->Checked);
			}
			if ((config->action == saWallpaper) || (config->action == saPreview)) {
				this->Opacity = 0;
				this->Location = Point(this->Width * -1, this->Height * -1);
			}
			checkInitCalendars();
			checkInitMetadata();

			config->setEngine(this);
		}

	public:
		void startAnimation() {
			if (config->cbAnimatedTransitions->Checked) {
				Random^ rnd = gcnew Random();
				tImage->Enabled = false;
				tClock->Enabled = false;
				animator = gcnew TAnimator(rnd->Next(nrTypes));
				tAnimations->Interval = 100;
				tAnimations->Enabled = true;
			}
		}

		/*void endAnimation() {
			for(int i=0; i < saverMonitors->Length; i++) {
				saverMonitors[i]->animatorEnded = true;
				if (animator != nullptr) animator->end();
			}	 
		}*/

		void setBackgroundText(String^ s) {
			for(int i=0; i < saverMonitors->Length; i++) {
				saverMonitors[i]->backgroundText = s;
			}
		}

		void showMessageAll(String^ s, TTextPosition position) {
			for(int i=0; i < saverMonitors->Length; i++) {
				saverMonitors[i]->showMessage(s, position);				
			}
		}

		void showMessageAll(String^ s) {
			showMessageAll(s, tpMiddleMiddle);
		}

		void showNav(String^ s, int step) {
			String^ t="";
			if (step > 1) t = " x " + step;
			showMessageAll(s+t, tpBottomMiddle);
			this->Refresh();
		}

		void lastFilesToConfig() {
			if (config->cbOrdered->Checked) {
				for(int i=0; i < saverMonitors->Length; i++) {
					config->lastFile[i] = saverMonitors[i]->filename;
				}
			}
		}

		// Returns true when config dialoge is shown or when editing calender positions.
		bool dialogsVisible() {
			if (config->Visible) return true;
			if (checkInitCalendars()) for(int i= 0; i < config->getNumberCalendars(); i++) {
				if (calendars[i]->FormBorderStyle == ::FormBorderStyle::Sizable) return true;
			}
			if (checkInitMetadata()) for(int i= 0; i < config->nupMetadata->Value; i++) {
				if (metadata[i]->FormBorderStyle == ::FormBorderStyle::Sizable) return true;
			}
			return false;
		}

/*		void getImage(TDirection direction, bool random, int step) {
			getImage(direction, random, step, false) {
		}*/
		void showMetadata() {
			if (checkInitMetadata()) {
				for(int i=0; i < config->nupMetadata->Value; i++) {
					metadata[i]->showMetadata(saverMonitors[metadata[i]->getMonitorNr()]->metadata);				
				}
			}
		}
/*
		private int bwLoadImagesToMonitor_DoWork(BackgroundWorker bw, int monitor, TDirection direction, bool random, int step, bool animated) {
			this->tImage->Enabled = false;
			if (conductor->getImageCount() > 0) {
				conductor->panorama = false;
				for(int i=0; i < saverMonitors->Length; i++) {
// Run following in new process?
					if (conductor->panorama) {
						saverMonitors[i]->LoadMonitorImageFromPanorama(conductor);
						conductor->adjustNextImg(direction);
					} else {
						int tryCount = 0;
						while(!saverMonitors[i]->LoadMonitorImage(conductor->getImage(i, direction, random, step), conductor)
							&& (tryCount++ < IMAGE_ERROR_RETRIES)) ;
					}
					saverMonitors[i]->skipAnim = !animated;
					//if (conductor->panorama) i = saverMonitors->Length;
				}
				showMetadata();
			}
		}

    // This event handler demonstrates how to interpret 
    // the outcome of the asynchronous operation implemented
    // in the DoWork event handler.
    private void bwLoadImagesToMonitor_Completed(
      object sender, 
      RunWorkerCompletedEventArgs e) {   

			// Reread timeout value incase updated
			this->tImage->Interval = (int)config->nupTimeout->Value*1000;
			this->tImage->Enabled = true;

      if (e.Cancelled) {
          // The user canceled the operation.
      } else if (e.Error != null) {
          // There was an error during the operation.
      } else {
				if (animated) this->startAnimation();
				this->Refresh();
        // The operation completed normally.
      }

	  }


*/
		void getImage(TDirection direction, bool random, int step, bool animated) {
			this->tImage->Enabled = false;
			if (conductor->getImageCount() > 0) {
				conductor->panorama = false;
				for(int i=0; i < saverMonitors->Length; i++) {
// Run following in new process?
					if (conductor->panorama) {
						saverMonitors[i]->LoadMonitorImageFromPanorama(conductor);
						conductor->adjustNextImg(direction);
					} else {
						int tryCount = 0;
						while(!saverMonitors[i]->LoadMonitorImage(conductor->getImage(i, direction, random, step), conductor)
							&& (tryCount++ < IMAGE_ERROR_RETRIES)) ;
					}
					saverMonitors[i]->skipAnim = !animated;
					//if (conductor->panorama) i = saverMonitors->Length;
				}
				showMetadata();
			}
			// Reread timeout value incase updated
			this->tImage->Interval = (int)config->nupTimeout->Value*1000;
			this->tImage->Enabled = true;
			if (animated) this->startAnimation();
			this->Refresh();
		}

		void randomizeWallpaper() {
			if (config->wallpaperFrequency != wfNone) {
				int count;
				if (config->cbWallpaperFolder->Checked) {
					conductor->readImageFolder(config->tbWallpaperFolder->Text, itWallpaper);
					count = conductor->getWallpaperImageCount();
				} else {
					count = (conductor->getImageCount() > 0);
				}

				if (count > 0) {
					Bitmap^ bmpWallpaper = gcnew Bitmap(this->Width, this->Height);
					Brush^ backgroundBrush = gcnew SolidBrush(config->btnWallpaperBackgroundColor->BackColor);
					Graphics ^ g = Graphics::FromImage(bmpWallpaper);

					float ratio = (float)this->Width / (float)this->Height;

					g->FillRectangle( backgroundBrush, 0, 0, this->Width, this->Height );
					bool panoramaStrecth = false;
						
					for(int i=0; i < saverMonitors->Length; i++) {
						String^ filename;
						Image^ image;
						
						bool readSuccess = false;
						int tryCount = 0;
						while(!readSuccess && (tryCount++ < IMAGE_ERROR_RETRIES)) {
							try {
								filename = conductor->getRandomWallpaper();//safe_cast<String^>(this->images[rnd->Next(this->images->Count)]);
								image = Image::FromFile(filename);
								readSuccess = true;
							} catch(OutOfMemoryException ^ex) {
								conductor->removeImageFromList(filename);
								readSuccess = false;
							} catch(FileNotFoundException ^ex) {
								conductor->removeImageFromList(filename);
								readSuccess = false;
							}
						}

						if (readSuccess) {
							float imgRatio = (float)image->Width / (float)image->Height;
							RectangleF targetRect;
							if ((config->cbPanoramaStretch->Checked) && (i == 0) && (imgRatio >= ratio)) {
								panoramaStrecth = true;
								targetRect = config->desktop;
							} else {
								targetRect = this->saverMonitors[i]->getRect();
							}

							GraphicsUnit units = GraphicsUnit::Pixel;
							RectangleF rect = TSaverMonitor::scaleRect(image->GetBounds(units), targetRect, config->cbWallpaperStretch->Checked, config->cbRandomPositions->Checked);
			 
							g->SmoothingMode = System::Drawing::Drawing2D::SmoothingMode::HighQuality;
							g->InterpolationMode = System::Drawing::Drawing2D::InterpolationMode::Bicubic;
							
							if ((panoramaStrecth) && (rect.X < 0)) {				
								RectangleF rectBackground = image->GetBounds(units);
								float split = Math::Abs(targetRect.Left) / targetRect.Width;
								RectangleF rightDest = RectangleF(0, rect.Y, int(rect.Width*(double)split), rect.Height);
								RectangleF rightSource = RectangleF(rectBackground.Width*split, rectBackground.Top, rectBackground.Width-(rectBackground.Width*split), rectBackground.Height);
								g->DrawImage(image, rightDest, rightSource, GraphicsUnit::Pixel);
								RectangleF leftDest = RectangleF(int(rect.Width*(double)split), rect.Y, int(rect.Width*(double)(1-split)), rect.Height);
								RectangleF leftSource = RectangleF(0, rectBackground.Top, rectBackground.Width-(rectBackground.Width*(1-split)), rectBackground.Height);
								g->DrawImage(image, leftDest, leftSource, GraphicsUnit::Pixel);
							} else {
								g->DrawImage(image, rect);
							}
							if (config->cbWallpaperFilenames->Checked) {
								saverMonitors[0]->WriteText(g, filename, this->saverMonitors[i]->getRect(), tpTopLeft, config->btnWallpaperFilenameFont->Font, config->btnWallpaperFilenameFont->ForeColor, true, this->saverMonitors[i]->getRect().Location);
							}
						}
						if (panoramaStrecth) i = saverMonitors->Length+1;
					}
					
					if (!Directory::Exists(config->appDataFolder)) {
						if (::DialogResult::OK == MessageBox::Show ("Create folder '" + config->appDataFolder + "'?\n\nOk: Creates folder for backgrounds\nCancel exits screensaver without changing background image.", "Installation folder for background not found!", MessageBoxButtons::OKCancel, MessageBoxIcon::Exclamation)) {
							Directory::CreateDirectory(config->appDataFolder);
						} else return;
					}

					bmpWallpaper->Save(config->appDataFolder + "rpsBackground.bmp", Drawing::Imaging::ImageFormat::Bmp);

					if (File::Exists(config->appDataFolder + "rpsBackground.bmp")) {
						RegistryKey^ rk = Registry::CurrentUser->OpenSubKey("Control Panel\\desktop", true);
						rk->SetValue("Wallpaper", config->appDataFolder + "rpsBackground.bmp");
						rk->SetValue("TileWallpaper", "1");
						if (config->wallpaperFrequency != wfDaily) SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, NULL, SPIF_SENDWININICHANGE);
					}
				}
			}
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Engine() {
			for(int i=0; i < metadata->Length; i++) {
				config->metadataForms[i] = metadata[i]->Bounds;
			}
			this->lastFilesToConfig();
			config->TransferPersistent(true);
			config->configChanged();

			if (components) {
				delete components;
			}
			//this->UnRegisterForPowerNotifications();
		}

    virtual void OnPaint( PaintEventArgs^ e ) override {
			if (saverMonitors != nullptr) {
				for(int i=0; i < saverMonitors->Length; i++) {
					saverMonitors[i]->DisplayCounts(conductor->getImageCount(), conductor->getDirCount());
					saverMonitors[i]->OnPaint( e );
				}
				if (config->action == saPreview) {
					saverMonitors[0]->OnPreviewPaint( e );
				}
				//grafx->Render(hdcPreview);
	//				grafx->Render(IntPtr(788112));
		//		}
			}
    }

	private: System::Windows::Forms::Timer^  tClock;
	private: System::Windows::Forms::Timer^  tImage;
	private: System::ComponentModel::IContainer^  components;
	//private: SystemEvents::PowerModeChangedEvent pmc;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Engine::typeid));
			this->tClock = (gcnew System::Windows::Forms::Timer(this->components));
			this->tImage = (gcnew System::Windows::Forms::Timer(this->components));
			this->bgwImageFolder = (gcnew System::ComponentModel::BackgroundWorker());
			this->tAnimations = (gcnew System::Windows::Forms::Timer(this->components));
			this->SuspendLayout();
			// 
			// tClock
			// 
			this->tClock->Interval = 1000;
			this->tClock->Tick += gcnew System::EventHandler(this, &Engine::tClock_Tick);
			// 
			// tImage
			// 
			this->tImage->Enabled = true;
			this->tImage->Interval = 5000;
			this->tImage->Tick += gcnew System::EventHandler(this, &Engine::tImage_Tick);
			// 
			// bgwImageFolder
			// 
			this->bgwImageFolder->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &Engine::DoWorkImageFolder);
			this->bgwImageFolder->ProgressChanged += gcnew System::ComponentModel::ProgressChangedEventHandler(this, &Engine::ProgressChangedImageFolder);
			// 
			// tAnimations
			// 
			this->tAnimations->Tick += gcnew System::EventHandler(this, &Engine::tAnimations_Tick);
			// 
			// Engine
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(292, 266);
			this->ControlBox = false;
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^  >(resources->GetObject(L"$this.Icon")));
			this->Name = L"Engine";
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::Manual;
			this->Text = L"Random Photo Screensaver 3.0";
			this->Load += gcnew System::EventHandler(this, &Engine::Engine_Load);
			this->TextChanged += gcnew System::EventHandler(this, &Engine::Engine_TextChanged);
			this->Click += gcnew System::EventHandler(this, &Engine::Engine_Click);
			this->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &Engine::Engine_Paint);
			this->DoubleClick += gcnew System::EventHandler(this, &Engine::Engine_DoubleClick);
			this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Engine::Engine_KeyDown);
			this->MouseEnter += gcnew System::EventHandler(this, &Engine::Engine_MouseEnter);
			this->MouseLeave += gcnew System::EventHandler(this, &Engine::Engine_MouseLeave);
			this->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &Engine::Engine_MouseMove);
			this->Resize += gcnew System::EventHandler(this, &Engine::Engine_Resize);
			this->ResumeLayout(false);

		}
#pragma endregion
/*	private: void PowerModeChanged (System::Object ^ sender, Microsoft::Win32::PowerModeChangedEventArgs ^ e) {
	  Console::Beep();
		int i = 1;
	}*/

	/** 
		Push screensaver application to forground to counter GDI+ Window race condition 
		http://support.microsoft.com/kb/943453	
	**/
	private: System::Void setActiveApp() {
		SetForegroundWindow((HWND)this->Handle.ToPointer());
		firstTick = false;
	}

	private: System::Void tClock_Tick(System::Object^  sender, System::EventArgs^  e) {
		if (firstTick) {
			this->setActiveApp();
		}
		for(int i=0; i < saverMonitors->Length; i++) {
			saverMonitors[i]->RedrawBuffer();
		}	 
		this->Refresh();
/*		if ((this->powerChecker->InfoAvailable) && (this->powerChecker->MonitorOn == false)) {
			//Console::Beep();
		} else {
			saverMonitors[0]->showMessage("MonitorOn true");
		}
		saverMonitors[0]->showMessage(this->powerChecker->report);*/
 	  if (this->monitorOffTimesOut) {
			DateTime dt = DateTime::Now;
			TimeSpan ts = dt.Subtract(this->dtLastUserInteraction);
			if (ts.TotalSeconds > this->monitorOffTimeOut) {
				//Application::Exit();
				tImage->Enabled = false;
				tClock->Enabled = false;
				tAnimations->Enabled = false;
				//tAnimated
			} else tClock->Enabled = true;
		}
  }
	private: System::Void tImage_Tick(System::Object^  sender, System::EventArgs^  e) {
	  this->getImage(dNext, true, 1, config->cbAnimatedTransitions->Checked);
	}

	private: System::Void Engine_Load(System::Object^  sender, System::EventArgs^  e) {
	}
	private: System::Void Engine_Click(System::Object^  sender, System::EventArgs^  e) {
		//this->nextImage();
	  Application::Exit();
	}
	private: System::Void Engine_DoubleClick(System::Object^  sender, System::EventArgs^  e) {
		Application::Exit();
	}
	private: System::Void Engine_Resize(System::Object^  sender, System::EventArgs^  e) {
		config->desktop = initMonitors(config->action==saPreview, hdcPreview);
		this->ClientSize = config->desktop.Size;
		this->Location = config->desktop.Location;
		this->DoubleBuffered = true;
		if (this->initialised) {
			for(int i=0; i < saverMonitors->Length; i++) {
				saverMonitors[i]->OnResize(System::Drawing::Rectangle(multiMonitors[i]->x, multiMonitors[i]->y, multiMonitors[i]->width, multiMonitors[i]->height));
			}
		} 
	}

	private: System::Void ProgressChangedImageFolder(System::Object^  sender, System::ComponentModel::ProgressChangedEventArgs^  e) {
  }

	public: TCalendarsVisibility getNextOpacity(TCalendarsVisibility visibility) {
		switch(visibility) {
			case cvLow: 
				return cvHigh; 
			break;
			case cvNone: 
				return cvLow; 
			break;
			case cvHigh: 
				return cvNone; 
			break;
		}
		return cvNone;
	}

	public: void setOpacity(Form^ form, TCalendarsVisibility visibility, int low, int high) {
		switch(visibility) {
			case cvLow: 
				form->Opacity = double(low)/100;
				form->Show(); 
			break;
			case cvHigh: 
				form->Opacity = double(high)/100;
				form->Show(); 
			break;
			case cvNone: 
				form->Hide(); 
			break;
		}
	}

	public: System::Void Engine_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
	  switch (e->KeyCode) {
			case Keys::M: {
				if (checkInitMetadata()) for(int i= 0; i < config->nupMetadata->Value; i++) {
					if (e->Control) {
						if (!metadata[i]->Visible) metadata[i]->FormBorderStyle = ::FormBorderStyle::Sizable;
						else if (metadata[i]->FormBorderStyle == ::FormBorderStyle::Sizable) metadata[i]->FormBorderStyle = ::FormBorderStyle::None;
						else metadata[i]->FormBorderStyle = ::FormBorderStyle::Sizable;
						if (metadata[i]->FormBorderStyle == ::FormBorderStyle::Sizable) {
							metadata[i]->viewSettings(true);
						} else {
							metadata[i]->viewSettings(false);
							config->metadataVisible = cvHigh;
						}
						metadata[i]->Show();
						::Cursor::Show();
					} else {
						if (i == 0) config->metadataVisible = getNextOpacity(config->metadataVisible);
						setOpacity((Form^)metadata[i], config->metadataVisible, config->tbMDLow->Value, config->tbMDHigh->Value);
					}
				}
			} break;
			case Keys::C: {
				if (config->action != saPreview) {
					if (checkInitCalendars()) for(int i= 0; i < config->getNumberCalendars(); i++) {
						if (e->Control) {
							if (!calendars[i]->Visible) calendars[i]->FormBorderStyle = ::FormBorderStyle::Sizable;
							else if (calendars[i]->FormBorderStyle == ::FormBorderStyle::Sizable) calendars[i]->FormBorderStyle = ::FormBorderStyle::None;
							else calendars[i]->FormBorderStyle = ::FormBorderStyle::Sizable;
							calendars[i]->Show();
							::Cursor::Show();
						} else {
							if (i == 0) config->calendarsVisible = getNextOpacity(config->calendarsVisible);
							setOpacity((Form^)calendars[i], config->calendarsVisible, config->tbLow->Value, config->tbHigh->Value);

						}
					}
					this->Activate();
				}
			} break;
			case Keys::N: {
				String^ s;
				config->cbMetadata->Checked = !config->cbMetadata->Checked ;
				if (config->cbMetadata->Checked) {
					s = "Simple metadata on";
				} else {
					s = "Simple metadata off";
				}
				showMessageAll(s);
			} break;
			case Keys::S: {
				::Cursor::Show();
				//config->reInitMouseMovement = true;
				config->Show();
			} break;
			case Keys::D1: case Keys::D2: case Keys::D3: case Keys::D4: case Keys::D5: case Keys::D6: case Keys::D7: case Keys::D8: case Keys::D9: {
				int m = (int)e->KeyCode - (int)Keys::D1;
				if (m < saverMonitors->Length) {
					for(int i=0; i < saverMonitors->Length; i++) {
						saverMonitors[i]->unidentify();
					}
					saverMonitors[m]->identify();
					currentMonitor = m;
				} else {
					for(int i=0; i < saverMonitors->Length; i++) {
						int cm; if (currentMonitor == CM_ALL) cm = i; else cm = currentMonitor;
						String^ s = "";
						if (saverMonitors->Length != 1) s = "s";
						saverMonitors[cm]->showMessage("Can't select monitor " + (m+1).ToString() + "\nOnly " + saverMonitors->Length.ToString() + " monitor" + s + " found.");
					}
				}
			} break;
			case Keys::D0: {
				currentMonitor = CM_ALL;
				for(int i=0; i < saverMonitors->Length; i++) {
					saverMonitors[i]->identify();
				}
			} break;
			case Keys::P: {
				if ((tImage->Enabled == true) || (tAnimations->Enabled == true)) {
					tImage->Enabled = false;
					tAnimations->Enabled = false;
					showNav("||", conductor->getStepSize(e));
				} else {
					tImage->Enabled = true;
					showNav(">", conductor->getStepSize(e));
				}
			} break;
			case Keys::NumPad4: case Keys::Left: {
				showNav("<<", conductor->getStepSize(e));
			  this->getImage(dPrev, true, conductor->getStepSize(e), false);
			} break;
			case Keys::NumPad6: case Keys::Right: {
				showNav(">>", conductor->getStepSize(e));
			  this->getImage(dNext, true, conductor->getStepSize(e), false);
			} break;
			case Keys::NumPad8: case Keys::Up: {
				showNav("^", conductor->getStepSize(e));
			  this->getImage(dPrev, false, conductor->getStepSize(e), false);
			} break;
			case Keys::NumPad2: case Keys::Down: {
				showNav("v", conductor->getStepSize(e));
			  this->getImage(dNext, false, conductor->getStepSize(e), false);
			} break;
			case Keys::NumPad1: case Keys::R: {
				String^ s;
				lastFilesToConfig();
			  config->cbOrdered->Checked = !config->cbOrdered->Checked;
				if (config->cbOrdered->Checked) {
					s = "Sequential";
					conductor->setInitialImage();
				} else {
					s = "Randomized";
				}
				for(int i=0; i < saverMonitors->Length; i++) saverMonitors[i]->showMessage(s);
				config->changed = true;
				this->Refresh();
			} break;
			case Keys::NumPad7: case Keys::F: {
				String^ s;
				config->cbDisplayFilenames->Checked = !config->cbDisplayFilenames->Checked;
				if (config->cbDisplayFilenames->Checked) {
					s = "Display filenames on";
				} else {
					s = "Display filenames off";
				}
				showMessageAll(s);
				config->changed = true;
				this->Refresh();
			} break;
			case Keys::Multiply: case Keys::G: {
				setFontSize(config->filenameFont, config->filenameFont->Size + 1);
				config->changed = true;
				this->Refresh();
			} break;
			case Keys::Divide: case Keys::H: {
				if (config->filenameFont->Size > 1) setFontSize(config->filenameFont, config->filenameFont->Size - 1);
				config->changed = true;
				this->Refresh();
			} break;
			case Keys::NumPad9: case Keys::A: {
				String^ s;
				config->cbAnimatedTransitions->Checked = !config->cbAnimatedTransitions->Checked;
				if (config->cbAnimatedTransitions->Checked) {
					s = "Animated transitions on";
				} else {
					s = "Animated transitions off";
				}
				showMessageAll(s);
				config->changed = true;
				this->Refresh();
			} break;
			case Keys::Add: case Keys::V: {
				for(int i=0; i < saverMonitors->Length; i++) {
					config->changed = true;
					int cm; if (currentMonitor == CM_ALL) cm = i; else cm = currentMonitor;
					if (cm == i) {
						setFontSize(config->clockFont[i], config->clockFont[i]->Size + 1);
					}
				}
			} break;
			case Keys::Subtract: case Keys::B: {
				for(int i=0; i < saverMonitors->Length; i++) {
					config->changed = true;
					int cm; if (currentMonitor == CM_ALL) cm = i; else cm = currentMonitor;
					if (cm == i) {
						if (config->clockFont[i]->Size > 1) setFontSize(config->clockFont[i], config->clockFont[i]->Size - 1);
					}
				}
			} break;
			case Keys::NumPad5: case Keys::T: {
				for(int i=0; i < saverMonitors->Length; i++) {
					String^ s;
					config->changed = true;
					int cm; if (currentMonitor == CM_ALL) cm = i; else cm = currentMonitor;
					if (cm == i) {
						switch (config->clockType[cm]) {
							case ctNone: 
								config->clockType[cm] = ctCurrent; 
								s = "Current time";
							break;
							case ctCurrent: 
								config->clockType[cm] = ctRunning; 
								s = "Screensaver running time";
							break;
							case ctRunning: 
								config->clockType[cm] = ctNone; 
								s = "No clock";
							break;		
						}
					}
					saverMonitors[cm]->showMessage(s);
				}
				config->changed = true;
				this->Refresh();
			} break;
			case Keys::ShiftKey: case Keys::ControlKey: case Keys::Menu: {
			} break;
			case Keys::E: {
				for(int i=0; i < saverMonitors->Length; i++) {
					config->changed = true;
					int cm; if (currentMonitor == CM_ALL) cm = i; else cm = currentMonitor;
					if ((cm == i) && ((!conductor->panorama) || (conductor->panorama && (i == 0)))) {
						Process^ p = Process::Start("explorer.exe", "/e,/select," + saverMonitors[i]->filename);
					}
				}
				if (config->cbCloseAfterImageLocate->Checked) Application::Exit();
			} break;
			case Keys::I: {
				System::Windows::Forms::MessageBox::Show("Found " + conductor->getImageCount().ToString("N0") + " images in " + conductor->getDirCount().ToString("N0") + " folders.");
			} break;
			case Keys::Escape: {
				Application::Exit();
			} break;
			case Keys::Delete: {
				if (config->cbDeleteKey->Checked) {
					for(int i=0; i < saverMonitors->Length; i++) {
						bool deleteFile = true;
						if (config->cbConfirmDelete->Checked) {
							::Cursor::Show();
							if (::DialogResult::Yes == MessageBox::Show ("Are you sure you want to sent '" + Path::GetFileName(saverMonitors[i]->filename) + "' to the Recycle Bin?", "Confirm File Delete", MessageBoxButtons::YesNo, MessageBoxIcon::Exclamation)) {
								deleteFile = true;
							} else deleteFile = false;
							::Cursor::Hide();
						}
						if (deleteFile) {
							System::ComponentModel::BackgroundWorker^ bgwDeleteFile = gcnew BackgroundWorker();
							bgwDeleteFile->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &Engine::bgwDeleteFile_DoWork);
							bgwDeleteFile->RunWorkerAsync(gcnew MonitorIDFilename(i, saverMonitors[i]->filename));
						}
					}
				}				
			} break;
			default:
				if (!config->cbOnlyEscapeExits->Checked) Application::Exit();
			break;
		}
		this->dtLastUserInteraction = DateTime::Now;
		// If not animating enabled clock to resume after monitor sleep.
		if (tAnimations->Enabled != true) tClock->Enabled = true;
	}

	Drawing::Font^  setFontSize(Drawing::Font^ %font, float size) {
   System::ComponentModel::TypeConverter^ converter =
		     System::ComponentModel::TypeDescriptor::GetConverter( System::Drawing::Font::typeid );
		font = dynamic_cast<System::Drawing::Font^>(converter->ConvertFromString( "Arial, " + size + "pt" ));
		return font;
	}

	private: System::Void Engine_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
		if (this->dialogsVisible()) {
			mouseX = -1;
			mouseY = -1;
		} else {
			if (mouseX == -1) mouseX = e->X;
			if (mouseY == -1) mouseY = e->Y;
			int sensitivity;
			switch(config->cbMouseSensitivity->SelectedIndex) {
				case 0: sensitivity = 0; break;
				case 1: sensitivity = 10; break;
				case 2: sensitivity = 50; break;
				// ignore mouse movements
				case 3: mouseX = e->X; mouseY = e->Y; break;
			}
			if ( (e->X > (mouseX + sensitivity)) || (e->X < (mouseX - sensitivity)) ||
				(e->Y > (mouseY + sensitivity)) || (e->Y < (mouseY - sensitivity)) ) {
				Application::Exit();
			}
			mouseX = e->X;
			mouseY = e->Y;
		}
	}
	private: System::Void Engine_MouseEnter(System::Object^  sender, System::EventArgs^  e) {
	  if (!this->dialogsVisible())	::Cursor::Hide();
	}
	private: System::Void Engine_MouseLeave(System::Object^  sender, System::EventArgs^  e) {
		if (!this->dialogsVisible())	::Cursor::Show();
		mouseX = -1;
		mouseY = -1;
	}
	private: System::Void tAnimations_Tick(System::Object^  sender, System::EventArgs^  e) {
		if (firstTick) {
			this->setActiveApp();
		}
	  animator->animateFrame(saverMonitors);
		for(int i=0; i < saverMonitors->Length; i++) {
			saverMonitors[i]->RedrawBuffer();
		}	 
		this->Refresh();
		if (animator->ended()) {
			tImage->Enabled = true;
			tClock->Enabled = true;
		}
	}
	private: System::Void Engine_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {
	}

	// !!Biggest hack in screensaver!!
	// Can't get proper communication from fConfig form to Engine form. fConfig will change Form Text when attention is required by Engine form.
	private: System::Void Engine_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		if (this->Text->EndsWith("##")) {
			switch(config->configAction) {
				case caFoldersChanged:
/*					array<String^> ^oldFolders = config->_tbFolderValue->Split(';');
					for each(String^ folder in oldFolders) {
	//					conductor->Delete(folder);
					}*/
					conductor->ClearImages();

					config->_tbFolderValue = config->tbFolder->Text;

					bgwImageFolder->CancelAsync();
					bgwImageFolder->RunWorkerAsync();
				break;
			}
			config->resetEngineText();
		}
	}
};
}