#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QSettings>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    QDir::setCurrent(QCoreApplication::applicationDirPath());

   // QSettings settings("HKEY_CLASSES_ROOT", QSettings::NativeFormat);
    //settings.setValue("UM", "TEST");
    QSettings settings2("HKEY_CLASSES_ROOT\\umanskiyTest", QSettings::NativeFormat);


    settings2.setValue("umanskiyTest", "TEST");
    settings2.setValue("Default", "URL: Foo Protocol");
    settings2.setValue("URL Protocol", "");
    QSettings settings3("HKEY_CLASSES_ROOT\\umanskiyTest\\shell", QSettings::NativeFormat);
    settings3.setValue("Default", "");
    QSettings settings4("HKEY_CLASSES_ROOT\\umanskiyTest\\shell\\open", QSettings::NativeFormat);
    settings4.setValue("Default", "");
    QSettings settings5("HKEY_CLASSES_ROOT\\umanskiyTest\\shell\\open\\command", QSettings::NativeFormat);
    settings5.setValue("Default", QDir::currentPath().replace('/','\\')+"\\token.exe \"%1\"");

    qDebug()<<"settings5.values:"<<settings5.value("Default");
    //settings.setValue("umanskiyTest\\URL Protocol", "");
    //settings.setValue("umanskiyTest\\shell\\open\\command\\", "C:\\Umanskiy2.exe");


    setlocale (0, "rus");
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "Umanskiy2_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    //MainClass mc("https://cloud-api.yandex.net","AQAAAAAXsJCoAADLWzhu3nZXOk1vjGkgtzWZAcM");


    MainWindow w("https://cloud-api.yandex.net","AQAAAAAXsJCoAADLWzhu3nZXOk1vjGkgtzWZAcM");
    w.show();
    return a.exec();
}
