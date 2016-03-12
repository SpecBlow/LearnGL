#pragma once

#ifndef LIGHTINGTECHNIQUE_H
#define LIGHTINGTECHNIQUE_H

#include "technique.h"
#include "math_3d.h"

#define MAX_POINT_LIGHTS 3

struct BaseLight
{
	Vector3f	Color;
	float		AmbientIntensity;
	float		DiffuseIntensity;

	BaseLight()
	{
		Color			 = Vector3f( 0.0f, 0.0f, 0.0f );
		AmbientIntensity = 0.0f;
		DiffuseIntensity = 0.0f;
	}
};


struct DirectionalLight: public BaseLight
{
	Vector3f	Direction;

	DirectionalLight()
	{
		Direction = Vector3f( 0.0f, 0.0f, 0.0f );
	}
};

struct PointLight : public BaseLight
{
	Vector3f Position;

	struct
	{
		float		Constant;
		float		Linear;
		float		Exp;
	} Attenuation;

	PointLight()
	{
		Position			 = Vector3f( 0.0f, 0.0f, 0.0f );
		Attenuation.Constant = 1.0f;
		Attenuation.Linear	 = 0.0f;
		Attenuation.Exp		 = 0.0f;
	}
};

class LightingTechnique : public Technique
{
public:
						LightingTechnique();
	virtual bool		Init() override;

	void				SetWVP( const Matrix4f& WVP );
	void				SetWorldMatrix( const Matrix4f& WVP );
	void				SetTextureUnit( unsigned int TextureUnit );
	void				SetDirectionalLight( const DirectionalLight& Light );

	void				SetEyeWorldPos( const Vector3f& EyeWorldPos );
	void				SetMatSpecularIntensity( float Intensity );
	void				SetMatSpecularPower( float Power );

	void				SetPointLights( unsigned int NumLights, const PointLight* pLights );

private:
	unsigned int		WVPLocation;
	unsigned int		WorldMatrixLocation;
	unsigned int		samplerLocation;

	unsigned int		eyeWorldPosLocation;
	unsigned int		matSpecularIntensityLocation;
	unsigned int		matSpecularPowerLocation;
	unsigned int		numPointLightsLocation;

	struct 
	{
		unsigned int	Color;
		unsigned int	AmbientIntensity;
		unsigned int	Direction;
		unsigned int	DiffuseIntensity;
	} dirLightLocation;

	struct 
	{
		unsigned int	Color;
		unsigned int	AmbientIntensity;
		unsigned int	DiffuseIntensity;
		unsigned int	Position;
		struct 
		{
			unsigned int Constant;
			unsigned int Linear;
			unsigned int Exp;
		} Atten;
	} pointLightsLocation[ MAX_POINT_LIGHTS ];

};

#endif // LIGHTINGTECHNIQUE_H
