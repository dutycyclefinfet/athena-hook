QT += qml quick gui

TEMPLATE = lib
CONFIG += c++17

SOURCES += AthenaXochitl.cpp
HEADERS += AthenaKernel.h AthenaSettings.h AthenaOPKG.h AthenaHook.h EventHook.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
