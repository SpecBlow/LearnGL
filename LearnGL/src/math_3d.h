#pragma once
#ifndef MATH_3D_H
#define	MATH_3D_H
#define _USE_MATH_DEFINES

#define ToRadian( x ) (float)( ( x ) * M_PI / 180.0f )
#define ToDegree( x ) (float)( ( x ) * 180.0f / M_PI )

struct Vector2i
{
	int x;
	int y;
};

struct Vector2f
{
	float x;
	float y;

	Vector2f();
	Vector2f( float _x, float _y );
};

struct Vector3f
{
	float x;
	float y;
	float z;

	Vector3f();
	Vector3f( float _x, float _y, float _z );

	Vector3f& operator+=( const Vector3f& r );
	Vector3f& operator-=( const Vector3f& r );
	Vector3f& operator*=( float f );

	// Возвращает вектор, перпендикулярный плоскости, определяемой исходными векторами.
	Vector3f Cross( const Vector3f& v ) const;

	// Нормализация вектора.
	Vector3f& Normalize();

	void Rotate( float Angle, const Vector3f& Axis );
};

inline Vector3f operator-( const Vector3f& l, const Vector3f& r )
{
	Vector3f Ret( l.x - r.x,
				  l.y - r.y,
				  l.z - r.z );
	return Ret;
}

inline Vector3f operator*( const Vector3f& l, float f )
{
	Vector3f Ret( l.x * f,
				  l.y * f,
				  l.z * f );
	return Ret;
}

class Matrix4f
{
public:
	float m[4][4];

	Matrix4f() {}

	inline void InitIdentity();

	inline Matrix4f operator*( const Matrix4f& Right ) const;

	void InitScaleTransform( float ScaleX, float ScaleY, float ScaleZ );
	void InitScaleTransform( const Vector3f& scale );

	void InitRotateTransform( float RotateX, float RotateY, float RotateZ );
	void InitRotateTransform( const Vector3f& rotate );

	void InitTranslationTransform( float x, float y, float z );
	void InitTranslationTransform( const Vector3f& transform );

	void InitCameraTransform( const Vector3f& Target, const Vector3f& Up );
	void InitPersProjTransform( float FOV, float Width, float Height, float zNear, float zFar );
};

struct Quaternion
{
	float x, y, z, w;

	Quaternion( float _x, float _y, float _z, float _w );

	void Normalize();

	Quaternion Conjugate();
};

Quaternion operator*( const Quaternion& l, const Quaternion& r );

Quaternion operator*( const Quaternion& q, const Vector3f& v );

#endif	/* MATH_3D_H */