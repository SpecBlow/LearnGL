#pragma once
#ifndef TEXTURE_H
#define	TEXTURE_H

#include <string>

class Texture
{
public:
				Texture( unsigned int TextureTarget, const std::string& FileName );

	bool		Load();
	void		Bind( unsigned int TextureUnit );

private:

	unsigned int*		LoadFile( const std::string& file, int &_width, int &_height );

	std::string			fileName;
	unsigned int		textureTarget;
	unsigned int		textureID;
	int					width, height;
};

#endif	/* TEXTURE_H */
