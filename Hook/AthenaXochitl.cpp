#include <QGuiApplication>
#include <QWindow>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlComponent>
#include <QQuickItem>
#include <QQuickWindow>

#include <dlfcn.h>
#include <stdlib.h>
#include "AthenaBase.h"
#include "AthenaSettings.h"
#include "AthenaSystem.h"
#include "AthenaKernel.h"
#include "AthenaOPKG.h"
#include "AthenaHook.h"
#include "EventHook.h"

const char hook_athena[] = 
"import QtQuick 2.0\n"
"Loader {\n"
"    id: hookEntryPoint\n"
"    objectName: \"hookEntryPointObj\"\n"
"    anchors.fill: parent\n"
"    property Item root_id: parent\n"
"    function root_keyHandler(down, keycode) {\n"
"        hookEntryPoint.item.keyHandler(down?\"pressed\":\"released\", {\"key\": keycode});\n"
"    }\n"
"    source: \"file:///usr/libexec/athenaXochitl/hook.qml\"\n"
"}";
const char hook_athena_stock[] = 
"import QtQuick 2.0\n"
"Loader {\n"
"    id: hookEntryPoint\n"
"    objectName: \"hookEntryPointObj\"\n"
"    anchors.fill: parent\n"
"    property Item root_id: parent\n"
"    function root_keyHandler(down, keycode) {\n"
"        hookEntryPoint.item.keyHandler(down?\"pressed\":\"released\", {\"key\": keycode});\n"
"    }\n"
"    source: \"file:///home/.rootdir/usr/libexec/athenaXochitl/hook.qml\"\n"
"}";

int QGuiApplication::exec() {
    static const auto orig_fn = (int (*)())dlsym(RTLD_NEXT, "_ZN15QGuiApplication4execEv");
    setenv("QML_XHR_ALLOW_FILE_READ", "1", 1);
    setenv("QML_XHR_ALLOW_FILE_WRITE", "1", 1);
    unsetenv("LD_PRELOAD");
    AthenaBase::init();

    auto app = static_cast<QGuiApplication*>(QGuiApplication::instance());
    auto windows = app->allWindows();
    auto w = qobject_cast<QQuickWindow*>(windows.at(0));
    auto c = w->contentItem();
    auto e = QQmlEngine::contextForObject(c)->engine();

    if (e != nullptr) {
        QQmlComponent component(e);
        component.setData(AthenaBase::isAthenaRunning() ? hook_athena : hook_athena_stock, QUrl("/AthenaXochitl.cpp"));
        auto hookItem = qobject_cast<QQuickItem*>(component.create());
        auto evHook = new EventHook(hookItem);

        auto athenaHook = new AthenaHook(c);
        auto athenaOPKG = new AthenaOPKG();
        auto athenaKernel = new AthenaKernel();
        auto athenaSettings = new AthenaSettings();
        auto athenaSystem = new AthenaSystem();
        
        c->installEventFilter(evHook);
        hookItem->setParentItem(c);
        
        e->addImportPath(AthenaBase::xochitlPluginsCommon());
        auto root = e->rootContext();
        root->setContextProperty("AthenaHook", athenaHook);
        root->setContextProperty("AthenaOPKG", athenaOPKG);
        root->setContextProperty("AthenaKernel", athenaKernel);
        root->setContextProperty("AthenaSettings", athenaSettings);
        root->setContextProperty("AthenaSystem", athenaSystem);
        
        AthenaBase::finish();
    } else {
        printf("[AUTOHOOKER] Fatal error: no engine instance found. Please report this!\n");
    }

    return orig_fn();
}
