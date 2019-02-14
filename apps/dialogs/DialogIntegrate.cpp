#include "DialogIntegrate.h"
#include "PeaksUtils.h"
#include "ui_DialogIntegrate.h"

DialogIntegrate::DialogIntegrate(const nsx::PeakList &peaks, QWidget *parent)
    : QDialog(parent), _ui(new Ui::DialogIntegrate) {
  _ui->setupUi(this);

  auto &&drange = dRange(peaks);
  _ui->dMin->setValue(drange.first);
  _ui->dMax->setValue(drange.second);

  connect(_ui->okButton, &QPushButton::clicked,
          [=] { this->done(QDialog::Accepted); });
  connect(_ui->cancelButton, &QPushButton::clicked,
          [=] { this->done(QDialog::Rejected); });

  _ui->method->addItem("background subtraction");
  _ui->method->addItem("3d gaussian fit");
  _ui->method->addItem("3d profilt fit (Kabsch)");
  _ui->method->addItem("3d profilt fit (detector)");
  _ui->method->addItem("I/sigma");
  _ui->method->addItem("1d profile fit");
}

std::string DialogIntegrate::integrator() const {
  const auto id = _ui->method->currentIndex();
  return _ui->method->itemText(id).toStdString();
}

void DialogIntegrate::setIntegrators(const std::vector<std::string> &names) {
  while (_ui->method->count()) {
    _ui->method->removeItem(0);
  }

  for (const auto &name : names) {
    _ui->method->addItem(name.c_str());
  }
}

DialogIntegrate::~DialogIntegrate() { delete _ui; }

double DialogIntegrate::peakScale() const { return _ui->peakScale->value(); }

double DialogIntegrate::bkgBegin() const { return _ui->bkgBegin->value(); }

double DialogIntegrate::bkgEnd() const { return _ui->bkgEnd->value(); }

double DialogIntegrate::dMin() const { return _ui->dMin->value(); }

double DialogIntegrate::dMax() const { return _ui->dMax->value(); }

double DialogIntegrate::radius() const { return _ui->radius->value(); }

double DialogIntegrate::nframes() const { return _ui->nframes->value(); }

bool DialogIntegrate::fitCenter() const { return _ui->fitCenter->isChecked(); }

bool DialogIntegrate::fitCov() const { return _ui->fitCov->isChecked(); }
