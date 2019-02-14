#pragma once

#include <QWidget>

namespace Ui {
class SamplePropertyWidget;
}

class SampleItem;

class SamplePropertyWidget : public QWidget {
  Q_OBJECT

public:
  explicit SamplePropertyWidget(SampleItem *caller, QWidget *parent = 0);

  ~SamplePropertyWidget();

private:
  Ui::SamplePropertyWidget *_ui;

  SampleItem *_sampleItem;
};
