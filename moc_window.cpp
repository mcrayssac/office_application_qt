/****************************************************************************
** Meta object code from reading C++ file 'window.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "window.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'window.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[25];
    char stringdata0[267];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 7), // "newFile"
QT_MOC_LITERAL(2, 19, 0), // ""
QT_MOC_LITERAL(3, 20, 4), // "open"
QT_MOC_LITERAL(4, 25, 4), // "save"
QT_MOC_LITERAL(5, 30, 6), // "saveAs"
QT_MOC_LITERAL(6, 37, 5), // "about"
QT_MOC_LITERAL(7, 43, 19), // "documentWasModified"
QT_MOC_LITERAL(8, 63, 4), // "bold"
QT_MOC_LITERAL(9, 68, 6), // "italic"
QT_MOC_LITERAL(10, 75, 9), // "underline"
QT_MOC_LITERAL(11, 85, 11), // "superscript"
QT_MOC_LITERAL(12, 97, 9), // "subscript"
QT_MOC_LITERAL(13, 107, 8), // "autoSave"
QT_MOC_LITERAL(14, 116, 16), // "increaseFontSize"
QT_MOC_LITERAL(15, 133, 16), // "decreaseFontSize"
QT_MOC_LITERAL(16, 150, 9), // "uppercase"
QT_MOC_LITERAL(17, 160, 9), // "lowercase"
QT_MOC_LITERAL(18, 170, 16), // "searchAndReplace"
QT_MOC_LITERAL(19, 187, 21), // "searchReplaceFunction"
QT_MOC_LITERAL(20, 209, 6), // "search"
QT_MOC_LITERAL(21, 216, 7), // "replace"
QT_MOC_LITERAL(22, 224, 14), // "findWholeWords"
QT_MOC_LITERAL(23, 239, 10), // "commitData"
QT_MOC_LITERAL(24, 250, 16) // "QSessionManager&"

    },
    "MainWindow\0newFile\0\0open\0save\0saveAs\0"
    "about\0documentWasModified\0bold\0italic\0"
    "underline\0superscript\0subscript\0"
    "autoSave\0increaseFontSize\0decreaseFontSize\0"
    "uppercase\0lowercase\0searchAndReplace\0"
    "searchReplaceFunction\0search\0replace\0"
    "findWholeWords\0commitData\0QSessionManager&"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  109,    2, 0x08 /* Private */,
       3,    0,  110,    2, 0x08 /* Private */,
       4,    0,  111,    2, 0x08 /* Private */,
       5,    0,  112,    2, 0x08 /* Private */,
       6,    0,  113,    2, 0x08 /* Private */,
       7,    0,  114,    2, 0x08 /* Private */,
       8,    0,  115,    2, 0x08 /* Private */,
       9,    0,  116,    2, 0x08 /* Private */,
      10,    0,  117,    2, 0x08 /* Private */,
      11,    0,  118,    2, 0x08 /* Private */,
      12,    0,  119,    2, 0x08 /* Private */,
      13,    0,  120,    2, 0x08 /* Private */,
      14,    0,  121,    2, 0x08 /* Private */,
      15,    0,  122,    2, 0x08 /* Private */,
      16,    0,  123,    2, 0x08 /* Private */,
      17,    0,  124,    2, 0x08 /* Private */,
      18,    0,  125,    2, 0x08 /* Private */,
      19,    3,  126,    2, 0x08 /* Private */,
      23,    1,  133,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Bool,
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::Bool,   20,   21,   22,
    QMetaType::Void, 0x80000000 | 24,    2,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->newFile(); break;
        case 1: _t->open(); break;
        case 2: { bool _r = _t->save();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 3: { bool _r = _t->saveAs();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 4: _t->about(); break;
        case 5: _t->documentWasModified(); break;
        case 6: _t->bold(); break;
        case 7: _t->italic(); break;
        case 8: _t->underline(); break;
        case 9: _t->superscript(); break;
        case 10: _t->subscript(); break;
        case 11: _t->autoSave(); break;
        case 12: _t->increaseFontSize(); break;
        case 13: _t->decreaseFontSize(); break;
        case 14: _t->uppercase(); break;
        case 15: _t->lowercase(); break;
        case 16: _t->searchAndReplace(); break;
        case 17: _t->searchReplaceFunction((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 18: _t->commitData((*reinterpret_cast< QSessionManager(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 19)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 19;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
