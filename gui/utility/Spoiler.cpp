//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      gui/utility/Spoiler.h
//! @brief     Defines class Spoiler
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "Spoiler.h"

#include <QCheckBox>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QToolButton>

Spoiler::Spoiler(const QString& title, bool isCheckable) : _animationDuration(100)
{
    _toggleButton = new QToolButton;
    _toggleButton->setStyleSheet("QToolButton { border: none; }");
    _toggleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    _toggleButton->setCheckable(true);

    // don't waste space
    _mainLayout = new QGridLayout;
    _mainLayout->setVerticalSpacing(0);
    _mainLayout->setContentsMargins(0, 0, 0, 0);

    if (isCheckable) {
        _select = new QCheckBox;
        _select->setText(title);
        QSizePolicy select_policy;
        select_policy.setHorizontalPolicy(QSizePolicy::Expanding);
        select_policy.setVerticalPolicy(QSizePolicy::Preferred);
        _select->setSizePolicy(select_policy);
        _select->setChecked(false);

        _toggleButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

        connect(_select, &QCheckBox::stateChanged, this, &Spoiler::checker);

        QHBoxLayout* l = new QHBoxLayout;
        l->addWidget(_toggleButton);
        l->addWidget(_select);

        _mainLayout->addLayout(l, 0, 0, 1, -1);
    } else {
        _toggleButton->setText(title);
        _mainLayout->addWidget(_toggleButton, 0, 0, 1, -1, Qt::AlignLeft);
    }

    _contentArea = new QWidget(this);
    _mainLayout->addWidget(_contentArea, 1, 0, 1, -1);

    // start out collapsed
    _toggleButton->setChecked(false);
    _toggleButton->setArrowType(Qt::ArrowType::RightArrow);
    _contentArea->setFixedHeight(0);

    setLayout(_mainLayout);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    connect(_toggleButton, &QAbstractButton::clicked, this, &Spoiler::toggler);
    connect(&_toggleAnimation, &QAbstractAnimation::finished, this, &Spoiler::onAnimationFinished);
}

void Spoiler::setContentLayout(QLayout& contentLayout, bool expanded)
{
    contentLayout.setSizeConstraint(QLayout::SetMinimumSize);

    if (_contentArea->layout() != &contentLayout) {
        delete _contentArea->layout();
        _contentArea->setLayout(&contentLayout);
    }

    const auto collapsedHeight = sizeHint().height() - _contentArea->maximumHeight()
        + 1; // #nsxUI this is very likely necessary because of pressed state of toggle button?!
    const auto contentHeight = contentLayout.sizeHint().height();
    const auto expandedHeight = collapsedHeight + contentHeight;

    QSignalBlocker b(_toggleButton);
    _toggleButton->setChecked(expanded);
    _toggleButton->setArrowType(expanded ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);

    _contentArea->setFixedHeight(expanded ? contentHeight : 0);
    setFixedHeight(expanded ? expandedHeight : collapsedHeight);

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
    addAnimation(_contentArea, "minimumHeight", 0, contentHeight);
    addAnimation(_contentArea, "maximumHeight", 0, contentHeight);

    // this is necessary if widgets on the spoiler are resized while collapsed (tableView). It
    // increases the layout, and therefore the spoiler title gets "compressed"
    if (!expanded)
        _contentArea->hide();

    if (_select != nullptr)
        setChecked(expanded);
}

QLayout* Spoiler::contentLayout()
{
    return _contentArea->layout();
}

void Spoiler::setExpanded(bool expand)
{
    if (_toggleButton->isChecked() != expand) {
        QSignalBlocker b(_toggleButton);
        _toggleButton->setChecked(expand);
    }
    toggler(expand);
}

bool Spoiler::isExpanded() const
{
    return _toggleButton->isChecked();
}

bool Spoiler::isChecked() const
{
    return _select != nullptr ? _select->isChecked() : false;
}

void Spoiler::setChecked(bool checked)
{
    if (_select == nullptr)
        return;

    if (_select->isChecked() != checked) {
        QSignalBlocker b(_select);
        _select->setChecked(checked);
    }

    _contentArea->setEnabled(checked);
}

void Spoiler::toggler(const bool checked)
{
    _toggleButton->setArrowType(checked ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);
    _toggleAnimation.setDirection(
        checked ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);

    if (isExpanded())
        _contentArea->show();

    _toggleAnimation.start();
}

void Spoiler::checker(const int state)
{
    _contentArea->setEnabled(state == Qt::Checked);
}

void Spoiler::showEvent(QShowEvent* event)
{
    // minimum height of content area is recalculated in some initialization methods (could not find
    // which one). Therefore it has to be re-initialized in here, in case we start collapsed
    if (!_toggleButton->isChecked())
        _contentArea->setFixedHeight(0);
}

void Spoiler::onAnimationFinished()
{
    if (!isExpanded())
        _contentArea->hide();
}
