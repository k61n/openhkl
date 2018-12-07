#pragma once

#include <QDialog>

class QHBoxLayout;
class QPushButton;

class DialogAbout : public QDialog
{
    Q_OBJECT

public:

    explicit DialogAbout(QWidget *parent = 0);

private:

    QPushButton* createLogo();

    QHBoxLayout* createPartnersLayout();
};
