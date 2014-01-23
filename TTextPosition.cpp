#include "stdafx.h"
#include "TTextPosition.h"

namespace nsRandomPhotoScreensaver {

TTextPosition::TTextPosition(String^ position)	{
	this->setTextPositionXY(position);
}
TTextPosition::TTextPosition(THorizontal x, TVertical y)	{
	this->setPositionXY(x, y);
}

TTextPosition::TTextPosition(int x, int y)	{
	this->setPositionXY(x, y);
}

TTextPosition::TTextPosition(String^ x, String^ y) {
	this->setTextPositionX(x);
	this->setTextPositionY(y);
}


TTextPosition::~TTextPosition() { }

String^ TTextPosition::getTextPositionX() {
	switch(this->x) {
		case THorizontal::tpXNone:	  return "None"; break;
		case THorizontal::tpXLeft:	  return "Left"; break;
		case THorizontal::tpXMiddle: return "Middle"; break;
		case THorizontal::tpXRight:  return "Right"; break;
		case THorizontal::tpXRandom: return "Random"; break;
	};
	return "None";
}
String^ TTextPosition::getTextPositionY() {
	switch(this->y) {
		case TVertical::tpYNone:	 return "None"; break;
		case TVertical::tpYTop:		 return "Top"; break;
		case TVertical::tpYMiddle: return "Middle"; break;
		case TVertical::tpYBottom: return "Bottom"; break;
		case TVertical::tpYRandom: return "Random"; break;
	};
	return "None";
}

TTextPosition::THorizontal TTextPosition::getPositionX() {
	return this->x;
}
TTextPosition::TVertical TTextPosition::getPositionY() {
	return this->y;
}
String^ TTextPosition::getTetPositionXY() {
	return this->getTextPositionX() + " " + this->getTextPositionY() ;
}
void TTextPosition::setPositionX(THorizontal x) {
	this->x = x;
}
void TTextPosition::setPositionY(TVertical y) {
	this->y = y;
}
void TTextPosition::setPositionXY(THorizontal x, TVertical y) {
	this->x = x;
	this->y = y;
}
void TTextPosition::setPositionX(int x) {
	switch(x) {
		case -1:this->x = THorizontal::tpXNone; break;
		case 0: this->x = THorizontal::tpXLeft; break;
		case 1: this->x = THorizontal::tpXMiddle; break;
		case 2: this->x = THorizontal::tpXRight; break;
		case 3: this->x = THorizontal::tpXRandom; break;
	}
}
void TTextPosition::setPositionY(int x) {
	switch(x) {
		case -1:this->y = TVertical::tpYNone; break;
		case 0: this->y = TVertical::tpYTop; break;
		case 1: this->y = TVertical::tpYMiddle; break;
		case 2: this->y = TVertical::tpYBottom; break;
		case 3: this->y = TVertical::tpYRandom; break;
	}
}
void TTextPosition::setPositionXY(int x, int y) {
	this->setPositionX(x);
	this->setPositionY(y);
}

void TTextPosition::setTextPositionX(String^ x) {
	if (String::Compare(x, "Left", StringComparison::InvariantCultureIgnoreCase) == 0) this->x = THorizontal::tpXLeft;
	else if (String::Compare(x, "Middle", StringComparison::InvariantCultureIgnoreCase) == 0) this->x = THorizontal::tpXMiddle;
	else if (String::Compare(x, "Right", StringComparison::InvariantCultureIgnoreCase) == 0) this->x = THorizontal::tpXRight;
	else if (String::Compare(x, "Random", StringComparison::InvariantCultureIgnoreCase) == 0) this->x = THorizontal::tpXRandom;
	else this->x = THorizontal::tpXNone;
}

void TTextPosition::setTextPositionY(String^ y) {
	if (String::Compare(y, "Top", StringComparison::InvariantCultureIgnoreCase) == 0) this->y = TVertical::tpYTop;
	else if (String::Compare(y, "Middle", StringComparison::InvariantCultureIgnoreCase) == 0) this->y = TVertical::tpYMiddle;
	else if (String::Compare(y, "Bottom", StringComparison::InvariantCultureIgnoreCase) == 0) this->y = TVertical::tpYBottom;
	else if (String::Compare(y, "Random", StringComparison::InvariantCultureIgnoreCase) == 0) this->y = TVertical::tpYRandom;
	else this->y = TVertical::tpYNone;
}

void TTextPosition::setTextPositionXY(String^ xy) {
	xy = xy->Trim();
	int p = xy->IndexOf(" ");
	this->setTextPositionX(xy->Substring(0, p));
	this->setTextPositionY(xy->Substring(p, xy->Length - p));
}		
}