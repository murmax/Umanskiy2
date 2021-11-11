#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QDir::setCurrent(QCoreApplication::applicationDirPath());
    setlocale (0, "rus");
    QFile file1("C:/test/test.txt");
    if (file1.open(QFile::ReadOnly))
    {
        QTextStream stream(&file1);
        QString str;
        stream>>str;
        qDebug()<<"Read from test.txt"<<str;
    }else
    {
        qDebug()<<"cannot open even first file"<<file1.errorString();
    }
    for (int i=0;i<argc;i++)
    {
        qDebug()<<argv[i];
        QString str = argv[i];
        if (str.contains("access_token"))
        {
            qDebug()<<"str"<<str<<"contain access_token";
            QDir dir(QDir::currentPath()+"/key");
            if (!dir.exists())
            {
                QDir::current().mkdir("key");
            }
            QFile file(QDir::currentPath()+"/key"+"/apikey2.txt");
            if (file.open(QFile::WriteOnly))
            {
                QTextStream stream(&file);
                int posStart = str.indexOf("access_token=")+QString("access_token=").length();
                int posTill =  str.indexOf('&',posStart);
                qDebug()<<"posStart="<<posStart<<"posTill="<<posTill;
                auto str2 = str.mid(posStart,posTill-posStart);
                qDebug()<<"accessToken="<<str2;
                stream<<str2;
                file.close();
            } else
            {
                qDebug()<<"cannot open file"<<file.errorString();
            }
        } else
        {

            qDebug()<<"str"<<str<<"not contain access_token";
        }
    }


    return a.exec();
}
