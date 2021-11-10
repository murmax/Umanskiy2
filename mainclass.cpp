#include "mainclass.h"



MainClass::MainClass(QString addressYandex, QString APIKey, QObject *parent) : QObject(parent)
{
    apikey = APIKey;
    addr = addressYandex;
    restConnector = new RestConnector(this);
    restConnector->initRequester()
}

void MainClass::syncWithDB()
{

}

void MainClass::sendGetUser()
{

}
