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
int th=0;         //  Azimuth of view angle
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
float zoom = 2.0;



int ball_ph = 0;
int ball_th = 0;

int forward, sideways = 0;

double ball_x, ball_z = 0.0;

//Texture array:
unsigned int texture[9]; // Texture names
unsigned int water_texture[7]; // water textures

int speed = 12;
double movement = 0.07;

//sloppy way of defining number of cans in the level
//so I don't have to update it everytime...probably not best practice
#define numcans 3

//array to determine whether the user has collected a can or not
int collected_cans[numcans] = {0, 0, 0};

//places to store x and z coordinates of cans
double cans_x[numcans] = {0.0,  -2.0, -4.0};
double cans_z[numcans] = {-2.0, -2.0, -4.0};



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
    *  Draw surface of  marble
    */
   //  Set texture
   glPushMatrix();
   glTranslated(x,y,z);
   if(forward){ //running into issues with glPopMatrix so I just added a bool here instead
      glRotated(ph,1,0,0);
      glRotated(th,0,0,1);
    }
   else if(sideways){
      glRotated(th,0,0,1);
      glRotated(ph,1,0,0);
    }


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
                 double th){
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

//TODO: remove ball?
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


/*
 *  DrawTeaCan - draws an Arizona tea can
 */
static void DrawTeaCan(double x,double y,double z, double delta_h, double s){
  const double d=0.25;
  float r = 0.4;
  //double h = 0.201 + delta_h;
  double h = delta_h;


  //  Save transformation
  glPushMatrix();
  //  Offset, scale and rotate
  glTranslated(x,y,z);
  float sm = 0.8;
  glScaled(sm*s, sm*s, sm*s);
  glRotated(10, 0, 0, 1);


  glColor3f(1, 1, 1);

  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
  //glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D,texture[7]); //teacan!


  //draw sides of cylinder
  glBegin(GL_QUAD_STRIP);
  //glNormal3d(0.0, h, 0.0);
    for(float i = 0; i <= 2.1*PI; i+=d){
      glNormal3d(r * cos(i), 0, r * sin(i));
      const float tc = ( i / (float)( 2 * PI ));
      glTexCoord2f(-tc, 0);

      glVertex3f(r * cos(i), -h, r * sin(i));
      glTexCoord2f(-tc, 2*h*s);

      glVertex3f(r * cos(i), h, r * sin(i));
    }
  glEnd();

  glNormal3d(0,+1,0); //reset normals

  glBindTexture(GL_TEXTURE_2D,texture[8]); //teacan top/bottom!

  glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f( 0.5, 0.5 );
        glVertex3f(0, h, 0);  /* center */
        for (double i = 2 * PI; i >= 0; i -= d)

        {
            glTexCoord2f( 0.5f * cos(i) + 0.5f, 0.5f * sin(i) + 0.5f );
            glVertex3f(r * cos(i), h, r * sin(i));
        }
        /* close the loop back to 0 degrees */
        glTexCoord2f( 0.5, 0.5 );
        glVertex3f(r, h, 0);
    glEnd();

    glNormal3d(0,-1,0); //reset normals


    /* bottom triangle: note: for is in reverse order */
    glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f( 0.5, 0.5 );
        glVertex3f(0, -h, 0);  /* center */
        for (double i = 0; i <= 2 * PI; i += d)
        {
            glTexCoord2f( 0.5f * cos(i) + 0.5f, 0.5f * sin(i) + 0.5f );
            glVertex3f(r * cos(i), -h, r * sin(i));
        }
    glEnd();

    glDisable(GL_TEXTURE_2D);


  //  Undo transformations
  glPopMatrix();
}



/*
 *  DrawPillarTube - draws the pillar tube for the pillar
 */
static void DrawPillarTube(double x,double y,double z, double h, double s){
  const double d=0.30;

  float r = 0.7;

  //  Save transformation
  glPushMatrix();
  //  Offset, scale and rotate
  glTranslated(x,y,z);

  //draw sides of cylinder
  glBegin(GL_QUAD_STRIP);
  //glNormal3d(0.0, h, 0.0);
    for(float i = 0; i <= 2.1*PI; i+=d){
      glNormal3d(r * cos(i), 0, r * sin(i));
      const float tc = ( i / (float)( 2 * PI ));
      glTexCoord2f(2*tc, 0);

      glVertex3f(r * cos(i), -h, r * sin(i));
      glTexCoord2f(2*tc, h*s);

      glVertex3f(r * cos(i), h, r * sin(i));
    }
  glEnd();

  //  Undo transformations
  glPopMatrix();
}


/*
 * Ground - draws the track
 */
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
  glColor3f(0.933, 0.510, 0.933);
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
  * Water - draws the animated water
  */
static void Water(double x, double y, double z, int ct){
  int s = 2;
  glPushMatrix();
  //  Offset, scale and rotate
  glTranslated(x,y,z);
  glScaled(s,s,s);


  double gridsize = s;

  //  Enable textures
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
  glBindTexture(GL_TEXTURE_2D, water_texture[ct]); //TODO: animated water
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
 * Draw the water floor!!
 */
void DrawWaterFloor(){
  glPushMatrix();
  int gridsize = 10;
  glTranslated(0, -6, 0);
  glScaled(2, 0, 2);

  int ct = zh / 30;
  ct %= 7;

  for(int i = -gridsize; i < gridsize; i+=2){
    for(int j = -gridsize; j < gridsize; j+=2){
      Water(i, 0, j, ct);
    }
  }
  glPopMatrix();
}

/*
 * Draw Floating Can draws the Arizona tea can, but floating and spinning like a powerup in a video game
 */
void SetFloatingCan(double x, double y, double z){
  glPushMatrix();
  glTranslated(x,y,z);
  glRotated(zh, 0, 1, 0);

  DrawTeaCan(0, 0.5+0.1*Sin(zh), 0,  1.0,  0.5);

  glPopMatrix();
}

void Reset(){
  th = 0;
  ph = 30;
  ball_x = 0;
  ball_z = 0;
}

/*
 * Will Draw collectabless based on if they've been picked up or not
 */
void DrawCollectables(){
  for(int i = 0; i < numcans; i++){
      if(collected_cans[i] == 0){ //if the can has NOT been collected
        SetFloatingCan(cans_x[i], 0.0, cans_z[i]); //draw the can
      }
  }
}

/*
 * Will determine whether the player has picked up a can
 * Called when the marble moves!
 */
void CheckPickup(){
  float range = 0.3;

  for(int i = 0; i < numcans; i++){
    //check if ball is within pickup range of can
    if(ball_x >= cans_x[i]-range && ball_x <= cans_x[i]+range){
      if(ball_z >= cans_z[i]-range && ball_z <= cans_z[i]+range){
         collected_cans[i] = 1; //set the can to picked up so it no longer displays
         //TODO: sound??
      }
    }
  }
}

/*
 * Draws a concrete pillar
 *
 */
void DrawPillar(double x, double y, double z, double h){
    //add concrete texture
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D,texture[2]); //concrete!
    glColor3f(1.0, 1.0, 1.0);
    float scale = 0.3;
    glTranslated(x, y+0.02, z);
    glScalef(scale, scale, scale);



    cube(x, y, z, 1, 0.1, 1, 0.0);
    cube(x, y+0.17, z, 0.8, 0.1, 0.8, 0.0);

    DrawPillarTube(x, y+(h/2), z, (h/2), 1.0);


    cube(x, y+h, z, 0.8, 0.1, 0.8, 0.0);
    cube(x, y+h+0.17, z, 1, 0.1, 1, 0.0);


    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}


void DrawLeaf(double x, double y, double z){
  glPushMatrix();
  //  Offset, scale and rotate
  glTranslated(x,y-2,z);
  glScaled(0.5,0,1);

  float tex_size = 1;
  //  Enable textures
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D,texture[6]); //fern leaf
  glBegin(GL_QUADS);
  //glColor3f(0.933, 0.510, 0.933);
  glNormal3f( 0,+1, 0);
  glTexCoord2f(0,0); glVertex3f(-1,0,+1);
  glTexCoord2f(tex_size,0); glVertex3f(+1,0,+1);
  glTexCoord2f(tex_size,tex_size); glVertex3f(+1,0,-1);
  glTexCoord2f(0,tex_size); glVertex3f(-1,0,-1);
  glEnd();
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();

}

/*
 *  Draws a fern object
 */
void DrawFern(double x, double y, double z){
  glPushMatrix();

  //draw leaves
  glTranslated(x, y+0.02, z);
  DrawLeaf(0, 0, 0);



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
   double Ex = -zoom*dim*Sin(th)*Cos(ph);
   double Ey = +zoom*dim        *Sin(ph);
   double Ez = +zoom*dim*Cos(th)*Cos(ph);
   Print("Ex, Ez=%f,%f,", Ex,Ez);
   gluLookAt(Ex+ball_x,Ey, Ez+ball_z, ball_x,0,ball_z , 0,Cos(ph),0);

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

   //Display ground

   DrawWaterFloor();


   //Instead of declaring these as objects, perhaps use an array
   //With given bounds, and then create those objects relative to the array
   Ground(0, 0,0, 1);
   Ground(0, 0,-2, 1);
   Ground(-2, 0,-2, 1);
   Ground(-4, 0,-2, 1);
   Ground(-4, 0,-4, 1);



   DrawMarble(ball_x,0.2,ball_z,     0.2, ball_th, ball_ph);
    //TODO: TURN ME INTO A GRID FUNCTION TO DETERMINE IF BALL IS OFFBASE
   Print("ball_x, ball_z = %f, %f", ball_x, ball_z); //TODO:REMOVE ME
   if(ball_x < -5){ //TODO: TURN THIS INTO A TRIGGER
     Reset();
   }

   DrawCollectables();


   double pilht = 4.0;

   DrawPillar(0,0,0,  pilht); //REMOVE ME
   DrawFern(0, pilht, 0);


   //Draw objects
   DrawPillar(-1,0,-0.6,   pilht);
   DrawPillar(1,0,-0.6,   pilht);

   DrawPillar(-4,0,-2.5,   pilht);
   DrawPillar(-2.1,0,-2.5,   pilht);



   //  Draw axes - no lighting from here on
   glDisable(GL_LIGHTING);
   glColor3f(1,1,1);
   if (axes) //TODO: remove axes
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
   double time = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*time,360.0);

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
   CheckPickup(); //determine if the can was picked up


   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0'){
      th = 0;
      ph = 30;
      zoom = 2.0; //reset zoom too
  }
   //  Toggle axes //TODO: REMOVE ME
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
   //  Move camera
   else if (ch == '+')
      zoom += 0.1;
   else if (ch == '-')
      zoom -= 0.1;
   // light height
   else if (ch == 'H') //REMOVE ME
      ylight += 0.5;
   else if (ch == 'h')
      ylight -= 0.5;
  // // lfov
  // else if (ch == ']')
  //     fov += 1;
  // else if (ch == '[')
  //     fov -= 1;

  //  Toggle light movement
  else if (ch == 'm' || ch == 'M') //REMOVE ME
      move = 1-move;

  //reset zoom if needed
  if(zoom < 0.8){
    zoom =0.6;
  }
  if(zoom > 2.7){
    zoom = 2.7;
  }

  ball_th %= 360;
  ball_ph %= 360;

  ////MARBLE MOVEMENT:
  if (ch == 'w'){ //FORWARD
     ball_ph -= speed;
     ball_z -= movement;
     forward = 1;
     sideways = 0;
  }
  else if (ch == 's'){ //BACKWARD
     ball_ph += speed;
     ball_z += movement;
     forward = 1;
     sideways = 0;
  }
  else if (ch == 'a'){ //LEFT
     ball_th += speed;
     ball_x -= movement;
     forward = 0;
     sideways = 1;
  }
  else if (ch == 'd'){ //RIGHT
     ball_th -= speed;
     ball_x += movement;
     forward = 0;
     sideways = 1;
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
   glutInitWindowSize(800,800);
   glutCreateWindow("Final Project, Jake Henson");
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
   texture[7] = LoadTexBMP("arizona_1.bmp");
   texture[8] = LoadTexBMP("arizona_2.bmp");

   water_texture[0] = LoadTexBMP("water2.bmp");
   water_texture[1] = LoadTexBMP("water3.bmp");
   water_texture[2] = LoadTexBMP("water4.bmp");
   water_texture[3] = LoadTexBMP("water5.bmp");
   water_texture[4] = LoadTexBMP("water6.bmp");
   water_texture[5] = LoadTexBMP("water7.bmp");
   water_texture[6] = LoadTexBMP("water8.bmp");

   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}
