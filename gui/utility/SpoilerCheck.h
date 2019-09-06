#include <QFrame>
#include <QGridLayout>
#include <QParallelAnimationGroup>
#include <QScrollArea>
#include <QToolButton>
#include <QGroupBox>
#include <QCheckBox>

class SpoilerCheck : public QGroupBox {
    Q_OBJECT

public:
    QScrollArea contentArea;
    explicit SpoilerCheck(const QString & title = "", const int animationDuration = 100, QWidget *parent = 0);
    void setContentLayout(QLayout & contentLayout, bool toggled = false);
    bool checked() const;

public slots:
    void toggler(const bool check);
    void checker(const int state);
    
private:
    QGridLayout mainLayout;
    QToolButton toggleButton;
    QFrame headerLine;
    QParallelAnimationGroup toggleAnimation;
    int animationDuration{300};
    QCheckBox select;

};