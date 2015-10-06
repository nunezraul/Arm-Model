
# include <iostream>
# include <gsim/gs.h>
# include "app_window.h"

AppWindow::AppWindow ( const char* label, int x, int y, int w, int h )
          :GlutWindow ( label, x, y, w, h )
 {
   initPrograms ();
   addMenuEntry ( "Option 0", evOption0 );
   addMenuEntry ( "Option 1", evOption1 );
   _viewaxis = true;
   _fovy = GS_TORAD(60.0f);
   _rotx = _roty = 0;
   _w = w;
   _h = h;
   rb = 0.4, rt = 0.4, len = 0.6;
   nfaces = 15;
   shading = true; rendering = true;
   sunx = 0.0f;
   suny = .5f;
   sunz = 0.8f;
   normshow = true;
   handx = 0;
   armx = 0;
   shoux = 0;
 }

void AppWindow::initPrograms ()
 {
   // Load your shaders and link your programs here:
   _vertexsh.load_and_compile ( GL_VERTEX_SHADER, "../vsh_mcol_flat.glsl" );
   _fragsh.load_and_compile ( GL_FRAGMENT_SHADER, "../fsh_flat.glsl" );
   _prog.init_and_link ( _vertexsh, _fragsh );

   _vertexshgou.load_and_compile(GL_VERTEX_SHADER, "../vsh_mcol_gou.glsl");
   _fragshgou.load_and_compile(GL_FRAGMENT_SHADER, "../fsh_gou.glsl");
   _proggou.init_and_link(_vertexshgou, _fragshgou);

   // Init my scene objects:
   _axis.init ( _prog );
   //Arm in triangles
   _arm1.init(_proggou, part0);
   _arm2.init(_proggou, part1);
   _arm3.init(_proggou, part2);
   //Arm in lines
   _wire1.init(_proggou, part0);
   _wire2.init(_proggou, part1);
   _wire3.init(_proggou, part2);
   _normals1.init(_prog, part0);
   _normals2.init(_prog, part1);
   _normals3.init(_prog, part2);
 }

// mouse events are in window coordinates, but your 2D scene is in [0,1]x[0,1],
// so make here the conversion when needed
GsVec2 AppWindow::windowToScene ( const GsVec2& v )
 {
   // GsVec2 is a lighteweight class suitable to return by value:
   return GsVec2 ( (2.0f*(v.x/float(_w))) - 1.0f,
                    1.0f - (2.0f*(v.y/float(_h))) );
 }

// Called every time there is a window event
void AppWindow::glutKeyboard ( unsigned char key, int x, int y )
 {
	 switch (key)
	 {
	 case ' ': _viewaxis = !_viewaxis; redraw(); break;
	 case 'q': shoux += 2; redraw(); break;
	 case 'a': shoux -= 2; redraw(); break;
	 case 'w': armx += 2; redraw(); break;
	 case 's': armx -= 2; redraw(); break;
	 case 'e': handx += 2; redraw(); break;
	 case 'd': handx -= 2; redraw(); break;
	 case 'z': rendering = !rendering; redraw(); break;
	 case 'x': if (shading) { shading = false; _arm1.changed = true; redraw(); } break;
	 case 'c': if (!shading) { shading = true; _arm1.changed = true; redraw(); } break;
	case 'v': normshow = !normshow; redraw(); break;

	case 27 : exit(1); // Esc was pressed
	
	}
 }

void AppWindow::glutSpecial ( int key, int x, int y )
 {
   bool rd=true;
   const float incr=GS_TORAD(2.5f);
   const float incf=0.05f;
   switch ( key )
    { case GLUT_KEY_LEFT:      _roty-=incr; break;
      case GLUT_KEY_RIGHT:     _roty+=incr; break;
      case GLUT_KEY_UP:        _rotx-=incr; break;
      case GLUT_KEY_DOWN:      _rotx+=incr; break;
      case GLUT_KEY_PAGE_UP:   _fovy-=incf; break;
      case GLUT_KEY_PAGE_DOWN: _fovy+=incf; break;
      case GLUT_KEY_HOME:      _fovy=GS_TORAD(60.0f); _rotx=_roty=0; break;
      default: return; // return without rendering
	}
   if (rd) redraw(); // ask the window to be rendered when possible
 }

void AppWindow::glutMouse ( int b, int s, int x, int y )
 {
   // The mouse is not used in this example.
   // Recall that a mouse click in the screen corresponds
   // to a whole line traversing the 3D scene.
 }

void AppWindow::glutMotion ( int x, int y )
 {
 }

void AppWindow::glutMenu ( int m )
 {
   std::cout<<"Menu Event: "<<m<<std::endl;
 }

void AppWindow::glutReshape ( int w, int h )
 {
   // Define that OpenGL should use the whole window for rendering
   glViewport( 0, 0, w, h );
   _w=w; _h=h;
 }

// here we will redraw the scene according to the current state of the application.
void AppWindow::glutDisplay ()
 {
   // Clear the rendering window
   glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

   // Build a cross with some lines (if not built yet):
   if ( _axis.changed ) // needs update
    { _axis.build(1.0f); // axis has radius 1.0
    }
   if (_arm1.changed) // needs update
   {
	   _arm1.build(len, rt, rb, nfaces, shading);
	   _arm2.build(len, rt, rb, nfaces, shading);
	   _arm3.build(len, rt, rb, nfaces, shading);
	   _normals1.build(len, rt, rb, nfaces, shading);
	   _normals2.build(len, rt, rb, nfaces, shading);
	   _normals3.build(len, rt, rb, nfaces, shading);
	   _arm1.changed = false;
   }
   if (_wire1.changed) {
	   _wire1.build(len, rt, rb, nfaces, shading);
	   _wire2.build(len, rt, rb, nfaces, shading);
	   _wire3.build(len, rt, rb, nfaces, shading);
	   _wire1.changed = false;
   }
   // Define our scene transformation:
   GsMat rx, ry, stransf, shrink, movehand, transarmhand, transarmsh, movearm, movesh, rotateh, rotatea, rotates, basicrot;
   rx.rotx ( _rotx );
   ry.roty ( _roty );
   stransf = rx*ry; // set the scene transformation matrix

   //Shrink to scale in axis
   shrink.scale(.02);

   //Movehand and arm
   movehand.translation(GsVec(0.0, -24.0*sin(2 * M_PI*armx / 360), 24.0*cos(2 * M_PI*armx / 360)));

   //Move arm + shoulder
   transarmsh.translation(GsVec(0.0, -28 * sin(2 * M_PI*shoux / 360), 28 * cos(2 * M_PI*shoux / 360)));

   //Translation of arm + hand
   transarmhand.translation(GsVec(0.0, -26 * sin(2 * M_PI*(shoux+armx) / 360), 26 * cos(2 * M_PI*(shoux+armx) / 360)));

   //Shift shoulder really to the left
   movesh.translation(0, 0, -42);

   //Rotate hand 
   rotateh.rotx(2 * M_PI * handx / 360);

   //Rotate arm
   rotatea.rotx(2 * M_PI * armx / 360);

   //Rotate shoulder
   rotates.rotx(2 * M_PI * shoux / 360);

   //Basic rotation
   basicrot = stransf*shrink*movesh;

   // Define our projection transformation:
   // (see demo program in gltutors-projection.7z, we are replicating the same behavior here)
   GsMat camview, persp, sproj;

   GsVec eye(0,0,2), center(0,0,0), up(0,1,0);
   camview.lookat ( eye, center, up ); // set our 4x4 "camera" matrix

   float aspect=1.0f, znear=0.1f, zfar=50.0f;
   persp.perspective ( _fovy, aspect, znear, zfar ); // set our 4x4 perspective matrix

   // Our matrices are in "line-major" format, so vertices should be multiplied on the 
   // right side of a matrix multiplication, therefore in the expression below camview will
   // affect the vertex before persp, because v' = (persp*camview)*v = (persp)*(camview*v).
   sproj = persp * camview; // set final scene projection

   //  Note however that when the shader receives a matrix it will store it in column-major 
   //  format, what will cause our values to be transposed, and we will then have in our 
   //  shaders vectors on the left side of a multiplication to a matrix.


   //Light and material
   const SoModel::Light lght(GsVec(sunx, suny, suny), GsColor::gray, GsColor::gray, GsColor::gray);

   // Draw:
   if ( _viewaxis ) _axis.draw ( stransf, sproj );
   if (rendering) {
	   if (_viewaxis) _arm1.draw(basicrot*transarmsh*transarmhand*rotates*rotatea*rotateh, sproj, lght);
	   if (_viewaxis) _arm2.draw(basicrot*transarmsh*rotates*rotatea, sproj, lght);
	   if (_viewaxis) _arm3.draw(basicrot*rotates, sproj, lght);
   }
   else {
	   if (_viewaxis) _wire1.draw(basicrot*transarmsh*transarmhand*rotates*rotatea*rotateh, sproj);
	   if (_viewaxis) _wire2.draw(basicrot*transarmsh*rotates*rotatea, sproj);
	   if (_viewaxis) _wire3.draw(stransf*shrink*movesh*rotates, sproj);
   }
   if (normshow) {
	   _normals1.draw(basicrot*transarmsh*transarmhand*rotates*rotatea*rotateh, sproj);
	   _normals2.draw(basicrot*transarmsh*rotates*rotatea, sproj);
	   _normals3.draw(basicrot*rotates, sproj);
   }

   // Swap buffers and draw:
   glFlush();         // flush the pipeline (usually not necessary)
   glutSwapBuffers(); // we were drawing to the back buffer, now bring it to the front
}

