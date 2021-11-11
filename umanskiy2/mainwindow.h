#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include <QtSql/QSqlDatabase>
#include "restconnector.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QString addressYandex, QString APIKey, QWidget *parent = nullptr);
    ~MainWindow();
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
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
