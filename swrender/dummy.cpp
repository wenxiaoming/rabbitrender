#include <GLES/gl.h>

#include <stdlib.h>

#include "context.h"
//#include "fp.h"
//#include "state.h"
//#include "array.h"
//#include "matrix.h"
//#include "vertex.h"
//#include "light.h"
//#include "texture.h"

void  glPointParameterf (GLenum pname, GLfloat param)
{
	return;
}

void glGetFloatv (GLenum pname, GLfloat *params)
{
    ogles_context_t* c = ogles_context_t::get();
    switch(pname)
    {
	case GL_MODELVIEW_MATRIX:
		memcpy( params,
			c->transforms.modelview.top().elements(),
			16*sizeof(GLint));
			break;			
	case GL_PROJECTION_MATRIX:
		memcpy( params,
			c->transforms.projection.top().elements(),
			16*sizeof(GLint));
		break;

	default:
		break;
	}
	return;
}

void glPointParameterfv (GLenum pname, const GLfloat *params)
{
	return;
}

void glTexEnvi (GLenum target, GLenum pname, GLint param)
{
	return;
}

void glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params)
{
	return;
}

GLboolean glIsEnabled (GLenum cap)
{
	return 0;
}