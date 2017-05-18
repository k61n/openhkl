#ifndef NSXQT_PLOTFACTORY_H
#define NSXQT_PLOTFACTORY_H

#include <string>

#include <nsxlib/kernel/Factory.h>
#include <nsxlib/kernel/Singleton.h>

#include "SXPlot.h"

class QWidget;

using namespace nsx;

class PlotFactory : public Factory<SXPlot,std::string,QWidget*>, public Singleton<PlotFactory,Constructor,Destructor>
{

private:

    friend class Constructor<PlotFactory>;
    friend class Destructor<PlotFactory>;
    PlotFactory();
    ~PlotFactory();
};

#endif // NSXQT_PLOTFACTORY_H
