/****************************************************************************
** Meta object code from reading C++ file 'roles.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/roles.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'roles.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Roles_t {
    QByteArrayData data[8];
    char stringdata0[79];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Roles_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Roles_t qt_meta_stringdata_Roles = {
    {
QT_MOC_LITERAL(0, 0, 5), // "Roles"
QT_MOC_LITERAL(1, 6, 9), // "RoleNames"
QT_MOC_LITERAL(2, 16, 10), // "SenderRole"
QT_MOC_LITERAL(3, 27, 11), // "SubjectRole"
QT_MOC_LITERAL(4, 39, 8), // "DateRole"
QT_MOC_LITERAL(5, 48, 7), // "UIDRole"
QT_MOC_LITERAL(6, 56, 9), // "EmailRole"
QT_MOC_LITERAL(7, 66, 12) // "SelectedRole"

    },
    "Roles\0RoleNames\0SenderRole\0SubjectRole\0"
    "DateRole\0UIDRole\0EmailRole\0SelectedRole"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Roles[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       1,   14, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // enums: name, alias, flags, count, data
       1,    1, 0x0,    6,   19,

 // enum data: key, value
       2, uint(Roles::SenderRole),
       3, uint(Roles::SubjectRole),
       4, uint(Roles::DateRole),
       5, uint(Roles::UIDRole),
       6, uint(Roles::EmailRole),
       7, uint(Roles::SelectedRole),

       0        // eod
};

void Roles::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject Roles::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_Roles.data,
    qt_meta_data_Roles,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Roles::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Roles::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Roles.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Roles::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE