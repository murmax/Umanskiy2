#include "mainclass.h"



MainClass::MainClass(QString addressYandex, QString APIKey, QObject *parent) : QObject(parent)
{
    apikey = APIKey;
    addr = addressYandex;
}
