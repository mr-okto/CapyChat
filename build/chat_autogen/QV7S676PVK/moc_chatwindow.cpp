/****************************************************************************
** Meta object code from reading C++ file 'chatwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.9.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../project/chatwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'chatwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.9.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ChatWindow_t {
    QByteArrayData data[17];
    char stringdata0[199];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ChatWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ChatWindow_t qt_meta_stringdata_ChatWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "ChatWindow"
QT_MOC_LITERAL(1, 11, 24), // "connection_to_other_room"
QT_MOC_LITERAL(2, 36, 0), // ""
QT_MOC_LITERAL(3, 37, 18), // "attempt_connection"
QT_MOC_LITERAL(4, 56, 19), // "connected_to_server"
QT_MOC_LITERAL(5, 76, 13), // "attempt_login"
QT_MOC_LITERAL(6, 90, 9), // "user_name"
QT_MOC_LITERAL(7, 100, 9), // "room_name"
QT_MOC_LITERAL(8, 110, 9), // "logged_in"
QT_MOC_LITERAL(9, 120, 16), // "message_received"
QT_MOC_LITERAL(10, 137, 6), // "sender"
QT_MOC_LITERAL(11, 144, 4), // "text"
QT_MOC_LITERAL(12, 149, 12), // "send_message"
QT_MOC_LITERAL(13, 162, 11), // "user_joined"
QT_MOC_LITERAL(14, 174, 8), // "username"
QT_MOC_LITERAL(15, 183, 9), // "user_left"
QT_MOC_LITERAL(16, 193, 5) // "error"

    },
    "ChatWindow\0connection_to_other_room\0"
    "\0attempt_connection\0connected_to_server\0"
    "attempt_login\0user_name\0room_name\0"
    "logged_in\0message_received\0sender\0"
    "text\0send_message\0user_joined\0username\0"
    "user_left\0error"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ChatWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x08 /* Private */,
       3,    0,   65,    2, 0x08 /* Private */,
       4,    0,   66,    2, 0x08 /* Private */,
       5,    2,   67,    2, 0x08 /* Private */,
       8,    0,   72,    2, 0x08 /* Private */,
       9,    2,   73,    2, 0x08 /* Private */,
      12,    0,   78,    2, 0x08 /* Private */,
      13,    1,   79,    2, 0x08 /* Private */,
      15,    1,   82,    2, 0x08 /* Private */,
      16,    0,   85,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    6,    7,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,   10,   11,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void,

       0        // eod
};

void ChatWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        ChatWindow *_t = static_cast<ChatWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->connection_to_other_room(); break;
        case 1: _t->attempt_connection(); break;
        case 2: _t->connected_to_server(); break;
        case 3: _t->attempt_login((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 4: _t->logged_in(); break;
        case 5: _t->message_received((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 6: _t->send_message(); break;
        case 7: _t->user_joined((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: _t->user_left((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->error(); break;
        default: ;
        }
    }
}

const QMetaObject ChatWindow::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ChatWindow.data,
      qt_meta_data_ChatWindow,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *ChatWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ChatWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ChatWindow.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ChatWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE