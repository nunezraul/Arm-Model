
// Ensure the header file is included only once in multi-file projects
#ifndef APP_WINDOW_H
#define APP_WINDOW_H

# include <gsim/gs_color.h>
# include <gsim/gs_vec.h>
# include "ogl_tools.h"
# include "glut_window.h"
# include "so_axis.h"
# include "visualc14\so_normal.h"
# include "so_model.h"
# include "visualc14\so_modelwire.h"

// The functionality of your application should be implemented inside AppWindow
class AppWindow : public GlutWindow
 { private :
    // OpenGL shaders and programs:
    GlShader _vertexsh, _fragsh;
    GlProgram _prog;
	//Gourad shading
	GlShader _vertexshgou, _fragshgou;
	GlProgram _proggou;
    // My scene objects:
    SoAxis _axis;
	SoModel _arm1, _arm2, _arm3;
	SoModelWire _wire1, _wire2, _wire3;
	SoNormal _normals1, _normals2, _normals3;
    // App data:
    enum MenuEv { evOption0, evOption1 };
    float _rotx, _roty, _fovy;
    bool  _viewaxis;
    int _w, _h;
	bool shading, normshow, rendering;
	float sunx, suny, sunz;
	int part0 = 0, part1 = 1, part2 = 2;
	double M_PI = 3.14159265358979323846;
	int handx, armx, shoux;

   public :
    AppWindow ( const char* label, int x, int y, int w, int h );
    void initPrograms ();
    GsVec2 windowToScene ( const GsVec2& v );
	double rt, rb, len;
	int nfaces;

   private : // functions derived from the base class
    virtual void glutMenu ( int m );
    virtual void glutKeyboard ( unsigned char key, int x, int y );
    virtual void glutSpecial ( int key, int x, int y );
    virtual void glutMouse ( int b, int s, int x, int y );
    virtual void glutMotion ( int x, int y );
    virtual void glutDisplay ();
    virtual void glutReshape ( int w, int h );
 };

#endif // APP_WINDOW_H
