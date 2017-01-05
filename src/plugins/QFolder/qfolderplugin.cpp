#include "qfolder.h"
#include "qfolderplugin.h"

#include <QtPlugin>

QFolderPlugin::QFolderPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void QFolderPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool QFolderPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *QFolderPlugin::createWidget(QWidget *parent)
{
    return new QFolder(parent);
}

QString QFolderPlugin::name() const
{
    return QLatin1String("QFolder");
}

QString QFolderPlugin::group() const
{
    return QLatin1String("");
}

QIcon QFolderPlugin::icon() const
{
    return QIcon();
}

QString QFolderPlugin::toolTip() const
{
    return QLatin1String("");
}

QString QFolderPlugin::whatsThis() const
{
    return QLatin1String("");
}

bool QFolderPlugin::isContainer() const
{
    return false;
}

QString QFolderPlugin::domXml() const
{
    return QLatin1String("<widget class=\"QFolder\" name=\"qFolder\">\n</widget>\n");
}

QString QFolderPlugin::includeFile() const
{
    return QLatin1String("qfolder.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(qfolderplugin, QFolderPlugin)
#endif // QT_VERSION < 0x050000
