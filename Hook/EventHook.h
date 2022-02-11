#ifndef __EVENTHOOK_H__
#define __EVENTHOOK_H__

#include <QObject>
#include <QVariant>
#include <QMetaObject>
#include <QKeyEvent>

class EventHook : public QObject {
    Q_OBJECT
private:
    QObject* hookItem = nullptr;
protected:
    bool eventFilter(QObject*, QEvent *ev) override {
        if (ev->type() == QEvent::KeyPress || ev->type() == QEvent::KeyRelease) {
            QVariant retVal;
            QMetaObject::invokeMethod(hookItem, "root_keyHandler", Q_RETURN_ARG(QVariant, retVal),
                                      Q_ARG(QVariant, ev->type()==QEvent::KeyPress), Q_ARG(QVariant, ((QKeyEvent*)ev)->key()));
            return retVal.toBool();
        }
        return false;
    }
public:
    EventHook(QObject* obj) : QObject(nullptr) {
        hookItem = obj;
    }
};

#endif //__EVENTHOOK_H__