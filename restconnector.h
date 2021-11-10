#ifndef RESTCONNECTOR_H
#define RESTCONNECTOR_H


#include <QObject>
#include <QBuffer>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <functional>

class RestConnector : public QObject
{
    Q_OBJECT
public:
    typedef std::function<void(const QJsonObject &)> handleFunc;
    typedef std::function<void()> finishFunc;

    static const QString KEY_QNETWORK_REPLY_ERROR;
    static const QString KEY_CONTENT_NOT_FOUND;

    enum class Type {
        POST,
        GET,
        PATCH,
        DELET
    };

    explicit RestConnector(QObject *parent = nullptr);

    void initRequester(const QString& host, QSslConfiguration *value);

    void sendRequest(const QString &apiStr,
                     const handleFunc &funcSuccess,
                     const handleFunc &funcError,
                     Type type = Type::GET,
                     const QVariantMap &data = QVariantMap());


    void sendMulishGetRequest(const QString &apiStr,
                              const handleFunc &funcSuccess,
                              const handleFunc &funcError,
                              const finishFunc &funcFinish);

    QString getToken() const;
    void setToken(const QString &value);

private:
    static const QString httpTemplate;
    static const QString httpsTemplate;

    QString host;
    QString token;
    QSslConfiguration *sslConfig;

    QString pathTemplate;

    QByteArray variantMapToJson(QVariantMap data);

    QNetworkRequest createRequest(const QString &apiStr);

    QNetworkReply *sendCustomRequest(QNetworkAccessManager *manager,
                                     QNetworkRequest &request,
                                     const QString &type,
                                     const QVariantMap &data);

    QJsonObject parseReply(QNetworkReply *reply);

    bool onFinishRequest(QNetworkReply *reply);

    void handleQtNetworkErrors(QNetworkReply *reply, QJsonObject &obj);
    QNetworkAccessManager *manager;

signals:
    void networkError();


public slots:
};


#endif // RESTCONNECTOR_H
