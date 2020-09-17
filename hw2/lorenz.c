/*
 * Lorenz Attractor Program
 * For fun and for science! (Mostly fun)
 * Modified Lorenz.c to work using OpenGL by Jake Henson - 105963531
 * University of Colorado Boulder, Fall 2020
 *
 * ~~ Key bindings ~~
 * d = Decrease Lorenz s parameter
 * s = Increase Lorenz s parameter
 * r = Decrease Lorenz r parameter
 * t = Increase Lorenz r parameter
 * 1 = Decrease the scale
 * 2 = Increase the scale
 * 0 = Reset viewport to default
 * arrow keys = change the angle of the view
 * ESC = Exit the program
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdarg.h>
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

//  Globals
int th=25;       // Azimuth of view angle
int ph=20;       // Elevation of view angle

// Lorenz Global Parameters
double s  = 10;
double b  = 2.6666;
double r  = 28;
double scale = .05;
double rcolor = 1.000; //for changing color depending on r parameter
double scolor = 1.000; //for changing color depending on s parameter


/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  // Maximum length of text string
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

/*
 *  Display the scene
 */
void display()
{
  //reset colors if needed so they don't go out of bounds
  if(rcolor <= 0)
     rcolor = 0.005;
  else if(rcolor >= 1)
     rcolor = 1.000;

  if(scolor <= 0)
     scolor = 0.005;
  else if(scolor >= 1)
     scolor = 1.000;


   //  Clear the image
   glClear(GL_COLOR_BUFFER_BIT);
   //  Reset previous transforms
   glLoadIdentity();
   //  Set view angle
   glRotated(ph,1,0,0);
   glRotated(th,0,1,0);

   //Salvaged from the original lorenz.c file, the main driver
   int i;
    /*  Coordinates  */
   double x = 1;
   double y = 1;
   double z = 1;
   /*  Time step  */
   double dt = 0.001;

   glColor3f(0,1,1);
   glBegin(GL_LINE_STRIP);
      /*
       *  Integrate 50,000 steps (50 time units with dt = 0.001)
       *  Explicit Euler integration
       */
      for (i=0;i<50000;i++)
      {
         double dx = s*(y-x);
         double dy = x*(r-z)-y;
         double dz = x*y - b*z;
         x += (dt*dx);
         y += (dt*dy);
         z += (dt*dz);

         //adjust colors depending on i, s, and r values! How fun!
         if(i <= 1000  || i >= 30000)
             glColor3f(0,scolor,rcolor);
         else
             glColor3f(1,abs(scolor-1),rcolor);

         //create a vertex point using the given scale!
         glVertex3f(x*scale,y*scale,z*scale);
      }
   glEnd();

   //  Draw axes in white
   glColor3f(1,1,1);
   glBegin(GL_LINES);
   glVertex3d(0,0,0);
   glVertex3d(1,0,0);
   glVertex3d(0,0,0);
   glVertex3d(0,1,0);
   glVertex3d(0,0,0);
   glVertex3d(0,0,1);
   glEnd();
   //  Label axes
   const double len = 1;
   glRasterPos3d(len,0,0);
   Print("X");
   glRasterPos3d(0,len,0);
   Print("Y");
   glRasterPos3d(0,0,len);
   Print("Z");
   glWindowPos2i(5,5);
   //Display scale, s and r parameters
   Print("scale=%0.1f        s=%0.1f  r=%0.f",(100 * scale),s,r);
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
  //check for scale so it doesn't invert itself
   if(scale <= 0.005)
      scale = 0.005;

   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
      th = ph = 0;

   // change s parameter
   else if(ch == 'd'){
      scolor += 0.05;
      s += 0.2;
   }

   else if(ch == 's'){
     scolor -= 0.05;
      s -= 0.2;
   }

  // change r parameter
   else if(ch == 'r'){
     rcolor += 0.05;
     r -= 1;
   }

   else if(ch == 't'){
     rcolor -= 0.05;
     r += 1;
   }


   //decrease scale
   else if (ch == '1')
      scale -= 0.005;

   //increase scale
   else if (ch == '2')
      scale += 0.005;

   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{

   //  Right arrow key - increase azimuth by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease azimuth by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{

   const double dim = 2;
   //  Ratio of the width to the height of the window
   double w2h = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Orthogonal projection box adjusted for the
   //  aspect ratio of the window
   glOrtho(-dim*w2h,+dim*w2h, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

/*
 *  Start up GLUT and tell it what to do, as we did with example 6
 */
int main(int argc,char* argv[])
{
  //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window
   glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
   //  Request 500 x 500 pixel window
   glutInitWindowSize(500,500);
   //  Create the window
   glutCreateWindow("Jake Henson - HW2");
   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
  //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   //  Return code
   return 0;
}
