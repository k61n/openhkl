#include <QPropertyAnimation>
#include <iostream>

#include "SpoilerCheck.h"

SpoilerCheck::SpoilerCheck(const QString& title, const int animationDuration, QWidget* parent)
    : QGroupBox(parent), animationDuration(animationDuration)
{
    toggleButton.setStyleSheet("QToolButton { border: none; }");
    toggleButton.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toggleButton.setArrowType(Qt::ArrowType::RightArrow);
    // toggleButton.setText(title);
    toggleButton.setCheckable(true);
    toggleButton.setChecked(false);

    QSizePolicy toggle_policy;
    toggle_policy.setHorizontalPolicy(QSizePolicy::Minimum);
    toggle_policy.setVerticalPolicy(QSizePolicy::Preferred);
    toggleButton.setSizePolicy(toggle_policy);

    // set the text
    select.setText(title);
    QSizePolicy select_policy;
    select_policy.setHorizontalPolicy(QSizePolicy::Expanding);
    select_policy.setVerticalPolicy(QSizePolicy::Preferred);
    select.setSizePolicy(select_policy);
    select.setChecked(false);

    // start out collapsed
    contentArea.setStyleSheet("QScrollArea { border: none; }");
    contentArea.setMaximumHeight(0);
    contentArea.setMinimumHeight(0);

    // let the entire widget grow and shrink with its content
    toggleAnimation.addAnimation(new QPropertyAnimation(this, "minimumHeight"));
    toggleAnimation.addAnimation(new QPropertyAnimation(this, "maximumHeight"));
    toggleAnimation.addAnimation(new QPropertyAnimation(&contentArea, "maximumHeight"));

    // don't waste space
    mainLayout.setVerticalSpacing(0);
    mainLayout.setContentsMargins(0, 0, 0, 0);
    int row = 0;
    mainLayout.addWidget(&toggleButton, row, 0, 1, 1, Qt::AlignLeft);
    mainLayout.addWidget(&select, row, 1, 1, 1);
    mainLayout.addWidget(&contentArea, ++row, 0, 1, 2);

    setLayout(&mainLayout);

    connect(&toggleButton, &QToolButton::clicked, this, &SpoilerCheck::toggler);
    connect(&select, &QCheckBox::stateChanged, this, &SpoilerCheck::checker);
}

void SpoilerCheck::setContentLayout(QLayout& contentLayout, bool toggled)
{
    delete contentArea.layout();
    contentArea.setLayout(&contentLayout);

    const auto collapsedHeight = sizeHint().height() - contentArea.maximumHeight();
    auto contentHeight = contentLayout.sizeHint().height();

    if (toggled) {
        toggleButton.blockSignals(true);
        toggleButton.setChecked(true);
        toggleButton.setArrowType(Qt::ArrowType::DownArrow);
        this->setMinimumHeight(collapsedHeight + contentHeight);
        this->setMinimumHeight(collapsedHeight + contentHeight);
        contentArea.setMaximumHeight(contentHeight);
        toggleButton.blockSignals(false);
    } else {
        toggleButton.blockSignals(true);
        toggleButton.setChecked(false);
        toggleButton.setArrowType(Qt::ArrowType::RightArrow);
        this->setMinimumHeight(collapsedHeight);
        this->setMinimumHeight(collapsedHeight);
        contentArea.setMaximumHeight(0);
        toggleButton.blockSignals(false);
    }

    for (int i = 0; i < toggleAnimation.animationCount() - 1; ++i) {
        QPropertyAnimation* spoilerAnimation =
            static_cast<QPropertyAnimation*>(toggleAnimation.animationAt(i));
        spoilerAnimation->setDuration(animationDuration);
        spoilerAnimation->setStartValue(collapsedHeight);
        spoilerAnimation->setEndValue(collapsedHeight + contentHeight);
    }
    QPropertyAnimation* contentAnimation = static_cast<QPropertyAnimation*>(
        toggleAnimation.animationAt(toggleAnimation.animationCount() - 1));
    contentAnimation->setDuration(animationDuration);
    contentAnimation->setStartValue(0);
    contentAnimation->setEndValue(contentHeight);

    checker(int(toggled) + 1);
}

void SpoilerCheck::toggler(const bool checked)
{
    toggleButton.setArrowType(checked ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);
    toggleAnimation.setDirection(
        checked ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
    toggleAnimation.start();
}

void SpoilerCheck::checker(const int state)
{
    contentArea.setEnabled((state == Qt::Checked));
}

bool SpoilerCheck::checked() const
{
    return !select.checkState();
}
