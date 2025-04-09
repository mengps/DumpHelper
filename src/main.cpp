#include "dumphelper.h"

#include <QGuiApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    DumpHelper::setDumpGenerationParams(app.applicationDirPath() + "/crash",
                                        []() -> QString {
                                            return QString("%1_%2.dmp").arg("crash", QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
                                        },
                                        []{
                                            qDebug() << "Crash! The dump files in:" << DumpHelper::dumpGenerationPath();
                                            QDesktopServices::openUrl(QUrl::fromLocalFile(DumpHelper::dumpGenerationPath()));
                                        },
                                        DumpHelper::MiniDumpScanMemory |
                                            DumpHelper::MiniDumpWithIndirectlyReferencedMemory);

    DumpHelper::runDumpProcess();

    int *p = nullptr;
    *p = 0;

    return app.exec();
}
