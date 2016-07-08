#include <GL/freeglut.h>
#include "camera.h"
#include <math.h>

const static float STEP_SCALE = 0.05f;
const static int MARGIN	= 10;

Camera::Camera( int _windowWidth, int _windowHeight ):
	pos			( Vector3f( 0.f, 0.f, 0.f ) ),
	target		( Vector3f( 0.f, 0.f, 1.f ) ),
	up			( Vector3f( 0.f, 1.f, 0.f ) ),
	windowWidth ( _windowWidth ),
	windowHeight( _windowHeight )
{
	Init();
}

Camera::Camera( int _windowWidth, int _windowHeight, const Vector3f& Pos, const Vector3f& Target, const Vector3f& Up ):
	pos			( Pos ),
	target		( Target ),
	up			( Up ),
	windowWidth	( _windowWidth ),
	windowHeight( _windowHeight )
{
	Init();
}

bool Camera::OnKeyboard( int Key )
{
	bool ret = false;

	switch ( Key )
	{
		case GLUT_KEY_UP:
		{
			pos += ( target * STEP_SCALE );
			ret	= true;
		} break;

		case GLUT_KEY_DOWN:
		{
			pos -= ( target * STEP_SCALE );
			ret = true;
		} break;

		case GLUT_KEY_LEFT:
		{
			Vector3f left = target.Cross( up );
			left.Normalize();
			left *= STEP_SCALE;
			pos += left;
			ret = true;
		} break;

		case GLUT_KEY_RIGHT:
		{
			Vector3f right = up.Cross(target);
			right.Normalize();
			right *= STEP_SCALE;
			pos += right;
			ret = true;
		} break;
	}

	return ret;
}

void Camera::OnMouse( int x, int y )
{
	if ( x == mousePos.x && y == mousePos.y ) 
		return;

	const int DeltaX = x - mousePos.x;
	const int DeltaY = y - mousePos.y;

	angleH += static_cast<float>( DeltaX ) / 20.0f;
	angleV += static_cast<float>( DeltaY ) / 20.0f;

	Update();
	glutWarpPointer( mousePos.x, mousePos.y);
}

void Camera::OnRender()
{
}

void Camera::Init()
{
	target.Normalize();
	up.Normalize();

	Vector3f HTarget( target.x, 0.f, target.z );
	HTarget.Normalize();

	if ( HTarget.z >= 0.f )
	{
		if ( HTarget.x >= 0.f )
		{
			angleH = 360.f - ToDegree( asin( HTarget.z ) );
		}
		else
		{
			angleH = 180.f + ToDegree( asin( HTarget.z ) );
		}
	}
	else
	{
		if ( HTarget.x >= 0.f )
		{
			angleH = ToDegree( asin( -HTarget.z ) );
		}
		else
		{
			angleH = 90.f + ToDegree( asin( -HTarget.z ) );
		}
	}

	angleV = -ToDegree( asin( target.y ) );

	mousePos.x	= windowWidth / 2;
	mousePos.y	= windowHeight / 2;

	glutWarpPointer( mousePos.x, mousePos.y);
}

void Camera::Update()
{
	const Vector3f Vaxis( 0.0f, 1.0f, 0.0f );

	// Rotate the view vector by the horizontal angle around the vertical axis
	Vector3f View( 1.0f, 0.0f, 0.0f );
	View.Rotate( angleH, Vaxis );
	View.Normalize();

	// Rotate the view vector by the vertical angle around the horizontal axis
	Vector3f Haxis = Vaxis.Cross( View );
	Haxis.Normalize();
	View.Rotate( angleV, Haxis );

	target = View;
	target.Normalize();

	up = target.Cross( Haxis );
	up.Normalize();
}