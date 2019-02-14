#include <string>

#include <QJsonArray>

#include <core/AABB.h>
#include <core/BoxMask.h>
#include <core/DataSet.h>
#include <core/Experiment.h>
#include <core/Logger.h>

#include "ExperimentItem.h"
#include "MetaTypes.h"
#include "NumorItem.h"
#include "NumorPropertyWidget.h"

NumorItem::NumorItem(nsx::sptrDataSet data)
    : InspectableTreeItem(), _data(data) {
  setText("Numor");

  QIcon icon(":/resources/numorIcon.png");
  setIcon(icon);

  setEditable(false);

  setDragEnabled(false);
  setDropEnabled(false);

  setSelectable(false);

  setCheckable(true);
}

NumorItem::~NumorItem() {}

QVariant NumorItem::data(int role) const {
  switch (role) {
  case (Qt::UserRole): {
    return QVariant::fromValue(_data);
  }
  }
  return InspectableTreeItem::data(role);
}

void NumorItem::exportHDF5(const std::string &filename) const {
  if (filename.empty()) {
    return;
  }

  if (filename.compare(_data->filename()) == 0) {
    return;
  }

  try {
    _data->saveHDF5(filename);
  } catch (...) {
    nsx::error() << "The filename " << filename
                 << " could not be saved. Maybe a permission problem.";
  }
}

QWidget *NumorItem::inspectItem() { return new NumorPropertyWidget(this); }
