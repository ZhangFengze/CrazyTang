#pragma once
#include "Math.h"

class Camera
{
public:
	Camera();
	void SetRect(const Rect&);
	Rect GetRect()const;

private:
	Rect m_Rect;
};
