#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include <QtSql/QSqlDatabase>
#include "restconnector.h"
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QRandomGenerator>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <math.h>
#include <QtCharts>

using namespace QtCharts;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
uint maxId = 1;
public:
    MainWindow(QString addressYandex, QString APIKey, QWidget *parent = nullptr);
    ~MainWindow();
     static void onSuccess(const QJsonObject &obj);
     static void onFail(const QJsonObject &obj);
     static void onSuccessPUT(const QJsonObject &obj);
     static void onSuccessGETADDRTOPUT(const QJsonObject &obj);
     static void onSuccessGETADDRTOPUT2(const QJsonObject &obj);
     int getInterval();
     static MainWindow* mw;

    void createDB();
    void sendObjectIntoDB(QDateTime time, QString nameBiggest,QString nameSmallest,uint smallest, uint avg, uint max, uint amount);

    void sendGetValues();
public slots:
    void checkForFile();
    void generatePix();
    void getValues();
    void makeReport();
private:
    QTimer timer;
    QTimer mainTimer;
    QTimer addingPicsTimer;
    QTimer timerReport;
    QSqlDatabase dbase;
    QString addr;
    QString path;
    QString apikey;
    RestConnector* restConnector=nullptr;
    static QMap<QString,QString> iniParams;
    QRandomGenerator gen;
    int offset=-100;

    QByteArray dataToPut;
    QByteArray dataToPutImage;
    bool image=false;;
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
