#ifndef PLOTFACTORY_H_
#define PLOTFACTORY_H_

#include <string>

#include "Factory.h"
#include "Singleton.h"
#include "SXPlot.h"

class QWidget;

using namespace SX::Kernel;

class PlotFactory : public Factory<SXPlot,std::string,QWidget*>, public Singleton<PlotFactory,Constructor,Destructor>
{
private:
    friend class Constructor<PlotFactory>;
    friend class Destructor<PlotFactory>;
    PlotFactory();
    ~PlotFactory();
};

#endif /* PLOTFACTORY_H_ */
