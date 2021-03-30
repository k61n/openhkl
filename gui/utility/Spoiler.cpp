#include <QPropertyAnimation>

#include "Spoiler.h"

Spoiler::Spoiler(const QString& title, const int animationDuration, QWidget* parent)
    : QGroupBox(parent), _animationDuration(animationDuration)
{
    _toggleButton.setStyleSheet("QToolButton { border: none; }");
    _toggleButton.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    _toggleButton.setArrowType(Qt::ArrowType::RightArrow);
    _toggleButton.setText(title);
    _toggleButton.setCheckable(true);
    _toggleButton.setChecked(false);

    // start out collapsed
    contentArea.setStyleSheet("QScrollArea { border: none; }");
    contentArea.setMaximumHeight(0);
    contentArea.setMinimumHeight(0);

    // let the entire widget grow and shrink with its content
    _toggleAnimation.addAnimation(new QPropertyAnimation(this, "minimumHeight"));
    _toggleAnimation.addAnimation(new QPropertyAnimation(this, "maximumHeight"));
    _toggleAnimation.addAnimation(new QPropertyAnimation(&contentArea, "maximumHeight"));

    // don't waste space
    _mainLayout.setVerticalSpacing(0);
    _mainLayout.setContentsMargins(0, 0, 0, 0);
    int row = 0;
    _mainLayout.addWidget(&_toggleButton, row, 0, 1, 1, Qt::AlignLeft);
    _mainLayout.addWidget(&_headerLine, row++, 2, 1, 1);
    _mainLayout.addWidget(&contentArea, row, 0, 1, 3);

    setLayout(&_mainLayout);

    QObject::connect(&_toggleButton, SIGNAL(clicked(bool)), this, SLOT(toggler(bool)));
}

void Spoiler::setContentLayout(QLayout& contentLayout, bool toggled)
{
    delete contentArea.layout();
    contentArea.setLayout(&contentLayout);

    const auto collapsedHeight = sizeHint().height() - contentArea.maximumHeight();
    auto contentHeight = contentLayout.sizeHint().height();

    if (toggled) {
        _toggleButton.blockSignals(true);
        _toggleButton.setChecked(true);
        _toggleButton.setArrowType(Qt::ArrowType::DownArrow);
        this->setMinimumHeight(collapsedHeight + contentHeight);
        this->setMinimumHeight(collapsedHeight + contentHeight);
        contentArea.setMaximumHeight(contentHeight);
        _toggleButton.blockSignals(false);
    } else {
        _toggleButton.blockSignals(true);
        _toggleButton.setChecked(false);
        _toggleButton.setArrowType(Qt::ArrowType::RightArrow);
        this->setMinimumHeight(collapsedHeight);
        this->setMinimumHeight(collapsedHeight);
        contentArea.setMaximumHeight(0);
        _toggleButton.blockSignals(false);
    }

    for (int i = 0; i < _toggleAnimation.animationCount() - 1; ++i) {
        QPropertyAnimation* spoilerAnimation =
            static_cast<QPropertyAnimation*>(_toggleAnimation.animationAt(i));
        spoilerAnimation->setDuration(_animationDuration);
        spoilerAnimation->setStartValue(collapsedHeight);
        spoilerAnimation->setEndValue(collapsedHeight + contentHeight);
    }
    QPropertyAnimation* contentAnimation = static_cast<QPropertyAnimation*>(
        _toggleAnimation.animationAt(_toggleAnimation.animationCount() - 1));
    contentAnimation->setDuration(_animationDuration);
    contentAnimation->setStartValue(0);
    contentAnimation->setEndValue(contentHeight);
}

void Spoiler::toggler(const bool checked)
{
    _toggleButton.setArrowType(checked ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);
    _toggleAnimation.setDirection(
        checked ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
    _toggleAnimation.start();
}