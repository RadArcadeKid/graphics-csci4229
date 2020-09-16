/*
 * Simple program to demonstrate generating coordinates
 * using the Lorenz Attractor
 *
 *
 * Jake Henson - 105963531
 * HW2 - Lorenz Attractor
 */


 #include <stdio.h>
 #include <stdlib.h>
 #include <stdarg.h>
 #include <math.h>
 //  OpenGL with prototypes for glext
 #define GL_GLEXT_PROTOTYPES
 #ifdef __APPLE__
 #include <GLUT/glut.h>
 #else
 #include <GL/glut.h>
 #endif


/*  Lorenz Parameters  */
double s  = 10;
double b  = 2.6666;
double r  = 28;

//GLUT globals:
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int axes=1;       //  Display axes


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
 * display - display the Lorenz attractor and the axis
 */
 void display() {
   //TODO: perhaps buffer/face culling


   const double length = 2.0;
   //  Draw axes in white
   glColor3f(1,1,1);
   glBegin(GL_LINES);
   glVertex3d(0,0,0);
   glVertex3d(length,0,0);
   glVertex3d(0,0,0);
   glVertex3d(0,length,0);
   glVertex3d(0,0,0);
   glVertex3d(0,0,length);
   glEnd();
   //  Label axes
   glRasterPos3d(1,0,0);
   Print("X");
   glRasterPos3d(0,1,0);
   Print("Y");
   glRasterPos3d(0,0,1);
   Print("Z");

   //  Display parameters
   // glWindowPos2i(5,5);
   // Print("View Angle=%d,%d  %s",th,ph);
   // if (mode==2)
   //    Print("  z=%.1f",z);
   // else if (mode==4)
   //    Print("  w=%.1f",w);
   //  Flush and swap
   glFlush();
   glutSwapBuffers();
 }

 /*
  *  GLUT calls this routine when an arrow key is pressed
  */
 void special(int key,int x,int y)
 {
    //  Right arrow key - increase angle by 5 degrees
    if (key == GLUT_KEY_RIGHT)
       th += 5;
    //  Left arrow key - decrease angle by 5 degrees
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
  *  GLUT calls this routine when a key is pressed
  */
 void key(unsigned char ch,int x,int y)
 {
    //  Exit on ESC
    if (ch == 27)
       exit(0);
    //  Reset view angle
    else if (ch == '0')
       th = ph = 0;
    //  Toggle axes
    else if (ch == 'a' || ch == 'A')
       axes = 1-axes;
    //  Tell GLUT it is necessary to redisplay the scene
    glutPostRedisplay();
 }

 /*
  *  GLUT calls this routine when the window is resized
  */
 void reshape(int width,int height)
 {
    const double dim=2.5;
    //  Ratio of the width to the height of the window
    double w2h = (height>0) ? (double)width/height : 1;
    //  Set the viewport to the entire window
    glViewport(0,0, width,height);
    //  Tell OpenGL we want to manipulate the projection matrix
    glMatrixMode(GL_PROJECTION);
    //  Undo previous transformations
    glLoadIdentity();
    //  Orthogonal projection
    glOrtho(-w2h*dim,+w2h*dim, -dim,+dim, -dim,+dim);
    //  Switch to manipulating the model matrix
    glMatrixMode(GL_MODELVIEW);
    //  Undo previous transformations
    glLoadIdentity();
 }




/*
 *  Main
 */
int main(int argc, char *argv[])
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



  //LORENZ -
  //May need to move this to its own function
   int i;
   /*  Coordinates  */
   double x = 1;
   double y = 1;
   double z = 1;
   /*  Time step  */
   double dt = 0.001;

   printf("%5d %8.3f %8.3f %8.3f\n",0,x,y,z);
   /*
    *  Integrate 50,000 steps (50 time units with dt = 0.001)
    *  Explicit Euler integration
    */
   for (i=0;i<50000;i++)
   {
      double dx = s*(y-x);
      double dy = x*(r-z)-y;
      double dz = x*y - b*z;
      x += dt*dx;
      y += dt*dy;
      z += dt*dz;
      printf("%5d %8.3f %8.3f %8.3f\n",i+1,x,y,z);
   }
   display();
   return 0;
}
