#include <QMessageBox>

#include <core/DataSet.h>
#include <core/Diffractometer.h>
#include <core/Gonio.h>
#include <core/Logger.h>
#include <core/MCAbsorption.h>
#include <core/Material.h>
#include <core/Peak3D.h>
#include <core/Sample.h>
#include <core/Source.h>
#include <core/Units.h>

#include "DialogAbsorption.h"
#include "SampleShapeItem.h"
#include "SampleShapePropertyWidget.h"

#include "ui_SampleShapePropertyWidget.h"

SampleShapePropertyWidget::SampleShapePropertyWidget(SampleShapeItem *caller,
                                                     QWidget *parent)
    : QWidget(parent), _caller(caller), ui(new Ui::SampleShapePropertyWidget) {
  ui->setupUi(this);
  setHullProperties();
}

SampleShapePropertyWidget::~SampleShapePropertyWidget() { delete ui; }

void SampleShapePropertyWidget::on_pushButton_LoadMovie_clicked() {
  DialogAbsorption *dialog =
      new DialogAbsorption(_caller->experiment(), nullptr);
  if (!dialog->exec()) {
    auto &sample = _caller->experiment()->diffractometer()->sample();
    auto &hull = sample.shape();
    if (hull.checkEulerConditions()) {
      ui->lineEdit_MovieFilename->setText(
          QString::fromStdString(dialog->getMovieFilename()));
      // The hull is translated to its center
      hull.translateToCenter();
      // The hull is rotated of -90 along chi axis (Y axis)
      Eigen::Matrix3d mat;
      mat << 0, 0, 1, 0, 1, 0, -1, 0, 0;
      hull.rotate(mat);
      // Convert to m
      hull.scale(nsx::mm);
      nsx::debug() << "Coordinates of the Hull at rest:" << hull;
      setHullProperties();
    }
  }
}

void SampleShapePropertyWidget::setHullProperties() {
  const auto &sample = _caller->experiment()->diffractometer()->sample();
  const auto &hull = sample.shape();

  ui->lineEdit_Volume->setText(QString::number(hull.volume() / nsx::mm3) +
                               " mm^3");
  ui->lineEdit_Faces->setText(QString::number(hull.nFaces()));
  ui->lineEdit_Edges->setText(QString::number(hull.nEdges()));
  ui->lineEdit_Vertices->setText(QString::number(hull.nVertices()));
}
