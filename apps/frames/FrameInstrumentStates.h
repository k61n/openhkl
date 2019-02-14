#pragma once

#include <core/DataTypes.h>

#include "NSXQFrame.h"

namespace Ui {
class FrameInstrumentStates;
}

class QAbstractButton;

class FrameInstrumentStates : public NSXQFrame {
  Q_OBJECT

public:
  static FrameInstrumentStates *create(const nsx::DataList &data);

  static FrameInstrumentStates *Instance();

  ~FrameInstrumentStates();

private slots:

  void slotActionClicked(QAbstractButton *button);

  void slotSelectedDataChanged(int selected_data);

  void slotSelectedFrameChanged(int selected_frame);

private:
  explicit FrameInstrumentStates(const nsx::DataList &data);

private:
  static FrameInstrumentStates *_instance;

  Ui::FrameInstrumentStates *_ui;
};
