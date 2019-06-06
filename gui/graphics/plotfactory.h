#pragma once

#include <string>

#include "core/utils/Factory.h"
#include "core/utils/Singleton.h"

#include "gui/graphics/nsxplot.h"

class QWidget;

class PlotFactory : public nsx::Factory<NSXPlot, std::string, QWidget*>,
                    public nsx::Singleton<PlotFactory, nsx::Constructor, nsx::Destructor> {

private:
    friend class nsx::Constructor<PlotFactory>;
    friend class nsx::Destructor<PlotFactory>;
    PlotFactory();
    ~PlotFactory();
};
