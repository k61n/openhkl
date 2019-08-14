#include <QFrame>
#include <QGridLayout>
#include <QParallelAnimationGroup>
#include <QScrollArea>
#include <QToolButton>
#include <QGroupBox>

class Spoiler : public QGroupBox {
    Q_OBJECT

public:
    QScrollArea contentArea;
    explicit Spoiler(const QString & title = "", const int animationDuration = 100, QWidget *parent = 0);
    void setContentLayout(QLayout & contentLayout, bool toggled = false);

public slots:
    void toggler(const bool check);
    
private:
    QGridLayout mainLayout;
    QToolButton toggleButton;
    QFrame headerLine;
    QParallelAnimationGroup toggleAnimation;
    int animationDuration{300};

};