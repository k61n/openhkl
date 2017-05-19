#include "opengl/GLActor.h"

GLActor::GLActor(const char* name):_pos(0,0,0),_name(name)
{
    _color[0]=0.4;
    _color[1]=0.0;
    _color[2]=0.2;
    _scale[0]=_scale[1]=_scale[2]=1.0;
}

void GLActor::draw(QGLWidget *w)
{
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,_color);
    glPushMatrix();
    glTranslatef(_pos[0],_pos[1],_pos[2]);
    glPushMatrix();
    glScaled(_scale[0],_scale[1],_scale[2]);
    glPushMatrix();
    GLCode();
    glPopMatrix();
    glDisable(GL_DEPTH_TEST);
    w->renderText(1,1,1,_name);
    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
}
void GLActor::drawforSelected()
{
    GLfloat c[4];
    c[0]=1;
    c[1]=1;
    c[2]=1;
    c[3]=0.2;
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,c);
    glPushMatrix();
    glTranslatef(_pos[0],_pos[1],_pos[2]);
    glPushMatrix();
    glScaled(_scale[0],_scale[1],_scale[2]);
    GLCode();
    glPopMatrix();
    glPopMatrix();
}
void GLActor::drawforPicking(int color)
{
    unsigned char rgb[3];
    rgb[0] =(color & 0xFF);
    rgb[1] =(color & 0x0000FF00) >> 8;
    rgb[2] =(color & 0x00FF0000) >> 16;
    glColor3ub(rgb[0],rgb[1],rgb[2]);
    glPushMatrix();
    glTranslatef(_pos[0],_pos[1],_pos[2]);
    glScaled(_scale[0],_scale[1],_scale[2]);
        glPushMatrix();
            GLCode();
        glPopMatrix();
    glPopMatrix();
}
void GLActor::setColor(double r, double g, double b)
{
    _color[0]=r;
    _color[1]=g;
    _color[2]=b;
}

void GLActor::setPos(double x, double y, double z)
{
    _pos<< x,y,z;
}

void GLActor::setScale(double scale)
{
    _scale[0]=_scale[1]=_scale[2]=scale;
}


void GLActor::setScale(double x, double y, double z)
{
    _scale[0]=x;
    _scale[1]=y;
    _scale[2]=z;
}
