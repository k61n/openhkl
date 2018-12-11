#include <QComboBox>
#include <QKeyEvent>
#include <QString>

#include "CustomEventFilters.h"

ShortcodeFilter::ShortcodeFilter(const QString& shortcode, QObject* parent)
    : QObject(parent), _shortcode(shortcode), _index(0)
{
}

bool ShortcodeFilter::eventFilter(QObject* obj, QEvent* event)
{
    Q_UNUSED(obj);
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (_shortcode.at(_index) == keyEvent->text()) {
            _index++;
            if (_index == _shortcode.length()) {
                emit found();
                _index = 0;
            }
        } else {
            int right = _index;
            while (_index > 0) {
                if (_shortcode.at(_index - 1) == keyEvent->text()
                    && _shortcode.left(_index - 1)
                        == _shortcode.mid(right - _index + 1, _index - 1))
                    break;
                _index--;
            }
        }
    }
    return false;
}
