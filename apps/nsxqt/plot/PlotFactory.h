#ifndef NSXQT_PLOTFACTORY_H
#define NSXQT_PLOTFACTORY_H

#include <string>

#include <nsxlib/Factory.h>
#include <nsxlib/Singleton.h>

#include "SXPlot.h"

class QWidget;

class PlotFactory : public nsx::Factory<SXPlot,std::string,QWidget*>, public nsx::Singleton<PlotFactory,nsx::Constructor,nsx::Destructor>
{

private:

    friend class nsx::Constructor<PlotFactory>;
    friend class nsx::Destructor<PlotFactory>;
    PlotFactory();
    ~PlotFactory();
};

#endif // NSXQT_PLOTFACTORY_H
