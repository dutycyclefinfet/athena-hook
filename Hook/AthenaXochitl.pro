QT += qml quick gui concurrent

TEMPLATE = lib
CONFIG += c++17

SOURCES += AthenaBase.cpp AthenaXochitl.cpp
HEADERS += AthenaBase.h AthenaKernel.h AthenaSettings.h AthenaOPKG.h AthenaHook.h AthenaSystem.h EventHook.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
