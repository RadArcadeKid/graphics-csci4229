  /*
 * FINAL PROJECT
 * Jacob (Jake) Henson - 105963531
 * CSCI4229/5229 Fall 2020
 *
 * A program demonstrating my ability to light a scene with custom objects
 *  Key bindings
*   a/A        Decrease/increase ambient light
*   d/D        Decrease/increase diffuse light
*   s/S        Decrease/increase specular light
*   e/E        Decrease/increase emitted light
*   n/N        Decrease/increase shininess
*   F1         Toggle smooth/flat shading
*   F2         Toggle local viewer mode
*   F3         Toggle light distance (1/5)
*   F8         Change ball increment
*   F9         Invert bottom normal
*   m          Toggles light movement
*   p          Toggles ortogonal/perspective projection
*   o          Cycles through objects
*   ]/[        Change light azimuth
*   h/H        Change light height
*   8/5        Raise/lower light
*   +/-        Change field of view of perspective
*   x          Toggle axes
*   arrows     Change view angle
*   0          Reset view angle
*   ESC        Exit

 */
#include "CSCIx229.h"

#define PI (3.1415927)

int axes=0;       //  Display axes start with off
int mode=1;       //  Projection mode
int th=-40;         //  Azimuth of view angle
int ph=30;         //  Elevation of view angle
int move = 1;      //move light
int fov=55;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=3;     //  Size of world
// Light values
int one       =   1;  // Unit value
int distance  =   5;  // Light distance
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  20;  // Ambient intensity (%)
int diffuse   =  50;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   1;  // Elevation of light



int ball_ph = 0;
int ball_th = 0;

double ball_x, ball_z = 0.0;

//Texture array:
unsigned int texture[8]; // Texture names


typedef struct {float x,y,z;} vtx;
typedef struct {int A,B,C;} tri;
#define n 500
vtx is[n];

/*
 *  Draw vertex in polar coordinates
 */
static void Vertex(int th,int ph)
{
   double x = Cos(th)*Cos(ph);
   double y = Sin(th)*Cos(ph);
   double z =         Sin(ph);
   glNormal3d(x,y,z);
   glTexCoord2d(th/360.0,ph/180.0+0.5); //TODO: FIX TEXTURE

   glVertex3d(x,y,z);
}

/*
 *  Draw marble
 */
void DrawMarble(double x,double y,double z, double s, double th, double ph)
{

   /*
    *  Draw surface of the planet
    */
   //  Set texture
   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(ph,1,0,0);
   glRotated(th,0,0,1);


   glScaled(s,s,s);

   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D,texture[0]); //flashy 90s pattern
   //  Latitude bands
   glColor3f(1,1,1);
   for (ph=-90;ph<90;ph+=5)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=5)
      {
         Vertex(th,ph);
         Vertex(th,ph+5);
      }
      glEnd();
   }
   glDisable(GL_TEXTURE_2D);

   glPopMatrix();
}

/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void cube(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th) //todo -- add
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx, dy, dz);

   int ts = 2;

   double sx = ts*dx;
   double sy = ts*dy;
   double sz = ts*dz;


   glBegin(GL_QUADS);
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
   glTexCoord2f(sx,0); glVertex3f(+1,-1, 1);
   glTexCoord2f(sx,sy); glVertex3f(+1,+1, 1);
   glTexCoord2f(0,sy);  glVertex3f(-1,+1, 1);

   //  Back
   glNormal3f( 0, 0,-1);
   glTexCoord2f(0,0);  glVertex3f(+1,-1,-1);
   glTexCoord2f(sx,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(sx,sy); glVertex3f(-1,+1,-1);
   glTexCoord2f(0,sy); glVertex3f(+1,+1,-1);
   //  Right
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0,0);   glVertex3f(+1,-1,+1);
   glTexCoord2f(sz,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(sz,sy); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,sy); glVertex3f(+1,+1,+1);
   //  Left
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(sz,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(sz,sy); glVertex3f(-1,+1,+1);
   glTexCoord2f(0,sy); glVertex3f(-1,+1,-1);
   //  Top
   glNormal3f( 0,+1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
   glTexCoord2f(sx,0); glVertex3f(+1,+1,+1);
   glTexCoord2f(sx,sz); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,sz); glVertex3f(-1,+1,-1);
   //  Bottom
   glNormal3f( 0,-one, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(sx,0);  glVertex3f(+1,-1,-1);
   glTexCoord2f(sx,sz); glVertex3f(+1,-1,+1);
   glTexCoord2f(0,sz); glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   //  Undo transofrmations
   glPopMatrix();
}

//TODO: remove ball from source code
/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 */
static void ball(double x,double y,double z,double r)
{
   int th,ph;
   float yellow[] = {1.0,1.0,0.0,1.0};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  White ball
   glColor3f(1,1,1);
   glMaterialf(GL_FRONT,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   //  Bands of latitude
   for (ph=-90;ph<90;ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=2*inc)
      {
         Vertex(th,ph);
         Vertex(th,ph+inc);
      }
      glEnd();
   }
   //  Undo transofrmations
   glPopMatrix();
}


// /*
//  *  DrawCyliner - useful for sides of controller + buttons
//  *  h == height; r == radius
//  */
// static void DrawCylinder(double x,double y,double z, double delta_h, double s){
//   const double d=0.05;
//   float r = 0.9;
//   double h = 0.201 + delta_h;
//
//   //  Save transformation
//   glPushMatrix();
//   //  Offset, scale and rotate
//   glTranslated(x,y,z);
//   glScaled(s,s,s);
//
//   //draw sides of cylinder
//   glBegin(GL_QUAD_STRIP);
//   //glNormal3d(0.0, h, 0.0);
//     for(float i = 0; i <= 2.1*PI; i+=d){
//       glNormal3d(r * cos(i), 0, r * sin(i));
//       const float tc = ( i / (float)( 2 * PI ));
//       glTexCoord2f(tc, - h * s);
//       glVertex3f(r * cos(i), -h, r * sin(i));
//       glTexCoord2f(tc,  h* s);
//       glVertex3f(r * cos(i), h, r * sin(i));
//     }
//   glEnd();
//
//   glNormal3d(0,+1,0); //reset normals
//
//
//   glBegin(GL_TRIANGLE_FAN);
//         glTexCoord2f( 0.5, 0.5 );
//         glVertex3f(0, h, 0);  /* center */
//         for (double i = 2 * PI; i >= 0; i -= d)
//
//         {
//             glTexCoord2f( 0.5f * cos(i) + 0.5f, 0.5f * sin(i) + 0.5f );
//             glVertex3f(r * cos(i), h, r * sin(i));
//         }
//         /* close the loop back to 0 degrees */
//         glTexCoord2f( 0.5, 0.5 );
//         glVertex3f(r, h, 0);
//     glEnd();
//
//     glNormal3d(0,-1,0); //reset normals
//
//
//     /* bottom triangle: note: for is in reverse order */
//     glBegin(GL_TRIANGLE_FAN);
//         glTexCoord2f( 0.5, 0.5 );
//         glVertex3f(0, -h, 0);  /* center */
//         for (double i = 0; i <= 2 * PI; i += d)
//         {
//             glTexCoord2f( 0.5f * cos(i) + 0.5f, 0.5f * sin(i) + 0.5f );
//             glVertex3f(r * cos(i), -h, r * sin(i));
//         }
//     glEnd();
//
//
//
//   //Shoutout to the kind soul on StackOverflow who happened to have a texture map which worked PERFECTLY
//   //with my implementation of the cylinder -- source: https://stackoverflow.com/questions/26536570/how-do-i-texture-a-cylinder-in-opengl-created-with-triangle-strip
//
//
//
//   //  Undo transformations
//   glPopMatrix();
// }

static void Ground(double x, double y, double z, double s){
  glPushMatrix();
  //  Offset, scale and rotate
  glTranslated(x,y,z);
  glScaled(s,s,s);


  double gridsize = s;

  //  Enable textures
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
  //glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D,texture[1]); //carpet
  glBegin(GL_QUADS);
  glColor3f(0.125, 0.698, 0.667);
  glNormal3f( 0,+1, 0);
  glTexCoord2f(0,0); glVertex3f(-1,0,+1);
  glTexCoord2f(gridsize,0); glVertex3f(+1,0,+1);
  glTexCoord2f(gridsize,gridsize); glVertex3f(+1,0,-1);
  glTexCoord2f(0,gridsize); glVertex3f(-1,0,-1);
  glEnd();
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   // glPushMatrix();
   // glTranslated(ball_x, 0, ball_z);
   // glPopMatrix();

   //TODO: background image

   //TODO: REMOVE AXES
   const double len=2.0;  //  Length of axes
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);

   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective - set eye position
   if (mode) //REMOVE THIS
   {
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      //gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
      Print("Ex, Ez=%f,%f  ", Ex,Ez);
      gluLookAt(Ex+ball_x,Ey, Ez+ball_z, ball_x,0,ball_z , 0,Cos(ph),0);

   }
   //  Orthogonal - set world orientation
   else //TODO: REMOVE ORTHO PERSPECTIVE
   {
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
   }

   //  Flat or smooth shading
   glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

      //  Translate intensity to color vectors
      float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0}; //todo: change these into normal value
      float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
      float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
      //  Light position
      float Position[]  = {distance*Cos(zh),ylight,distance*Sin(zh),1.0};
      //  Draw light position as ball (still no lighting here)
      glColor3f(1,1,1);
      //ball(1, 1, 1, 0.1);
      ball(Position[0],Position[1],Position[2] , 0.1);

      //  OpenGL should normalize normal vectors
      glEnable(GL_NORMALIZE);
      //  Enable lighting
      glEnable(GL_LIGHTING);
      //  Location of viewer for specular calculations
      glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
      //  glColor sets ambient and diffuse color materials
      glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);
      //  Enable light 0
      glEnable(GL_LIGHT0);
      //  Set ambient, diffuse, specular components and position of light 0
      glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
      glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
      glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
      glLightfv(GL_LIGHT0,GL_POSITION,Position);

   //Display objects!!

   //Display ground
   Ground(0, 0,0, 1);
   Ground(0, 0,-2, 1);


   DrawMarble(ball_x,0.3,ball_z,  0.3, ball_th, ball_ph);

   //  Draw axes - no lighting from here on
   glDisable(GL_LIGHTING);
   glColor3f(1,1,1);
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
   }

   //  Display parameters //TODO: REMOVE ME
    glWindowPos2i(5,5);
   // Print("Angle=%d,%d  FOV=%d Projection=%s Light=%s",
   //   th,ph,fov,mode?"Perpective":"Orthogonal",light?"On":"Off");
   // if (light)
   // {
   //    glWindowPos2i(5,45);
   //    Print("Model=%s LocalViewer=%s Distance=%d Elevation=%.1f",smooth?"Smooth":"Flat",local?"On":"Off",distance,ylight);
   //    glWindowPos2i(5,25);
   //    Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f",ambient,diffuse,specular,emission,shiny);
   //    Print("ylight = %d", ylight);
   //
   // }


   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when idling
 */
void idle()
{
   //  Elapsed time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,360.0);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
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
   //  Smooth color model
   else if (key == GLUT_KEY_F1)
      smooth = 1-smooth;
   //  Local Viewer
   else if (key == GLUT_KEY_F2)
      local = 1-local;
   else if (key == GLUT_KEY_F3)
      distance = (distance==1) ? 5 : 1;
   //  Toggle ball increment
   else if (key == GLUT_KEY_F8)
      inc = (inc==10)?3:10;
   //  Flip sign
   else if (key == GLUT_KEY_F9)
      one = -one;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project(mode?fov:0,asp,dim);
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
      th = -35;
      ph = 40;
  }
   //  Toggle axes
   else if (ch == 'x' || ch == 'X')
      axes = 1-axes;
   //  Switch projection mode
   else if (ch == 'p' || ch == 'P') //REMOVE PROJECTION MODE SWITCH
      mode = 1-mode;
   // //  Ambient level
   // else if (ch=='3' && ambient>0) //TODO: MAYBE PLAY WITH AMBIENT LIGHT A BIT??
   //    ambient -= 5;
   // else if (ch=='4' && ambient<100)
   //    ambient += 5;
   //  Diffuse level
   else if (ch=='v' && diffuse>0)
      diffuse -= 5;
   else if (ch=='V' && diffuse<100)
      diffuse += 5;
   //  Move light
   else if (ch == '+')
      zh += 1;
   else if (ch == '-')
      zh -= 1;
   // light height
   else if (ch == 'H') //REMOVE ME
      ylight += 0.5;
   else if (ch == 'h')
      ylight -= 0.5;
  // lfov
  else if (ch == ']')
      fov += 1;
  else if (ch == '[')
      fov -= 1;

  //  Toggle light movement
  else if (ch == 'm' || ch == 'M') //REMOVE ME
      move = 1-move;


  ball_th %= 360;
  ball_ph %= 360;

  ////MARBLE MOVEMENT:
  int speed = 10;
  double movement = 0.04;
  if (ch == 'w'){ //FORWARD
     ball_ph -= speed;
     ball_z -= movement;
  }
  else if (ch == 's'){ //BACKWARD
     ball_ph += speed;
     ball_z += movement;
  }
  else if (ch == 'a'){ //LEFT
     ball_th += speed;
     ball_x -= movement;
  }
  else if (ch == 'd'){ //RIGHT
     ball_th -= speed;
     ball_x += movement;
  }


   //  Specular level
   else if (ch=='s' && specular>0)
      specular -= 5;
   else if (ch=='S' && specular<100)
      specular += 5;
   //  Emission level
   else if (ch=='e' && emission>0)
      emission -= 5;
   else if (ch=='E' && emission<100)
      emission += 5;
   //  Shininess level
   else if (ch=='n' && shininess>-1)
      shininess -= 1;
   else if (ch=='N' && shininess<7)
      shininess += 1;
   //  Translate shininess power to value (-1 => 0)
   shiny = shininess<0 ? 0 : pow(2.0,shininess);
   //  Reproject
   Project(mode?fov:0,asp,dim);
   //  Animate if requested
   glutIdleFunc(move?idle:NULL);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project(mode?fov:0,asp,dim);
}

/*
 *  Random numbers from min to max to the power p
 */
static float frand(float min,float max,float p)
{
   return pow(rand()/(float)RAND_MAX,p)*(max-min)+min;
}

/*
 * Initialize icosasphere locations
 */
void Init()
{
   for (int i=0;i<n;i++)
   {
      float th = frand(0,360,1);
      float ph = frand(-90,+90,1);
      float r  = frand(0.1,0.7,3);
      is[i].x = r*Sin(th)*Cos(ph);
      is[i].y = r*Cos(th)*Cos(ph);
      is[i].z = r*Sin(ph) + 1.0;
   }
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize
   Init();
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(600,600);
   glutCreateWindow("HW6: Textures, Jake Henson");
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutIdleFunc(idle);
   glutIdleFunc(idle);

   //  Load textures
   texture[0] = LoadTexBMP("90s_2_sphere.bmp"); //flashy 90s texture
   texture[1] = LoadTexBMP("checker.bmp");
   texture[2] = LoadTexBMP("concrete.bmp");
   texture[3] = LoadTexBMP("error_1.bmp");
   texture[4] = LoadTexBMP("error_2.bmp");
   texture[5] = LoadTexBMP("palm_bark.bmp");
   texture[6] = LoadTexBMP("palm.bmp");
   texture[7] = LoadTexBMP("placeholder_water.bmp");
   //TODO: find more textures


   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}
