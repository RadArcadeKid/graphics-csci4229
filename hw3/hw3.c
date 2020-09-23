/*
 * hw3: Scene in 3D
 *
 * Jacob (Jake) Henson - 105963531
 * CSCI4229/5229 Fall 2020
 *
 * A program demonstrating my ability to create, rotate, scale, and draw 3D objects.
 * Also a cute lil scene with some flamingos!
 *
 * Key bindings
 * a          Toggle axes + view angle text
 * arrows     Change view angle
 * 0          Reset view angle
 * ESC        Exit
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

int th=-10;          //  Azimuth of view angle
int ph=40;          //  Elevation of view angle
int axes=0;        //  Display axes toggle

//  Cosine and Sine in degrees
#define Cos(x) (cos((x)*3.1415927/180))
#define Sin(x) (sin((x)*3.1415927/180))

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 * Used from ex8!
 */
#define LEN 8192  //  Maximum length of text string
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
 *  Draw vertex in polar coordinates
 *  NOTE - Used from ex8, BUT updated to work with Flamingo
 */
static void Vertex(double th,double ph)
{
   glColor3f(1.0, 0.0, 1.0 - (0.5 * Sin(th))); //pink sphere w/ really pretty gradient
   glVertex3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
}

/*
 *  Draw vertex in polar coordinates, adapted from the above to draw a cirle
 * This is useful for drawing ponds with DrawCircle
 */
static void VertexCircle(double th,double ph){
   glColor3f(0.0, 0.9, 1.0); //blueCircle
   glVertex3d(Sin(th)*Cos(ph) , 0, Cos(th)*Cos(ph));
}

/*
 *
 *  rawCircle adapted from Sphere2 from ex8
 *   Used to...well, draw a circle (or rather, a pond)
 *  Takes in x and y placement of the circle, as well as its radius
 *    since this scene requires 2d ponds on the floor
 */
static void DrawCircle(double x, double z, double r){
    const int d=15;
    int th,ph;

    //  Save transformation
    glPushMatrix();
    //  Offset and scale
    //y is implicitly -0.01, so that circles don't collide with flamingo feet
    glTranslated(x,-0.01,z);
    glScaled(r,r,r);

    //  Latitude bands
    for (ph=-90;ph<90;ph+=d)
    {
       glBegin(GL_QUAD_STRIP);
       for (th=0;th<=360;th+=d)
       {
          VertexCircle(th,ph);
          VertexCircle(th,ph+d);
       }
       glEnd();
    }

    //  Undo transformations
    glPopMatrix();
}


/*
 * NeckCube - used for drawing the cube for the neck, since it needs to be smaller
 * TODO: add an S curve (perhaps around a sine wave?)
*/
static void NeckCube(double x, double y, double z){
  glColor3f(1,0.6,1);
  glBegin(GL_QUADS);
  //  Front
  glVertex3f(-x,-y, z);
  glVertex3f(+x,-y, z);
  glVertex3f(+x,+y, z);
  glVertex3f(-x,+y, z);
  //  Back
  glVertex3f(+x,-y,-z);
  glVertex3f(-x,-y,-z);
  glVertex3f(-x,+y,-z);
  glVertex3f(+x,+y,-z);
  //  Right
  glVertex3f(+x,-y,+z);
  glVertex3f(+x,-y,-z);
  glVertex3f(+x,+y,-z);
  glVertex3f(+x,+y,+z);
  //  Left
  glVertex3f(-x,-y,-z);
  glVertex3f(-x,-y,+z);
  glVertex3f(-x,+y,+z);
  glVertex3f(-x,+y,-z);
  // Top
  glVertex3f(-x,+y,+z);
  glVertex3f(+x,+y,+z);
  glVertex3f(+x,+y,-z);
  glVertex3f(-x,+y,-z);
}

/*
 *  Flamingo Object - draw the flamingo
 * x,y,z are for object placement
 * r is for radius (and by extension, scale of flamingo)
 *
 */
static void Flamingo(double x, double y, double z, float s, float ry){
    const int d=15;
    int th,ph;

    double r = s * 0.35; //redeclare r so delcaration of Flamingo is normalized

    //Rotate sphere like 15 degrees or so to give that flamingo body look
    glPushMatrix();
    glRotatef(-15,0,0,1); //rotate 15 degrees to get tilted down body

      glPushMatrix();
      //  Offset, scale, and rotate flamingo body
      glTranslated(x,y+1,z);
      glScaled(1.3*r, 0.9 *r ,r);

      //  Latitude bands
      for (ph=-90;ph<90;ph+=d)
      {
         glBegin(GL_QUAD_STRIP);
         for (th=0;th<=360;th+=d)
         {
            Vertex(th,ph);
            Vertex(th,ph+d);
         }
         glEnd();
      }

      glPopMatrix();
    glPopMatrix(); //I'm sure it's really really bad to nest PopMatrix like this...

    //draw feathers
       glPushMatrix();
       // //  Offset
       glTranslated(x,y,z);

       glColor3f(1,0,0.6); //set feather color similar to gradient of back of body
       glBegin(GL_TRIANGLES);
        //useful for offsetting on L/R sides of body
       float z1 = -0.2;
       float z2 = 0.2;
       //draw
       glVertex3f(0.3, 0.8,z1);
       glVertex3f(0.7, 0.5,z1);
       glVertex3f(0.65, 0.9,z1);

       glVertex3f(0.1, 0.8,z1);
       glVertex3f(0.5, 0.5,z1);
       glVertex3f(0.45, 0.8,z1);


       glVertex3f(0.3, 0.8,z2);
       glVertex3f(0.7, 0.5,z2);
       glVertex3f(0.65, 0.9,z2);

       glVertex3f(0.1, 0.8,z2);
       glVertex3f(0.5, 0.5,z2);
       glVertex3f(0.45, 0.8,z2);

       glEnd();
       //  Undo transformations
       glPopMatrix();



       //leggy legs
          glPushMatrix();
          // //  Offset
          glTranslated(x,y,z);

          glColor3f(1,0.6,1); //flamingos should have lighter legs
          glBegin(GL_POLYGON);
          float w = 0.03;
          float h = 1;
          float offset = 0.1; //(in retrospect this is TERRIBLE lol, idk why i did this)

          glVertex3f(0+offset,0,offset);
          glVertex3f(w+offset,0,offset);
          glVertex3f(w+offset,h,offset);
          glVertex3f(0+offset,h,offset);
          glVertex3f(0+offset,h,w+offset);
          glVertex3f(0+offset,h,w+offset);
          glVertex3f(0+offset,0,w+offset);
          glVertex3f(w+offset,h,w+offset);
          glEnd();

          glBegin(GL_POLYGON);
          offset = -0.1;
          glVertex3f(0-offset,0,offset);
          glVertex3f(w-offset,0,offset);
          glVertex3f(w-offset,h,offset);
          glVertex3f(0-offset,h,offset);
          glVertex3f(0-offset,h,w+offset);
          glVertex3f(0-offset,h,w+offset);
          glVertex3f(0-offset,0,w+offset);
          glVertex3f(w-offset,h,w+offset);
          glEnd();

          //draw little triangle feet
          glBegin(GL_TRIANGLES);
          glColor3f(1,0.6,1); //should be same as legs, but just in case...
          float feet_offset = 0.12;
          glVertex3f(0, 0, 0.1 + feet_offset);
          glVertex3f(0, 0, -0.1 +feet_offset);
          glVertex3f(0.15, 0, 0 +feet_offset);

          glVertex3f(0, 0, 0.1 -feet_offset);
          glVertex3f(0, 0, -0.1 -feet_offset);
          glVertex3f(0.15, 0, 0 -feet_offset);
          glEnd();


          //draw big neck
          glTranslated(x,y+1.5,z);
          glScaled(0.5, 0.5, 0.5);
          glBegin(GL_QUAD_STRIP);
              glColor3f(1,0,1);
              NeckCube(0.05, 1, 0.05); //delegated to its own function for simplicity
          glEnd();

          //draw head
          glTranslated(x-0.25,y+1,z);
          glScaled(1.1*r, 0.8 *r ,r);
          for (ph=-90;ph<90;ph+=d)
          {
             glBegin(GL_QUAD_STRIP);
             for (th=0;th<=360;th+=d)
             {
                Vertex(th,ph);
                Vertex(th,ph+d);
             }
             glEnd();
          }

          //TODO: draw eyes?


          //draw beak
          glTranslated(x-2.2,y-1,z);
          glScaled(s, s, s);

          glBegin(GL_TRIANGLES);
          glColor3f(1,1,1);
          glVertex3f(0.3, 0.4,0.0);
          glVertex3f(1.5, 0.2,0.0);
          glVertex3f(1.5, 1.5,0.0);
          glEnd();

   glPopMatrix();
}

/*
 * Wrapper function to draw flamingos since i somehow messed up the object along the way
 * Yes, I know this a VERY sloppy way of doing this, but for the time being I'll leave it here
 */
static void DrawFlamingos(double x, double y, double z, double s, float rot, double yscale){
   glPushMatrix();
   glTranslated(x,y,z);
   glRotatef(rot,0,1,0); //rotate flamingo around y axis
   glScaled(s,s * yscale,s); //scale the flamingo depending on input
   Flamingo(0,0,0,   1.0,   s);
   glPopMatrix();
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   glClearColor(0.0f, 0.45f, 0.34f, 1.0f); //change background color!

   const double len=1.5;  //  Length of axes
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();
   //  Set view angle
   glRotatef(ph,1,0,0);
   glRotatef(th,0,1,0);

   //Draw pond(s)
   DrawCircle(-0.5, 0, 1);
   DrawCircle(0.5, 0, 0.75);
   DrawCircle(-0.75, 0.75, 0.6);


   //Draw Flamingos!
   DrawFlamingos(1,0,-1,   0.5, 50, 1);
   DrawFlamingos(0,0,-1,   0.6, 85, 1);

   DrawFlamingos(-1.5,0,-0.2,   0.6, 160, 0.8);
   //DrawFlamingos(-1.6,0,-0.1,   0.6, 180, 1); //too many of them!


   DrawFlamingos(0,0,1,   0.35, -40, 1.2);
   DrawFlamingos(0.7,0,1.2,   0.4, 270, 1);


   //  White
   glColor3f(1,1,1);
   //  Draw axes
   if (axes)
   {
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   //  Five pixels from the lower left corner of the window
   glWindowPos2i(5,5);
   //  Print the text string
      Print("Angle=%d,%d",th,ph);
   //  Render the scene
   }

   glFlush();
   //  Make the rendered scene visible
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
    //TODO: Add/remove respective key bindings
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0'){
     th = -10;
     ph = 40;
   }

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
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitWindowSize(600,600);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   //  Create the window
   glutCreateWindow("Jake Henson - HW3");
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
   return 0;
}
