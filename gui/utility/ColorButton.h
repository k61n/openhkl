#ifndef NSX_GUI_UTILITY_COLORBUTTON_H
#define NSX_GUI_UTILITY_COLORBUTTON_H

#include <QPushButton>
#include <QtGui>

class ColorButton : public QPushButton {
    Q_OBJECT
 public:
    explicit ColorButton(const QColor& color = Qt::black, QWidget* parent = 0);
    QColor getColor();

 signals:
    void colorChanged(QColor);

 public slots:
    void changeColor(const QColor&);
    void chooseColor();
    void paintEvent(QPaintEvent* event);

 private:
    QColor currentColor;
};

#endif // NSX_GUI_UTILITY_COLORBUTTON_H