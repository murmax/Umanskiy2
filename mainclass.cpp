#include "mainclass.h"



MainClass::MainClass(QString addressYandex, QString APIKey, QObject *parent) : QObject(parent)
{
    apikey = APIKey;
    addr = addressYandex;
    restConnector = new RestConnector(this);
    restConnector->initRequester("cloud-api.yandex.net",new QSslConfiguration());
    sendGetUser();
}

void MainClass::onSuccess(const QJsonObject &obj)
{
    qDebug()<<obj;
}

void MainClass::syncWithDB()
{

}

void MainClass::sendGetUser()
{
    restConnector->setToken("AQAAAAAXsJCoAADLWzhu3nZXOk1vjGkgtzWZAcM");
    restConnector->sendRequest("v1/disk",onSuccess,onSuccess);
}
