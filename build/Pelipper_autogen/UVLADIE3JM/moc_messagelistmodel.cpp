/****************************************************************************
** Meta object code from reading C++ file 'messagelistmodel.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/messagelistmodel.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'messagelistmodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MessageListModel_t {
    QByteArrayData data[16];
    char stringdata0[199];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MessageListModel_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MessageListModel_t qt_meta_stringdata_MessageListModel = {
    {
QT_MOC_LITERAL(0, 0, 16), // "MessageListModel"
QT_MOC_LITERAL(1, 17, 15), // "onMessagesReady"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 16), // "QList<Message*>*"
QT_MOC_LITERAL(4, 51, 7), // "msgList"
QT_MOC_LITERAL(5, 59, 14), // "onMessageReady"
QT_MOC_LITERAL(6, 74, 8), // "Message*"
QT_MOC_LITERAL(7, 83, 3), // "msg"
QT_MOC_LITERAL(8, 87, 14), // "messageLoading"
QT_MOC_LITERAL(9, 102, 22), // "messageLoadingFinished"
QT_MOC_LITERAL(10, 125, 14), // "loadingChanged"
QT_MOC_LITERAL(11, 140, 16), // "onFolderSelected"
QT_MOC_LITERAL(12, 157, 10), // "foldername"
QT_MOC_LITERAL(13, 168, 11), // "addMessages"
QT_MOC_LITERAL(14, 180, 10), // "addMessage"
QT_MOC_LITERAL(15, 191, 7) // "loading"

    },
    "MessageListModel\0onMessagesReady\0\0"
    "QList<Message*>*\0msgList\0onMessageReady\0"
    "Message*\0msg\0messageLoading\0"
    "messageLoadingFinished\0loadingChanged\0"
    "onFolderSelected\0foldername\0addMessages\0"
    "addMessage\0loading"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MessageListModel[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       2,   72, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       5,    1,   57,    2, 0x06 /* Public */,
       8,    0,   60,    2, 0x06 /* Public */,
       9,    0,   61,    2, 0x06 /* Public */,
      10,    0,   62,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    1,   63,    2, 0x08 /* Private */,
      13,    1,   66,    2, 0x08 /* Private */,
      14,    1,   69,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,   12,
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 6,    7,

 // properties: name, type, flags
      15, QMetaType::Bool, 0x00495103,
      15, QMetaType::Bool, 0x00095401,

 // properties: notify_signal_id
       4,
       0,

       0        // eod
};

void MessageListModel::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MessageListModel *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onMessagesReady((*reinterpret_cast< QList<Message*>*(*)>(_a[1]))); break;
        case 1: _t->onMessageReady((*reinterpret_cast< Message*(*)>(_a[1]))); break;
        case 2: _t->messageLoading(); break;
        case 3: _t->messageLoadingFinished(); break;
        case 4: _t->loadingChanged(); break;
        case 5: _t->onFolderSelected((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->addMessages((*reinterpret_cast< QList<Message*>*(*)>(_a[1]))); break;
        case 7: _t->addMessage((*reinterpret_cast< Message*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Message* >(); break;
            }
            break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< Message* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MessageListModel::*)(QList<Message*> * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MessageListModel::onMessagesReady)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MessageListModel::*)(Message * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MessageListModel::onMessageReady)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (MessageListModel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MessageListModel::messageLoading)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (MessageListModel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MessageListModel::messageLoadingFinished)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (MessageListModel::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MessageListModel::loadingChanged)) {
                *result = 4;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<MessageListModel *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->loading(); break;
        case 1: *reinterpret_cast< bool*>(_v) = _t->loading(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<MessageListModel *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setLoading(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject MessageListModel::staticMetaObject = { {
    QMetaObject::SuperData::link<QAbstractListModel::staticMetaObject>(),
    qt_meta_stringdata_MessageListModel.data,
    qt_meta_data_MessageListModel,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MessageListModel::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MessageListModel::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MessageListModel.stringdata0))
        return static_cast<void*>(this);
    return QAbstractListModel::qt_metacast(_clname);
}

int MessageListModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractListModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void MessageListModel::onMessagesReady(QList<Message*> * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MessageListModel::onMessageReady(Message * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MessageListModel::messageLoading()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void MessageListModel::messageLoadingFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void MessageListModel::loadingChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
