#ifndef GLSphere_H
#define GLSphere_H
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
