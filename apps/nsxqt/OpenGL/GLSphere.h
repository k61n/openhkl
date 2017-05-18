#ifndef NSXQT_GLSphere_H
#define NSXQT_GLSphere_H

#include "GLActor.h"

class GLSphere : public GLActor
{
public:
    GLSphere(const char* name);
    int pickableElements() {return 1;}
private:
    virtual void GLCode();
};

#endif // GLSphere_H
