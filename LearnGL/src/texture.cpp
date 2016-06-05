#include "texture.h"
#include <GL/glew.h>
#include <objidl.h>
#include <gdiplus.h>

using namespace Gdiplus;

// Нужно придумать, как отлавливать и показывать ошибки загрузки.

#define loop( v, m )	for( int v = 0; v < m; v++ )
#define loopi( m )		loop( i, m )
#define loopj( m )		loop( j, m )
#define RGBA( r, g, b, a )     ( (DWORD)( ( ( (DWORD)(BYTE)(a) ) << 24 ) | RGB( r, g, b ) ) )

Texture::Texture( unsigned int TextureTarget, const std::string& FileName ):
	fileName		( FileName ),
	textureTarget	( TextureTarget ),
	textureID		( 0 ),
	width			( 0 ),
	height			( 0 )
{
}

bool Texture::Load()
{
	unsigned int *data = LoadFile( fileName, width, height );
	
	if (width == 0 || height == 0)
		return false;

	glGenTextures	( 1, &textureID );
	glBindTexture	( textureTarget, textureID );
	glTexImage2D	( textureTarget, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );
	glTexParameterf	( textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameterf	( textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	
	delete[] data;
	return true;
}

void Texture::Bind( unsigned int TextureUnit )
{
	glActiveTexture( TextureUnit );
	glBindTexture( textureTarget, textureID );
}

unsigned* Texture::LoadFile( const std::string& file, int& _width, int& _height )
{
	std::wstring widestr = std::wstring( file.begin(), file.end() );
	const wchar_t *buf = widestr.c_str();
	Bitmap bitmap( buf );
	_width = bitmap.GetWidth();
	_height = bitmap.GetHeight();
	unsigned int *pix = new unsigned int[ _width * _height ];

	loopj( _height ) 
		loopi( _width )
		{
			Color c;
			bitmap.GetPixel( i, j, &c );
			pix[ i + j * _width ] = RGBA( c.GetR(), c.GetG(), c.GetB(), c.GetA() );
		}

	return pix;
}