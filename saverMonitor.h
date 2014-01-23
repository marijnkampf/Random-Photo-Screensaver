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
/*
	Changes:
	20090420
		Changed: Fixed multi display issues.
		Updated: 
			void LoadMonitorImageFromPanorama(TConductor^ %conductor)
			bool LoadMonitorImage(String^ filename, TConductor^ %conductor)
		By:			 MK.
*/
#pragma once
#include "fConfig.h"
#include "TConductor.h"
#include "TMetadata.h"
#include "TTextPosition.h"
/*#include "image.hpp"
#include "exif.hpp"
#include "string"*/

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Globalization;
using namespace System::Drawing::Imaging;
using namespace System::Text;

#define DEBUGINFOFILE "debuginfo.txt"
#define MAXERRORCOUNT 25

extern gcroot<nsRandomPhotoScreensaver::fConfig^> gConfig;

namespace nsRandomPhotoScreensaver {
	// http://msdn.microsoft.com/en-us/library/1b4az623.aspx
	void setFontSize(Font^ %font, int size) {
		if (size <= 0) size = 1;
		font = gcnew Font(font->FontFamily, size, font->Style, font->Unit, font->GdiCharSet, font->GdiVerticalFont);
	}

	//	enum TVertical { tpNone, tpTop, tpMiddle, tpBottom, tpRandom }
//	enum THorizontal { tpNone, tpLeft, tpMiddle, tpRight, tpRandom }
/*
	String^ getTextPosition(TTextPosition tp) {
		switch (tp) {
			case tpNone: 				return "None"; break;
			case tpTopLeft: 			return "Top Left"; break;
			case tpTopMiddle: 		return "Top Middle"; break;
			case tpTopRight: 		return "Top Right"; break;
			case tpMiddleLeft: 	return "Middle Left"; break;
			case tpMiddleMiddle: return "Middle Middle"; break;
			case tpMiddleRight: 	return "Middle Right"; break;
			case tpBottomLeft: 	return "Bottom Left"; break;
			case tpBottomMiddle: return "Bottom Middle"; break;
			case tpBottomRight: 	return "Bottom Right"; break;
			case tpRandom: 	return "Random"; break;
		}
		return "";
	}

	TTextPosition getTextPosition(String^ tp) {
		if (String::Compare(tp, "None", StringComparison::InvariantCultureIgnoreCase) == 0) return tpNone;
		else if (String::Compare(tp, "Top Left", StringComparison::InvariantCultureIgnoreCase) == 0) return tpTopLeft;
		else if (String::Compare(tp, "Top Middle", StringComparison::InvariantCultureIgnoreCase) == 0) return tpTopMiddle;
		else if (String::Compare(tp, "Top Right", StringComparison::InvariantCultureIgnoreCase) == 0) return tpTopRight;
		else if (String::Compare(tp, "Middle Left", StringComparison::InvariantCultureIgnoreCase) == 0) return tpMiddleLeft;
		else if (String::Compare(tp, "Middle Middle", StringComparison::InvariantCultureIgnoreCase) == 0) return tpMiddleMiddle;
		else if (String::Compare(tp, "Middle Right", StringComparison::InvariantCultureIgnoreCase) == 0) return tpMiddleRight;
		else if (String::Compare(tp, "Bottom Left", StringComparison::InvariantCultureIgnoreCase) == 0) return tpBottomLeft;
		else if (String::Compare(tp, "Bottom Middle", StringComparison::InvariantCultureIgnoreCase) == 0) return tpBottomMiddle;
		else if (String::Compare(tp, "Bottom Right", StringComparison::InvariantCultureIgnoreCase) == 0) return tpBottomRight;
		else if (String::Compare(tp, "Random", StringComparison::InvariantCultureIgnoreCase) == 0) return tpRandom;
		return tpNone;
	}*/

	public ref class TSaverMonitor {
	private:
		fConfig^ config;
		IntPtr hdcPreview;
		int id;
		int mmWidth;
		int mmHeight;
		int panoX;
		Graphics^ graphics;
		BufferedGraphicsContext^ context;
		BufferedGraphics^ grafx;

		float previewScale;

		int fileCount, dirCount;
		Brush^ backgroundBrush;
		
		DateTime identifyTimeout;
		DateTime infoTimeout;
		String^ messageText;
		DateTime messageTimeout;
		TTextPosition^ messagePosition;

		float multiMonitorRatio;
		String^ simpleMetadata;

		int errorCount;

		//String^ metadata;
		//System::Drawing::Point Location;
		//System::Drawing::Size Size;
		//int Width;
		//int Height;
	public:
		// Buffer[2] New image
		// Buffer[1] Old image
		// Buffer[0] Screen buffer
		TMetadata^ metadata;
		array<Bitmap^>^buffers; 
		Bitmap^ bPreview;
		//Graphics^ gPreview;
		Image^ image;
		String^ filename;
		String^ rawCachedFilename;
		String^ showFilename;
		String^ backgroundText;
		System::Drawing::Rectangle r;
		bool skipAnim;
//		bool animatorEnded;

	public:	
		TSaverMonitor(Graphics^ graphics, System::Drawing::Rectangle rectangle, int id, int mmWidth, int mmHeight, int panoX, IntPtr hdcPreview)	{
			config = gConfig;
			this->errorCount = 0;
			this->hdcPreview = hdcPreview;
			previewScale = 1;
			if (config->action == saPreview) {
				this->bPreview = gcnew Bitmap(rectangle.Width, rectangle.Height);
				//MessageBox::Show(hdcPreview.ToString() + " " + rectangle.Width + ", "+rectangle.Height, "Info", MessageBoxButtons::OK, MessageBoxIcon::Exclamation );
			}
			//this->engine = engine;
 		  //this->Location = Point(x,y);
			//this->Size = System::Drawing::Size(width, height);
			//this->Width = width;
			//this->Height = height;
			this->panoX = panoX;
			this->backgroundText = "Loading";
			this->id = id;
			this->multiMonitorRatio = (float)mmWidth / (float)mmHeight;
			this->mmWidth = mmWidth;
			this->mmHeight = mmHeight;

			this->backgroundBrush = gcnew SolidBrush(config->btnBackgroundColor->BackColor);

			this->simpleMetadata = "";

			this->graphics = graphics;

			this->skipAnim = false;
			
			this->infoTimeout = DateTime::Now.AddSeconds((double)config->nupFolderInfoTimeout->Value);
//			animatorEnded = true;

			//this->Bounds = System::Drawing::Rectangle(x, y, width, height);
			buffers = gcnew array<Bitmap^>(2);

			context = BufferedGraphicsManager::Current;
			this->OnResize(rectangle);
		}

		System::Drawing::Rectangle getRect() {
			return r;
		}

		void unidentify() {
			identifyTimeout = DateTime::Now;
		}

		void identify(int timeout) {
			identifyTimeout = DateTime::Now.AddSeconds(timeout);
		}

		void identify() {
			identify(4);
		}

		void showMessage(String^ message, TTextPosition^ position, int timeout) {
			this->messagePosition = position;
			this->messageText = message;
			messageTimeout = DateTime::Now.AddSeconds(timeout);
		}

		void showMessage(String^ message, TTextPosition^ position) {
			showMessage(message, position, 4);
		}

		void showMessage(String^ message) {
			showMessage(message, gcnew TTextPosition(TTextPosition::THorizontal::tpXMiddle, TTextPosition::TVertical::tpYMiddle), 4);
		}

		void debugFile(int m, String^ filename) {
		  debugFile(m, filename, "");
		}
		
		void debugFile(int m, String^ filename, String^ error) {
		  if (config->cbDebug->Checked) {
				StreamWriter^ sw;
				try {
					if (!File::Exists(config->addTrailingSlash(config->appDataFolder) + DEBUGINFOFILE)) {
						sw = File::CreateText(config->addTrailingSlash(config->appDataFolder) + DEBUGINFOFILE);
						sw->WriteLine("Date & Time\tMonitor\tFilename\t[Error]");
					} else {
						sw = File::AppendText(config->addTrailingSlash(config->appDataFolder) + DEBUGINFOFILE);
					}
					if ((error != "") && (this->errorCount < MAXERRORCOUNT)) {
						this->errorCount++;
						sw->WriteLine(DateTime::Now.ToString() + "\t" + (m + 1).ToString() + "\t" + filename + "\t[" + error + "]");
						if (this->errorCount == MAXERRORCOUNT) sw->WriteLine(DateTime::Now.ToString() + "\tMax error count (" + MAXERRORCOUNT + ") reached for monitor. No further errors recorded.");
					} else {
						sw->WriteLine(DateTime::Now.ToString() + "\t" + (m + 1).ToString() + "\t" + filename);
					}
				} finally {
					delete (IDisposable^)sw;
				}
			}
		}

		virtual void OnPaint( PaintEventArgs^ e ) {
      grafx->Render( e->Graphics );
    }

		void OnPreviewPaint( PaintEventArgs^ e ) {
      grafx->Render( e->Graphics );
			//graphics->Render
			graphics->DrawImageUnscaled(bPreview, Point(0,0));

	
			//grafx->Render( e->Graphics );
			//Graphics^ g = Graphics::FromHwnd(IntPtr(853648));
			//graphics->FillRectangle(b, 10, 10, 147, 103);
			//graphics->DrawImageUnscaled(buffers[0], Point(0,0));

      //grafx->Render(g);
    }

	protected: 
		~TSaverMonitor() {
			if (components) {
				delete components;
			}
		}

	public:
		// Fits rSource centered into rDest maintaining aspect ratios
		// If randomPositionSmall is true selects random position on destination.
		static RectangleF scaleRect(RectangleF rSource, RectangleF rDest, bool stretchSmall, bool randomPositionSmall) {
			RectangleF result = RectangleF(0, 0, 0, 0);
			float sRatio, dRatio;
			float scaleBy = 0;
			bool randomPos = false;

			sRatio = (float)rSource.Width / (float)rSource.Height;
			dRatio = (float)rDest.Width / (float)rDest.Height;

			if ((!stretchSmall) && 
								(rDest.Height >= rSource.Height) && (rDest.Width >= rSource.Width)) {
				scaleBy = 1;
				if (randomPositionSmall) randomPos = true;
			} else {
				if (sRatio < dRatio) {
					scaleBy = (float)rDest.Height / (float)rSource.Height;
				} else {
					scaleBy = (float)rDest.Width / (float)rSource.Width;
				}
			}

			result.Size = System::Drawing::Size((int)(rSource.Width * scaleBy), (int)(rSource.Height * scaleBy));
			if (randomPos) {
				Random^ rnd = gcnew Random();
				result.Location = System::Drawing::Point((int)rnd->Next(rDest.Width - result.Width), 
															(int)rnd->Next(rDest.Height - result.Height));
			} else {
				result.Location = System::Drawing::Point((int)(rDest.Left + (rDest.Width - result.Width) / 2), 
															(int)(rDest.Top + (rDest.Height - result.Height) / 2));
			}
			//dest.right = dest.left + destWidth;
			//dest.bottom = dest.top + destHeight;
			return result;
		}
		//*/

		void LoadMonitorImageFromPanorama(TConductor^ %conductor) {
			if (this->id > 0) {
				this->filename = "";
				this->showFilename = "";
				this->rawCachedFilename = "";
			}
			Graphics ^ g = Graphics::FromImage(this->buffers[1]);
			GraphicsUnit units = GraphicsUnit::Pixel;

			//RectangleF r = RectangleF(Point(this->panoX, this->r.Y), this->r.Size);
			//RectangleF r = RectangleF(Point(this->r.X, this->r.Y), this->r.Size);
			RectangleF r = RectangleF(Point(this->panoX, this->r.Y), Size(this->r.Width, this->r.Height));
			//r.Location.X = 300;
			g->DrawImage(conductor->panoramaBuffer, this->buffers[1]->GetBounds(units), r, units);
			DrawToBuffer( grafx->Graphics );
		}

		bool LoadImage(String^ filename, TConductor^ %conductor) {
			//if (filename == "") return false;
			this->showFilename = this->filename = filename;
			this->rawCachedFilename = conductor->checkImageCache(filename);
			if (this->rawCachedFilename == "") {
				conductor->removeImageFromList(filename);
				debugFile(this->id, filename, "No output from RAW conversion");
				return false;
			}

			if ((!config->lviFileInfoBase->Checked) || (!config->lviFileInfoSubFolders->Checked) || (!config->lviFileInfoFilename->Checked) || (!config->lviFileInfoExt->Checked)) {
				array<String^> ^parts = gcnew array<String^>(4);

				array<String^> ^folders = config->tbFolder->Text->Split(';');
				for each(String^ folder in folders) {
					if (this->filename->StartsWith(folder)) {
						parts[0] = folder->Trim('\\') + "\\";
						parts[1] = Path::GetDirectoryName(this->filename->Substring(folder->Length))->Trim('\\') + "\\";
					}
				}				
				parts[2] = Path::GetFileNameWithoutExtension(filename);
				parts[3] = Path::GetExtension(filename);
				this->showFilename = "";
				if (config->lviFileInfoBase->Checked) this->showFilename += parts[0];
				if (config->lviFileInfoSubFolders->Checked) this->showFilename += parts[1];
				if (config->lviFileInfoFilename->Checked) this->showFilename += parts[2];
				if (config->lviFileInfoExt->Checked) this->showFilename += parts[3];
				this->showFilename = this->showFilename->Trim('\\');
			}
				
			if (this->rawCachedFilename != filename) debugFile(this->id, filename + " <" + this->rawCachedFilename + ">");
			else debugFile(this->id, filename);

			try {
				this->backgroundBrush = gcnew SolidBrush(config->btnBackgroundColor->BackColor);

				if (config->cbAnimatedTransitions->Checked) {
					// Copy image to old image buffer
					//this->buffers[0] = (System::Drawing::Bitmap^)this->buffers[0]->Clone();
					//this->buffers[1] = (System::Drawing::Bitmap^)this->buffers[1]->Clone();
				}

				this->metadata = gcnew TMetadata(this->rawCachedFilename);
/*				ArrayList^ orientation = this->metadata->find("Exif.Image.Orientation");
				RotateFlipType rotation = RotateFlipType::RotateNoneFlipNone;
				if (orientation->Count > 0) {
					switch(System::Convert::ToInt32(safe_cast<TMetadataItem^>(orientation[0])->plainValue)) {
						case 1: break;
						case 2: rotation = RotateFlipType::RotateNoneFlipX; break;
						case 3: rotation = RotateFlipType::Rotate180FlipNone; break;
						case 4: rotation = RotateFlipType::Rotate180FlipX; break;
						case 5: rotation = RotateFlipType::Rotate90FlipX; break;
						case 6: rotation = RotateFlipType::Rotate90FlipNone; break;
						case 7: rotation = RotateFlipType::Rotate270FlipX; break;
						case 8: rotation = RotateFlipType::Rotate270FlipNone; break;
					}
				}*/
				RotateFlipType rotation = this->metadata->getExifImageOrientationRFT();

				image = Image::FromFile(this->rawCachedFilename);
				if (config->cbExifRotate->Checked) image->RotateFlip(rotation);
			} catch(OutOfMemoryException ^ex) {
				debugFile(this->id, filename, "Error reading");
				showMessage("Error reading: '" + filename + "'");
				conductor->removeImageFromList(filename);
				return false;
			} catch(FileNotFoundException ^ex) {
				debugFile(this->id, filename, "File not found");
				showMessage("File not found: '" + filename + "'");
				conductor->removeImageFromList(filename);
				return false;
			} catch(Exception ^ex) {
				debugFile(this->id, filename, "Other error: " + ex->Message);
				showMessage("Other error: " + ex->Message + " '" + filename + "'");
				conductor->removeImageFromList(filename);
				return false;
			}
			return true;
		}

		bool SizeImage(TConductor^ %conductor) {
			try {
				GraphicsUnit units = GraphicsUnit::Pixel;

				float imgRatio = (float)image->Width / (float)image->Height;
				RectangleF targetRect;
				conductor->panorama = false;
				if ((config->cbPanoramaStretch->Checked) && (this->id == 0) && (imgRatio >= multiMonitorRatio)) {
					conductor->panorama = true;
					targetRect = RectangleF(0, 0, (float)mmWidth, (float)mmHeight);
				} else {
					targetRect = this->buffers[1]->GetBounds(units);
				}

				RectangleF rect = this->scaleRect(image->GetBounds(units), targetRect, config->cbStretchSmall->Checked, config->cbRandomPositions->Checked);			 

				if (conductor->panorama) {
					Graphics ^ g = Graphics::FromImage(conductor->panoramaBuffer);
					g->FillRectangle( this->backgroundBrush, 0, 0, config->desktop.Width, config->desktop.Height );
					g->DrawImage(image, rect);
					//RectangleF rect = RectangleF(Point(this->panoX, this->r.Y), Size(this->r.Width, this->r.Height));
					LoadMonitorImageFromPanorama(conductor);

				} else {
					Graphics ^ g = Graphics::FromImage(this->buffers[1]);
					g->FillRectangle( this->backgroundBrush, 0, 0, this->r.Width, this->r.Height );
					g->DrawImage(image, rect);
					DrawToBuffer( grafx->Graphics );
				}
			} catch(OutOfMemoryException ^ex) {
				debugFile(this->id, filename, "Error reading");
				showMessage("Error reading: '" + filename + "'");
				conductor->removeImageFromList(filename);
				return false;
			} catch(Exception ^ex) {
				debugFile(this->id, filename, "Other error: " + ex->Message);
				showMessage("Other error: " + ex->Message + " '" + filename + "'");
				conductor->removeImageFromList(filename);
				return false;
			}
			return true;
		}

		bool LoadMonitorImage(String^ filename, TConductor^ %conductor) {
			if (!this->LoadImage(filename, conductor)) return false;
			if (!this->SizeImage(conductor)) return false;

			if (config->cbMetadata) {
				TMetaTemplate^ metaTemplate = gcnew TMetaTemplate(config->tbSimpleMetadata->Text);
				//TMetaTemplate^ metaTemplate = gcnew TMetaTemplate("D:\\abScreensavers.com\\Random Photo Screensaver\\metadata\\default.html", true);
				//this->metadata = gcnew TMetadata(filename);
				this->simpleMetadata = metaTemplate->processMetadata(this->metadata);
			}
			return true;
		}

		bool RotateImage(RotateFlipType angle, TConductor^ %conductor) {
			if (image != nullptr) {
				image->RotateFlip(angle);
				return this->SizeImage(conductor);
			}
			return true;
			//DrawToBuffer( grafx->Graphics );
		}

		void RedrawBuffer() {
			DrawToBuffer( grafx->Graphics );
		}

		void OnResize(System::Drawing::Rectangle rectangle)  {
			this->r = rectangle;
			this->r.Location.X = 0;

			// Re-create the graphics buffer for a new window size.
			context->MaximumBuffer = System::Drawing::Size( this->r.Width + 1, this->r.Height + 1 );
			if ( grafx != nullptr )
			{
				delete grafx;
				grafx = nullptr;
			}

			//grafx = context->Allocate( this->graphics, System::Drawing::Rectangle(this->r.Left, this->r.Top, this->r.Width,this->r.Height) );
			grafx = context->Allocate( this->graphics, System::Drawing::Rectangle(this->r.Left - config->desktop.Left, this->r.Top - config->desktop.Top, this->r.Width,this->r.Height) );
			//grafx = context->Allocate( this->graphics, System::Drawing::Rectangle(0, 0, this->r.Width,this->r.Height) );
			for (int i=0; i < buffers->Length; i++) {
			 if (buffers[i] != nullptr) {
				 delete buffers[i];
				 buffers[i] = nullptr;
			 }
			 buffers[i] = gcnew Bitmap(this->r.Width, this->r.Height);//this->CreateGraphics();

			 Graphics ^ g = Graphics::FromImage(buffers[i]);
			 g->FillRectangle( this->backgroundBrush, 0, 0, this->r.Width, this->r.Height );
			}
			int h = this->r.Height;

			previewScale = (float)this->r.Height / (float)1024;
			if (previewScale == 0) previewScale = 1;

			DrawToBuffer( grafx->Graphics );
		}

		void DisplayCounts(int fileCount, int dirCount) {
			this->fileCount = fileCount;
			this->dirCount = dirCount;
		}
		
		void WriteText(Graphics^ g, String^ text, Drawing::Rectangle rect, TTextPosition^ position, Font^ font, Color color, bool shadow) {
			WriteText(g, text, rect, position, font, color, shadow, Point(0, 0));
		}

		void WriteText(Graphics^ g, String^ text, Drawing::Rectangle rect, TTextPosition^ position, Font^ font, Color color, bool shadow, Point^ base) {
			int x = 0, y = 0;
			if (config->action == saPreview) {
				setFontSize(font, font->Size * previewScale);
			}
			int shadowOffset = (abs((int)font->Size) / 40) + 1;
			TextFormatFlags tfFlags = TextFormatFlags::Default;
			SolidBrush^ sb = gcnew SolidBrush(color);
			Size size = TextRenderer::MeasureText(g, text, font);

			Random^ rnd = gcnew Random();

			switch(position->getPositionX()) { // Horizontal
				case TTextPosition::THorizontal::tpXMiddle: {
					tfFlags = tfFlags | TextFormatFlags::HorizontalCenter;
					x = rect.Width;
				} break;
				case TTextPosition::THorizontal::tpXRight: {
					// For some reason right align makes text disappear.
					//tfFlags = tfFlags | TextFormatFlags::Right;
					x = rect.Width - size.Width - shadowOffset;
				} break;
				case TTextPosition::THorizontal::tpXRandom:
					x = rnd->Next(rect.Width - size.Width - shadowOffset);
				break;
			}
			switch(position->getPositionY()) { // Vertical
				case TTextPosition::TVertical::tpYTop: {
				} break;
				case TTextPosition::TVertical::tpYMiddle: {
					y = (rect.Height - size.Height) / 2;				
				} break;
				case TTextPosition::TVertical::tpYBottom: {
					y = rect.Height - size.Height - shadowOffset;
				} break;
				case TTextPosition::TVertical::tpYRandom:
					y = rnd->Next(rect.Height - size.Height - shadowOffset);
				break;
			}
			if (shadow) TextRenderer::DrawText(g, text, font, Point(base->X+x+shadowOffset, base->Y+y+shadowOffset), config->adjustToBrightness(color), tfFlags);
			TextRenderer::DrawText(g, text, font, Point(base->X+x, base->Y+y), color, tfFlags);
		}
	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;
//		void WriteText(Graphics^ g, String^ text, TTextPosition position, Font font, Color color) {
			//WriteText(g, text, position, font, color, true);
		//}

		void DrawToBuffer( Graphics^ g ) {
			int baseX = this->r.Left - config->desktop.Left;
			int baseY = this->r.Top - config->desktop.Top;

			if (config->action == saPreview) {
				g = Graphics::FromImage(bPreview);
				g->DrawImageUnscaled(this->buffers[1], Point(0,0));
			}

			if (config->cbAnimatedTransitions->Checked) {
				if (skipAnim) 
					g->DrawImage(this->buffers[1], Point(baseX, baseY));
				else 
					g->DrawImage(this->buffers[0], Point(baseX, baseY));
			} else {
				g->DrawImage(this->buffers[1], Point(baseX, baseY));
			}

/*			Pen^ rp = gcnew Pen( Color::Red,10.0f );
			g->DrawLine( rp, Point(0, 0), Point(2720, 1024) );
			g->DrawLine( rp, Point(this->r.Left, 0), Point(this->r.Left+this->r.Width, this->r.Height) );
			g->DrawLine( rp, Point(this->r.Width, 0), Point(0, this->r.Height) );
*/
			if (this->filename == nullptr) {
				WriteText(g, backgroundText, this->r, gcnew TTextPosition(TTextPosition::THorizontal::tpXMiddle, TTextPosition::TVertical::tpYMiddle), gcnew System::Drawing::Font( "Arial", 32 ), Color::White, true);
			} 
			DateTime dt = DateTime::Now;

			if (DateTime::Compare(dt, identifyTimeout) < 0) WriteText(g, (this->id+1).ToString(), this->r, gcnew TTextPosition(TTextPosition::THorizontal::tpXMiddle, TTextPosition::TVertical::tpYMiddle), gcnew System::Drawing::Font( "Arial", float(int(this->r.Height) / 1.75) ), Color::White, true);

			if (config->cbDisplayFilenames->Checked) {
				WriteText(g, this->showFilename, this->r, gcnew TTextPosition(TTextPosition::THorizontal::tpXLeft, TTextPosition::TVertical::tpYTop), config->filenameFont, config->btnFilenamefont->ForeColor, true);
			}

			if (config->cbMetadata->Checked && (this->simpleMetadata != "")) {
				WriteText(g, this->simpleMetadata, this->r, gcnew TTextPosition(config->cbMetaDataHorz->Text, config->cbMetaDataVert->Text), config->btnMetadataFont->Font, config->btnMetadataFont->ForeColor, true);
			}

			if (config->clockType[id] != ctNone) {
				String^ t;
				if (config->clockType[id] == ctCurrent) {
					t = dt.ToString(config->tbClockMask->Text);
				} else {
					TimeSpan ts = dt.Subtract(config->dtStart);
					t = ts.ToString();
					int p = t->LastIndexOf(".");
					if (p > 0) t = t->Substring(0, p); // Bug fix if time is 0.
				}
				WriteText(g, t, this->r, gcnew TTextPosition((int)config->clockPosHorz[this->id], (int)config->clockPosVert[this->id]), config->clockFont[id], config->clockColor[id], true);
			}

			if (DateTime::Compare(dt, messageTimeout) < 0) WriteText(g, this->messageText, this->r, this->messagePosition, gcnew System::Drawing::Font( "Arial", 18 ), Color::White, true);

			if ((!config->cbFolderInfo->Checked) || ((config->cbFolderInfo->Checked) && (DateTime::Compare(dt, infoTimeout) < 0))) {				
				String^ sFileCount = "",^ sDirCount = "";
				if (this->fileCount != 1) sFileCount = "s";
				if (this->dirCount != 1) sDirCount = "s";
				WriteText(g, "Found " + this->fileCount.ToString("N0") + " image" + sFileCount + " in " + this->dirCount.ToString("N0") + " folder" + sDirCount + ".", this->r, gcnew TTextPosition(TTextPosition::THorizontal::tpXLeft, TTextPosition::TVertical::tpYBottom), gcnew System::Drawing::Font( "Arial", 8 ), Color::White, true);
			}
			if (config->action == saPreview) {
				try {
					g = Graphics::FromHwnd(hdcPreview);
					g->DrawImageUnscaled(bPreview, Point(0,0));
				} catch(OutOfMemoryException ^ex) {
					Application::Exit();
				}
			}
		}
	};
}











