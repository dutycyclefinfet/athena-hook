#include "AthenaBase.h"

namespace {
    const QString basePath = "/home/root/.xochitlPlugins/";
}

QString AthenaBase::m_xochitlPluginsPath = basePath;
QString AthenaBase::m_xochitlPluginsCommon = basePath + ".common/";
QString AthenaBase::m_athenaRoot = "/home/.rootdir/";
bool AthenaBase::m_athenaIsRunning = false;
