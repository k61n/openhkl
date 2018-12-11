#pragma once

#include <QObject>
#include <QString>

//! Event filter for global tracking of shortcodes.

class ShortcodeFilter : public QObject {
    Q_OBJECT

public:
    ShortcodeFilter(const QString& shortcode, QObject* parent = 0);

signals:
    void found();

protected:
    bool eventFilter(QObject* obj, QEvent* event);
    QString _shortcode;
    int _index;
};
