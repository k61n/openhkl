//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/widgets/ScientificNotationSpinBox.h
//! @brief     Defines class ScientificNotationSpinBox
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#pragma once

#include <QDoubleSpinBox>
#include <QLineEdit>


class ScientificNotationSpinBox : public QDoubleSpinBox {
    Q_OBJECT
 public:
    ScientificNotationSpinBox(QWidget* parent = 0);

    int decimals() const;
    void setDecimals(int value);

    QString textFromValue(double value) const;
    double valueFromText(const QString& text) const;

 private:
    int dispDecimals;
    QChar delimiter, thousand;
    QDoubleValidator* v;

 private:
    void initLocalValues(QWidget* parent);
    bool isIntermediateValue(const QString& str) const;
    QVariant validateAndInterpret(QString& input, int& pos, QValidator::State& state) const;
    QValidator::State validate(QString& text, int& pos) const;
    void fixup(QString& input) const;
    QString stripped(const QString& t, int* pos) const;
    double round(double value) const;
    void stepBy(int steps);
    bool isIntermediateValueHelper(qint64 num, qint64 minimum, qint64 maximum, qint64* match = 0) const;

 public slots:
    void stepDown();
    void stepUp();
};
