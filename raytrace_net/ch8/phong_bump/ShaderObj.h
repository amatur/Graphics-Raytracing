#ifndef __ShaderObj_H__
#define __ShaderObj_H__

/*!
 * Coded:	7/28/04 \n
 * By:		Rob Hall (rwhall@student.umass.edu) \n
 * Purpose:	A class to abstract over the GLSL interfaces using the shader				
 *			object related extensions (GLEW is needed to handle the extensions			
 *			it is a extension managing util at glew.sourceforge.net and it dominates).
 */

#include <windows.h>
#include <GL\glew.h>
#include <fstream>
#include <string>
#include <map>

class ShaderObj {

public:

	//! Type representing a map from string to int.
	typedef std::map<std::string, GLint>				stringToIntMap_t;
	//! Iterator for string to int map.
	typedef std::map<std::string, GLint>::iterator		stringToIntMapIterator_t;

	ShaderObj();
	~ShaderObj();

	//! Function to load and compile a vertex shader
	/*!
	 *\param fileName - the name of the file containing vertex shader source code.
	 */
	bool	loadVertShader( std::string fileName );		// factory can call these on construction

	//! Function to load and compile a fragment shader
	/*!
	 *\param fileName - the name of the file containing fragment shader source code.
	 */
	bool	loadFragShader( std::string fileName );

	//! Function to link the vertex and fragment shaders
	/*!
	 *\param bindings - map from strings to GLints, corresponding to required
	 *					variable bindings.
	 */
	bool	link( const stringToIntMap_t & bindings );	// resource sys can make this for me

	//! Returns the index for the specified variable name
	/*!
	 *\param varName - the name of the variable.
	 */
	GLint	getVariableIndex( std::string varName );

	//! Returns the index of a specified uniform variable.
	/*!
	 *\param varName - name of uniform.
	 */
	GLint	getUniformLocation( const char * varName );

	//! Function to bind the shader object
	void	bind();
	
private:

	GLint				m_nID;
	GLboolean			m_vertCompiled;
	GLboolean			m_fragCompiled;
	GLboolean			m_linked;

	stringToIntMap_t	m_bindingMap;

	GLhandleARB			m_programObject;
	GLhandleARB			m_vertexShaderObject;
	GLhandleARB			m_fragmentShaderObject;
	
};


#endif