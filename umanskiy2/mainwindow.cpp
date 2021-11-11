#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDesktopServices>
#include <QDir>
#include <QFile>

MainWindow::MainWindow(QString addressYandex, QString APIKey,QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    apikey = APIKey;
        addr = addressYandex;
        restConnector = new RestConnector(this);
        restConnector->initRequester("cloud-api.yandex.net",new QSslConfiguration());

        //https://oauth.yandex.ru/authorize?
     //   response_type=token
     // & client_id=<идентификатор приложения>
        QDesktopServices::openUrl( QUrl("https://oauth.yandex.ru/authorize?"
                                        "response_type=token"
                                        "&client_id=e366e0a0f27d48b7adb7af2724f33903"
                                        "&redirect_uri=umanskiytest:/", QUrl::TolerantMode));
        connect(&timer,SIGNAL(timeout()),this,SLOT(checkForFile()));
        timer.setSingleShot(false);
        timer.start();
        timer.setInterval(250);
}


void MainWindow::checkForFile()
{
    QFile file(QDir::currentPath()+"/key"+"/apikey2.txt");
    if (file.exists() && file.open(QFile::ReadOnly))
    {
        QString key = file.readAll();
        key = key.remove('\r').remove('\n');
        qDebug()<<"key:"<<key;
        if (key.length()>0){
            apikey = key;
            timer.stop();
            file.close();
            sendGetUser();
        }
    }
}

void MainWindow::onSuccess(const QJsonObject &obj)
{
    //qDebug()<<QJsonDocument(obj);
    auto map = obj.toVariantMap();
    QFile file("fileOut.json");
    file.open(QFile::WriteOnly);
    QTextStream stream(&file);
    stream<<QJsonDocument(obj).toJson(QJsonDocument::JsonFormat::Indented);
    auto sizes = map.keys();
    int i=0;
    for (auto size : sizes)
    {
        qDebug()<<"key#"<<i++<<"="<<size;
    }
    auto emb = obj.value("_embedded");
    auto embObj = obj.value("_embedded").toObject();
    auto itemsV = obj.value("_embedded").toObject().value("items");
    auto items = obj.value("_embedded").toObject().value("items").toArray();
    int maxsize = 0;
    QString maxSizeName = "Name";
    if (items.size()>0)
    {
        for (auto item : items)
        {
            auto sizeObj = item.toObject().value("size");
            if (!sizeObj.isUndefined())
            {
                auto size = sizeObj.toInt();
                auto name = item.toObject().value("name").toString();

                qDebug()<<"item.size:"<<size<<"name:"<<name;
                if (maxsize<size)
                {
                    maxsize = size;
                    maxSizeName = name;
                }
            }
        }
    }
    qDebug()<<"maxSizeName:"<<maxSizeName<<"maxsize:"<<maxsize;





}

void MainWindow::syncWithDB()
{

}

void MainWindow::sendGetUser()
{
    restConnector->setToken(apikey/*"AQAAAAAXsJCoAADLWzhu3nZXOk1vjGkgtzWZAcM"*/);
    restConnector->sendRequest("v1/disk/resources?path=%D0%92%D0%B8%D0%BD%D0%B4%D0%B0",onSuccess,onSuccess);
}

MainWindow::~MainWindow()
{
    delete ui;
}

