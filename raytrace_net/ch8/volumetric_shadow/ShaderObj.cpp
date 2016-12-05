#include "ShaderObj.h"

ShaderObj::ShaderObj() {

	// init some vars
	m_programObject = glCreateProgramObjectARB();
	m_vertCompiled  = GL_FALSE;
	m_fragCompiled  = GL_FALSE;
	m_linked		= GL_FALSE;

}

ShaderObj::~ShaderObj() {


}

// loadVertShader //////////////////////////////////////////////////////////////////////////
bool ShaderObj::loadVertShader( std::string fileName ) {

	GLcharARB *		pInfoLog;
	GLint			maxlength;
	const char *	s;
	int				l;

	// make a vert shader maybe???
	std::string			vertCode = "";
	std::string			line;
	std::ifstream		vertFile( fileName.c_str() );

	vertFile.unsetf( std::ios::skipws );

	while( ! vertFile.eof() ) {

		std::getline( vertFile, line ); // read line by line
		vertCode += line;

	}

	vertFile.close();

	s = vertCode.c_str();
	l = strlen( s );
	
	m_vertexShaderObject = glCreateShaderObjectARB( GL_VERTEX_SHADER_ARB );

	// bind source
	glShaderSourceARB( m_vertexShaderObject, 1, &s, &l );
	// compile
	glCompileShaderARB( m_vertexShaderObject );
	// verify success
	glGetObjectParameterivARB( m_vertexShaderObject,
							   GL_OBJECT_COMPILE_STATUS_ARB, ( int * )&m_vertCompiled);


	if( m_vertCompiled == GL_FALSE ) {

		glGetObjectParameterivARB( m_vertexShaderObject,
								   GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxlength);
		// get error log
		pInfoLog = new GLcharARB[ maxlength ];
		glGetInfoLogARB( m_vertexShaderObject, maxlength, &l, pInfoLog );
		// show error log
		MessageBox( NULL, pInfoLog, "COMPILATION LOG", MB_OK );
		delete [] pInfoLog;

	}
	
	return( m_vertCompiled == GL_TRUE );

}

// loadFragShader //////////////////////////////////////////////////////////////////////////
bool ShaderObj::loadFragShader( std::string fileName ) {

	GLcharARB *		pInfoLog;
	GLint			maxlength;
	const char *	s;
	int				l;

	// same as above except for frag shader
	std::string			fragCode = "";
	std::string			line;
	std::ifstream		fragFile( fileName.c_str() );

	fragFile.unsetf( std::ios::skipws );

	while( ! fragFile.eof() ) {

		std::getline( fragFile, line );
		fragCode += line;

	}

	fragFile.close();

	s = fragCode.c_str();
	l = strlen( s );
	
	m_fragmentShaderObject = glCreateShaderObjectARB( GL_FRAGMENT_SHADER_ARB );

	glShaderSourceARB( m_fragmentShaderObject, 1, &s, &l );

	glCompileShaderARB( m_fragmentShaderObject );
	glGetObjectParameterivARB( m_fragmentShaderObject,
							   GL_OBJECT_COMPILE_STATUS_ARB, ( int * )&m_fragCompiled);
	
	if( m_fragCompiled == GL_FALSE ) {

		glGetObjectParameterivARB( m_fragmentShaderObject,
								   GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxlength);
		// get error log
		pInfoLog = new GLcharARB[ maxlength ];
		glGetInfoLogARB( m_fragmentShaderObject, maxlength, &l, pInfoLog );
		// show error log
		MessageBox( NULL, pInfoLog, "COMPILATION LOG", MB_OK );
		delete [] pInfoLog;

	}
	
	return( m_fragCompiled == GL_TRUE );


}

// link ///////////////////////////////////////////////////////////////////////////////
bool ShaderObj::link( const stringToIntMap_t & bindings ) {

	GLint		maxlength, l;
	GLcharARB *	pInfoLog;

	m_bindingMap = bindings;

	if( m_vertCompiled == GL_FALSE || m_fragCompiled == GL_FALSE ) {

		MessageBox( NULL, "link failed - compile first", "LINK ERROR", MB_OK );
			return( false );
	}

	// attach then lose the individual progs
	glAttachObjectARB( m_programObject, m_vertexShaderObject );
	glAttachObjectARB( m_programObject, m_fragmentShaderObject );

	glDeleteObjectARB( m_vertexShaderObject );
	glDeleteObjectARB( m_fragmentShaderObject );

	// bind indices to variable names
	for( stringToIntMapIterator_t i = m_bindingMap.begin();
		 i != m_bindingMap.end();
		 ++i ) {

			 glBindAttribLocationARB( m_programObject, (*i).second, (*i).first.c_str() );

	}

	// attempt link
	glLinkProgramARB( m_programObject );
	glGetObjectParameterivARB( m_programObject, GL_OBJECT_LINK_STATUS_ARB,
							   ( int * )&m_linked);

	if ( m_linked ) {

		// bind
		// glUseProgramObjectARB( m_programObject );

	} else {

		glGetObjectParameterivARB( m_programObject,
								   GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxlength );
		pInfoLog = new GLcharARB [ maxlength ];
		glGetInfoLogARB( m_programObject, maxlength, &l, pInfoLog );
		
		MessageBox( NULL, pInfoLog, "LINK REPORT", MB_OK );
		delete [] pInfoLog;

	}

	return( m_linked == GL_TRUE );

}

// getVariableIndex ////////////////////////////////////////////////////////////////
GLint ShaderObj::getVariableIndex( std::string varName ) {

	stringToIntMapIterator_t i;
	
	if( m_bindingMap.find( varName ) == m_bindingMap.end() )
		return( -1 );

	return( m_bindingMap[ varName ] );


}

// bind /////////////////////////////////////////////////////////////////////////////
void ShaderObj::bind() {

	glUseProgramObjectARB( m_programObject );

}

// getUniformLocation ///////////////////////////////////////////////////////////////
GLint ShaderObj::getUniformLocation( const char * varName ) {

	return ( glGetUniformLocationARB( m_programObject, varName ) );
}

