#include "lighting_technique.h"
#include <GL/glew.h>
#include "util.h"

#define INVALID_UNIFORM_LOCATION 0xFFFFFFFF

static const char* pVS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
layout (location = 1) in vec2 TexCoord;                                             \n\
layout (location = 2) in vec3 Normal;                                               \n\
                                                                                    \n\
uniform mat4 gWVP;                                                                  \n\
uniform mat4 gWorld;                                                                \n\
                                                                                    \n\
out vec2 TexCoord0;                                                                 \n\
out vec3 Normal0;                                                                   \n\
out vec3 WorldPos0;                                                                 \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    gl_Position = gWVP * vec4(Position, 1.0);                                       \n\
    TexCoord0   = TexCoord;                                                         \n\
    Normal0     = (gWorld * vec4(Normal, 0.0)).xyz;                                 \n\
    WorldPos0   = (gWorld * vec4(Position, 1.0)).xyz;                               \n\
}";

static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
const int MAX_POINT_LIGHTS = 3;                                                     \n\
                                                                                    \n\
in vec2 TexCoord0;                                                                  \n\
in vec3 Normal0;                                                                    \n\
in vec3 WorldPos0;                                                                  \n\
                                                                                    \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
struct BaseLight                                                                    \n\
{                                                                                   \n\
    vec3 Color;                                                                     \n\
    float AmbientIntensity;                                                         \n\
    float DiffuseIntensity;                                                         \n\
};                                                                                  \n\
                                                                                    \n\
struct DirectionalLight                                                             \n\
{                                                                                   \n\
    BaseLight Base;																	\n\
    vec3 Direction;                                                                 \n\
};                                                                                  \n\
                                                                                    \n\
struct Attenuation                                                                  \n\
{                                                                                   \n\
    float Constant;                                                                 \n\
    float Linear;                                                                   \n\
    float Exp;                                                                      \n\
};                                                                                  \n\
                                                                                    \n\
struct PointLight                                                                           \n\
{                                                                                           \n\
    BaseLight Base;																			\n\
    vec3 Position;                                                                          \n\
    Attenuation Atten;                                                                      \n\
};                                                                                          \n\
                                                                                            \n\
uniform int gNumPointLights;                                                                \n\
uniform DirectionalLight gDirectionalLight;                                                 \n\
uniform PointLight gPointLights[MAX_POINT_LIGHTS];                                          \n\
uniform sampler2D gSampler;                                                                 \n\
uniform vec3 gEyeWorldPos;                                                                  \n\
uniform float gMatSpecularIntensity;                                                        \n\
uniform float gSpecularPower;                                                               \n\
                                                                                            \n\
vec4 CalcLightInternal(BaseLight Light, vec3 LightDirection, vec3 Normal)            \n\
{                                                                                           \n\
    vec4 AmbientColor = vec4(Light.Color, 1.0f) * Light.AmbientIntensity;                   \n\
    float DiffuseFactor = dot(Normal, -LightDirection);                                     \n\
                                                                                            \n\
    vec4 DiffuseColor  = vec4(0, 0, 0, 0);                                                  \n\
    vec4 SpecularColor = vec4(0, 0, 0, 0);                                                  \n\
                                                                                            \n\
    if (DiffuseFactor > 0) {                                                                \n\
        DiffuseColor = vec4(Light.Color, 1.0f) * Light.DiffuseIntensity * DiffuseFactor;    \n\
                                                                                            \n\
        vec3 VertexToEye = normalize(gEyeWorldPos - WorldPos0);                             \n\
        vec3 LightReflect = normalize(reflect(LightDirection, Normal));                     \n\
        float SpecularFactor = dot(VertexToEye, LightReflect);                              \n\
        SpecularFactor = pow(SpecularFactor, gSpecularPower);                               \n\
        if (SpecularFactor > 0) {                                                           \n\
            SpecularColor = vec4(Light.Color, 1.0f) *                                       \n\
                            gMatSpecularIntensity * SpecularFactor;                         \n\
        }                                                                                   \n\
    }                                                                                       \n\
                                                                                            \n\
    return (AmbientColor + DiffuseColor + SpecularColor);                                   \n\
}                                                                                           \n\
                                                                                            \n\
vec4 CalcDirectionalLight(vec3 Normal)                                                      \n\
{                                                                                           \n\
    return CalcLightInternal(gDirectionalLight.Base, gDirectionalLight.Direction, Normal); \n\
}                                                                                           \n\
                                                                                            \n\
vec4 CalcPointLight(int Index, vec3 Normal)                                                 \n\
{                                                                                           \n\
    vec3 LightDirection = WorldPos0 - gPointLights[Index].Position;                         \n\
    float Distance = length(LightDirection);                                                \n\
    LightDirection = normalize(LightDirection);                                             \n\
                                                                                            \n\
    vec4 Color = CalcLightInternal(gPointLights[Index].Base, LightDirection, Normal);       \n\
    float Attenuation =  gPointLights[Index].Atten.Constant +                               \n\
                         gPointLights[Index].Atten.Linear * Distance +                      \n\
                         gPointLights[Index].Atten.Exp * Distance * Distance;               \n\
                                                                                            \n\
    return Color / Attenuation;                                                             \n\
}                                                                                           \n\
                                                                                            \n\
void main()                                                                                 \n\
{                                                                                           \n\
    vec3 Normal = normalize(Normal0);                                                       \n\
    vec4 TotalLight = CalcDirectionalLight(Normal);                                         \n\
                                                                                            \n\
    for (int i = 0 ; i < gNumPointLights ; i++) {                                           \n\
        TotalLight += CalcPointLight(i, Normal);                                            \n\
    }                                                                                       \n\
                                                                                            \n\
    FragColor = texture2D(gSampler, TexCoord0.xy) * TotalLight;                             \n\
}";

LightingTechnique::LightingTechnique():
	WVPLocation							( 0 ),
	WorldMatrixLocation					( 0 ),
	samplerLocation						( 0 ),
	eyeWorldPosLocation					( 0 ),
	matSpecularIntensityLocation		( 0 ),
	matSpecularPowerLocation			( 0 ),
	numPointLightsLocation				( 0 )
{
}

bool LightingTechnique::Init()
{
	if ( !Technique::Init() )
	{
		return false;
	}

	if ( !AddShader( GL_VERTEX_SHADER, pVS ) )
	{
		return false;
	}

	if ( !AddShader( GL_FRAGMENT_SHADER, pFS ) )
	{
		return false;
	}

	if ( !Finalize() )
	{
		return false;
	}

	WVPLocation							= GetUniformLocation( "gWVP" );
	WorldMatrixLocation					= GetUniformLocation( "gWorld" );
	samplerLocation						= GetUniformLocation( "gSampler" );
															  
	eyeWorldPosLocation					= GetUniformLocation( "gEyeWorldPos" );
															  
	dirLightLocation.Color				= GetUniformLocation( "gDirectionalLight.Base.Color" );
	dirLightLocation.AmbientIntensity	= GetUniformLocation( "gDirectionalLight.Base.AmbientIntensity" );
	dirLightLocation.Direction			= GetUniformLocation( "gDirectionalLight.Direction" );
	dirLightLocation.DiffuseIntensity	= GetUniformLocation( "gDirectionalLight.Base.DiffuseIntensity" );
															  
	matSpecularIntensityLocation		= GetUniformLocation( "gMatSpecularIntensity" );
	matSpecularPowerLocation			= GetUniformLocation( "gSpecularPower" );
															  
	numPointLightsLocation				= GetUniformLocation( "gNumPointLights" );
	
	for ( unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS( pointLightsLocation ); i++ ) 
	{
		char Name[128];
		memset( Name, 0, sizeof( Name ) );
		snprintf( Name, sizeof( Name ), "gPointLights[%d].Base.Color", i );
		pointLightsLocation[i].Color = GetUniformLocation( Name );

		snprintf( Name, sizeof( Name ), "gPointLights[%d].Base.AmbientIntensity", i);
		pointLightsLocation[i].AmbientIntensity = GetUniformLocation( Name );

		snprintf( Name, sizeof( Name ), "gPointLights[%d].Position", i );
		pointLightsLocation[i].Position = GetUniformLocation( Name );

		snprintf( Name, sizeof( Name ), "gPointLights[%d].Base.DiffuseIntensity", i );
		pointLightsLocation[i].DiffuseIntensity = GetUniformLocation( Name );

		snprintf( Name, sizeof( Name ), "gPointLights[%d].Atten.Constant", i );
		pointLightsLocation[i].Atten.Constant = GetUniformLocation( Name );

		snprintf( Name, sizeof( Name ), "gPointLights[%d].Atten.Linear", i );
		pointLightsLocation[i].Atten.Linear = GetUniformLocation( Name );

		snprintf( Name, sizeof( Name ), "gPointLights[%d].Atten.Exp", i );
		pointLightsLocation[i].Atten.Exp = GetUniformLocation(Name);

		if ( pointLightsLocation[i].Color				== INVALID_UNIFORM_LOCATION ||
			 pointLightsLocation[i].AmbientIntensity	== INVALID_UNIFORM_LOCATION ||
			 pointLightsLocation[i].Position			== INVALID_UNIFORM_LOCATION ||
			 pointLightsLocation[i].DiffuseIntensity	== INVALID_UNIFORM_LOCATION ||
			 pointLightsLocation[i].Atten.Constant		== INVALID_UNIFORM_LOCATION ||
			 pointLightsLocation[i].Atten.Linear		== INVALID_UNIFORM_LOCATION ||
			 pointLightsLocation[i].Atten.Exp			== INVALID_UNIFORM_LOCATION ) 
		{
			return false;
		}
	}

	if ( dirLightLocation.AmbientIntensity	== INVALID_UNIFORM_LOCATION ||
		 WVPLocation						== INVALID_UNIFORM_LOCATION ||
		 WorldMatrixLocation				== INVALID_UNIFORM_LOCATION ||
		 samplerLocation					== INVALID_UNIFORM_LOCATION ||
		 dirLightLocation.Color				== INVALID_UNIFORM_LOCATION ||
		 dirLightLocation.DiffuseIntensity	== INVALID_UNIFORM_LOCATION ||
		 eyeWorldPosLocation				== INVALID_UNIFORM_LOCATION ||
		 matSpecularIntensityLocation		== INVALID_UNIFORM_LOCATION ||
		 matSpecularPowerLocation			== INVALID_UNIFORM_LOCATION ||
		 dirLightLocation.Direction			== INVALID_UNIFORM_LOCATION ||
		 numPointLightsLocation				== INVALID_UNIFORM_LOCATION )
	{
		return false;
	}

	return true;
}

void LightingTechnique::SetWVP( const Matrix4f& WVP )
{
	glUniformMatrix4fv( WVPLocation, 1, GL_TRUE, ( const GLfloat* )WVP.m );
}

void LightingTechnique::SetWorldMatrix( const Matrix4f& WorldInverse )
{
	glUniformMatrix4fv( WorldMatrixLocation, 1, GL_TRUE, ( const GLfloat* )WorldInverse.m );
}

void LightingTechnique::SetTextureUnit( unsigned int TextureUnit )
{
	glUniform1i( samplerLocation, TextureUnit );
}


void LightingTechnique::SetDirectionalLight( const DirectionalLight& Light )
{
	glUniform3f( dirLightLocation.Color, Light.Color.x, Light.Color.y, Light.Color.z );
	glUniform1f( dirLightLocation.AmbientIntensity, Light.AmbientIntensity );
	Vector3f Direction = Light.Direction;
	Direction.Normalize();
	glUniform3f( dirLightLocation.Direction, Direction.x, Direction.y, Direction.z );
	glUniform1f( dirLightLocation.DiffuseIntensity, Light.DiffuseIntensity );
}

void LightingTechnique::SetEyeWorldPos( const Vector3f& EyeWorldPos )
{
	glUniform3f( eyeWorldPosLocation, EyeWorldPos.x, EyeWorldPos.y, EyeWorldPos.z );
}

void LightingTechnique::SetMatSpecularIntensity( float Intensity )
{
	glUniform1f( matSpecularIntensityLocation, Intensity );
}

void LightingTechnique::SetMatSpecularPower( float Power )
{
	glUniform1f( matSpecularPowerLocation, Power );
}

void LightingTechnique::SetPointLights( unsigned NumLights, const PointLight* pLights )
{
	glUniform1i( numPointLightsLocation, NumLights );

	for ( unsigned int i = 0; i < NumLights; i++ ) 
	{
		glUniform3f( pointLightsLocation[i].Color,				pLights[i].Color.x, pLights[i].Color.y, pLights[i].Color.z );
		glUniform1f( pointLightsLocation[i].AmbientIntensity,	pLights[i].AmbientIntensity );
		glUniform1f( pointLightsLocation[i].DiffuseIntensity,	pLights[i].DiffuseIntensity );
		glUniform3f( pointLightsLocation[i].Position,			pLights[i].Position.x, pLights[i].Position.y, pLights[i].Position.z );
		glUniform1f( pointLightsLocation[i].Atten.Constant,		pLights[i].Attenuation.Constant );
		glUniform1f( pointLightsLocation[i].Atten.Linear,		pLights[i].Attenuation.Linear );
		glUniform1f( pointLightsLocation[i].Atten.Exp,			pLights[i].Attenuation.Exp );
	}
}