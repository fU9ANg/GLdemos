

#include <time.h>
#include "../../../src/shared/gltools.h"

static GLint fElect0 = 0;
static GLint fElect1 = 0;
static GLint fElect2 = 0;
static GLint fElect3 = 0;
static time_t tSystemTime;
static struct tm *tmTime;

/*
11:20:10

// HOUR
11/1 * (30)
+
20/60 * (30)
+
10/3600 * (30)
=
? % 360

// MINUTE
20/1 * (6)
+
10/60 * (6)
=
?

// SECOND
10 * (6)
=
?
*/

/*
=====================
  DegSecond
=====================
*/
float DegSecond (int sec)
{
	float rlt = 0.0f;
	rlt = (float) sec * 6.0f;
	return (360.0f - rlt);
}

/*
=====================
  DegMinute
=====================
*/
float DegMinute (struct tm * ptm)
{
	float rlt = 0.0f;
	rlt = (float) (
		((float)ptm->tm_min * 6.0f) +
		((float)ptm->tm_sec / 60.0f * 6.0f));
	return (360.0 - rlt);
}

/*
=====================
  DegHour
=====================
*/
float DegHour (struct tm * ptm)
{
	float rlt = 0.0f;
	rlt = (float) (
		((float)(ptm->tm_hour % 12)   * 30.0f) +
		(((float)ptm->tm_min  / 60.0f)   * 30.0f) +
		(((float)ptm->tm_sec  / 3600.0f) * 30.0f));
	return (360.0f - rlt);
}

/*
=====================
  ChangeSize
=====================
*/
void ChangeSize (int w, int h)
{
    GLfloat fAspect;

    if (h == 0)
        h  = 1;

    glViewport (0, 0, w, h);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();

    fAspect = (GLfloat)w / (GLfloat)h;

    gluPerspective (45.0f, fAspect, 1.0f, 1000.0f);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();

    glTranslatef (0.0f, 0.0f, -100.0f);
    glRotatef (90.0f, 0.0f, 0.0f, 1.0f);
}

/*
=====================
  RenderScene
=====================
*/
void RenderScene (void)
{
	// Update time
	tSystemTime = time (NULL);
	tmTime = localtime (&tSystemTime);
	fElect1 = DegSecond (tmTime->tm_sec);
	fElect2 = DegMinute (tmTime);
	fElect3 = DegHour (tmTime);

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix ();

        // Face
        glPushMatrix ();
            GLUquadric *pObj;
            pObj = gluNewQuadric ();
            gluQuadricNormals (pObj, GL_SMOOTH);
            glColor3f (1.0f, 1.0f, 1.0f);
            gluDisk (pObj, 12.6f, 13.0f, 100, 100);
            gluDeleteQuadric (pObj);
        glPopMatrix ();

        // Dial
        glPushMatrix ();
            glColor3f (1.0f, 1.0f, 1.0f);
            while (1)
            {
                if (fElect0 == -360)
                {
                    fElect0 = 0;
                    break;
                }
                glPushMatrix ();
                glRotatef ((GLfloat)fElect0, 0.0f, 0.0f, 1.0f);
                glTranslatef (12.0f, 0.0f, 0.0f);
                glBegin (GL_LINES);
                    if ((fElect0 % 30) == 0)
                        glVertex3f (-1.0f, 0.0f, 0.0f);
                    else
                        glVertex3f (0.0f, 0.0f, 0.0f);
                    glVertex3f (1.0f, 0.0f, 0.0f);
                glEnd ();
                glPopMatrix ();
                fElect0 -= 6;
            }
        glPopMatrix ();

        // Second
        glPushMatrix ();
        glColor3f (1.0f, 0.0f, 0.0f);
        glRotatef ((GLfloat)fElect1, 0.0f, 0.0f, 1.0f);

        glBegin (GL_LINES);
            glVertex3f (0.0f, 0.0f, 0.0f);
            glVertex3f (9.0f, 0.0f, 0.0f);
        glEnd ();
        glPopMatrix ();

        // Minute
        glPushMatrix ();
        glColor3f (0.0f, 1.0f, 0.0f);
        glRotatef ((GLfloat)fElect2, 0.0f, 0.0f, 1.0f);
        glBegin (GL_LINES);
            glVertex3f (0.0f, 0.0f, 0.0f);
            glVertex3f (7.0f, 0.0f, 0.0f);
        glEnd ();
        glPopMatrix ();

        // Hour
        glPushMatrix ();
        glColor3f (0.0f, 0.0f, 1.0f);
        glRotatef ((GLfloat)fElect3, 0.0f, 0.0f, 1.0f);
        glBegin (GL_LINES);
            glVertex3f (0.0f, 0.0f, 0.0f);
            glVertex3f (5.0f, 0.0f, 0.0f);
        glEnd ();
        glPopMatrix ();


    glPopMatrix ();

    glutSwapBuffers ();
}

/*
=====================
  SetupRC
=====================
*/
void SetupRC (void)
{
    glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
	
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable (GL_BLEND);

    glEnable (GL_POINT_SMOOTH);
    glHint (GL_POINT_SMOOTH_HINT, GL_NICEST);

    glEnable (GL_LINE_SMOOTH);
    glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);

    glEnable (GL_POLYGON_SMOOTH);
    glHint (GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	
}

/*
=====================
  TimerFunc
=====================
*/
void TimerFunc (int value)
{
    glutPostRedisplay ();

#if 0
	printf ("hour: %d, minute: %d, second: %d\n", \
		tmTime->tm_hour, \
		tmTime->tm_min, \
		tmTime->tm_sec);
	printf ("second: %f\n", fElect1);
	printf ("minute: %f\n", fElect2);
	printf ("hour: %f\n", fElect3);
#endif

    glutTimerFunc (1000, TimerFunc, 1);
}

/*
=====================
  main
=====================
*/
int main (int argc, char **argv)
{
    glutInit (&argc, argv);
    glutInitDisplayMode (GL_DOUBLE | GL_RGB | GL_DEPTH);
    glutInitWindowSize (320, 480);
    glutCreateWindow ("OpenGL Clock");

    glutReshapeFunc (ChangeSize);
    glutDisplayFunc (RenderScene);
    glutTimerFunc (1000, TimerFunc, 1);

    SetupRC ();

    glutMainLoop ();
}
