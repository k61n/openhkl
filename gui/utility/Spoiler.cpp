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

    // don't waste space
    _mainLayout.setVerticalSpacing(0);
    _mainLayout.setContentsMargins(0, 0, 0, 0);

    _mainLayout.addWidget(&_toggleButton, 0, 0, 1, 1, Qt::AlignLeft);
    _mainLayout.addWidget(&_headerLine, 0, 2, 1, 1);
    _mainLayout.addWidget(&contentArea, 1, 0, 1, 3);

    setLayout(&_mainLayout);

    QObject::connect(&_toggleButton, &QAbstractButton::clicked, this, &Spoiler::toggler);
}

void Spoiler::setContentLayout(QLayout& contentLayout, bool expanded)
{
    delete contentArea.layout();
    contentArea.setLayout(&contentLayout);

    const auto collapsedHeight = sizeHint().height() - contentArea.maximumHeight() + 1;
    const auto contentHeight = contentLayout.sizeHint().height();
    const auto expandedHeight = collapsedHeight + contentHeight;

    if (expanded) {
        QSignalBlocker b(_toggleButton);
        _toggleButton.setChecked(true);
        _toggleButton.setArrowType(Qt::ArrowType::DownArrow);
        contentArea.setMaximumHeight(contentLayout.sizeHint().height());
        setMinimumHeight(expandedHeight);
    } else {
        QSignalBlocker b(_toggleButton);
        _toggleButton.setChecked(false);
        _toggleButton.setArrowType(Qt::ArrowType::RightArrow);
        setMinimumHeight(collapsedHeight);
        contentArea.setMaximumHeight(0);
    }

    // Add animations, to let the entire widget grow and shrink with its content
    const auto addAnimation = [=](QObject* target, const QByteArray propertyName, int start,
                                  int end) {
        auto a = new QPropertyAnimation(target, propertyName);
        a->setDuration(_animationDuration);
        a->setStartValue(start);
        a->setEndValue(end);
        _toggleAnimation.addAnimation(a);
    };

    _toggleAnimation.clear();
    addAnimation(this, "minimumHeight", collapsedHeight, expandedHeight);
    addAnimation(this, "maximumHeight", collapsedHeight, expandedHeight);
    addAnimation(&contentArea, "minimumHeight", 0, contentHeight);
    addAnimation(&contentArea, "maximumHeight", 0, contentHeight);
}

void Spoiler::setExpanded(bool expand)
{
    if (_toggleButton.isChecked() != expand) {
        QSignalBlocker b(_toggleButton);
        _toggleButton.setChecked(expand);
    }
    toggler(true);
}

void Spoiler::toggler(const bool checked)
{
    _toggleButton.setArrowType(checked ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);
    _toggleAnimation.setDirection(
        checked ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
    _toggleAnimation.start();
}