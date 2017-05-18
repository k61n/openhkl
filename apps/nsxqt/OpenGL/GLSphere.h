#ifndef NSXQT_GLSPHERE_H
#define NSXQT_GLSPHERE_H

#include "GLActor.h"

class GLSphere : public GLActor
{
public:
    GLSphere(const char* name);
    int pickableElements() {return 1;}
private:
    virtual void GLCode();
};

#endif // NSXQT_GLSPHERE_H
