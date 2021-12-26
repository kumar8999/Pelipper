/****************************************************************************
** Meta object code from reading C++ file 'messageitem.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/messageitem.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'messageitem.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MessageItem_t {
    QByteArrayData data[14];
    char stringdata0[149];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MessageItem_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MessageItem_t qt_meta_stringdata_MessageItem = {
    {
QT_MOC_LITERAL(0, 0, 11), // "MessageItem"
QT_MOC_LITERAL(1, 12, 14), // "contentChanged"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 9), // "toChanged"
QT_MOC_LITERAL(4, 38, 11), // "fromChanged"
QT_MOC_LITERAL(5, 50, 14), // "subjectChanged"
QT_MOC_LITERAL(6, 65, 20), // "hasAttachmentChanged"
QT_MOC_LITERAL(7, 86, 15), // "datetimeChanged"
QT_MOC_LITERAL(8, 102, 7), // "content"
QT_MOC_LITERAL(9, 110, 2), // "to"
QT_MOC_LITERAL(10, 113, 4), // "from"
QT_MOC_LITERAL(11, 118, 7), // "subject"
QT_MOC_LITERAL(12, 126, 13), // "hasAttachment"
QT_MOC_LITERAL(13, 140, 8) // "datetime"

    },
    "MessageItem\0contentChanged\0\0toChanged\0"
    "fromChanged\0subjectChanged\0"
    "hasAttachmentChanged\0datetimeChanged\0"
    "content\0to\0from\0subject\0hasAttachment\0"
    "datetime"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MessageItem[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       6,   50, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x06 /* Public */,
       3,    0,   45,    2, 0x06 /* Public */,
       4,    0,   46,    2, 0x06 /* Public */,
       5,    0,   47,    2, 0x06 /* Public */,
       6,    0,   48,    2, 0x06 /* Public */,
       7,    0,   49,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
       8, QMetaType::QString, 0x00495001,
       9, QMetaType::QString, 0x00495001,
      10, QMetaType::QString, 0x00495001,
      11, QMetaType::QString, 0x00495001,
      12, QMetaType::Bool, 0x00495001,
      13, QMetaType::QDateTime, 0x00495001,

 // properties: notify_signal_id
       0,
       1,
       2,
       3,
       4,
       5,

       0        // eod
};

void MessageItem::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MessageItem *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->contentChanged(); break;
        case 1: _t->toChanged(); break;
        case 2: _t->fromChanged(); break;
        case 3: _t->subjectChanged(); break;
        case 4: _t->hasAttachmentChanged(); break;
        case 5: _t->datetimeChanged(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MessageItem::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MessageItem::contentChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MessageItem::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MessageItem::toChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (MessageItem::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MessageItem::fromChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (MessageItem::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MessageItem::subjectChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (MessageItem::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MessageItem::hasAttachmentChanged)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (MessageItem::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MessageItem::datetimeChanged)) {
                *result = 5;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<MessageItem *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->content(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->to(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->from(); break;
        case 3: *reinterpret_cast< QString*>(_v) = _t->subject(); break;
        case 4: *reinterpret_cast< bool*>(_v) = _t->hasAttachment(); break;
        case 5: *reinterpret_cast< QDateTime*>(_v) = _t->datetime(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject MessageItem::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_MessageItem.data,
    qt_meta_data_MessageItem,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MessageItem::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MessageItem::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MessageItem.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int MessageItem::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 6;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void MessageItem::contentChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void MessageItem::toChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void MessageItem::fromChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void MessageItem::subjectChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void MessageItem::hasAttachmentChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void MessageItem::datetimeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
