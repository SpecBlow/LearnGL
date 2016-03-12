#pragma once
#include "texture.h"

#include <GL/glew.h>
#include <GL/glut.h>
#include <objidl.h>
#include <gdiplus.h>

#include "pipeline.h"
#include "camera.h"
#include "lighting_technique.h"
#include "glut_backend.h"
#include "util.h"

using namespace Gdiplus;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

struct Vertex
{
	Vector3f pos;
	Vector2f tex;
	Vector3f m_normal;

	Vertex() {}

	Vertex( Vector3f _pos, Vector2f _tex ):
		pos			( _pos ),
		tex			( _tex ),
		m_normal	( Vector3f( 0.f, 0.f, 0.f ) )
	{
	}
};

class Main : public ICallbacks
{
	public:
		Main()
		{
			pGameCamera		= nullptr;
			pTexture		= nullptr;
			pEffect			= nullptr;
			scale			= 0.0f;
			directionalLight.Color				= Vector3f( 1.0f, 1.0f, 1.0f );
			directionalLight.AmbientIntensity	= -0.1f;
			directionalLight.DiffuseIntensity	= 0.0f;
			directionalLight.Direction			= Vector3f( 1.0f, 0.0f, 0.0f );
			VBO				= 0;
			IBO				= 0;
		}

		~Main()
		{
			delete pEffect;
			delete pGameCamera;
			delete pTexture;
		}

		bool Init()
		{
			unsigned long token;

			Vector3f Pos( 0.0f, 0.0f, -3.0f );
			Vector3f Target( 0.0f, 0.0f, 1.0f );
			Vector3f Up( 0.0, 1.0f, 0.0f );

			pGameCamera = new Camera( WINDOW_WIDTH, WINDOW_HEIGHT, Pos, Target, Up );

			unsigned int Indices[] = {	0, 3, 1,
										1, 3, 2,
										2, 3, 0,
										1, 2, 0  };

			CreateIndexBuffer( Indices, sizeof( Indices ) );
			CreateVertexBuffer( Indices, ARRAY_SIZE_IN_ELEMENTS( Indices ) );

			pEffect = new LightingTechnique();

			if ( !pEffect->Init() )
			{
				printf( "Error initializing the lighting technique!\n" );
				return false;
			}

			pEffect->Enable();
			pEffect->SetTextureUnit( 0 );

			// Нужно проинициализировать и запустить GdiplusStartupInput
			// Он требуется для загрузки текстур, например.
			GdiplusStartupInput input;
			GdiplusStartup( &token, &input, nullptr );

			pTexture = new Texture( GL_TEXTURE_2D, "test.png" );

			if ( !pTexture->Load() ) 
			{
				printf( "Textura: '%s' - not loaded!\n","test.png" );
				return false;
			}

			return true;
		}

		void Run()
		{
			GLUTBackendRun( this );
		}

		virtual void RenderSceneCB() override
		{
			pGameCamera->OnRender();

			glClear( GL_COLOR_BUFFER_BIT );

			scale += 0.02f;

			// Точечные источники света.
			PointLight pl[3];
			pl[0].DiffuseIntensity = 0.5f;
			pl[0].Color = Vector3f( 1.0f, 0.0f, 0.0f );
			pl[0].Position = Vector3f( sinf( scale ) * 10, 1.0f, cosf( scale ) * 10 );
			pl[0].Attenuation.Linear = 0.1f;

			pl[1].DiffuseIntensity = 0.5f;
			pl[1].Color = Vector3f( 0.0f, 1.0f, 0.0f );
			pl[1].Position = Vector3f( sinf( scale + 2.1f ) * 10, 1.0f, cosf( scale + 2.1f ) * 10 );
			pl[1].Attenuation.Linear = 0.1f;

			pl[2].DiffuseIntensity = 0.5f;
			pl[2].Color = Vector3f( 0.0f, 0.0f, 1.0f );
			pl[2].Position = Vector3f( sinf( scale + 4.2f ) * 10, 1.0f, cosf( scale + 4.2f ) * 10 );
			pl[2].Attenuation.Linear = 0.1f;

			pEffect->SetPointLights( 3, pl );


			Pipeline p;
			p.Rotate( 0.0f, 0.0f, 0.0f );
			p.WorldPos( 0.0f, 0.0f, 1.0f );

			// Тут устанавливаем нашу камеру.
			p.SetCamera( pGameCamera->GetPos(), pGameCamera->GetTarget(), pGameCamera->GetUp() );
			p.SetPerspectiveProj( 60.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 100.0f );

			// Передадим данные в шейдер в переменную
			pEffect->SetWVP( p.GetWVPTrans() );
			const Matrix4f& WorldTransformation = p.GetWorldTrans();
			pEffect->SetWorldMatrix( WorldTransformation );
			pEffect->SetDirectionalLight( directionalLight );
			pEffect->SetEyeWorldPos( pGameCamera->GetPos() );
			pEffect->SetMatSpecularIntensity( 1.0f );
			pEffect->SetMatSpecularPower( 100 );

			glEnableVertexAttribArray( 0 );
			glEnableVertexAttribArray( 1 );
			glEnableVertexAttribArray( 2 );

			glBindBuffer( GL_ARRAY_BUFFER, VBO );

			glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), nullptr );
			glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( const GLvoid* )12 );
			glVertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, sizeof( Vertex ), ( const GLvoid* )20 );

			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, IBO );

			pTexture->Bind( GL_TEXTURE0 );

			glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr );

			glDisableVertexAttribArray( 0 );
			glDisableVertexAttribArray( 1 );
			glDisableVertexAttribArray( 2 );

			glutSwapBuffers();
		}

		virtual void IdleCB() override
		{
			RenderSceneCB();
		}

		virtual void SpecialKeyboardCB( int Key, int x, int y ) override
		{
			pGameCamera->OnKeyboard( Key );
		}

		virtual void KeyboardCB( unsigned char Key, int x, int y ) override
		{
			switch (Key) 
			{
			case 'q':
				exit( 0 );
				break;

			case 'a':
				directionalLight.AmbientIntensity += 0.05f;
				break;

			case 's':
				directionalLight.AmbientIntensity -= 0.05f;
				break;

			case 'z':
				directionalLight.DiffuseIntensity += 0.05f;
				break;

			case 'x':
				directionalLight.DiffuseIntensity -= 0.05f;
				break;
			}
		}

		virtual void PassiveMouseCB( int x, int y ) override
		{
			pGameCamera->OnMouse( x, y );
		}

	private:

		void CalcNormals( const unsigned int* pIndices, unsigned int IndexCount, Vertex* pVertices, unsigned int VertexCount ) 
		{
			for ( unsigned int i = 0; i < IndexCount; i += 3 ) 
			{
				unsigned int Index0 = pIndices[i];
				unsigned int Index1 = pIndices[i + 1];
				unsigned int Index2 = pIndices[i + 2];
				Vector3f v1 = pVertices[Index1].pos - pVertices[Index0].pos;
				Vector3f v2 = pVertices[Index2].pos - pVertices[Index0].pos;
				Vector3f Normal = v1.Cross(v2);
				Normal.Normalize();

				pVertices[Index0].m_normal += Normal;
				pVertices[Index1].m_normal += Normal;
				pVertices[Index2].m_normal += Normal;
			}

			for ( unsigned int i = 0; i < VertexCount; i++ )
			{
				pVertices[i].m_normal.Normalize();
			}
		}

		void CreateVertexBuffer( const unsigned int* pIndices, unsigned int IndexCount )
		{
			//Vertex Vertices[4] = {	Vertex( Vector3f( -1.0f, -1.0f,  0.5773f ),	Vector2f( 0.0f, 0.0f ) ),
			//						Vertex( Vector3f(  0.0f, -1.0f, -1.15475 ), Vector2f( 0.5f, 0.0f ) ),
			//						Vertex( Vector3f(  1.0f, -1.0f,  0.5773f ), Vector2f( 1.0f, 0.0f ) ),
			//						Vertex( Vector3f(  0.0f,  1.0f,  0.0f ),	Vector2f( 0.5f, 1.0f ) )  };

			Vertex Vertices[4] = {  Vertex( Vector3f( -10.0f, -2.0f, -10.0f ), Vector2f( 0.0f, 0.0f ) ),
									Vertex( Vector3f(  10.0f, -2.0f, -10.0f ), Vector2f( 1.0f, 0.0f ) ),
									Vertex( Vector3f(  10.0f, -2.0f,  10.0f ), Vector2f( 1.0f, 1.0f ) ),
									Vertex( Vector3f( -10.0f, -2.0f,  10.0f ), Vector2f( 0.0f, 1.0f ) ) };

			unsigned int VertexCount = ARRAY_SIZE_IN_ELEMENTS( Vertices );

			CalcNormals( pIndices, IndexCount, Vertices, VertexCount );

			glGenBuffers( 1, & VBO );
			glBindBuffer( GL_ARRAY_BUFFER, VBO );
			glBufferData( GL_ARRAY_BUFFER, sizeof( Vertices ), Vertices, GL_STATIC_DRAW );
		}

		void CreateIndexBuffer( const unsigned int* pIndices, unsigned int SizeInBytes )
		{
			glGenBuffers( 1, &IBO );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, IBO );
			glBufferData( GL_ELEMENT_ARRAY_BUFFER, SizeInBytes, pIndices, GL_STATIC_DRAW );
		}

		GLuint				VBO;
		GLuint				IBO;
		LightingTechnique*	pEffect;
		Texture*			pTexture;
		Camera*				pGameCamera;
		float				scale;
		DirectionalLight	directionalLight;
};

int main( int argc, char** argv )
{
	GLUTBackendInit( argc, argv );

	if ( !GLUTBackendCreateWindow( WINDOW_WIDTH, WINDOW_HEIGHT, 32, false, "OpenGL" ) ) 
	{
		return 1;
	}

	Main* pApp = new Main();

	if ( !pApp->Init() ) 
	{
		return 1;
	}

	pApp->Run();

	delete pApp;

	return 0;
}