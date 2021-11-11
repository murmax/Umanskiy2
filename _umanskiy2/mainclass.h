#ifndef MAINCLASS_H
#define MAINCLASS_H

#include <QObject>
#include <QTimer>

#include <QtSql/QSqlDatabase>
#include "restconnector.h"

class MainClass : public QObject
{
    Q_OBJECT
public:
    MainClass(QString addressYandex, QString APIKey, QObject* parent=nullptr);
    static void onSuccess(const QJsonObject &obj);
    static void onFail();

    void syncWithDB();

    void sendGetUser();
public slots:
    void checkForFile();
private:
    QTimer timer;
    QSqlDatabase dbase;
    QString addr;
    QString apikey;
    RestConnector* restConnector=nullptr;
};

#endif // MAINCLASS_H