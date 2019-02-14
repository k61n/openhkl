#include "GLActor.h"
#include "GLScene.h"

GLScene::GLScene():_current(0)
{
}
GLScene::~GLScene()
{

}

GLActor* GLScene::addActor(GLActor * object)
{
    if (!_numbers.isEmpty())
        _actors[_numbers.dequeue()]=object;
    else
        _actors[++_current]=object;

    return object;
}

void GLScene::deleteActor(int n)
{
    if (_actors.remove(n) && n<_current)
        _numbers.enqueue(n);
}

void GLScene::render(QGLWidget *w)
{
    for (QHash<int,GLActor*>::const_iterator it=_actors.begin();it!=_actors.end();++it)
    {
        if (it.key()==_selected)
            it.value()->drawforSelected();
        else
             it.value()->draw(w);
    }
}

void GLScene::renderForPicking()
{
    for (QHash<int,GLActor*>::const_iterator it=_actors.begin();it!=_actors.end();++it)
    {
       it.value()->drawforPicking(it.key());
    }
}

GLActor* GLScene::picking(int r, int g, int b)
{
    int index=0;
    index |= r ;
    index |= g  << 8;
    index |= b << 16;
    if (index!=0)
    {
     QHash<int,GLActor*>::const_iterator it=_actors.find(index);
      if (it!=_actors.end())
      {
        _selected=it.key();
        return it.value();
      }
    }
    return nullptr;
}

