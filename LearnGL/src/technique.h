#pragma once
#ifndef TEXHNIQUE_H
#define TEXHNIQUE_H

#include <list>

class Technique
{
public:
					Technique();
	virtual			~Technique();

	virtual bool	Init();
	void			Enable();

protected:
	bool			AddShader( unsigned int ShaderType, const char* pShaderText );
	bool			Finalize();
	int				GetUniformLocation( const char* pUniformName );

private:
	unsigned int					shaderProg;
	typedef std::list<unsigned int>	ShaderObjList;
	ShaderObjList					shaderObjList;
};

#endif /* TEXHNIQUE_H */