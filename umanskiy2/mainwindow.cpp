#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QAbstractBarSeries>
#include <QBarSeries>
#include <QBarSet>
#include <QMessageBox>
#define SQL_QUERY(X) sr = query.exec(X);  if (!sr){qDebug()<<tr("Database Error")+" on query: "+X+" error:"+dbase.lastError().text();}


//dbase.setDatabaseName(iniParams.value("PostgreSQL_DBName")/*"myDB1"*/);
//dbase.setPort(iniParams.value("PostgreSQL_Port").toInt(&ok)/*5432*/);
//dbase.setHostName(iniParams.value("PostgreSQL_HostName")/*"localhost"*/);
//dbase.setUserName(iniParams.value("PostgreSQL_UserName")/*"User1"*/);
//dbase.setPassword(iniParams.value("PostgreSQL_UserPassword")/*"123"*/);
QMap<QString,QString> MainWindow::iniParams=
{
    {"PostgreSQL_HostName","localhost"},
    {"PostgreSQL_Port","5432"},
    {"PostgreSQL_DBName","umanskiyProjDB"},
    {"PostgreSQL_UserName","User1"},
    {"PostgreSQL_UserPassword","123"},
};
MainWindow* MainWindow::mw=nullptr;

MainWindow::MainWindow(QString addressYandex, QString APIKey,QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    apikey = APIKey;
    mw = this;
        addr = addressYandex;
        path = "UmanskiyProg";
        restConnector = new RestConnector(this);


        //https://oauth.yandex.ru/authorize?
     //   response_type=token
     // & client_id=<идентификатор приложения>
        createDB();
        QFile file(QDir::currentPath()+"/key"+"/apikey2.txt");
        if (!file.exists()){
            QDesktopServices::openUrl( QUrl("https://oauth.yandex.ru/authorize?"
                                            "response_type=token"
                                            "&client_id=e366e0a0f27d48b7adb7af2724f33903"
                                            "&redirect_uri=umanskiytest:/", QUrl::TolerantMode));
        }

        connect(&timer,SIGNAL(timeout()),this,SLOT(checkForFile()));
        timer.setSingleShot(false);
        timer.start();
        timer.setInterval(250);

        mainTimer.setInterval(15000);
        mainTimer.setSingleShot(false);
        connect(&mainTimer,SIGNAL(timeout()),this,SLOT(getValues()));


        addingPicsTimer.setInterval(getInterval());
        addingPicsTimer.setSingleShot(true);
        connect(&addingPicsTimer,SIGNAL(timeout()),this,SLOT(generatePix()));


        timerReport.setInterval(10000);
        timerReport.setSingleShot(false);
        connect(&timerReport,SIGNAL(timeout()),this,SLOT(makeReport()));
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
            addingPicsTimer.start();
            mainTimer.start();
            timerReport.start();




            /*qDebug()<<"QNetworkAccessManager START SOME DATA";
            QNetworkAccessManager manager;
            QByteArray PIStr = "Some data";
            QUrl url = QUrl("https://uploader26g.disk.yandex.net:443/upload-target/20211111T123450.420.utd.4e70dhrwj357sl9xxz7fdrvyi-k26g.1664313");

            qDebug()<<"url:"<<url.isValid();
            QNetworkRequest request(url);
            request.setHeader(QNetworkRequest::ContentTypeHeader, QString("text/plain"));
            request.setRawHeader("Authorization",QString("OAuth %1").arg(apikey).toUtf8());
            QNetworkReply* reply = manager.put(request,PIStr);
            qDebug()<<"reply sent. finished:"<<reply->isFinished();

            QEventLoop loop;
            connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
            qDebug()<<"in loop";
            loop.exec();

            qDebug()<<"out of loop:"<<reply->readAll()<<"error:"<<reply->error();*/

        }
    }
}

void MainWindow::generatePix()
{
    if (mutex) return;
    QImage img(128,128,QImage::Format::Format_RGB32);
    for (int i=0;i<img.width();i++)
    {
        for (int j=0;j<img.height();j++)
        {
            QColor color(gen.bounded(0,4) * 64,gen.bounded(0,4)* 64,gen.bounded(0,4) * 64);
            img.setPixel(i,j,color.rgb());
        }
    }


    addingPicsTimer.setInterval(getInterval());
    addingPicsTimer.start();
    //https://cloud-api.yandex.net/v1/disk/resources/upload?path=UmanskiyProg%5Clog3.txt
    restConnector->initRequester("cloud-api.yandex.net",new QSslConfiguration());
    restConnector->setToken(apikey);
    static int logCount = 10;
    QString path = "UmanskiyProg/log"+QString::number(logCount++)+".png";
    mutex=true;
    dataToPut = QVariant(img).toByteArray();
    QBuffer buffer(&dataToPut);
    buffer.open(QIODevice::WriteOnly);
    img.save(&buffer, "PNG");
    restConnector->sendRequest("v1/disk/resources/upload?path="+path,
                            onSuccessGETADDRTOPUT,onFail,RestConnector::Type::GET);



}

void MainWindow::getValues()
{
    sendGetValues();

}

void MainWindow::makeReport()
{
    if (mutex) return;
    QSqlQuery query(dbase);
    bool sr;
    SQL_QUERY("SELECT * FROM filesData;")
//            SQL_QUERY("CREATE TABLE IF NOT EXISTS filesData ("
//                          "id INT  NOT NULL PRIMARY KEY, "
//                          "nameBiggest VARCHAR(255),"
//                          " time TIMESTAMP,"
//                          " amount INT ,"
//                          " maxSize INT ,"nameSmallest
//                          " avgSize INT "
//                      ");");
    QSqlRecord rec = query.record();


    QBarSeries *seriesMax = new QBarSeries(this);
    QBarSet * barSetMax = new QBarSet("Максимум");
    QBarSet * barSetMin = new QBarSet("Минимум");
    QBarSet * barSetAvg = new QBarSet("Среднее");

    while (query.next())
    {
        QString key = query.value(rec.indexOf("time")).toDateTime().toString("hh:mm:ss dd.MM.yyyy");
        int maxSize = query.value(rec.indexOf("maxSize")).toInt();
        int avgSize = query.value(rec.indexOf("avgSize")).toInt();
        int minSize = query.value(rec.indexOf("minSize")).toInt();
        *barSetMax << maxSize;
        *barSetMin << minSize;
        *barSetAvg << avgSize;




        //qDebug()<<"key:"<<key<<"value"<<value;

    }
    seriesMax->append(barSetMax);
    seriesMax->append(barSetMin);
    seriesMax->append(barSetAvg);
    seriesMax->setLabelsVisible(true);
    seriesMax->setLabelsPosition(QAbstractBarSeries::LabelsInsideEnd);
    seriesMax->setLabelsPrecision(9);
    QChart *chart = new QChart();
    chart->addSeries(seriesMax);
    chart->setTitle("Графики размеров файлов");
    chart->setAnimationOptions(QChart::NoAnimation);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    QChartView *chartView = new QChartView(chart,this);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->chart()->createDefaultAxes();
    //chartView->setMinimumSize(1500,1500);
    //chartView->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    //chartView->show();
    chartView->setObjectName("chart");

    for (int i=0;i<ui->horizontalLayout->count();i++)
    {
        QLayoutItem* item = ui->horizontalLayout->itemAt(i);
        if (item->widget())
        {
            QWidget* widg = item->widget();
            if (widg->objectName() == "chart")
            {
                ui->horizontalLayout->removeItem(item);
                delete widg;
            }
        }
    }
    ui->horizontalLayout->addWidget(chartView);

    QEventLoop loop;
    QTimer tm;
    tm.setInterval(100);
    tm.start();
    connect(&tm,SIGNAL(timeout()),&loop,SLOT(quit()));
    loop.exec();


    const auto dpr = chartView->devicePixelRatioF();
    QPixmap bufferPixMap(chartView->width() * dpr, chartView->height() * dpr);
    bufferPixMap.setDevicePixelRatio(dpr);
    bufferPixMap.fill(Qt::transparent);

    QPainter *paint = new QPainter(&bufferPixMap);
    paint->setPen(*(new QColor(255,34,255,255)));
    chartView->render(paint);

    QImage img = bufferPixMap.toImage();

    QFile file("image.png");
    file.open(QIODevice::WriteOnly);
    bufferPixMap.save(&file, "PNG");


    /*img.fill(Qt::transparent);
    QPainter painter(&img);
    chartView->render(&painter);*/
    /*ui->verticalLayout_4->insertWidget(ui->verticalLayout_4->count(),chartView);
    this->chart = chartView;
    ui->verticalSpacer->changeSize(0,0,QSizePolicy::Fixed);*/



    restConnector->initRequester("cloud-api.yandex.net",new QSslConfiguration());
    restConnector->setToken(apikey);
    QString path = "UmanskiyReports/report"+QString::number(maxId)+".png";
    mutex=true;
    dataToPut = QVariant(img).toByteArray();

    QBuffer buffer(&dataToPut);
    buffer.open(QIODevice::WriteOnly);
    img.save(&buffer, "PNG");
    restConnector->sendRequest("v1/disk/resources/upload?path="+path,
                            onSuccessGETADDRTOPUT,onFail,RestConnector::Type::GET);
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
    /*for (auto size : sizes)
    {
        qDebug()<<"key#"<<i++<<"="<<size;
    }*/
    auto emb = obj.value("_embedded");
    auto embObj = obj.value("_embedded").toObject();
    auto itemsV = obj.value("_embedded").toObject().value("items");
    auto items = obj.value("_embedded").toObject().value("items").toArray();
    uint minsize = UINT_MAX;
    uint maxsize = 0;
    uint avgSize = 0;
    uint amount = 0;
    QString minSizeName = "MinName";
    QString maxSizeName = "MaxName";
    if (items.size()>0)
    {
        for (auto item : items)
        {
            amount++;
            auto sizeObj = item.toObject().value("size");
            if (!sizeObj.isUndefined())
            {
                uint size = sizeObj.toInt();
                QString name = item.toObject().value("name").toString();

                //qDebug()<<"item.size:"<<size<<"name:"<<name;
                if (minsize>size)
                {
                    minsize = size;
                    minSizeName = name;
                }
                if (maxsize<size)
                {
                    maxsize = size;
                    maxSizeName = name;
                }
                avgSize+=size;
            }
        }
    }
    if (amount!=0){
        avgSize = avgSize/amount;
        mw->sendObjectIntoDB(QDateTime::currentDateTime(),maxSizeName,minSizeName,minsize,avgSize,maxsize,amount);
        qDebug()<<"maxSizeName:"<<maxSizeName<<"maxsize:"<<minsize;
    }





}

void MainWindow::onFail(const QJsonObject &obj)
{
    qDebug()<<__func__<<obj;
    mw->mutex=false;
}

void MainWindow::onSuccessPUT(const QJsonObject &obj)
{
    qDebug()<<__func__<<obj;
    mw->mutex=false;
}

void MainWindow::onSuccessGETADDRTOPUT(const QJsonObject &obj)
{
    qDebug()<<__func__<<obj;
    QVariantMap data;
    data.insert("raw",mw->dataToPut);
    mw->mutex = false;
    QString href = obj.value("href").toString();
    int posFirst = href.indexOf("https://")+QString("https://").length();
    int posSecond = href.indexOf("yandex.net:443")+QString("yandex.net:443").length();
    QString addr = href.mid(
                posFirst,
                posSecond- posFirst
                );

    QString otherPart = href.mid(
                posSecond+1
                );

    mw->restConnector->initRequester(addr,new QSslConfiguration());
    mw->restConnector->setToken(mw->apikey);
    mw->restConnector->sendRequest(otherPart,
                            onSuccessPUT,onFail,RestConnector::Type::PUT,data);

}

int MainWindow::getInterval()
{
    return gen.bounded(1250,12500);
}

void MainWindow::createDB()
{
    qDebug()<<"START"<<__func__;
    dbase = QSqlDatabase::addDatabase("QPSQL");
    //const auto defaultDbname = "postgres";
    //dbase.setDatabaseName(defaultDbname);
    dbase.setPort(iniParams.value("PostgreSQL_Port").toInt());
    dbase.setHostName(iniParams.value("PostgreSQL_HostName"));
    dbase.setUserName(iniParams.value("PostgreSQL_UserName"));
    dbase.setPassword(iniParams.value("PostgreSQL_UserPassword"));
    auto dbname = iniParams.value("PostgreSQL_DBName");
    dbase.setDatabaseName(dbname);
    if (dbase.open())
    {
        qDebug()<<"DBASE OPEN";
        QSqlQuery query(dbase);
        bool sr;

        /*SQL_QUERY("CREATE DATABASE IF NOT EXISTS "+dbname);
        SQL_QUERY("USE "+dbname);*/
        SQL_QUERY("CREATE TABLE IF NOT EXISTS filesData ("
                        "id INT  NOT NULL PRIMARY KEY, "
                        "nameBiggest VARCHAR(255),"
                        "nameSmallest VARCHAR(255),"
                        " time TIMESTAMP,"
                        " amount INT ,"
                        " minSize INT ,"
                        " maxSize INT ,"
                        " avgSize INT "
                  ");");
        SQL_QUERY("SELECT * FROM filesData;")

        QSqlRecord rec = query.record();

        while (query.next())
        {
            uint id = query.value(rec.indexOf("id")).toUInt();
            maxId = qMax(id, maxId);
        }
        qDebug()<<"maxId:"<<maxId;

    } else{

        QMessageBox::warning(this,"ERROR","CANNOT OPEN DBASE"+dbase.lastError().text());
    }


    qDebug()<<"END"<<__func__;
}

void MainWindow::sendObjectIntoDB(QDateTime time, QString nameBiggest, QString nameSmallest, uint smallest, uint avg, uint max, uint amount)
{

    if (dbase.open() && amount>0)
    {
        QSqlQuery query(dbase);
        bool sr;
#define SQL_QUERY(X) sr = query.exec(X);  if (!sr){qDebug()<<tr("Database Error")+" on query: "+X+" error:"+dbase.lastError().text();}

        QString str_insert = "INSERT INTO filesData(id, nameBiggest,nameSmallest, time, amount,minSize, maxSize, avgSize) "
                            "VALUES (%1,  '%2','%3', '%4',%5, %6, %7, %8);";
        maxId++;
        QString str = str_insert
                .arg(QString::number(maxId))
                .arg(nameBiggest)
                .arg(nameSmallest)
                .arg(QVariant(time).toString())
                .arg(QString::number(amount))
                .arg(QString::number(smallest))
                .arg(QString::number(max))
                .arg(QString::number(avg));

        SQL_QUERY(str);


    }
}

void MainWindow::sendGetValues()
{
    restConnector->initRequester("cloud-api.yandex.net",new QSslConfiguration());
    restConnector->setToken(apikey);
    restConnector->sendRequest("v1/disk/resources?path="+path,onSuccess,onFail,RestConnector::Type::GET);
}

MainWindow::~MainWindow()
{
    delete ui;
}

