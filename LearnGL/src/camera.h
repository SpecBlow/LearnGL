#pragma once
#ifndef CAMERA_H
#define	CAMERA_H

#include "math_3d.h"

class Camera
{
public:
						Camera( int windowWidth, int windowHeight );
						Camera( int windowWidth, int windowHeight, const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up );

	bool				OnKeyboard( int Key );
	void				OnMouse( int x, int y );
	void				OnRender();

	const Vector3f&		GetPos()	const { return pos;  }
	const Vector3f&		GetTarget() const { return target; }
	const Vector3f&		GetUp()		const { return up; }

private:

	void				Init();
	void				Update();

	Vector3f			pos;
	Vector3f			target;
	Vector3f			up;

	int					windowWidth;
	int					windowHeight;

	float				angleH;
	float				angleV;

	bool				OnUpperEdge;
	bool				OnLowerEdge;
	bool				OnLeftEdge;
	bool				OnRightEdge;

	Vector2i			mousePos;
};

#endif	/* CAMERA_H */