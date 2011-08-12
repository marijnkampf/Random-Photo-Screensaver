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
#include "TConductor.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Globalization;
using namespace System::Drawing::Imaging;
using namespace System::Text;

extern std::vector<TMultiMonitor*> multiMonitors;

namespace nsRandomPhotoScreensaver {
		const int atNone = -1;
		const int atRandom = 0;
		const int atSweep = 1;
		const int atBlinds = 2;
		const int atFromCorners = 3;
		const int atRandomBits = 4;
		const int atWindmill = 5;
		const int atSnake = 6;
		const int nrTypes = 6;


	public ref class TAnimator {
			array<int, 3> ^grid; // monitor, x, y
			int xsize;
			int ysize;
			int xmax;
			int ymax;
			int xmin;
			int ymin;
			int frameNr;
			int maxFrames;
			int animType;
			int direction;

	public: 
		TAnimator(int _animType) : animType(_animType) {
				int frameCount;
				int rndDirX, rndDirY, rndDir4;

				frameNr = 0;
				if (animType == 0) animType = (rand() % nrTypes) + 1;
				//animType = atWindmill;
				switch(animType) {
					case atNone: {
						xsize = 1;
						ysize = 1;
					} break;
					case atSweep: case atSnake: {
						xsize = 8;
						ysize = 5;
					} break;
					case atBlinds: {
						xsize = 16;
						ysize = 16;
					} break;
					case atFromCorners: {
						xsize = 20;
						ysize = 16;
					} break;
					case atRandomBits: {
						frameCount = 35;
						xsize = 40;
						ysize = 32;
					} break;
					case atWindmill: {
						xsize = 10;
						ysize = 8;
					} break;
					default: {
						xsize = 10;
						ysize = 8;
					};
				}

				int c, tcount = xsize*ysize;
				frameNr = 0;
				grid = gcnew array<int, 3>(multiMonitors.size(),xsize,ysize);
				for(unsigned int m = 0; m < multiMonitors.size(); m++) {
					if (rand() % 2 == 0) rndDirX = -1; else rndDirX = 1;
					if (rand() % 2 == 0) rndDirY = -1; else rndDirY = 1;
					rndDir4 = (rand() % 4)+1;
					// Used by windmill
					if (rand() % 2 == 0) direction = 0;
					else direction = xsize*ysize;

					c = 0;
					//grid.push_back(vector <vector <int> >());
					for(int x = 0; x < xsize; x++) {
						//grid[m].push_back(vector <int>());
						for(int y = 0; y < ysize; y++) {
							//grid[m][x].push_back(0);

							switch(animType) {
								case atFromCorners:{
									switch(rndDir4) {
										case 1: grid[m,x,y] = y + x; break;
										case 2: grid[m,x,y] = (ysize - y) + (xsize - x); break;
										case 3: grid[m,x,y] = y + (xsize - x); break;
										case 4: grid[m,x,y] = (ysize - y) + x; break;
									}
								} break;
								case atSweep: {
									switch(rndDir4) {
										case 1: grid[m,x,y] = c++; break;
										case 2: grid[m,x,y] = tcount - c++; break;
										case 3: grid[m,x,y] = tcount - (x+(y*xsize)); break;
										case 4: grid[m,x,y] = x+(y*xsize); break;
									}
								} break;
								case atSnake: {
									c++;
									switch(rndDir4) {
										case 1:
											grid[m,x,y] = ysize*x+y;
											if ((x%2)==0) grid[m,x,y] = ysize*x+ysize-y-1;
										break;
										case 2:
											grid[m,x,y] = tcount-ysize*x+y;
											if ((x%2)==0) grid[m,x,y] = tcount-ysize*x+ysize-y-1;
										break;
										case 3:
											grid[m,x,y] = tcount-xsize*y+x;
											if ((y%2)==0) grid[m,x,y] = tcount-xsize*y+xsize-x-1;
										break;
										case 4:
											grid[m,x,y] = xsize*y+x;
											if ((y%2)==0) grid[m,x,y] = xsize*y+xsize-x-1;
										break;
									}
								} break;
								case atRandomBits: {
										grid[m,x,y] = (rand() % (frameCount));
								} break;
								case atBlinds: {
									switch(rndDir4) {
										case 1: grid[m,x,y] = xsize - x -1; break;
										case 2: grid[m,x,y] = x; break;
										case 3: grid[m,x,y] = ysize - y -1; break;
										case 4: grid[m,x,y] = y; break;
									}
								} break;
							}
							if (grid[m,x,y] > maxFrames) maxFrames = grid[m,x,y];
						}
					}
					switch(animType) {
						case atWindmill: {
							int base=1;
							xmax = xsize;
							ymax = ysize;
							xmin = 0;
							ymin =0;
							maxFrames = 0;
							frameNr = 0;
							int xtsize, ytsize;

							while((xmin < xmax) && (ymin < ymax)) {
								xtsize = xmax - xmin;
								ytsize = ymax - ymin;

								for(int x = xmin; x < xmax; x++) {
									grid[m,x,ymin] = abs(direction-(base+x-xmin));
									grid[m,x,ymax-1] = abs(direction-(base+xtsize+ytsize+xmax-x-3));
								}
								for(int y = ymin+1; y < ymax-1; y++) {
									grid[m,xmin,y] = abs(direction-(base+2*xtsize+ytsize+ymax-y-4));
									grid[m,xmax-1,y] = abs(direction-(base+y-ymin+xtsize-1));
								}
								base = base+2*(xtsize-1+ytsize-1);
								xmin++;
								ymin++;
								if (xmax > xmin) xmax--;
								if (ymax > ymin) ymax--;
							}
							maxFrames = xsize*ysize;
						} break;
					}
				}
				maxFrames++;
			}

			int animateFrame(array<TSaverMonitor^>^% saverMonitors) {
				int wSquare, hSquare, cx, cy;
				for(int m = 0; m < saverMonitors->Length; m++) {
					//saverMonitors[m]->buffers[0] = (System::Drawing::Bitmap^)saverMonitors[m]->buffers[1]->Clone();
					Graphics ^ g = Graphics::FromImage(saverMonitors[m]->buffers[0]);
					g->SmoothingMode = Drawing2D::SmoothingMode::None;
					//saverMonitors[m]->buffers[1]-
					//g->InterpoliationMode = Drawing2D::InterpoliationMode::Low;
					if (frameNr == maxFrames) {
						g->DrawImage(saverMonitors[m]->buffers[1], 0, 0);
					} else {
						wSquare = saverMonitors[m]->r.Width / xsize;
						hSquare = saverMonitors[m]->r.Height / ysize;
						for(int x = 0; x < xsize; x++) {
							for(int y = 0; y < ysize; y++) {
								//saverMonitors[m]->showMessage(xsize.ToString() + "x" + ysize.ToString() + ";" +frameNr.ToString());
								if (grid[m,x,y] == frameNr) {
									cx = x*wSquare;
									cy = y*hSquare;
									g->DrawImage(saverMonitors[m]->buffers[1], cx, cy, Drawing::Rectangle(Point(cx,cy),Size(wSquare,hSquare)), GraphicsUnit::Pixel);
									//StretchBlt(saverMonitors[m]->animdc,cx,cy,wSquare,hSquare,saverMonitors[m]->imgdc,cx,cy,wSquare,hSquare,SRCCOPY);
								}
							}
						}
					}
				}
				frameNr++;
				if (frameNr > maxFrames) return false;
				else return true;
			}

			void end() {
				frameNr = maxFrames;
			}

			bool ended() {
				return (frameNr > maxFrames);
			}
		};
}











