/*
 *  Computer Graphics Project
 *  Refererred from CSCI 5229 examples by professor william A Schreuder
 *  Adapted by Srivishnu Alvakonda
 *
 *  Demonstrates mimic of Interstellar docking scene 
 *
 *  Key bindings:
 *  i/I        Ranger X increase
 *  o/O        Ranger X decrease
 *  j/J        Ranger Y increase
 *  k/K        Ranger Y decrease
 *  v/V        Ranger z increase
 *  b/B        Ranger z decrease            
 *  l          Toggles lighting
 *  a/A        Decrease/increase ambient light
 *  d/D        Decrease/increase diffuse light
 *  s/S        Decrease/increase specular light
 *  e/E        Decrease/increase emitted light
 *  n/N        Decrease/increase shininess
 *  F1         Toggle smooth/flat shading
 *  F2         Toggle local viewer mode
 *  F3         Toggle light distance (1/5)
 *  F8         Change ball increment
 *  F9         Invert bottom normal
 *  m          Toggles light movement
 *  []         Lower/rise light
 *  p          Toggles ortogonal/perspective projection
 *  +/-        Change field of view of perspective
 *  x          Toggle axes
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */


// when ranger 1(0.3,4.9,-0.6) and rotation matches then docking successfull
#include "CSCIx229.h"
#include<stdbool.h>

int tex_mode=0;    //  Texture mode
int axes=1;       //  Display axes
int mode=1;       //  Projection mode
int move=1;       //  Move light
int th=-320;         //  Azimuth of view angle
int end_th=0;
int ph=-325;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
int light=1;      //  Lighting
double asp=1;     //  Aspect ratio
double dim=25.0;   //  Size of world   7.3
// Light values
int one       =   1;  // Unit value
int distance  =   5;  // Light distance
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   6.9;  // Elevation of light
int ntex=0;    //  Texture
unsigned int texture[20];  //  Texture names
double rep=1;  //  Repetition
int    box=1;    //  Draw sky
int    sky[2];   //  Sky textures  
double ranger1_x = -7.0;  //-1.0
double ranger1_y = 3.0;   //5.2
double ranger1_z = -5.0;   //-1.0
double ranger2_x = -3;   //-3
double ranger2_y = -5;    //5
double ranger2_z = 12;   //12
double docking_x=0;
double docking_y=0;
double docking_z=0;
double docking_x2=0;
double docking_y2=0;
double docking_z2=0;
int range_th = 0;
int dock =0;
int dock_loop =0;
int dock_attach = 0;
bool auto_dock = 0;
int blast = 0;
int auto_dock_2 = 0;
int dock_2 =0;
int dock_loop_2 =0;
bool message_done_1 = 1;
bool message_done_2 = 1;
int ranger_movement = 0;
int ranger_Translated = 0;
int ranger_rotation = 0;
int ranger_rotated = 0;
int mann_movement = 0;
int mann_moved = 0;

int    n=8;       // Number of slices
typedef struct {float x,y,z;} Point;
Point  Lp;        // Light position in local coordinate system
Point  Nc,Ec;     // Far or near clipping plane in local coordinate system
#define MAXN 64    // Maximum number of slices (n) and points in a polygon

/*
 * Finding Normal vectors for the polygons
 */

static void normalize(double a1, double b1, double c1, double a2, double b2, double c2, double a3, double b3, double c3)
{
 double x = ((b2-b1)*(c3-c1))-((b3-b1)*(c2-c1));
 double y = (-1) * (((a2-a1)*(c3-c1))-((a3-a1)*(c2-c1)));
 double z = ((a2-a1)*(b3-b1))-((a3-a1)*(b2-b1));
 glNormal3d(x,y,z);
}

void idle()
{
   //  Elapsed time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,360.0);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  Draw a cylinder
*/
static void Cylinder(float x,float y,float z , float th,float ph , float R,float H)
{
   int i,j;   // Counters
   int N=4*n; // Number of slices

   glPushMatrix();
   //glLoadIdentity(x,y,z);
   glTranslated(x,y,z);
   glRotated(ph,1,0,0);
   glRotated(th,0,1,0);
   glScaled(R,R,H);

   //Color(0,1,1);
   //  Two end caps (fan of triangles)
  // glDisable(GL_TEXTURE_2D);
   glColor3f(1,1,1);
   for (j=-1;j<=1;j+=2)
      for (i=0;i<N;i++)
      {
         float th0 = j* i   *360.0/N;
         float th1 = j*(i+1)*360.0/N;
         Point P[3] = { {0,0,j} , {Cos(th0),Sin(th0),j} , {Cos(th1),Sin(th1),j} };
         Point N[3] = { {0,0,j} , {       0,       0,j} , {       0,       0,j} };
         Point T[3] = { {0,0,0} , {Cos(th0),Sin(th0),0} , {Cos(th1),Sin(th1),0} };
         glBegin(GL_POLYGON);
      for (int k=0;k<3;k++)
      {
         glNormal3f(N[k].x,N[k].y,N[k].z);
         glTexCoord2f(T[k].x,T[k].y);
         glVertex3f(P[k].x,P[k].y,P[k].z);
      }
      glEnd();
      }
   //  Cylinder Body (strip of quads)
   for (i=0;i<N;i++)
   {
      float th0 =  i   *360.0/N;
      float th1 = (i+1)*360.0/N;
      Point P[4] = { {Cos(th0),Sin(th0),+1} , {Cos(th0),Sin(th0),-1} , {Cos(th1),Sin(th1),-1} , {Cos(th1),Sin(th1),+1} };
      Point N[4] = { {Cos(th0),Sin(th0), 0} , {Cos(th0),Sin(th0), 0} , {Cos(th1),Sin(th1), 0} , {Cos(th1),Sin(th1), 0} };
      Point T[4] = { {       0,th0/90.0, 0} , {       2,th0/90.0, 0} , {       2,th1/90.0, 0} , {       0,th1/90.0, 0} };
      glBegin(GL_POLYGON);
      for (int k=0;k<4;k++)
      {
         glNormal3f(N[k].x,N[k].y,N[k].z);
         glTexCoord2f(T[k].x,T[k].y);
         glVertex3f(P[k].x,P[k].y,P[k].z);
      }
      glEnd();
   }

   glPopMatrix();
}




/*
 *  Draw the cockpit as an overlay
 *  Must be called last
 */
void message_display1(int message)
{
   //  Screen edge
   //float w = asp>2 ? asp : 2;
   //  Save transform attributes (Matrix Mode and Enabled Modes)
   glPushAttrib(GL_TRANSFORM_BIT|GL_ENABLE_BIT);
   //  Save projection matrix and set unit transform
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   glOrtho(-asp,+asp,-1,1,-1,1);
   //  Save model view matrix and set to indentity
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();
   //  Draw instrument panel with texture
   glColor3f(1,1,1);
   glEnable(GL_TEXTURE_2D);
   if(message ==1)
   glBindTexture(GL_TEXTURE_2D,texture[10]);
   else if(message ==2)
   glBindTexture(GL_TEXTURE_2D,texture[12]);
   else if(message ==3)
   glBindTexture(GL_TEXTURE_2D,texture[14]);
   glBegin(GL_QUADS);
   glTexCoord2d(0,0);glVertex2f(-1.5,0.5);
   glTexCoord2d(1,0);glVertex2f(-1.0,0.5);
   glTexCoord2d(1,1);glVertex2f(-1.0,1.0);
   glTexCoord2d(0,1);glVertex2f(-1.5,1.0);
   glEnd();
   glDisable(GL_TEXTURE_2D);
   //  Reset model view matrix
   glPopMatrix();
   //  Reset projection matrix
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   //  Pop transform attributes (Matrix Mode and Enabled Modes)
   glPopAttrib();
}

void message_display2(int message)
{
   //  Screen edge
   //float w = asp>2 ? asp : 2;
   //  Save transform attributes (Matrix Mode and Enabled Modes)
   glPushAttrib(GL_TRANSFORM_BIT|GL_ENABLE_BIT);
   //  Save projection matrix and set unit transform
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   glOrtho(-asp,+asp,-1,1,-1,1);
   //  Save model view matrix and set to indentity
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();
   //  Draw instrument panel with texture
   glColor3f(1,1,1);
   glEnable(GL_TEXTURE_2D);
   if(message ==1)
   glBindTexture(GL_TEXTURE_2D,texture[11]);
   else if(message ==2)
   glBindTexture(GL_TEXTURE_2D,texture[13]);
   glBegin(GL_QUADS);
   glTexCoord2d(0,0);glVertex2f(-1.5,0);
   glTexCoord2d(1,0);glVertex2f(-1.0,0);
   glTexCoord2d(1,1);glVertex2f(-1.0,0.5);
   glTexCoord2d(0,1);glVertex2f(-1.5,0.5);
   glEnd();
   glDisable(GL_TEXTURE_2D);
   //  Reset model view matrix
   glPopMatrix();
   //  Reset projection matrix
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   //  Pop transform attributes (Matrix Mode and Enabled Modes)
   glPopAttrib();
}

void message_display3(int message)
{
   //  Screen edge
   //float w = asp>2 ? asp : 2;
   //  Save transform attributes (Matrix Mode and Enabled Modes)
   glPushAttrib(GL_TRANSFORM_BIT|GL_ENABLE_BIT);
   //  Save projection matrix and set unit transform
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   glOrtho(-asp,+asp,-1,1,-1,1);
   //  Save model view matrix and set to indentity
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();
   //  Draw instrument panel with texture
   glColor3f(1,1,1);
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D,texture[15]);
   glBegin(GL_QUADS);
   glTexCoord2d(0,0);glVertex2f(-1.5,-0.5);
   glTexCoord2d(1,0);glVertex2f(-1.0,-0.5);
   glTexCoord2d(1,1);glVertex2f(-1.0,0);
   glTexCoord2d(0,1);glVertex2f(-1.5,0);
   glEnd();
   glDisable(GL_TEXTURE_2D);
   //  Reset model view matrix
   glPopMatrix();
   //  Reset projection matrix
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   //  Pop transform attributes (Matrix Mode and Enabled Modes)
   glPopAttrib();
}


/*
 *  Draw vertex in polar coordinates with normal
 */
static void Vertex(double th,double ph)
{
   double x = Sin(th)*Cos(ph);
   double y = Cos(th)*Cos(ph);
   double z =         Sin(ph);
   //  For a sphere at the origin, the position
   //  and normal vectors are the same
   glNormal3d(x,y,z);
   glVertex3d(x,y,z);
}

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
         glTexCoord2f(0.5,0.5); Vertex(th,ph);
         Vertex(th,ph+inc);
      }
      glEnd();
   }
   //  Undo transofrmations
   glPopMatrix();
}

// cube 
static void cube(double x,double y,double z,
                 double dx,double dy,double dz,
                 double thx,double thy,double thz, unsigned int part)
{

   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(thx,1,0,0);
   glRotated(thy,0,1,0);
   glRotated(thz,0,0,1);
   glScaled(dx,dy,dz);

   //  Cube
   glBindTexture(GL_TEXTURE_2D,texture[2]);
   glBegin(GL_QUADS);
   //  Front
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1, 1);
   glTexCoord2f(rep,0.0); glVertex3f(+1,-1, 1);
   glTexCoord2f(rep,rep); glVertex3f(+1,+1, 1);
   glTexCoord2f(0.0,rep); glVertex3f(-1,+1, 1);
   
   glEnd();
   //  Back
   glBindTexture(GL_TEXTURE_2D,texture[2]);
    glBegin(GL_QUADS);
   glNormal3f( 0, 0, -1);
   glTexCoord2f(0.0,0.0); glVertex3f(+1,-1,-1);
   glTexCoord2f(rep,0.0); glVertex3f(-1,-1,-1);
   glTexCoord2f(rep,rep); glVertex3f(-1,+1,-1);
   glTexCoord2f(0.0,rep); glVertex3f(+1,+1,-1);
   glEnd();
   //  Right
   glBindTexture(GL_TEXTURE_2D,texture[2]);
   glBegin(GL_QUADS);
   glNormal3f( 1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(+1,-1,+1);
   glTexCoord2f(rep,0.0); glVertex3f(+1,-1,-1);
   glTexCoord2f(rep,rep); glVertex3f(+1,+1,-1);
   glTexCoord2f(0.0,rep); glVertex3f(+1,+1,+1);
   glEnd();
   //  Left
   if(part == 1)
   {
   glBindTexture(GL_TEXTURE_2D,texture[3]);
   }
   else
   {
   glBindTexture(GL_TEXTURE_2D,texture[2]);
   }
   glBegin(GL_QUADS);
   glNormal3f( -1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1,-1);
   glTexCoord2f(rep,0.0); glVertex3f(-1,-1,+1);
   glTexCoord2f(rep,rep); glVertex3f(-1,+1,+1);
   glTexCoord2f(0.0,rep); glVertex3f(-1,+1,-1);
   glEnd();
   //  Top
    
   if(part==3)
   {
   glBindTexture(GL_TEXTURE_2D,texture[0]);
   }
   else if(part==2)
   {
   glBindTexture(GL_TEXTURE_2D,texture[4]);
   }
   else
   {
   glBindTexture(GL_TEXTURE_2D,texture[2]);
   }
   glBegin(GL_QUADS);
   glNormal3f( 0, 1, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,+1,+1);
   glTexCoord2f(rep,0.0); glVertex3f(+1,+1,+1);
   glTexCoord2f(rep,rep); glVertex3f(+1,+1,-1);
   glTexCoord2f(0.0,rep); glVertex3f(-1,+1,-1);
   //glDisable(GL_TEXTURE_2D);
   glEnd();
   
   //  Bottom
   glBindTexture(GL_TEXTURE_2D,texture[2]);
    glBegin(GL_QUADS);
   glNormal3f( 0, -one, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1,-1);
   glTexCoord2f(rep,0.0); glVertex3f(+1,-1,-1);
   glTexCoord2f(rep,rep); glVertex3f(+1,-1,+1);
   glTexCoord2f(0.0,rep); glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   //  Undo transformations
   glPopMatrix();
}

// cube 
static void cube1(double x,double y,double z,
                 double dx,double dy,double dz,
                 double thx,double thy,double thz, unsigned int part)
{

   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(thx,1,0,0);
   glRotated(thy,0,1,0);
   glRotated(thz,0,0,1);
   glScaled(dx,dy,dz);

   if(part ==0 )
   {
    //  Cube
   glBindTexture(GL_TEXTURE_2D,texture[9]);
   glBegin(GL_QUADS);
   //  Front
  glNormal3f( 0, 0, 1);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1, 1);
   glTexCoord2f(rep,0.0); glVertex3f(+1,-1, 1);
   glTexCoord2f(rep,rep); glVertex3f(+1,+1, 1);
   glTexCoord2f(0.0,rep); glVertex3f(-1,+1, 1);
   
   glEnd();
   //  Back
    glBindTexture(GL_TEXTURE_2D,texture[9]);
    glBegin(GL_QUADS);
   glNormal3f( 0, 0, -1);
   glTexCoord2f(0.0,0.0); glVertex3f(+1,-1,-1);
   glTexCoord2f(rep,0.0); glVertex3f(-1,-1,-1);
   glTexCoord2f(rep,rep); glVertex3f(-1,+1,-1);
   glTexCoord2f(0.0,rep); glVertex3f(+1,+1,-1);
   glEnd();
   //  Right
   glBindTexture(GL_TEXTURE_2D,texture[9]);
   glBegin(GL_QUADS);
   glNormal3f( 1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(+1,-1,+1);
   glTexCoord2f(rep,0.0); glVertex3f(+1,-1,-1);
   glTexCoord2f(rep,rep); glVertex3f(+1,+1,-1);
   glTexCoord2f(0.0,rep); glVertex3f(+1,+1,+1);
   glEnd();
   //  Left
   glBindTexture(GL_TEXTURE_2D,texture[9]);
   glBegin(GL_QUADS);
   glNormal3f( -1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1,-1);
   glTexCoord2f(rep,0.0); glVertex3f(-1,-1,+1);
   glTexCoord2f(rep,rep); glVertex3f(-1,+1,+1);
   glTexCoord2f(0.0,rep); glVertex3f(-1,+1,-1);
   glEnd();
   //  Top
   glBindTexture(GL_TEXTURE_2D,texture[9]);
   glBegin(GL_QUADS);
   glNormal3f( 0, 1, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,+1,+1);
   glTexCoord2f(rep,0.0); glVertex3f(+1,+1,+1);
   glTexCoord2f(rep,rep); glVertex3f(+1,+1,-1);
   glTexCoord2f(0.0,rep); glVertex3f(-1,+1,-1);
   glEnd();
   //  Bottom
     glBindTexture(GL_TEXTURE_2D,texture[9]);
    glBegin(GL_QUADS);
   glNormal3f( 0, -one, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1,-1);
   glTexCoord2f(rep,0.0); glVertex3f(+1,-1,-1);
   glTexCoord2f(rep,rep); glVertex3f(+1,-1,+1);
   glTexCoord2f(0.0,rep); glVertex3f(-1,-1,+1);
   //  End
   glEnd();


   }
   else 
   {
   //  Cube
   glBegin(GL_QUADS);
   //  Front
   glNormal3f( 0, 0, 1);
   glVertex3f(-1,-1, 1);
   glVertex3f(+1,-1, 1);
   glVertex3f(+1,+1, 1);
   glVertex3f(-1,+1, 1);
   
   glEnd();
   //  Back
    glBegin(GL_QUADS);
   glNormal3f( 0, 0, -1);
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   glEnd();
   //  Right
    glBegin(GL_QUADS);
   glNormal3f( 1, 0, 0);
   glVertex3f(+1,-1,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,+1);
   glEnd();
   //  Left
   glBegin(GL_QUADS);
   glNormal3f( -1, 0, 0);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,-1,+1);
   glVertex3f(-1,+1,+1);
   glVertex3f(-1,+1,-1);
   glEnd();
   //  Top
   glBegin(GL_QUADS);
   glNormal3f( 0, 1, 0);
   glVertex3f(-1,+1,+1);
   glVertex3f(+1,+1,+1);
   glVertex3f(+1,+1,-1);
   glVertex3f(-1,+1,-1);
   glEnd();
   //  Bottom
    glBegin(GL_QUADS);
   glNormal3f( 0, -one, 0);
   glVertex3f(-1,-1,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,-1,+1);
   glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   }
   //  Undo transformations
   glPopMatrix();
}


// cube 
static void cube2(double x,double y,double z,
                 double dx,double dy,double dz,
                 double thx,double thy,double thz, unsigned int part)
{
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(thx,1,0,0);
   glRotated(thy,0,1,0);
   glRotated(-2*end_th,0,1,0);
   glRotated(thz,0,0,1);
   glScaled(dx,dy,dz);

   //  Cube
   glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_QUADS);
   //  Front
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1, 1);
   glTexCoord2f(rep,0.0); glVertex3f(+1,-1, 1);
   glTexCoord2f(rep,rep); glVertex3f(+1,+1, 1);
   glTexCoord2f(0.0,rep); glVertex3f(-1,+1, 1);
   
   glEnd();
   //  Back
    glBindTexture(GL_TEXTURE_2D,texture[7]);
    glBegin(GL_QUADS);
   glNormal3f( 0, 0, -1);
   glTexCoord2f(0.0,0.0); glVertex3f(+1,-1,-1);
   glTexCoord2f(rep,0.0); glVertex3f(-1,-1,-1);
   glTexCoord2f(rep,rep); glVertex3f(-1,+1,-1);
   glTexCoord2f(0.0,rep); glVertex3f(+1,+1,-1);
   glEnd();
   //  Right
   glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_QUADS);
   glNormal3f( 1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(+1,-1,+1);
   glTexCoord2f(rep,0.0); glVertex3f(+1,-1,-1);
   glTexCoord2f(rep,rep); glVertex3f(+1,+1,-1);
   glTexCoord2f(0.0,rep); glVertex3f(+1,+1,+1);
   glEnd();
   //  Left
   glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_QUADS);
   glNormal3f( -1, 0, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1,-1);
   glTexCoord2f(rep,0.0); glVertex3f(-1,-1,+1);
   glTexCoord2f(rep,rep); glVertex3f(-1,+1,+1);
   glTexCoord2f(0.0,rep); glVertex3f(-1,+1,-1);
   glEnd();
   //  Top
   glBindTexture(GL_TEXTURE_2D,texture[5]);
   glBegin(GL_QUADS);
   glNormal3f( 0, 1, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,+1,+1);
   glTexCoord2f(rep,0.0); glVertex3f(+1,+1,+1);
   glTexCoord2f(rep,rep); glVertex3f(+1,+1,-1);
   glTexCoord2f(0.0,rep); glVertex3f(-1,+1,-1);
   glEnd();
   //  Bottom
     glBindTexture(GL_TEXTURE_2D,texture[5]);
    glBegin(GL_QUADS);
   glNormal3f( 0, -one, 0);
   glTexCoord2f(0.0,0.0); glVertex3f(-1,-1,-1);
   glTexCoord2f(rep,0.0); glVertex3f(+1,-1,-1);
   glTexCoord2f(rep,rep); glVertex3f(+1,-1,+1);
   glTexCoord2f(0.0,rep); glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   //  Undo transformations
   glPopMatrix();
}

/*
 * Draw a ranger
 */

static void ranger(double x,double y,double z,
                 double dx,double dy,double dz,
                 double range_th,int part)
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
   glRotated(range_th, 0,1,0);
   glScaled(dx,dy,dz);
   //  Ranger back 1st cube
   glColor3f(0.4,0.4,0.4);
   cube(-1.65,0,0,0.1,0.1,0.5,0,0,0,1);  //0
   // Ranger 2nd cube from back
   glColor3f(1,1,1);
   cube(-0.65,0,0,1,0.1,0.5,0,0,0,2); //1
   // ranger top floor 
   glColor3f(0.4,0.4,0.4);
   cube(-0.15,0.05,0,0.5,0.15,0.5,0,0,0,3);  //1.5


   // ranger top design front
   if(part == 1)
   glColor3f(0,0,1);
   else
   glColor3f(1,0,0);
   cube1(0.45,0.2,0,0.1,0.02,0.6,0,0,0,4);  //2.1
   // ranger top design right side front
   if(part == 1)
   glColor3f(0,0,1);
   else
   glColor3f(1,0,0);
   cube1(0.1,0.1,-0.5,0.3,0.02,0.05,0,0,20,5); //1.75
   // ranger top design left side front
   if(part == 1)
   glColor3f(0,0,1);
   else
   glColor3f(1,0,0);
   cube1(0.1,0.1,0.5,0.3,0.02,0.05,0,0,20,6); //1.75
   // ranger top design right side back
   if(part == 1)
   glColor3f(0,0,1);
   else
   glColor3f(1,0,0);
   cube1(-0.40,0.1,-0.5,0.3,0.02,0.05,0,0,-20,7);  //1.25
   // ranger top design left side  back
   if(part == 1)
   glColor3f(0,0,1);
   else
   glColor3f(1,0,0);
   cube1(-0.40,0.1,0.5,0.3,0.02,0.05,0,0,-20,8); //1.25
   // ranger top design right side angle
   if(part == 1)
   glColor3f(0,0,1);
   else
   glColor3f(1,0,0);
   cube1(-0.90,0.2,-0.38,0.3,0.02,0.05,0,30,0,9); //0.75
   // ranger top design left side angle
   if(part == 1)
   glColor3f(0,0,1);
   else
   glColor3f(1,0,0);
   cube1(-0.90,0.2,0.38,0.3,0.02,0.05,0,-30,0,10); //0.75
   if(part == 1)
   glColor3f(0,0,1);
   else
   glColor3f(1,0,0);
   cube1(-1.65,0.1,0,0.1,0.05,0.5,0,0,0,11);  //0
   

   glBindTexture(GL_TEXTURE_2D,texture[2]);
   glBegin(GL_TRIANGLES);  
   glColor3f(1,1,1);
   normalize(+0.35, -0.12, 0.5,0.65, 0, 0.5,0.35, 0.18, 0.5);
   glTexCoord2f(0.0,0.0); glVertex3f(0.35, +0.18, 0.5);    //2
   glTexCoord2f(rep,0.0); glVertex3f(0.35, -0.12, 0.5);  //2 
   glTexCoord2f(rep,rep); glVertex3f(0.65, 0, 0.5);  //2.3
   glEnd();

   glBegin(GL_TRIANGLES);  
   glColor3f(1,1,1);
   normalize(+0.35, -0.12, -0.5,0.65, 0, -0.5,0.35, 0.18, -0.5);
   glTexCoord2f(0.0,0.0); glVertex3f(0.35, +0.18, -0.5); //2  
   glTexCoord2f(rep,0.0); glVertex3f(0.35, -0.12, -0.5); //2
   glTexCoord2f(rep,rep); glVertex3f(0.65, 0, -0.5); //2.3
   glEnd();

   glBegin(GL_POLYGON);  
   glColor3f(1,1,1);
   
   normalize(+0.35, 0.18, -0.5,0.35, 0.18, 0.5,0.65, 0, -0.5);
   glTexCoord2f(0.0,0.0); glVertex3f(0.35, +0.18, 0.5);  //2 
   glTexCoord2f(rep,0.0); glVertex3f(0.35, +0.18, -0.5); //2
   glTexCoord2f(rep,rep); glVertex3f(0.65, 0, -0.5);//2.3
   glTexCoord2f(0.0,rep); glVertex3f(0.65, 0, 0.5); //2.3
   glEnd();


   glBegin(GL_POLYGON);  
   glColor3f(1,1,1);
   normalize(+0.35, -0.12, -0.5,0.65, 0, -0.5,0.35, -0.12, 0.5);
   glTexCoord2f(0.0,0.0); glVertex3f(0.35, -0.12, 0.5);  
   glTexCoord2f(rep,0.0); glVertex3f(0.35, -0.12, -0.5); 
   glTexCoord2f(rep,rep); glVertex3f(0.65, 0, -0.5);
   glTexCoord2f(0.0,rep); glVertex3f(0.65, 0, 0.5);
   glEnd();
   
  
    glPopMatrix();
   //glDisable(GL_TEXTURE_2D);
   //cube(0.66,1.33,0,0.2,1,0.2,0);
   //chimney
   
   
}


static void connector(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)

{

   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
    Cylinder(0,0,0,0,0,0.05,0.9); //x,y,x,th,ph,r,h
    Cylinder(0,0,0,27,0,0.05,1.1); //x,y,x,th,ph,r,h
    Cylinder(0,0,0,60,0,0.05,1.1); //x,y,x,th,ph,r,h
    Cylinder(0,0,0,90,0,0.05,1.1); //x,y,x,th,ph,r,h
    Cylinder(0,0,0,120,0,0.05,1.1); //x,y,x,th,ph,r,h
    Cylinder(0,0,0,150,0,0.05,1.1); //x,y,x,th,ph,r,h


glPopMatrix();

}




/*
 * Draw Endurance
 */

static void endurance(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
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
   glRotated(end_th,0,1,0);
   glScaled(dx,dy,dz);
 
   connector(0,0.2,0,1,1,1,0);
   connector(0,-0.2,0,1,1,1,0);

   cube1(0,0,0,0.1,0.6,0.1,0,0,0,0);
   cube2(0,0,1,0.1,0.3,0.1,0,0,0,1);
   cube2(0,0,-1,0.1,0.3,0.1,0,0,0,2);
   cube2(-0.5,0,1,0.1,0.3,0.1,0,0,0,3);
   cube2(-0.5,0,-1,0.1,0.3,0.1,0,0,0,4);
   cube2(0.5,0,1,0.1,0.3,0.1,0,0,0,5);
   cube2(0.5,0,-1,0.1,0.3,0.1,0,0,0,6);
   cube2(1,0,0,0.1,0.3,0.1,0,0,0,7);
   cube2(-1,0,0,0.1,0.3,0.1,0,0,0,8);
   cube2(1,0,-0.5,0.1,0.3,0.1,0,0,0,9);
   cube2(-1,0,0.5,0.1,0.3,0.1,0,0,0,10);
   cube2(-1,0,-0.5,0.1,0.3,0.1,0,0,0,11);
   cube2(1,0,0.5,0.1,0.3,0.1,0,0,0,12);
   
  
  glPopMatrix();
}


static void connector_damaged(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)

{

   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
    Cylinder(0,0,0,60,0,0.05,1.1); //x,y,x,th,ph,r,h
    Cylinder(0,0,0,90,0,0.05,1.1); //x,y,x,th,ph,r,h
    Cylinder(0,0,0,120,0,0.05,1.1); //x,y,x,th,ph,r,h
    Cylinder(0,0,0,150,0,0.05,1.1); //x,y,x,th,ph,r,h


glPopMatrix();

}
static void endurance_damaged(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
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
   glRotated(end_th,0,1,0);
   glScaled(dx,dy,dz);
 
   connector_damaged(0,0.2,0,1,1,1,0);
   connector_damaged(0,-0.2,0,1,1,1,0);

   cube1(0,0,0,0.1,0.6,0.1,0,0,0,0);
   //cube2(0,0,1,0.1,0.3,0.1,0,0,0,1);
   //cube2(0,0,-1,0.1,0.3,0.1,0,0,0,2);  //
   cube2(-0.5,0,1,0.1,0.3,0.1,0,0,0,3);
   //cube2(-0.5,0,-1,0.1,0.3,0.1,0,0,0,4);
   //cube2(0.5,0,1,0.1,0.3,0.1,0,0,0,5);
   cube2(0.5,0,-1,0.1,0.3,0.1,0,0,0,6);
   cube2(1,0,0,0.1,0.3,0.1,0,0,0,7);  //
   cube2(-1,0,0,0.1,0.3,0.1,0,0,0,8);
   cube2(1,0,-0.5,0.1,0.3,0.1,0,0,0,9);
   cube2(-1,0,0.5,0.1,0.3,0.1,0,0,0,10);  //
   cube2(-1,0,-0.5,0.1,0.3,0.1,0,0,0,11);
   cube2(1,0,0.5,0.1,0.3,0.1,0,0,0,12);
   
  
  glPopMatrix();
}
 

/* 
 *  Draw sky box
 */
static void Sky(double D)
{
   glColor3f(1,1,1);
   glEnable(GL_TEXTURE_2D);

   //  Sides
   glBindTexture(GL_TEXTURE_2D,sky[0]);
   glBegin(GL_QUADS);
   glTexCoord2f(0.00,0); glVertex3f(-D,-D,-D);
   glTexCoord2f(0.25,0); glVertex3f(+D,-D,-D);
   glTexCoord2f(0.25,1); glVertex3f(+D,+D,-D);
   glTexCoord2f(0.00,1); glVertex3f(-D,+D,-D);

   glTexCoord2f(0.25,0); glVertex3f(+D,-D,-D);
   glTexCoord2f(0.50,0); glVertex3f(+D,-D,+D);
   glTexCoord2f(0.50,1); glVertex3f(+D,+D,+D);
   glTexCoord2f(0.25,1); glVertex3f(+D,+D,-D);

   glTexCoord2f(0.50,0); glVertex3f(+D,-D,+D);
   glTexCoord2f(0.75,0); glVertex3f(-D,-D,+D);
   glTexCoord2f(0.75,1); glVertex3f(-D,+D,+D);
   glTexCoord2f(0.50,1); glVertex3f(+D,+D,+D);

   glTexCoord2f(0.75,0); glVertex3f(-D,-D,+D);
   glTexCoord2f(1.00,0); glVertex3f(-D,-D,-D);
   glTexCoord2f(1.00,1); glVertex3f(-D,+D,-D);
   glTexCoord2f(0.75,1); glVertex3f(-D,+D,+D);
   glEnd();

   //  Top and bottom
   glBindTexture(GL_TEXTURE_2D,sky[1]);
   glBegin(GL_QUADS);
   glTexCoord2f(0.0,0); glVertex3f(+D,+D,-D);
   glTexCoord2f(0.5,0); glVertex3f(+D,+D,+D);
   glTexCoord2f(0.5,1); glVertex3f(-D,+D,+D);
   glTexCoord2f(0.0,1); glVertex3f(-D,+D,-D);

   glTexCoord2f(1.0,1); glVertex3f(-D,-D,+D);
   glTexCoord2f(0.5,1); glVertex3f(+D,-D,+D);
   glTexCoord2f(0.5,0); glVertex3f(+D,-D,-D);
   glTexCoord2f(1.0,0); glVertex3f(-D,-D,-D);
   glEnd();

   glDisable(GL_TEXTURE_2D);
}


void docking_cooper(double x,double y,double z,
                 double dx,double dy,double dz,
                 double thx,double thy,double thz)
{
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
  //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(thx,1,0,0);
   glRotated(thy,0,1,0);
   glRotated(thz,0,0,1);
   glScaled(dx,dy,dz);
   glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE ,tex_mode?GL_REPLACE:GL_MODULATE);
  
  ranger(ranger1_x,ranger1_y,ranger1_z,1,1,1,range_th,1); //-7,1,-3,1,1,1,1
  glDisable(GL_TEXTURE_2D);
   
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE ,tex_mode?GL_REPLACE:GL_MODULATE);
  endurance_damaged(0,0,0,4,4,4,0);
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();


}

void docking_mann(double x,double y,double z,
                 double dx,double dy,double dz,
                 double thx,double thy,double thz)
{
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
  //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(thx,1,0,0);
   glRotated(thy,0,1,0);
   glRotated(thz,0,0,1);
   glScaled(dx,dy,dz);
   glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE ,tex_mode?GL_REPLACE:GL_MODULATE);
  
  ranger(ranger1_x,ranger1_y,ranger1_z,1,1,1,range_th,1); //-7,1,-3,1,1,1,1
  glDisable(GL_TEXTURE_2D);
   
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE ,tex_mode?GL_REPLACE:GL_MODULATE);
  endurance(0,0,0,4,4,4,0);
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE ,tex_mode?GL_REPLACE:GL_MODULATE);
  ranger(ranger2_x,ranger2_y,ranger2_z,1,1,1,1,2); //-3,5,8,1,1,1,1
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();


}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   const double len=2.0;  //  Length of axes
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);

   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective - set eye position
   if (mode)
   {
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   }
   //  Orthogonal - set world orientation
   else
   {
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
   }

   //  Flat or smooth shading
   glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

   if (box) Sky(3.5*dim);

   //  Light switch
   if (light)
   {
      //  Translate intensity to color vectors
      float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
      float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
      float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
      //  Light position
      float Position[]  = {distance*Cos(zh),ylight,distance*Sin(zh),1.0};
      //  Draw light position as ball (still no lighting here)
      glColor3f(1,1,1);
      ball(Position[0],Position[1],Position[2] , 0.7);
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
   }
   else
   {
      glDisable(GL_LIGHTING);
   }
  
  if(blast == 0)
  {
  docking_mann(docking_x2,docking_y2,docking_z2,1.5,1.5,1.5,0,0,0);
  }
  else
  {
  docking_cooper(docking_x,docking_y,docking_z,1.5,1.5,1.5,0,0,0);
  }
//  Switch off textures so it doesn't apply to the rest
  
   
  
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

   //  Display parameters
   glWindowPos2i(5,5);
   Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s Light=%s",
     th,ph,dim,fov,mode?"Perpective":"Orthogonal",light?"On":"Off");
   if (light)
   {
      glWindowPos2i(5,85);
      Print("Model=%s LocalViewer=%s Distance=%d Elevation=%.1f",smooth?"Smooth":"Flat",local?"On":"Off",distance,ylight);
      glWindowPos2i(5,65);
      Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f",ambient,diffuse,specular,emission,shiny);
      glWindowPos2i(5,45);
      Print("Ranger1_X=%lf  Ranger1_Y=%lf Ranger1_Z=%lf",ranger1_x,ranger1_y,ranger1_z);
      glWindowPos2i(5,25);
      Print("Ranger2_X=%lf  Ranger2_Y=%lf Ranger2_Z=%lf",ranger2_x,ranger2_y,ranger2_z);
   }

   if((dock_loop_2 > 40) && ( message_done_1 == 1))
    {
       message_display1(1);
    }
   if((dock_loop > 90) && ( message_done_2 ==1))
    {
       message_display2(1);
    }
   if(ranger_movement == 1)
    {
       message_display1(2);
       message_display3(1);
    }
   if(ranger_rotation == 1)
    {
       message_display2(2);
    }
   if(mann_movement ==1)
    {
       message_display1(3);
    }

   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}


/*
 *  GLUT calls this routine every 50ms
 */
void timer(int x)
{

if((ranger_Translated == 0) && (blast == 1) && (auto_dock == 1) && (dock == 0))
{
message_done_1=0;
ranger_movement = 1;
if(ranger1_x >= 0.01)
{
ranger1_x -=0.04;
}
else if(ranger1_x < -0.01)
{
ranger1_x +=0.04;
}
if(ranger1_y >= 2.51)
{
ranger1_y -=0.01;
}
else if(ranger1_x < 2.49)
{
ranger1_y +=0.01;
}
if(ranger1_z >= 0.01)
{
ranger1_z -=0.04;
}
else if(ranger1_z < -0.01)
{
ranger1_z +=0.04;
}
}


if((auto_dock_2 == 1) && (dock_2 == 0))
{

if(ranger2_x >= 0.01)
{
ranger2_x -=0.04;
}
else if(ranger2_x < -0.01)
{
ranger2_x +=0.04;
}
if(ranger2_y >= -2.49)
{
ranger2_y -=0.01;
}
else if(ranger2_y < -2.51)
{
ranger2_y +=0.01;
}
if(ranger2_z >= 0.01)
{
ranger2_z -=0.04;
}
else if(ranger2_z < -0.01)
{
ranger2_z +=0.04;
}
}


if((blast == 1) && (dock == 0) && (docking_x <= 100.0))
{   
docking_x +=0.1;
}
else if((blast == 1) && (dock == 0) && (docking_x>100.0))
{
docking_x = 0.0;
} 

if((blast == 1) && (dock ==1) )
{

docking_x -= 0.3;
if(dim >=17.0)
{
dim -=0.15;
}
if(docking_x < 5.0 )
{ 
docking_x = 0;
message_done_2 = 0;
ranger_rotation = 0;
//dim = 7.3;

}
}

if((blast == 1) && (dock == 0))
{
end_th += 1;
end_th %= 360;
//printf("X = %lf  Y = %lf  Z = %lf\n", ranger1_x, ranger1_y, ranger1_z);
}


	if(((blast == 1) && (ranger1_x > -0.01)&&(ranger1_x < 0.01))&&((ranger1_y > 2.49)&&(ranger1_y < 2.51))&&((ranger1_z > -0.01)&&(ranger1_z < 0.01)))
	{
		
		ranger_Translated = 1;
                ranger_movement = 0;
                if(dock == 0)
                {
                ranger_rotation = 1;
                }
                range_th += 1;
		range_th %= 360;
		if (dock_loop < 100)
		{
                     dock_loop++;
                if (dock_loop ==99)
                   {
                    ranger_rotation = 0;
                   }
                     
                }
                else
                   { 			
			dock = 1;
 			end_th =0;
 			range_th =0;                        		
		   }
	} 


if(dock_2 == 0)

{

if(((ranger2_x > -0.01)&&(ranger2_x < 0.01))&&((ranger2_y > -2.51)&&(ranger2_y < -2.49))&&((ranger2_z > -0.01)&&(ranger2_z < 0.01)))
{
               mann_movement = 0;
               if (dock_loop_2 < 50)
		{
                     dock_loop_2++;
                     
                }
                else
                   { 			
			blast = 1;
                        dock_2 = 1;                        		
		   }

}


}

   
   //  Set timer to go again
   glutTimerFunc(50,timer,0);
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
   //  PageUp key - increase dim
   else if (key == GLUT_KEY_PAGE_DOWN)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_UP && dim>1)
      dim -= 0.1;
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
   else if (ch == '0')
      th = ph = 0;
   //  Toggle axes
   else if (ch == 'x' || ch == 'X')
      axes = 1-axes;
   //  Toggle lighting
   else if (ch == 'l' || ch == 'L')
      light = 1-light;
   //  Switch projection mode
   else if (ch == 'p' || ch == 'P')
      mode = 1-mode;
   //  Toggle light movement
   else if (ch == 'm' || ch == 'M')
      move = 1-move;
   //  Move light
   else if (ch == '<')
      zh += 1;
   else if (ch == '>')
      zh -= 1;
   //  Change field of view angle
   else if (ch == '-' && ch>1)
      fov--;
   else if (ch == '+' && ch<179)
      fov++;
   //  Light elevation
   else if (ch=='[')
      ylight -= 0.1;
   else if (ch==']')
      ylight += 0.1;
   //  Ambient level
   else if (ch=='a' && ambient>0)
      ambient -= 5;
   else if (ch=='A' && ambient<100)
      ambient += 5;
   //  Diffuse level
   else if (ch=='d' && diffuse>0)
      diffuse -= 5;
   else if (ch=='D' && diffuse<100)
      diffuse += 5;
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
   //  Cycle texture
   else if (ch == 't' || ch == 'T')
      ntex = (ntex+2)%4-1;
    //  Toggle texture mode
   else if (ch == 'g' || ch == 'G')
      tex_mode = 1-tex_mode;
   //  Cycle texture
   else if (ch == 't' || ch == 'T')
      ntex = (ntex+2)%4-1;
   else if (ch == 'i' || ch == 'I')
      ranger1_x += 0.1;
   else if (ch == 'o' || ch == 'O')
      ranger1_x -= 0.1;
   else if (ch == 'j' || ch == 'J')
      ranger1_y += 0.1;
   else if (ch == 'k' || ch == 'K')
      ranger1_y -= 0.1;
   else if (ch == 'v' || ch == 'V')
      ranger1_z += 0.1;
   else if (ch == 'b' || ch == 'B')
      ranger1_z -= 0.1;
   else if (ch == 'q' || ch == 'Q')
     {
       th = -100;
       ph = 20;
       dim = 35;
      }
   else if (ch == 'w' || ch == 'W')
      {
       th = -320;
       ph = -325;
       dim = 25;
      }
   else if (ch == 'z' || ch == 'Z')
      {
      auto_dock = !(auto_dock);
      message_done_1 = 0;
      } 
   else if (ch == 'r')
      {
      auto_dock = 1;
      dock = 0;
      ranger1_x = -7.0;  //-1.0
      ranger1_y = 3.0;   //5.2
      ranger1_z = -5.0;
      dock_loop = 0;
      ranger2_x = -3;   //-3
      ranger2_y = -5;    //5
      ranger2_z = 12;   //12
      dock_loop_2 = 0;
      auto_dock_2 = 1;
      dock_2 = 0;
      blast = 0;
      message_done_1 = 1;
      message_done_2 = 1;
      ranger_Translated = 0;
      mann_movement =1;
      }
   else if (ch == '1')
      ranger2_x += 0.1;
   else if (ch == '2')
      ranger2_x -= 0.1;
   else if (ch == '3')
      ranger2_y += 0.1;
   else if (ch == '4')
      ranger2_y -= 0.1;
   else if (ch == '5')
      ranger2_z += 0.1;
   else if (ch == '6')
      ranger2_z -= 0.1;
   else if (ch == 'u')
      {
      auto_dock_2=1;
      mann_movement =1;
      }
   
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
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //th= -320;
   //ph = -325;
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(1600,900);
   glutCreateWindow("Srivishnu Alvakonda-Project Interstellar Docking");
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutIdleFunc(idle);
   texture[0] = LoadTexBMP("ranger_topview.bmp");
   texture[1] = LoadTexBMP("ranger_backtop.bmp");
   texture[2] = LoadTexBMP("Ranger_other.bmp");
   texture[3] = LoadTexBMP("ranger_back.bmp");
   texture[4] = LoadTexBMP("ranger_topview_behind.bmp");
   texture[5] = LoadTexBMP("cube_top.bmp");
   texture[6] = LoadTexBMP("cube_front.bmp");
   texture[7] = LoadTexBMP("cube_side1.bmp");
   texture[8] = LoadTexBMP("cube_side2.bmp");
   texture[9] = LoadTexBMP("cube_center.bmp");
   texture[10] = LoadTexBMP("mann.bmp");
   texture[11] = LoadTexBMP("cooper.bmp");
   texture[12] = LoadTexBMP("rangermovement.bmp");
   texture[13] = LoadTexBMP("rangerrotation.bmp");
   texture[14] = LoadTexBMP("unauthorized.bmp");
   texture[15] = LoadTexBMP("Mann_destroyed.bmp");
   
   
   

   //  Load skybox texture
   sky[0] = LoadTexBMP("spacesides.bmp");
   sky[1] = LoadTexBMP("spacetopbottom.bmp");
   //  Pass control to GLUT so it can interact with the user
   timer(1);
   ErrCheck("init");
   glutMainLoop();
   return 0;
}
