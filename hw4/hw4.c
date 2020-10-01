/*
 * hw4: Projections
 *
 * Jacob (Jake) Henson - 105963531
 * CSCI4229/5229 Fall 2020
 *
 * A program demonstrating my ability to create, rotate, scale, and draw 3D objects.
 * Also a cute lil scene with some flamingos!
 *
 * Key bindings
 * m/M               - Switch between viewing modes (Orthographic, Perspective, First-Person)
 * W, A, S, D        - Move Forward, Backward, Left, Right respectively (first-person only)
 * shift + W,A,S,D   - Move faster/slower depending on if inputs are capitalized (first-person only)
 * 8                 - Decrease first-person camera view sensitivity (first-person only)
 * 9                 - Increase first-person camera sensitivity (first-person only)
 * o/O               - Decrease field of view
 * p/P               - Increase  field of view
 * 1 (left bracket)  - Decrease world size (disabled in first-person)
 * ] (right bracket) - Increase world size (disabled in first-person)
 * arrow keys        - Change view angle
 * 0                 - Reset view angle
 * ESC               - Exit
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


int th_alt=-135;  //  Azimuth of view angle (and first person version)
int ph_alt=0;  //  Elevation of view angle (and first person version)

int th=-10;          //  Azimuth of view angle
int ph=40;          //  Elevation of view angle
double zh=0;       //  Rotation
int mode = 0;       //projection mode
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=2.5;    //World size

double fp_x = 1.0; //first person coordinates
double fp_y = 1.0;
double fp_z = 1.0;

double sensitivity = 2.4; //first person arrow key sensitivity

//int axes=0;        //  Display axes toggle
const char* text[] = {"Orthographic","Perspective","First-Person"};


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
 *  Set projection
 */
static void Project()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   if (mode)
      gluPerspective(fov,asp,dim/4,4*dim);

      //gluPerspective(fov,asp,dim/4,4*dim);
   //  Orthogonal projection
   else
      glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
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
 *  Draw vertex in polar coordinates
 *  NOTE - Used from ex8, BUT updated to work with beach ball
 */
static void VertexBB(double th,double ph)
{
   glVertex3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
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


static void DrawBeachBall(double x,double y,double z,double r, double zh)
{
   const int d=15;
   int th,ph;

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,r);
   glRotatef(zh,0,0.75,1); //rotate beachball

   //  Latitude bands
   for (ph=-90;ph<90;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=d)
      {
        //add beachball colors
         glColor3f(0.000, 0.7, 0.7);
         if(th < 60)
            glColor3f(0.000, 1.000, 0.498);

         if(th > 220)
            glColor3f(0.933, 0.510, 0.933);

         VertexBB(th,ph);
         VertexBB(th,ph+d);
      }
      glEnd();
   }

   //  Undo transformations
   glPopMatrix();
}


/*
 * NeckCube - used for drawing the cube for the neck, since it needs to be smaller
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
              NeckCube(0.05, 1.1, 0.05); //delegated to its own function for simplicity
          glEnd();


          //Rotate head depending on size
          if(ry < 0.6)
            glRotatef(25*Sin(zh),0,1,0); //rotate 15 degrees to get tilted down body

          else
            glRotatef(3.5*Sin(zh),0,1,1); //rotate 15 degrees to get tilted down body


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


          //draw beak
          glTranslated(x-2.2,y-1,z);
          glScaled(s, s, s);

          glBegin(GL_TRIANGLES);
          glColor3f(1,1,1);
          glVertex3f(0.3, 0.4,0.0);
          glVertex3f(1.5, 0.2,0.0);
          glVertex3f(1.5, 1.5,0.0);
          glEnd();

          //END rotation
          glRotatef(-15,0,0,1); //rotate 15 degrees to get tilted down body

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

   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();

   if (mode == 1) //projection
   {
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   }
   else if (mode == 2) //FIRST PERSON CAMERA
       gluLookAt(fp_x, fp_y, fp_z,
       Cos(th_alt) + fp_x,
       Sin(ph_alt) + fp_y,
       Sin(th_alt) + fp_z,0,1,0);
   else //ortho
   {
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
   }

   //Draw pond(s)
   DrawCircle(-0.5, 0, 1);
   DrawCircle(0.5, 0, 0.75);
   DrawCircle(-0.75, 0.75, 0.6);


   //Draw beachball!
   float bb_float_y = 0.05 * Sin(zh);
   float bb_float_z = 0.05 * Sin(zh);
   DrawBeachBall(0,bb_float_z,bb_float_y, 0.2, 2*zh);

   //Draw Flamingos!
   DrawFlamingos(1,0,-1,   0.5, -zh, 1);
   DrawFlamingos(0,0,-1,   0.6, 85, 1);

   DrawFlamingos(-1.5,0,-0.2,   0.6, 160, 0.8);

   DrawFlamingos(0,0,1,   0.35, -40, 1.2);
   DrawFlamingos(0.7,0,1.2,   0.4, 270, 1);

   DrawFlamingos(1.5,0,1.5,   0.5, 355, 1);
   DrawFlamingos(-1.5,0,-1.5,   0.4, 185, 1);


   //  White
   glColor3f(1,1,1);
   //  Five pixels from the lower left corner of the window
   glWindowPos2i(5,5);

   //Print parameters depending on view
   if(mode == 2) //first person; separate print statement for alt angles
     Print("Angle=%d,%d  Sensitivity=%.1f FOV=%d Mode=%s",th_alt,ph_alt,sensitivity,fov,text[mode]);

   else     //ortho/projection
     Print("Angle=%d,%d  Dim=%.1f FOV=%d Mode=%s",th,ph,dim,fov,text[mode]);

   //  Render the scene

   glFlush();
   //  Make the rendered scene visible
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
  //Create an alternate scheme to rotate the angle around, so that the arrow keys control FPS perspective propely
  if(mode == 2){ //FIRST PERSON
      //  Right arrow key - increase angle by 5 degrees
      if (key == GLUT_KEY_RIGHT)
        th_alt += sensitivity;
      //  Left arrow key - decrease angle by 5 degrees
      else if (key == GLUT_KEY_LEFT)
        th_alt -= sensitivity;
      //  Up arrow key - increase elevation by 5 degrees
      else if (key == GLUT_KEY_UP && ph_alt<100) //RESTRICT movement so the player can't disorient themselves too much
        ph_alt += sensitivity;
      //  Down arrow key - decrease elevation by 5 degrees
      else if (key == GLUT_KEY_DOWN && ph_alt>-100)
        ph_alt -= sensitivity;
      //  Keep angle to +/-360 degrees
      th_alt %= 360;
  }
  else //otherwise, rotate normally around the scene
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
  }
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project();
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
   else if (ch == '0'){
     th = -10;
     ph = 40;
     th_alt=-135; //reset first-person camera, too
     ph_alt=0;
     sensitivity = 2.5;
   }   //Change perspective mode
   else if (ch == 'm' || ch == 'M'){
     mode = (mode+1)%3;
   }

   //  Change field of view angle
   else if (ch == 'o' && ch>1)
      fov--;
   else if (ch == 'p' && ch<179)
      fov++;

   //  Right bracket key - increase dim
   else if (ch == ']' && dim <5 && mode != 2)
        dim += 0.1;
   //  Left bracket key - decrease dim
   else if (ch == '[' && dim>1 && mode != 2)
        dim -= 0.1;


   //if first person mode, allow for controls to navigate around the scene:
   if(mode == 2){
     //walk
     if (ch == 'w'){ //FORWARD
        fp_x += 0.1*Cos(th_alt);
        fp_z += 0.1*Sin(th_alt);
     }
     else if (ch == 's'){ //BACKWARD
        fp_x -= 0.1*Cos(th_alt);
        fp_z -= 0.1*Sin(th_alt);
     }
     else if (ch == 'a'){ //LEFT
        fp_x += 0.1*Sin(th_alt);
        fp_z -= 0.1*Cos(th_alt);
     }
     else if (ch == 'd'){ //RIGHT
        fp_x -= 0.1*Sin(th_alt);
        fp_z += 0.1*Cos(th_alt);
     }

     //////////// move faster if shift key is held (really just caps lock, but it works )
     else if (ch == 'W'){ //FORWARD
        fp_x += 0.2*Cos(th_alt);
        fp_z += 0.2*Sin(th_alt);
     }
     else if (ch == 'S'){ //BACKWARD
        fp_x -= 0.2*Cos(th_alt);
        fp_z -= 0.2*Sin(th_alt);
     }
     else if (ch =='A'){ //LEFT
        fp_x += 0.2*Sin(th_alt);
        fp_z -= 0.2*Cos(th_alt);
     }
     else if (ch == 'D'){ //RIGHT
        fp_x -= 0.2*Sin(th_alt);
        fp_z += 0.2*Cos(th_alt);
     }
     ////////////


     //First person - adjust camera sensitivity
     else if (ch == '8' && sensitivity > 1.0)
        sensitivity -= 0.2;

     else if (ch == '9' && sensitivity < 7)
        sensitivity += 0.2;

  }

   //  Reprojection
   Project();
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized; newer reshape function
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project();
}

/*
 *  GLUT calls this toutine when there is nothing else to do
 */
void idle()
{
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,360);
   glutPostRedisplay();
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
   glutCreateWindow("HW4 Projections: Jake Henson");

   //Idle:
   glutIdleFunc(idle);

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
