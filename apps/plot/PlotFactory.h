#pragma once

#include <string>

#include <core/Factory.h>
#include <core/Singleton.h>

#include "SXPlot.h"

class QWidget;

class PlotFactory
    : public nsx::Factory<SXPlot, std::string, QWidget *>,
      public nsx::Singleton<PlotFactory, nsx::Constructor, nsx::Destructor> {

private:
  friend class nsx::Constructor<PlotFactory>;
  friend class nsx::Destructor<PlotFactory>;
  PlotFactory();
  ~PlotFactory();
};
