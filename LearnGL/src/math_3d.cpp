#include "math_3d.h"
#include  <math.h>

Vector2f::Vector2f():
	x ( 0.f ),
	y ( 0.f )
{
}

Vector2f::Vector2f( float _x, float _y ):
	x ( _x ),
	y ( _y )
{
}

Vector3f::Vector3f():
	x ( 0.f ),
	y ( 0.f ),
	z ( 0.f )
{
}

Vector3f::Vector3f( float _x, float _y, float _z ):
	x ( _x ),
	y ( _y ),
	z ( _z )
{
}

Vector3f& Vector3f::operator+=( const Vector3f& r )
{
	x += r.x;
	y += r.y;
	z += r.z;

	return *this;
}

Vector3f& Vector3f::operator-=( const Vector3f& r )
{
	x -= r.x;
	y -= r.y;
	z -= r.z;

	return *this;
}

Vector3f& Vector3f::operator*=( float f )
{
	x *= f;
	y *= f;
	z *= f;

	return *this;
}

Vector3f Vector3f::Cross( const Vector3f& v ) const
{
	const float _x = y * v.z - z * v.y;
	const float _y = z * v.x - x * v.z;
	const float _z = x * v.y - y * v.x;

	return Vector3f( _x, _y, _z );
}

Vector3f& Vector3f::Normalize()
{
	const float Length = sqrtf( x * x + y * y + z * z );

	x /= Length;
	y /= Length;
	z /= Length;

	return *this;
}

void Vector3f::Rotate( float Angle, const Vector3f& Axis )
{
	const float SinHalfAngle = sinf( ToRadian( Angle / 2 ) );
	const float CosHalfAngle = cosf( ToRadian( Angle / 2 ) );

	const float Rx = Axis.x * SinHalfAngle;
	const float Ry = Axis.y * SinHalfAngle;
	const float Rz = Axis.z * SinHalfAngle;
	const float Rw = CosHalfAngle;
	Quaternion RotationQ( Rx, Ry, Rz, Rw );

	Quaternion ConjugateQ = RotationQ.Conjugate();
	//  ConjugateQ.Normalize();
	Quaternion W = RotationQ * (*this) * ConjugateQ;

	x = W.x;
	y = W.y;
	z = W.z;
}

void Matrix4f::InitIdentity()
{
	m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
	m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
	m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
	m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
}

Matrix4f Matrix4f::operator*(const Matrix4f& Right) const
{
	Matrix4f Ret;

	for ( unsigned int i = 0; i < 4; i++ )
	{
		for ( unsigned int j = 0; j < 4; j++ )
		{
			Ret.m[i][j] =	m[i][0] * Right.m[0][j] +
							m[i][1] * Right.m[1][j] +
							m[i][2] * Right.m[2][j] +
							m[i][3] * Right.m[3][j];
		}
	}

	return Ret;
}

void Matrix4f::InitScaleTransform( float ScaleX, float ScaleY, float ScaleZ )
{
	m[0][0] = ScaleX; m[0][1] = 0.0f;   m[0][2] = 0.0f;   m[0][3] = 0.0f;
	m[1][0] = 0.0f;   m[1][1] = ScaleY; m[1][2] = 0.0f;   m[1][3] = 0.0f;
	m[2][0] = 0.0f;   m[2][1] = 0.0f;   m[2][2] = ScaleZ; m[2][3] = 0.0f;
	m[3][0] = 0.0f;   m[3][1] = 0.0f;   m[3][2] = 0.0f;   m[3][3] = 1.0f;
}

void Matrix4f::InitScaleTransform( const Vector3f& scale )
{
	InitScaleTransform( scale.x, scale.y, scale.z );
}

void Matrix4f::InitRotateTransform( float RotateX, float RotateY, float RotateZ )
{
	Matrix4f rx, ry, rz;

	const float x = ToRadian( RotateX );
	const float y = ToRadian( RotateY );
	const float z = ToRadian( RotateZ );

	rx.m[0][0] = 1.0f; rx.m[0][1] = 0.0f;		rx.m[0][2] = 0.0f;		rx.m[0][3] = 0.0f;
	rx.m[1][0] = 0.0f; rx.m[1][1] = cosf(x);	rx.m[1][2] = -sinf(x);	rx.m[1][3] = 0.0f;
	rx.m[2][0] = 0.0f; rx.m[2][1] = sinf(x);	rx.m[2][2] = cosf(x);	rx.m[2][3] = 0.0f;
	rx.m[3][0] = 0.0f; rx.m[3][1] = 0.0f;		rx.m[3][2] = 0.0f;		rx.m[3][3] = 1.0f;

	ry.m[0][0] = cosf(y);	ry.m[0][1] = 0.0f; ry.m[0][2] = -sinf(y);	ry.m[0][3] = 0.0f;
	ry.m[1][0] = 0.0f;		ry.m[1][1] = 1.0f; ry.m[1][2] = 0.0f;		ry.m[1][3] = 0.0f;
	ry.m[2][0] = sinf(y);	ry.m[2][1] = 0.0f; ry.m[2][2] = cosf(y);	ry.m[2][3] = 0.0f;
	ry.m[3][0] = 0.0f;		ry.m[3][1] = 0.0f; ry.m[3][2] = 0.0f;		ry.m[3][3] = 1.0f;

	rz.m[0][0] = cosf(z);	rz.m[0][1] = -sinf(z);	rz.m[0][2] = 0.0f; rz.m[0][3] = 0.0f;
	rz.m[1][0] = sinf(z);	rz.m[1][1] = cosf(z);	rz.m[1][2] = 0.0f; rz.m[1][3] = 0.0f;
	rz.m[2][0] = 0.0f;		rz.m[2][1] = 0.0f;		rz.m[2][2] = 1.0f; rz.m[2][3] = 0.0f;
	rz.m[3][0] = 0.0f;		rz.m[3][1] = 0.0f;		rz.m[3][2] = 0.0f; rz.m[3][3] = 1.0f;

	*this = rz * ry * rx;
}

void Matrix4f::InitRotateTransform( const Vector3f& rotate )
{
	InitRotateTransform( rotate.x, rotate.y, rotate.z );
}

void Matrix4f::InitTranslationTransform(float x, float y, float z)
{
	m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = x;
	m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = y;
	m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = z;
	m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
}

void Matrix4f::InitTranslationTransform( const Vector3f& transform )
{
	InitTranslationTransform( transform.x, transform.y, transform.z );
}

void Matrix4f::InitCameraTransform( const Vector3f& Target, const Vector3f& Up )
{
	Vector3f N = Target;
	Vector3f U = Up;

	N.Normalize();
	U.Normalize();

	U = U.Cross( N );

	Vector3f V = N.Cross( U );

	m[0][0] = U.x;   m[0][1] = U.y;   m[0][2] = U.z;   m[0][3] = 0.0f;
	m[1][0] = V.x;   m[1][1] = V.y;   m[1][2] = V.z;   m[1][3] = 0.0f;
	m[2][0] = N.x;   m[2][1] = N.y;   m[2][2] = N.z;   m[2][3] = 0.0f;
	m[3][0] = 0.0f;  m[3][1] = 0.0f;  m[3][2] = 0.0f;  m[3][3] = 1.0f;
}

void Matrix4f::InitPersProjTransform( float FOV, float Width, float Height, float zNear, float zFar )
{
	const float ar			= Width / Height;
	const float zRange		= zNear - zFar;
	const float tanHalfFOV	= tanf( ToRadian( FOV / 2.0f ) );

	m[0][0] = 1.0f / ( tanHalfFOV * ar ); 
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = 0.0;

	m[1][0] = 0.0f;
	m[1][1] = 1.0f / tanHalfFOV;
	m[1][2] = 0.0f;
	m[1][3] = 0.0;
	
	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = ( -zNear - zFar ) / zRange;
	m[2][3] = 2.0f * zFar*zNear / zRange;

	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = 1.0f;
	m[3][3] = 0.0;
}

Quaternion::Quaternion( float _x, float _y, float _z, float _w ):
	x ( _x ),
	y ( _y ),
	z ( _z ),
	w ( _w )
{
}

void Quaternion::Normalize()
{
	float Length = sqrtf( x * x + y * y + z * z + w * w );

	x /= Length;
	y /= Length;
	z /= Length;
	w /= Length;
}

Quaternion Quaternion::Conjugate()
{
	Quaternion ret( -x, -y, -z, w );
	return ret;
}

Quaternion operator*( const Quaternion& l, const Quaternion& r )
{
	const float w = ( l.w * r.w ) - ( l.x * r.x ) - ( l.y * r.y ) - ( l.z * r.z );
	const float x = ( l.x * r.w ) + ( l.w * r.x ) + ( l.y * r.z ) - ( l.z * r.y );
	const float y = ( l.y * r.w ) + ( l.w * r.y ) + ( l.z * r.x ) - ( l.x * r.z );
	const float z = ( l.z * r.w ) + ( l.w * r.z ) + ( l.x * r.y ) - ( l.y * r.x );

	Quaternion ret(x, y, z, w);

	return ret;
}

Quaternion operator*( const Quaternion& q, const Vector3f& v )
{
	const float w =-( q.x * v.x ) - ( q.y * v.y ) - ( q.z * v.z );
	const float x = ( q.w * v.x ) + ( q.y * v.z ) - ( q.z * v.y );
	const float y = ( q.w * v.y ) + ( q.z * v.x ) - ( q.x * v.z );
	const float z = ( q.w * v.z ) + ( q.x * v.y ) - ( q.y * v.x );

	Quaternion ret(x, y, z, w);

	return ret;
}