#pragma once

#include <QWidget>

namespace Ui {
class SourcePropertyWidget;
}

class SourceItem;

class SourcePropertyWidget : public QWidget {
  Q_OBJECT

public:
  explicit SourcePropertyWidget(SourceItem *caller, QWidget *parent = 0);

  ~SourcePropertyWidget();

private:
  void onHeightChanged(double height);

  void onSelectedMonochromatorChanged(int index);

  void onWavelengthChanged(double wavelength);

  void onWidthChanged(double width);

private:
  Ui::SourcePropertyWidget *_ui;

  SourceItem *_caller;
};
