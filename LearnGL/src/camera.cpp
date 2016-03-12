#include <GL/glut.h>
#include "camera.h"
#include <math.h>

const static float StepScale = 0.05f;
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
			pos += ( target * StepScale );
			ret	= true;
		} break;

		case GLUT_KEY_DOWN:
		{
			pos -= ( target * StepScale );
			ret = true;
		} break;

		case GLUT_KEY_LEFT:
		{
			Vector3f left = target.Cross( up );
			left.Normalize();
			left *= StepScale;
			pos += left;
			ret = true;
		} break;

		case GLUT_KEY_RIGHT:
		{
			Vector3f right = up.Cross(target);
			right.Normalize();
			right *= StepScale;
			pos += right;
			ret = true;
		} break;
	}

	return ret;
}

void Camera::OnMouse( int x, int y )
{
	const int DeltaX = x - mousePos.x;
	const int DeltaY = y - mousePos.y;

	mousePos.x = x;
	mousePos.y = y;

	angleH += static_cast<float>( DeltaX ) / 20.0f;
	angleV += static_cast<float>( DeltaY ) / 20.0f;

	if ( DeltaX == 0 )
	{
		if ( x <= MARGIN ) 
		{
			OnLeftEdge = true;
		}
		else if ( x >= windowWidth - MARGIN ) 
		{
			OnRightEdge = true;
		}
	}
	else 
	{
		OnLeftEdge	= false;
		OnRightEdge = false;
	}

	if ( DeltaY == 0 ) 
	{
		if ( y <= MARGIN )
		{
			OnUpperEdge = true;
		}
		else if ( y >= windowHeight - MARGIN ) 
		{
			OnLowerEdge = true;
		}
	}
	else
	{
		OnUpperEdge = false;
		OnLowerEdge = false;
	}

	Update();
}

void Camera::OnRender()
{
	bool ShouldUpdate = false;

	if ( OnLeftEdge ) 
	{
		angleH -= 0.1f;
		ShouldUpdate = true;
	}
	else if ( OnRightEdge) 
	{
		angleH += 0.1f;
		ShouldUpdate = true;
	}

	if ( OnUpperEdge ) 
	{
		if ( angleV > -90.0f ) 
		{
			angleV -= 0.1f;
			ShouldUpdate = true;
		}
	}
	else if ( OnLowerEdge )
	{
		if ( angleV < 90.0f ) 
		{
			angleV += 0.1f;
			ShouldUpdate = true;
		}
	}

	if ( ShouldUpdate ) 
	{
		Update();
	}
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

	OnUpperEdge	= false;
	OnLowerEdge = false;
	OnLeftEdge	= false;
	OnRightEdge = false;
	mousePos.x	= windowWidth / 2;
	mousePos.y	= windowHeight / 2;

	glutWarpPointer( mousePos.x, mousePos.y);
}

void Camera::Update()
{
	const Vector3f Vaxis(0.0f, 1.0f, 0.0f);

	// Rotate the view vector by the horizontal angle around the vertical axis
	Vector3f View( 1.0f, 0.0f, 0.0f );
	View.Rotate( angleH, Vaxis );
	View.Normalize();

	// Rotate the view vector by the vertical angle around the horizontal axis
	Vector3f Haxis = Vaxis.Cross( View );
	Haxis.Normalize();
	View.Rotate( angleV, Haxis );
	View.Normalize();

	target = View;
	target.Normalize();

	up = target.Cross( Haxis );
	up.Normalize();
}