#pragma once

using namespace System;

namespace nsRandomPhotoScreensaver {
public ref class TTextPosition
{
public:
	enum class THorizontal { tpXNone, tpXLeft, tpXMiddle, tpXRight, tpXRandom };
	enum class TVertical { tpYNone, tpYTop, tpYMiddle, tpYBottom, tpYRandom };

private: THorizontal x;
private: TVertical y;

public:
	TTextPosition(String^ position);
	TTextPosition(THorizontal x, TVertical y);
	TTextPosition(int x, int y);
	TTextPosition(String^ x, String^ y);
	TTextPosition::~TTextPosition();

	String^ getTextPositionX();
	String^ getTextPositionY();
	TTextPosition::THorizontal getPositionX();
	TTextPosition::TVertical getPositionY();
	String^ getTetPositionXY();
	void setPositionX(THorizontal x);
	void setPositionY(TVertical y);
	void setPositionXY(THorizontal x, TVertical y);

	void TTextPosition::setPositionX(int x);
	void TTextPosition::setPositionY(int x);
	void TTextPosition::setPositionXY(int x, int y);

	void setTextPositionX(String^ x);
	void setTextPositionY(String^ y);
	void setTextPositionXY(String^ xy);
};
}