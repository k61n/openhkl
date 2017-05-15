#ifndef PLOTFACTORY_H_
#define PLOTFACTORY_H_

#include <string>

#include <nsxlib/kernel/Factory.h>
#include <nsxlib/kernel/Singleton.h>
#include "SXPlot.h"

class QWidget;

using namespace nsx::Kernel;

class PlotFactory : public Factory<SXPlot,std::string,QWidget*>, public Singleton<PlotFactory,Constructor,Destructor>
{
private:
    friend class Constructor<PlotFactory>;
    friend class Destructor<PlotFactory>;
    PlotFactory();
    ~PlotFactory();
};

#endif /* PLOTFACTORY_H_ */
