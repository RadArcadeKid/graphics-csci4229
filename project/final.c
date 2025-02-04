
  /*
 * FINAL PROJECT
 * Jacob (Jake) Henson - 105963531
 * CSCI4229/5229 Fall 2020
 *
 **TO RUN:**
 * > make
 * > ./final
 *
 * **KEY BINDINGS:**
 *  w/a/s/d      move marble forward/left/backward/right (depending on camera position)
 *  v/V          Reset marble position
 *  r/R          Reset game entirely (marble + cans)
 *
 *   arrow keys   change camera angle
 *   +/-          zoom camera closer/farther
 *   x/X          toggle HUD (number of collectibles indicator)
 *   0            Reset zoom and view angle
 *   ESC          Exit
 *
 */




#include "CSCIx229.h"

#define PI (3.1415927)

int hud=1;       //  Display hud start with off
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
int local     =   1;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   =  50;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        =  90;  // azimuth
float zoom = 2.0;
int old_t;
float dt;

int dolphin;
int skull;

int ball_ph = 0;
int ball_th = 0;

int forward, sideways = 0;

int secret = 0; //for the secret easter egg

double ball_x, ball_z = 0.0;
double ball_y = 0.2;

//Texture arrays:
unsigned int texture[17]; // misc textures
unsigned int water_texture[7]; // water textures
unsigned int skybox[5]; // skybox textures


int speed = 12;
double movement = 0.08;


//sloppy way of defining number of cans in the level
//so I don't have to update it everytime...probably not best practice
#define numcans 12

//array to determine whether the user has collected a can or not
int collected_cans[numcans] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

//places to store x and z coordinates of cans
double cans_x[numcans] = {0.0,  -2.0, -4.0, -4.0,  0.0,     -8,  -12, 4.0, 8.0, 8.0, 12.0, 6.0};
double cans_z[numcans] = {-2.0, -2.0, -4.0, -10.0,  -10.0,  -5.6,-5.6, -8.0, -8.0, -4.0, -4.0, -2.0};



#define numground 25

//places to store the x and y coordinates of the ground
double ground_x[numground] = {0.0, 0.0,-2.0,-4.0,    -6.0,-8.0,-10.0,-12.0,     -4.0, -4.0,-4.0, -4.0,    -2.0 , 0.0,  2.0,   2.0,    4.0, 6.0,  8.0, 8.0,     8.0, 10.0, 12.0, 8.0, 6.0};
double ground_z[numground] = {0.0,-2.0,-2.0,-2.0,    -5.6,-5.6,-5.6,-5.6,       -4.0, -6.0,-8.0,-10.0,   -10.0,-10.0, -10.0, -8.0,   -8.0, -8.0,-8.0,-6.0,    -4.0, -4.0, -4.0,-2.0,-2.0};


typedef struct {float x,y,z;} vtx;
typedef struct {int A,B,C;} tri;
#define n 500
vtx is[n];

int ctrlmode = 0;

int collectedTape = 0;


///SKYBOX
void drawSkybox(double s, double x, double z){

  glPushMatrix();
  glTranslated(x,-5,z);
  glScaled(s,s,s);

  double sx = 1;
  double sy = 1;
  double sz = 1;

  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
  glBindTexture(GL_TEXTURE_2D,skybox[0]);

  glBegin(GL_QUADS);
  glNormal3f( 0, 0, -1);
  glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
  glTexCoord2f(sx,0); glVertex3f(+1,-1, 1);
  glTexCoord2f(sx,sy); glVertex3f(+1,+1, 1);
  glTexCoord2f(0,sy);  glVertex3f(-1,+1, 1);
  glEnd();

  //  Back
  glBindTexture(GL_TEXTURE_2D,skybox[1]);
  glBegin(GL_QUADS);
  glNormal3f( 0, 0,1);
  glTexCoord2f(0,0);  glVertex3f(+1,-1,-1);
  glTexCoord2f(sx,0); glVertex3f(-1,-1,-1);
  glTexCoord2f(sx,sy); glVertex3f(-1,+1,-1);
  glTexCoord2f(0,sy); glVertex3f(+1,+1,-1);
  glEnd();

  //  Right
  glBindTexture(GL_TEXTURE_2D,skybox[3]);
  glBegin(GL_QUADS);
  glNormal3f(-1, 0, 0);
  glTexCoord2f(0,0);   glVertex3f(+1,-1,+1);
  glTexCoord2f(sz,0); glVertex3f(+1,-1,-1);
  glTexCoord2f(sz,sy); glVertex3f(+1,+1,-1);
  glTexCoord2f(0,sy); glVertex3f(+1,+1,+1);
  glEnd();

  //  Left
  glBindTexture(GL_TEXTURE_2D,skybox[2]);
  glBegin(GL_QUADS);
  glNormal3f(+1, 0, 0);
  glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
  glTexCoord2f(sz,0); glVertex3f(-1,-1,+1);
  glTexCoord2f(sz,sy); glVertex3f(-1,+1,+1);
  glTexCoord2f(0,sy); glVertex3f(-1,+1,-1);
  glEnd();

  //  Top
  glBindTexture(GL_TEXTURE_2D,skybox[4]);
  glBegin(GL_QUADS);
  glNormal3f( 0,-1, 0);
  glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
  glTexCoord2f(sx,0); glVertex3f(+1,+1,+1);
  glTexCoord2f(sx,sz); glVertex3f(+1,+1,-1);
  glTexCoord2f(0,sz); glVertex3f(-1,+1,-1);
  glEnd();

  glDisable(GL_TEXTURE_2D);


  //  Undo transofrmations
  glPopMatrix();
}



/*
 * Reset resets ball position, camera position
 */
void Reset(int hardreset){
  th = 0;
  ph = 30;
  ball_x = 0;
  ball_z = 0;
  ball_y = 0.2;
  ctrlmode = 0;
  if(hardreset){
    //reset all cans in the world, too!
    for(int i = 0; i < numcans; i++){
        collected_cans[i] = 0;
    }
  }
}

/*
 *  Draw vertex in polar coordinates
 */
static void Vertex(int th,int ph)
{
   double x = Cos(th)*Cos(ph);
   double y = Sin(th)*Cos(ph);
   double z =         Sin(ph);
   glNormal3d(x,y,z);
   glTexCoord2d(th/360.0,ph/180.0+0.5);

   glVertex3d(x,y,z);
}


/*
 * Ground - draws the track
 */
static void Ground(double x, double y, double z, double s, double dy, int col){
  glPushMatrix();
  //  Offset, scale and rotate
  glTranslated(x,-dy,z);
  glScaled(s,dy,s);


  int gridsize = 1;

  double sx = gridsize*1;
  double sy = (gridsize*dy);
  double sz = gridsize*1;

  //  Enable textures
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
  //glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D,texture[1]); //carpet
  glBegin(GL_QUADS);
  if(col)
    glColor3f(0.933, 0.510, 0.933);
  else
    glColor3f(0.498, 1.000, 0.831); //for secret ground


 //OLD GROUND
//////////////////////////////////////////////////
  // glNormal3f( 0,+1, 0);
  // glTexCoord2f(0,0); glVertex3f(-1,0,+1);
  // glTexCoord2f(gridsize,0); glVertex3f(+1,0,+1);
  // glTexCoord2f(gridsize,gridsize); glVertex3f(+1,0,-1);
  // glTexCoord2f(0,gridsize); glVertex3f(-1,0,-1);
///////////////////////////////////////////////////////


 //NEW, 3D GROUND:
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
   glTexCoord2f(-sx,0); glVertex3f(+1,-1, 1);
   glTexCoord2f(-sx,sy); glVertex3f(+1,+1, 1);
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

  glEnd();
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
}

 /*
  * Water - draws the animated water
  */
static void Water(int x, int y, int z, int ct, int flip){
  int s = 1;
  glPushMatrix();

  //  Offset, scale and rotate
  glTranslated(x,y,z);
  glScaled(s,s,s);

  if(flip){
    glRotated(180, 0,0,1);
    glRotated(180, 0,1,0);
 }



  double gridsize = s;

  //  Enable textures
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
  glBindTexture(GL_TEXTURE_2D, water_texture[ct]);
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
 *
 */
static void DrawGround(){
  for(int i = 0; i < numground; i++){
    Ground(ground_x[i], 0, ground_z[i], 1, 0.05, 1);
  }

  if(secret){
    Ground(8, 0, -10, 1, 0.05, 0); //secret ground
    Ground(8, 0, -12, 1, 0.05, 0); //secret ground
  }
}

/*
 * Drops the ball once it's fallen off the track
 */
void DropBall(){
   double yvel = -5 + (ball_y-0.2); //y velocity
   yvel += (-9.8 * dt);
   ball_y += yvel * dt; //drop the ball relative to time

   //if ball has touched the water:
   if(ball_y < -7){
     Reset(0); //reset it's poistion to the beginning (but not the cans)
   }
}


/*
 * CheckGround checks if the marble is on the track
 */
static void CheckGround(double ball_x, double ball_z){
  double range = 1.1; //range the ball can be within until it falls
  int outOfRange = 1; //false, ball is IN range

  //make sure the ball isn't already falling!
  if(ball_y == 0.2){
    //check all groundspaces to determine if the ball is out of range
    for(unsigned int i = 0; i < numground; i++){
      if(ball_x > ground_x[i]-range && ball_x < ground_x[i]+range){
        if(ball_z >  ground_z[i]-range && ball_z < ground_z[i]+range){
          outOfRange = 0;
        }
      }
      if(ball_z >  ground_z[i]-range && ball_z < ground_z[i]+range){
        if(ball_x > ground_x[i]-range && ball_x < ground_x[i]+range){
          outOfRange = 0;
        }
      }
    }
  }

  //if the Easter Egg is active
  //check if it's on the secret tiles
  if(secret){
    if(ball_x > 8-range && ball_x < 8+range){
      if(ball_z >  -12-range && ball_z < -10+range){
        outOfRange = 0;
      }
    }
    if(ball_z >  -12-range && ball_z < -10+range){
      if(ball_x > 8-range && ball_x < 8+range){
        outOfRange = 0;
      }
    }
  }


  //if the ball has fallen off the track
  if(outOfRange == 1){
    DropBall(); //drop it
  }
}



/*
 *  Draw marble
 */
void DrawMarble(double x,double y,double z, double s, double th, double ph)
{
   const int d = 15;
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
   glDisable(GL_TEXTURE_2D);

   glPopMatrix();
}

/*
 *  DrawFlowerpot - draws an Arizona tea can
 */
static void DrawFlowerpot(double x,double y,double z, double delta_h, double s){
  const double d=2*PI/12;
  float r = 0.45;
  double h = delta_h;


  //  Save transformation
  glPushMatrix();
  //  Offset, scale and rotate
  glTranslated(x,y-0.1,z);
  float sm = 0.8;
  glScaled(sm*s, sm*s, sm*s);

  glColor3f(0.2, 0.749, 0.740);

  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
  //glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D,texture[2]); //concrete


  //draw sides of cylinder
  glBegin(GL_QUAD_STRIP);
  //glNormal3d(0.0, h, 0.0);
    for(float i = 0; i <= 2.1*PI; i+=d){
      glNormal3d(r * cos(i), 0, r * sin(i));
      const float tc = ( i / (float)( 2 * PI ));
      glTexCoord2f(-tc, 0);

      glVertex3f(0.5*r * cos(i), -h, 0.5*r * sin(i));
      glTexCoord2f(-tc, 2*h*s);

      glVertex3f(r * cos(i), 0, r * sin(i));
    }
  glEnd();

  glNormal3d(0,+1,0); //reset normals

  glColor3f(0.627, 0.322, 0.176);

  glBindTexture(GL_TEXTURE_2D,texture[5]); //dirt-esq

  glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f( 0.5, 0.5 );
        glVertex3f(0, 0, 0);  /* center */
        for (double i = 2 * PI; i >= 0; i -= d)

        {
            glTexCoord2f( 0.5f * cos(i) + 0.5f, 0.5f * sin(i) + 0.5f );
            glVertex3f(r * cos(i), 0, r * sin(i));
        }
        /* close the loop back to 0 degrees */
        glTexCoord2f( 0.5, 0.5 );
        glVertex3f(r, 0, 0);
    glEnd();

    glNormal3d(0,-1,0); //reset normals


  //  Undo transformations
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

/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void cube2(double x,double y,double z,
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

   double dst = 0.7;

   glBegin(GL_QUADS);
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
   glTexCoord2f(sx,0); glVertex3f(+1,-1, 1);
   glTexCoord2f(sx,sy); glVertex3f(+1,+1, 1);
   glTexCoord2f(0,sy);  glVertex3f(-1,+1, 1);

   //  Back
   glNormal3f( 0, 0,-1);
   glTexCoord2f(0,0);  glVertex3f(+dst,-dst,-1);
   glTexCoord2f(sx,0); glVertex3f(-dst,-dst,-1);
   glTexCoord2f(sx,sy); glVertex3f(-dst,+dst,-1);
   glTexCoord2f(0,sy); glVertex3f(+dst,+dst,-1);
   //  Right

   glNormal3f(+1, 0, 0);
   glTexCoord2f(0,0);   glVertex3f(+1,-1,+1);
   glTexCoord2f(sz,0); glVertex3f(+dst,-dst,-1);
   glTexCoord2f(sz,sy); glVertex3f(+dst,+dst,-1);
   glTexCoord2f(0,sy); glVertex3f(+1,+1,+1);
   //  Left
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(-dst,-dst,-1);
   glTexCoord2f(sz,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(sz,sy); glVertex3f(-1,+1,+1);
   glTexCoord2f(0,sy); glVertex3f(-dst,+dst,-1);
   //  Top
   glNormal3f( 0,+1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
   glTexCoord2f(sx,0); glVertex3f(+1,+1,+1);
   glTexCoord2f(sx,sz); glVertex3f(+dst,+dst,-1);
   glTexCoord2f(0,sz); glVertex3f(-dst,+dst,-1);
   //  Bottom
   glNormal3f( 0,-one, 0);
   glTexCoord2f(0,0); glVertex3f(-dst,-dst,-1);
   glTexCoord2f(sx,0);  glVertex3f(+dst,-dst,-1);
   glTexCoord2f(sx,sz); glVertex3f(+1,-1,+1);
   glTexCoord2f(0,sz); glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   //  Undo transofrmations
   glPopMatrix();
}


/*
 *  DrawLeaf draws a tiiiny leaf
 */
void DrawLeaf(double x, double y, double z, double tz, double startsize){
  glPushMatrix();
  glTranslated(x, y, z);
  //  Offset, scale and rotate
  float tex_size = 1;

  //  Enable textures
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D,texture[6]); //fern leaf
  glBegin(GL_QUADS);
  glNormal3f( 0,+1, 0);
  glTexCoord2f(0,0); glVertex3f(-startsize,0,+1);
  glTexCoord2f(tex_size,0); glVertex3f(+startsize,0,+1);
  glTexCoord2f(tex_size,tex_size); glVertex3f(+tz,0,-1);
  glTexCoord2f(0,tex_size); glVertex3f(-tz,0,-1);
  glEnd();
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();

}

/*
 *  Draws a single fern stem
 */
void DrawFern(double x, double y, double z){
  glPushMatrix();
    //draw leaves
    glTranslated(x, y, z-0.3);
    glScaled(0.2, 0.2, 0.3);
    glRotated(40, 1,0,0);

    DrawLeaf(0, 0, 0,      0.7, 1.0);
    glRotated(-30, 1,0,0);
    DrawLeaf(0, +0.5, -1.7,     0.5, 0.7);
    glRotated(-30, 1,0,0);
    DrawLeaf(0, +1.8, -3,     0, 0.5);
  glPopMatrix();
}

void DrawPlant(double x, double y, double z){

  glPushMatrix();
  glScaled(0.55, 1, 0.55);

    glTranslated(x, y*0.3+0.5, z);

    //draw flowerpot
    DrawFlowerpot(0,0,0, 1, 1);


    glColor3f(1,1,1);
    //draw leaves
      DrawFern(0,0,0);

      glRotated(40, 0,1,0);
      DrawFern(0,0,0);

      glRotated(45, 0,1,0);
      DrawFern(0,0,0);

      glRotated(50, 0,1,0);
      DrawFern(0,0,0);

      glRotated(30, 0.1,1,0);
      DrawFern(0,0,0);

      glRotated(50, 0,1,0);
      DrawFern(0,0,0);

      glRotated(30, 0,1,0);
      DrawFern(0,0,0);

      glRotated(55, 0.1,1,0);
      DrawFern(0,0,0);




  glPopMatrix();
}

/*
 *  DrawTeaCan - draws an Arizona tea can
 */
static void DrawTeaCan(double x,double y,double z, double delta_h, double s){
  const double d=2*PI / 12;
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
 *  DrawJazzyCup - draws an jazzy 90s cup
 */
static void JazzyCup(double x,double y,double z, double delta_h, double s, double th){
  const double d=2*PI/12;
  float r = 0.45;
  double h = delta_h;


  //  Save transformation
  glPushMatrix();
  //  Offset, scale and rotate
  glTranslated(x,y-0.1,z);
  glRotated(th, 0,1,0);
  float sm = 1;
  glScaled(sm*s, sm*s, sm*s);

  glColor3f(0.2, 0.749, 0.740);

  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
  glBindTexture(GL_TEXTURE_2D,texture[12]); //jazzy_pattern

  glColor3f(1, 1, 1);

  //draw sides of cylinder
  glBegin(GL_QUAD_STRIP);
  //glNormal3d(0.0, h, 0.0);
    for(float i = 0; i <= 2.1*PI; i+=d){
      glNormal3d(r * cos(i), 0, r * sin(i));
      const float tc = ( i / (float)( 2 * PI ));
      glTexCoord2f(-tc*2, 0);

      glVertex3f(0.7*r * cos(i), -h, 0.7*r * sin(i));
      glTexCoord2f(-tc*2, h*s);

      glVertex3f(r * cos(i), 0+0.1, r * sin(i));
    }
  glEnd();

  glNormal3d(0,+1,0); //reset normals

  glColor3f(1, 1, 1);

  glBindTexture(GL_TEXTURE_2D,texture[13]); //drink

  glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f( 0.5, 0.5 );
        glVertex3f(0, 0, 0);
        for (double i = 2 * PI; i >= 0; i -= d)

        {
            glTexCoord2f( 0.5f * cos(i) + 0.5f, 0.5f * sin(i) + 0.5f );
            glVertex3f(0.95*r * cos(i), 0, 0.95*r * sin(i));
        }
        glTexCoord2f( 0.5, 0.5 );
        glVertex3f(r, 0, 0);
    glEnd();



  glBindTexture(GL_TEXTURE_2D,texture[12]); //jazzy pattern for bottom of cup


    glNormal3d(0,-1,0); //reset normals
    glColor3f(1, 1, 1);

    glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f( 0.5, 0.5 );
        glVertex3f(0, -h, 0);
        for (double i = 0; i <= 2 * PI; i += d)
        {
            glTexCoord2f(cos(i) + 0.5f, sin(i) + 0.5f );
            glVertex3f(0.7*r * cos(i), 0, 0.7*r * sin(i));
        }
    glEnd();

    glNormal3d(0,-1,0); //reset normals

  glDisable(GL_TEXTURE_2D);


  //  Undo transformations
  glPopMatrix();
}




/*
 *  DrawPillarTube - draws the pillar tube for the pillar
 */
static void DrawPillarTube(double x,double y,double z, double h, double s){
  const double d=2*PI/12;

  float r = 0.7;

  //  Save transformation
  glPushMatrix();
  //  Offset, scale and rotate
  glTranslated(x,y,z);
  glColor3f(1, 1, 1);

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
 * Draw the water floor!!
 */
void DrawWaterFloor(){
  glPushMatrix();
  glScaled(2,2,2);
  int gridsize = 17;
  glTranslated(-gridsize, -2.5, -gridsize);

  int ct = zh / 30;
  ct %= 7;

  for(int i = 0; i < gridsize; i++){
    for(int j = 0; j < gridsize; j++){
      if(j %2 )
        Water(i*2, 0, j*2, ct, 0);
      else
        Water(i*2, 0, j*2, ct, 1);

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
  float range = 0.36;

  for(int i = 0; i < numcans; i++){
    //check if ball is within pickup range of can
    if(ball_x >= cans_x[i]-range && ball_x <= cans_x[i]+range){
      if(ball_z >= cans_z[i]-range && ball_z <= cans_z[i]+range){
         collected_cans[i] = 1; //set the can to picked up so it no longer displays
      }
    }
  }

  //check for tape
  if(ball_x >= 8-range && ball_x <= 8+range){
    if(ball_z >= -12-range && ball_z <= -12+range){
       collectedTape = 1;
    }
  }
}

int getNumCansLeft(){
  int collected = 0;
  for(int i = 0; i < numcans; i++){
    if(collected_cans[i] == 1){ //if the user has picked up a can
      collected+=1;
    }
  }

  if(collected == numcans)
    secret = 1;

  return collected;
}

/*
 * Draws a concrete pillar
 *
 */
void DrawPillar(double x, double y, double z, double h, int includeplant){

    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    float scale = 0.3;
    glTranslated(x, y+0.02, z);

    if(includeplant) //check whether to draw a plant
      DrawPlant(x*0.55, h, z*0.55);


    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D,texture[2]); //concrete!
    glColor3f(1, 1, 1);


    glScalef(scale, scale, scale);




    cube(x, y, z, 1, 0.1, 1, 0.0);
    cube(x, y+0.17, z, 0.8, 0.1, 0.8, 0.0);

    DrawPillarTube(x, y+(h/2), z, (h/2), 1.0);


    cube(x, y+h, z, 0.8, 0.1, 0.8, 0.0);
    cube(x, y+h+0.17, z, 1, 0.1, 1, 0.0);


    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

/*
 * Draw the front of the computer
 */
void computer(double x, double y, double z, double dx,double dy,double dz, double th){
  //  Save transformation
  glPushMatrix();
  //  Offset, scale and rotate
  glTranslated(x,y,z);
  glRotated(th,0,1,0);
  glScaled(dx, dy, dz);

  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
  glBindTexture(GL_TEXTURE_2D,texture[11]); //computer front
  glBegin(GL_QUADS);
  glNormal3f( 0, 0, 1);
  glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
  glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
  glTexCoord2f(1,1); glVertex3f(+1,+1, 1);
  glTexCoord2f(0,1);  glVertex3f(-1,+1, 1);
  glEnd();
  glDisable(GL_TEXTURE_2D);

  //  Back
  glBegin(GL_QUADS);
  glNormal3f( 0, 0,-1);
  glTexCoord2f(0,0);  glVertex3f(+1,-1,-1);
  glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
  glTexCoord2f(1,1); glVertex3f(-1,+1,-1);
  glTexCoord2f(0,1); glVertex3f(+1,+1,-1);
  glEnd();
  //  Right
  glBegin(GL_QUADS);
  glNormal3f(+1, 0, 0);
  glTexCoord2f(0,0);   glVertex3f(+1,-1,+1);
  glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
  glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
  glTexCoord2f(0,1); glVertex3f(+1,+1,+1);
  glEnd();
  //  Left
  glBegin(GL_QUADS);
  glNormal3f(-1, 0, 0);
  glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
  glTexCoord2f(1,0); glVertex3f(-1,-1,+1);
  glTexCoord2f(1,1); glVertex3f(-1,+1,+1);
  glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
  glEnd();
  //  Top
  glBegin(GL_QUADS);
  glNormal3f( 0,+1, 0);
  glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
  glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
  glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
  glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
  glEnd();
  //  Bottom
  glBegin(GL_QUADS);
  glNormal3f( 0,-1, 0);
  glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
  glTexCoord2f(1,0);  glVertex3f(+1,-1,-1);
  glTexCoord2f(1,1); glVertex3f(+1,-1,+1);
  glTexCoord2f(0,1); glVertex3f(-1,-1,+1);
  glEnd();

  //  Undo transofrmations
  glPopMatrix();
}

/*
 * Draw a keyboard
 */
void keyboard(double x, double y, double z, double dx,double dy,double dz, double th){
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx, dy, dz);



   glBegin(GL_QUADS);
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
   glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
   glTexCoord2f(1,1); glVertex3f(+1,+1, 1);
   glTexCoord2f(0,1);  glVertex3f(-1,+1, 1);
   glEnd();


   //  Back
   glBegin(GL_QUADS);
   glNormal3f( 0, 0,-1);
   glTexCoord2f(0,0);  glVertex3f(+1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,-1);
   glEnd();

   //  Right
   glBegin(GL_QUADS);
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0,0);   glVertex3f(+1,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,+1);
   glEnd();
   //  Left
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();
   //  Top

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D,texture[10]); //keyboard top
   glBegin(GL_QUADS);
   glNormal3f( 0,+1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();
   glDisable(GL_TEXTURE_2D);



   //  Bottom
   glBegin(GL_QUADS);
   glNormal3f( 0,-1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0);  glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,-1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,-1,+1);
   glEnd();

   //  Undo transofrmations
   glPopMatrix();
 }

/*
 *  Draw the windows 95 computer screen
 */
void screen(double x, double y, double z, double s){

  glPushMatrix();
  //  Offset, scale and rotate
  glTranslated(x,y,z);
  glScaled(s,s,s);
  glRotated(90, 1,0,0);

  //  Enable textures
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
  glBindTexture(GL_TEXTURE_2D,texture[9]); //win95Screen
  glBegin(GL_QUADS);
  glNormal3f( 0,+1, 0);
  glTexCoord2f(0,0); glVertex3f(-1,0,+1);
  glTexCoord2f(1,0); glVertex3f(+1,0,+1);
  glTexCoord2f(1,1); glVertex3f(+1,0,-1);
  glTexCoord2f(0,1); glVertex3f(-1,0,-1);
  glEnd();
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
}

/*
 *  drawError draws a fake windows 95 error!
 */
void drawError(double x, double y, double z, double s, double th, int type){
  float Emission[]  = {0.0,0.0,0.01*2,1.0};
  glMaterialfv(GL_FRONT,GL_EMISSION,Emission);

  glPushMatrix();
  //  Offset, scale and rotate
  glTranslated(x,y,z);
  glRotated(th,0,1,0);
  if(type == 0)
    glScaled(s,s*0.6,s); //error_2
  else if (type == 1)
    glScaled(s,s*0.4,s); //error_1

  glRotated(90, 1,0,0);

  //  Enable textures
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
  if(type == 0)
    glBindTexture(GL_TEXTURE_2D,texture[4]); //error_2
  else if (type == 1)
    glBindTexture(GL_TEXTURE_2D,texture[3]); //error_1

  glBegin(GL_QUADS);
  glNormal3f( 0,+1, 0);
  glTexCoord2f(0,0); glVertex3f(-1,0,+1);
  glTexCoord2f(1,0); glVertex3f(+1,0,+1);
  glTexCoord2f(1,1); glVertex3f(+1,0,-1);
  glTexCoord2f(0,1); glVertex3f(-1,0,-1);
  glEnd();
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
}


/*
 *  Draw The Secret Tape
 * Why a secret tape? This is a reference to the old Tony Hawk's Pro Skater games
 * Where you'd have to collect the secret skater tape just out of bounds
 * Not sure why I included it as an easter egg, but hey, here we are lol
 */
void DrawSecretTape(double x, double y, double z, double dx,double dy,double dz, double th){
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx, dy, dz);

   glColor3f(0.2, 0.2, 0.2);


   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
   glBindTexture(GL_TEXTURE_2D,texture[14]); //secret tape front
   glBegin(GL_QUADS);
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
   glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
   glTexCoord2f(1,1); glVertex3f(+1,+1, 1);
   glTexCoord2f(0,1);  glVertex3f(-1,+1, 1);
   glEnd();


   //  Back
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
   glBindTexture(GL_TEXTURE_2D,texture[15]); //secret tape back
   glBegin(GL_QUADS);
   glNormal3f( 0, 0,-1);
   glTexCoord2f(0,0);  glVertex3f(+1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,-1);
   glEnd();
   glDisable(GL_TEXTURE_2D);


   //  Right
   glBegin(GL_QUADS);
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0,0);   glVertex3f(+1,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,+1);
   glEnd();
   //  Left
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();
   //  Top
   glBegin(GL_QUADS);
   glNormal3f( 0,+1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();


   //  Bottom
   glBegin(GL_QUADS);
   glNormal3f( 0,-1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0);  glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,-1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,-1,+1);
   glEnd();

   //  Undo transofrmations
   glPopMatrix();
 }

static void DrawComputer(double x, double y, double z, double s, double th, double ph){
  //  Save transformation
  glPushMatrix();
  //  Offset, scale and rotate
  glTranslated(x,y+0.5,z);
  glRotated(th,0,1,0);
  glRotated(ph,1,0,0);
  glScaled(s*1.1, s, s);


  //draw Monitor
  //glColor3f(1.000, 0.973, 0.863);
  glColor3f(1.000, 0.922, 0.804);

  computer(0,-0.3,0, 0.6,0.15,0.6,   0);

  cube2(0,0.35,0, 0.42,0.42,0.4,   0);
  cube(0,0.35,+0.4, 0.5,0.5,0.15,   0);
  cube(0,0,-0.2, 0.1,0.2,0.1,   0);

  screen(0,0.35,0.56, 0.4);

  glRotated(10, 1,0,0);
  keyboard(0,-0.3,+1, 0.55,0.06,0.25,   0);


  glPopMatrix();
}

static void SetFloatingTape(double x, double y, double z){
  glPushMatrix();

  glTranslated(x,y,z);
  glRotated(zh, 0, 1, 0);
  DrawSecretTape(0, 0.5+0.1*Sin(zh), 0,    .4,0.23,0.07, 0);

  glPopMatrix();
}


/*
 *  Draw a cube for the parthenon!
 *
 */
static void cubeParth(double x,double y,double z,
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
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D,texture[2]); //concrette
   glColor3f(1,1,1);

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



/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void cubeParth2(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th){
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx, dy, dz);

   double top = 0.02;

   int ts = 2;

   double sx = ts*dx;
   double sy = ts*dy;
   double sz = ts*dz;
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D,texture[2]); //concrette
   glColor3f(1,1,1);

   glBegin(GL_QUADS);
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
   glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
   glTexCoord2f(1,1); glVertex3f(+top,+1, 1);
   glTexCoord2f(0,1);  glVertex3f(-top,+1, 1);

   //  Back
   glNormal3f( 0, 0,-1);
   glTexCoord2f(0,0);  glVertex3f(+1,-1,-1);
   glTexCoord2f(sx,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(sx,sy); glVertex3f(-top,+1,-1);
   glTexCoord2f(0,sy); glVertex3f(+top,+1,-1);
   //  Right
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0,0);   glVertex3f(+1,-1,+1);
   glTexCoord2f(sz,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(sz,sy); glVertex3f(+top,+1,-1);
   glTexCoord2f(0,sy); glVertex3f(+top,+1,+1);
   //  Left
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(sz,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(sz,sy); glVertex3f(-top,+1,+1);
   glTexCoord2f(0,sy); glVertex3f(-top,+1,-1);
   //  Top
   glNormal3f( 0,+1, 0);
   glTexCoord2f(0,0); glVertex3f(-top,+1,+1);
   glTexCoord2f(sx,0); glVertex3f(+top,+1,+1);
   glTexCoord2f(sx,sz); glVertex3f(+top,+1,-1);
   glTexCoord2f(0,sz); glVertex3f(-top,+1,-1);
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


void DrawParthenon(double x, double y, double z, double s, double th){
  //  Save transformation
  glPushMatrix();
  //  Offset, scale and rotate
  glTranslated(x,y,z);
  glRotated(th,0,1,0);
  glScaled(s*0.5, s*0.5, s*0.5);

  cubeParth(0,-0.1,-1.6,   2, 0.08, 3, 0);
  cubeParth(0,-0.28,-1.6,   2.1, 0.08, 3.1, 0);

  DrawPillar(-1.2,0,0.8,   6.0, 0);
  DrawPillar(-0.4,0,0.8,   6.0, 0);

  DrawPillar(-0.4,0,0,   6.0, 0);
  DrawPillar(0.4,0,0,   6.0, 0);


  DrawPillar(0.4,0,0.8,   6.0, 0);
  DrawPillar(1.2,0,0.8,   6.0, 0);

  DrawPillar(-1.2,0,0,   6.0, 0);
  DrawPillar(1.2,0,0,   6.0, 0);

  DrawPillar(-1.2,0,-0.8,   6.0, 0);
  DrawPillar(1.2,0,-0.8,   6.0, 0);

  DrawPillar(-1.2,0,-1.6,   6.0, 0);
  DrawPillar(1.2,0,-1.6,   6.0, 0);

  DrawPillar(-1.2,0,-2.4,   6.0, 0);
  DrawPillar(1.2,0,-2.4,   6.0, 0);

  DrawPillar(-1.2,0,-3.2,   6.0, 0);
  DrawPillar(-0.4,0,-3.2,   6.0, 0);

  DrawPillar(0.4,0,-3.2,   6.0, 0);
  DrawPillar(1.2,0,-3.2,   6.0, 0);


  cubeParth(0,2,-1.6,   2, 0.09, 3, 0);
  cubeParth(0,2.09,-1.6,   2.1, 0.04, 3.1, 0);

  cubeParth2(0,2.4,-1.6,   2, 0.3, 3, 0);


  glPopMatrix();
}

static void paint1(double x, double y, double z, double dx,double dy){
    glPushMatrix();
    glTranslated(x,y,z);
    glScaled(dx, dy, 1);
    glRotated(90, 1,0,0);
    //  Enable textures
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D,texture[16]); //paint window
    glBegin(GL_QUADS);
    glNormal3f( 0,+1, 0);
    glTexCoord2f(0,0); glVertex3f(-1,0,+1);
    glTexCoord2f(1,0); glVertex3f(+1,0,+1);
    glTexCoord2f(1,0.19); glVertex3f(+1,0,-1);
    glTexCoord2f(0,0.19); glVertex3f(-1,0,-1);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

static void paint2(double x, double y, double z, double dx,double dy){
    glPushMatrix();
    glTranslated(x,y,z);
    glScaled(dx, dy, 1);
    glRotated(90, 1,0,0);

    //  Enable textures
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D,texture[16]); //paint window
    glBegin(GL_QUADS);
    glNormal3f( 0,+1, 0);
    glTexCoord2f(0,0.21); glVertex3f(-1,0,+1);
    glTexCoord2f(0.15,0.21); glVertex3f(+1,0,+1);
    glTexCoord2f(0.15,0.895); glVertex3f(+1,0,-1);
    glTexCoord2f(0,0.895); glVertex3f(-1,0,-1);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}


static void paint3(double x, double y, double z, double dx,double dy){
    glPushMatrix();
    glTranslated(x,y,z);
    glScaled(dx, dy, 1);
    glRotated(90, 1,0,0);

    //  Enable textures
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D,texture[16]); //paint window
    glBegin(GL_QUADS);
    glNormal3f( 0,+1, 0);
    glTexCoord2f(0.948,0.21); glVertex3f(-1,0,+1);
    glTexCoord2f(1,0.21); glVertex3f(+1,0,+1);
    glTexCoord2f(1,0.89); glVertex3f(+1,0,-1);
    glTexCoord2f(0.948,0.89); glVertex3f(-1,0,-1);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

static void paint4(double x, double y, double z, double dx,double dy){
    glPushMatrix();
    glTranslated(x,y,z);
    glScaled(dx, dy, 1);
    glRotated(90, 1,0,0);
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D,texture[16]); //paint window
    glBegin(GL_QUADS);
    glNormal3f( 0,+1, 0);
    glTexCoord2f(0,0.895); glVertex3f(-1,0,+1);
    glTexCoord2f(1,0.895); glVertex3f(+1,0,+1);
    glTexCoord2f(1,1); glVertex3f(+1,0,-1);
    glTexCoord2f(0,1); glVertex3f(-1,0,-1);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}


static void DrawPaintWindow(double x, double y, double z, double s, double th, double ph){
   glPushMatrix();
   glTranslated(x, y, z);
   glRotated(ph,1,0,0);
   glRotated(th,0,1,0);
   glScaled(s,s,s);



   //draw paint windows!
   paint1(0,0,0, 2, 0.4);
   paint2(-1.73,1.6,0, 0.27, 1.2);
   paint3(1.9,1.6,0, 0.1, 1.2);
   paint4(0,3,0, 2, 0.2);



   glPopMatrix();
}


static void DrawDolphin(double x, double y, double z, double s, double r, double th, double ph){
  //  Draw the model
  glPushMatrix();

  //RGBA
  float RGBA[4] = {0.933, 0.8, 0.933, 1};  //  Colors
  int spc = 32;
  float Shinyness[] = {24};
  float Specular[]  = {spc,spc,spc,1.0};

  glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,Shinyness);
  glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,RGBA);
  glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,RGBA);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Specular);
  glColor3f(0.933, 0.4, 0.933);
  glTranslated(x,y,z);
  glRotated(th,0,1,0);
  glRotated(ph,1,0,0);

  glScaled(s,s,s);
  glTranslated(0, +r, 0);
  glCallList(dolphin);
  glPopMatrix();
}


static void DrawSkull(double x, double y, double z, double s, double th, double ph){
  //  Draw the model
  glPushMatrix();
  glColor3f(0.25, 0.8, 0.8);

  glTranslated(x,y,z);
  glRotated(th,0,1,0);
  glRotated(ph,1,0,0);
  glScaled(s,s,s);
  glCallList(skull);
  glPopMatrix();
}

/*
 * Get mode of control depending on which way the camera is facing
 */
static void SetControlMode(){
  if(th > 0){
    if(th <= 45 || th >= 315)
      ctrlmode = 0;
    if(th < 315 && th > 225)
      ctrlmode = 1;
    if(th <= 225 && th >= 135)
      ctrlmode = 2;
    if(th < 135 && th > 45)
      ctrlmode = 3;
  }
  if(th < 0){
    if(th >= -45 || th <= -315)
      ctrlmode = 0;
    if(th > -315 && th < -45)
      ctrlmode = 1;
    if(th >= -225 && th <= -135)
      ctrlmode = 2;
    if(th > -315 && th < -225)
      ctrlmode = 3;
  }
}


/*
 * Forward Backward Left Right determine which way the marble should move when called!
 */
static void MoveForward(){
  ball_ph -= speed;
  ball_z -= movement;
  forward = 1;
  sideways = 0;
}
static void MoveBackward(){
  ball_ph += speed;
  ball_z += movement;
  forward = 1;
  sideways = 0;
}
static void MoveLeft(){
  ball_th += speed;
  ball_x -= movement;
  forward = 0;
  sideways = 1;
}
static void MoveRight(){
  ball_th -= speed;
  ball_x += movement;
  forward = 0;
  sideways = 1;
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
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
   int cans_left =  getNumCansLeft(); //check how many cans to go
   gluLookAt(Ex+ball_x,Ey, Ez+ball_z, ball_x,0,ball_z , 0,Cos(ph),0);

   //  Flat or smooth shading
   glShadeModel(GL_SMOOTH);

      //  Translate intensity to color vectors
      float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
      float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
      float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
      //  Light position
      //float Position[]  = {distance*Cos(zh),1,distance*Sin(zh),1.0};
      float Position[]  = {4,1,-1,  1.0};

      //  Draw light position as ball (still no lighting here)
      glColor3f(1,1,1);

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



   //Draw Skybox
   drawSkybox(20, ball_x, ball_z);

   //Display water floor
   DrawWaterFloor();

   //Draw Dolphins
   DrawDolphin(2,-5, -2, 1, 2, -90, zh);
   DrawDolphin(2,-5, -4, 0.7, 2, -90, zh+95);
   DrawDolphin(6,-5, -10, 1, 3, 0, zh+180);


   //Draw Skull
   double sklook = (6*ball_x)+50;
   DrawSkull(-9, 2, -10,  1.7, sklook, 0);



   //Instead of declaring these as individual objects, use an array to map track
   DrawGround();


   DrawMarble(ball_x,ball_y,ball_z,     0.2, ball_th, ball_ph);
   //UNCOMMENT ME
   CheckGround(ball_x, ball_z);//determine if the ball can drop

   DrawCollectables(); //draw the cans in the map

   DrawComputer(-1,0.2,-5.6,   1.4,  0,0);

   DrawParthenon(-15,0,-5.7, 2, 90);
   drawError(-13.6, 1.3, -5.7,   1, 90, 1);

   DrawPillar(-6,0,-5.3,   3, 0);
   DrawPillar(-6,0,-3.3,   3, 0);


   DrawPillar(-7.8,0,-5.3,   3, 0);
   DrawPillar(-7.8,0,-3.3,   3, 0);

   DrawPillar(-9.8,0,-5.3,   3, 0);
   DrawPillar(-9.8,0,-3.3,   3, 0);


   //Draw pillars
   double pilht = 4.0;

   DrawPillar(-4,0,-2.5,   pilht, 1);
   DrawPillar(-2.1,0,-2.5,   pilht, 1);

   DrawPillar(-4,0,-6,   pilht, 1);
   DrawPillar(-2.1,0,-6,   pilht, 1);


   DrawPillar(-1,0,-6.7,   pilht, 1);
   DrawPillar(-1,0,-8.7,   pilht, 1);


   DrawPillar(4,0,-5.18,   pilht, 1);
   DrawPillar(4,0,-7.18,   pilht, 1);


   drawError(-3+0.2*Sin(zh), 1, -11, 0.9, 0, 0);
   drawError(-3.1+0.3*Sin(zh), 1.1, -11.1, 0.9, 0, 0);
   drawError(-3.2+0.4*Sin(zh), 1.2, -11.2, 0.9, 0, 0);
   drawError(-3.3+0.5*Sin(zh), 1.3, -11.3, 0.9, 0, 0);
   drawError(-3.4+0.6*Sin(zh), 1.4, -11.4, 0.9, 0, 0);


   drawError(-5.2, 1, -9.5-0.2*Cos(zh), 0.9, 90, 0);
   drawError(-5.3, 1.1, -9.5-0.3*Cos(zh), 0.9, 90, 0);
   drawError(-5.4, 1.2, -9.5-0.4*Cos(zh), 0.9, 90, 0);
   drawError(-5.5, 1.3, -9.5-0.5*Cos(zh), 0.9, 90, 0);
   drawError(-5.6, 1.4, -9.5-0.6*Cos(zh), 0.9, 90, 0);



   DrawPillar(1.5,0,-8.7,   pilht, 1);
   drawError(3.2, 1, -10.3+0.2*Cos(zh), 0.9, 270, 0);
   drawError(3.3, 1.1, -10.3+0.3*Cos(zh), 0.9, 270, 0);
   drawError(3.4, 1.2, -10.3+0.4*Cos(zh), 0.9, 270, 0);
   drawError(3.5, 1.3, -10.3+0.5*Cos(zh), 0.9, 270, 0);
   drawError(3.6, 1.4, -10.3+0.6*Cos(zh), 0.9, 270, 0);


   DrawComputer(11.6,0.2,-8.0,   1.5,  270,0);


   DrawPillar(3.62,0,-2.49,   pilht, 1);
   DrawPillar(3.62,0,-0.49,   pilht, 1);
   DrawPillar(7.1,0,-0.49,   pilht, 1);
   drawError(13.5+(Sin(zh)*0.3), 1.5, -3.9,   1, 270, 1);


   JazzyCup(10, 1, -5.5, 1, 1, -th);
   JazzyCup(10, 1, -2.5, 1, 1, th);

   JazzyCup(13, 1, -5.5, 1, 1, th);
   JazzyCup(13, 1, -2.5, 1, 1, -th);


   //easter egg area
   DrawPaintWindow(8, -0.5, -11, 0.7, 0, 0);

   if(!collectedTape)
      SetFloatingTape(8, 0, -12);

   DrawPillar(6.15,-3,-9.2,   12.5, 0); //pillar to hold secret tape


   //  Draw hud - no lighting from here on
   glDisable(GL_LIGHTING);

   //  Display HUD for text
   if(hud){
      Print("Cans:%d/%d", cans_left,numcans);
      Print("  Secret:%d/1", collectedTape);
  }

   glWindowPos2i(20,20);



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
   double time = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*time,360.0);

   int t;
   t = glutGet(GLUT_ELAPSED_TIME);
   dt = (t - old_t) / 1000.0;
   old_t = t;

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

   th %= 360;
   ph %= 360;
   //  Update projection
   Project(mode?fov:0,asp,dim);

   //set control mode
   SetControlMode();


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
      ctrlmode = 0; //and control mode!
  }
   //  Toggle hud
   else if (ch == 'x' || ch == 'X')
      hud = 1-hud;
   //  Reset Marble Position
   else if (ch=='v' || ch=='V')
      Reset(0);
   //  Reset Game
   else if (ch=='r' || ch=='R')
      Reset(1);

   //  Move camera
   else if (ch == '-')
      zoom += 0.1;
   else if (ch == '+')
      zoom -= 0.1;

  // //  Toggle light movement
  // else if (ch == 'm' || ch == 'M') //REMOVE 'move' variable!!!
  //     move = 1-move;

  //reset zoom if needed
  if(zoom < 0.8){
    zoom = 0.8;
  }
  if(zoom > 3.5){
    zoom = 3.5;
  }

  ball_th %= 360;
  ball_ph %= 360;

  ////MARBLE MOVEMENT
  //Determine which way to move the marble
  //based on the current way the camera is facing
  if (ch == 'w' || ch=='W'){
    if(ctrlmode == 0) MoveForward();
    if(ctrlmode == 1) MoveLeft();
    if(ctrlmode == 2) MoveBackward();
    if(ctrlmode == 3) MoveRight();
  }
  else if (ch == 's' || ch=='S'){
    if(ctrlmode == 0) MoveBackward();
    if(ctrlmode == 1) MoveRight();
    if(ctrlmode == 2) MoveForward();
    if(ctrlmode == 3) MoveLeft();
  }
  else if (ch == 'a' || ch == 'A'){
    if(ctrlmode == 0) MoveLeft();
    if(ctrlmode == 1) MoveBackward();
    if(ctrlmode == 2) MoveRight();
    if(ctrlmode == 3) MoveForward();
  }
  else if (ch == 'd' || ch == 'D'){
    if(ctrlmode == 0) MoveRight();
    if(ctrlmode == 1) MoveForward();
    if(ctrlmode == 2) MoveLeft();
    if(ctrlmode == 3) MoveBackward();
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

   //zoom = 1.0+(width/height);
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
   old_t = glutGet(GLUT_ELAPSED_TIME);
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
   glutInitWindowSize(700,700);
   glutCreateWindow("Final Project, Jake Henson");
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutIdleFunc(idle);
   //glutIdleFunc(idle);

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
   texture[9] = LoadTexBMP("win95.bmp");
   texture[10] = LoadTexBMP("keyboard.bmp");
   texture[11] = LoadTexBMP("compfront.bmp");
   texture[12] = LoadTexBMP("jazzy.bmp");
   texture[13] = LoadTexBMP("drink.bmp");
   texture[14] = LoadTexBMP("secret_tape_front.bmp");
   texture[15] = LoadTexBMP("secret_tape_back.bmp");
   texture[16] = LoadTexBMP("paint.bmp");


   water_texture[0] = LoadTexBMP("water2.bmp");
   water_texture[1] = LoadTexBMP("water3.bmp");
   water_texture[2] = LoadTexBMP("water4.bmp");
   water_texture[3] = LoadTexBMP("water5.bmp");
   water_texture[4] = LoadTexBMP("water6.bmp");
   water_texture[5] = LoadTexBMP("water7.bmp");
   water_texture[6] = LoadTexBMP("water8.bmp");


   skybox[0] = LoadTexBMP("box_Front.bmp");
   skybox[1] = LoadTexBMP("box_back.bmp");
   skybox[2] = LoadTexBMP("box_Left.bmp");
   skybox[3] = LoadTexBMP("box_Right.bmp");
   skybox[4] = LoadTexBMP("box_Top.bmp");

   //load objs


   //LOW POLY DOLPHIN by rkuhl from: https://www.cgtrader.com/free-3d-models/animals/mammal/dolphin-9b7a7368-38e7-4ec1-ba0a-173d603ecc13
   //modified in Blender
   dolphin = LoadOBJ("dolphin.obj");

   //LOW POLY SKULL by Vladmir E., from: https://sketchfab.com/3d-models/low-poly-skull-7dc37b0f8edf4e89843b28db268fc973#download
   //modified in Blender, corrected normals
   skull = LoadOBJ("skull.obj");

   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}
