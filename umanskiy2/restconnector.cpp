#include "restconnector.h"

#include "restconnector.h"

const QString RestConnector::httpTemplate = "http://%1/%2";
const QString RestConnector::httpsTemplate = "https://%1/%2";
const QString RestConnector::KEY_QNETWORK_REPLY_ERROR = "QNetworkReplyError";
const QString RestConnector::KEY_CONTENT_NOT_FOUND = "ContentNotFoundError";

const QString yandexDiskAddr = "https://cloud-api.yandex.net/v1/disk";

RestConnector::RestConnector(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
}

void RestConnector::initRequester(const QString &host,  QSslConfiguration *value)
{
    this->host = host;
    sslConfig = value;
    if (sslConfig != nullptr)
        pathTemplate = httpsTemplate;
    else
        pathTemplate = httpTemplate;
}

void RestConnector::sendRequest(const QString &apiStr,
                            const handleFunc &funcSuccess,
                            const handleFunc &funcError,
                            RestConnector::Type type,
                            const QVariantMap &data)
{
    QNetworkRequest request = createRequest(apiStr);
    qDebug()<<"Request. URL:"<<request.url()<<"headers:"<<request.rawHeaderList();

    QNetworkReply *reply;
    switch (type) {
    case Type::POST: {
        QByteArray postDataByteArray = variantMapToJson(data);
        reply = manager->post(request, postDataByteArray);
        break;
    } case Type::GET: {
        reply = manager->get(request);
        break;
    } case Type::PUT: {
        if (data.contains("raw"))
        {
            reply = manager->put(request,data.value("raw").toByteArray());
        //QJsonDocument testJsonData = QJsonDocument::fromVariant(data);
        } else
        {
            reply = manager->put(request,"Some random data");
        }
        break;
    } case Type::DELET: {
        if (data.isEmpty())
            reply = manager->deleteResource(request);
        else
            reply = sendCustomRequest(manager, request, "DELETE", data);
        break;
    } case Type::PATCH: {
        reply = sendCustomRequest(manager, request, "PATCH", data);
        break;
    } default:
        reply = nullptr;
        Q_ASSERT(false);
    }

    connect(reply, &QNetworkReply::finished, this,
            [this, funcSuccess, funcError, reply]() {
        QJsonObject obj = parseReply(reply);

        if (onFinishRequest(reply)) {
            if (funcSuccess != nullptr)
                funcSuccess(obj);
        } else {
            if (funcError != nullptr) {
                handleQtNetworkErrors(reply, obj);
                funcError(obj);
            }
        }
        reply->close();
        reply->deleteLater();
    } );

}

void RestConnector::sendMulishGetRequest(const QString &apiStr, //а ничего что здесь нигде не проверяется func != nullptr?
                                     const handleFunc &funcSuccess,
                                     const handleFunc &funcError,
                                     const finishFunc &funcFinish)
{
    QNetworkRequest request = createRequest(apiStr);
    //    QNetworkReply *reply;
    qInfo() << "GET REQUEST " << request.url().toString() << "\n";
    auto reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this,
            [this, funcSuccess, funcError, funcFinish, reply]() {
        QJsonObject obj = parseReply(reply);
        if (onFinishRequest(reply)) {
            if (funcSuccess != nullptr)
                funcSuccess(obj);
            QString nextPage = obj.value("next").toString();
            if (!nextPage.isEmpty()) {
                QStringList apiMethodWithPage = nextPage.split("api/");
                sendMulishGetRequest(apiMethodWithPage.value(1),
                                     funcSuccess,
                                     funcError,
                                     funcFinish
                                     );
            } else {
                if (funcFinish != nullptr)
                    funcFinish();
            }
        } else {
            handleQtNetworkErrors(reply, obj);
            if (funcError != nullptr)
                funcError(obj);
        }
        reply->close();
        reply->deleteLater();
    });
}


QString RestConnector::getToken() const
{
    return token;
}

void RestConnector::setToken(const QString &value)
{
    token = value;
}

QByteArray RestConnector::variantMapToJson(QVariantMap data)
{
    QJsonDocument postDataDoc = QJsonDocument::fromVariant(data);
    QByteArray postDataByteArray = postDataDoc.toJson();

    return postDataByteArray;
}

QNetworkRequest RestConnector::createRequest(const QString &apiStr)
{
    QNetworkRequest request;
    QString url = pathTemplate.arg(host).arg(apiStr);
    request.setUrl(QUrl(url));
    request.setRawHeader("Content-Type","application/json");
    if(!token.isEmpty())
        request.setRawHeader("Authorization",QString("OAuth %1").arg(token).toUtf8());
    if (sslConfig != nullptr)
        request.setSslConfiguration(*sslConfig);

    return request;
}

QNetworkReply* RestConnector::sendCustomRequest(QNetworkAccessManager* manager,
                                            QNetworkRequest &request,
                                            const QString &type,
                                            const QVariantMap &data)
{
    request.setRawHeader("HTTP", type.toUtf8());
    QByteArray postDataByteArray = variantMapToJson(data);
    QBuffer *buff = new QBuffer;
    buff->setData(postDataByteArray);
    buff->open(QIODevice::ReadOnly);
    QNetworkReply* reply =  manager->sendCustomRequest(request, type.toUtf8(), buff);
    buff->setParent(reply);
    return reply;
}

QJsonObject RestConnector::parseReply(QNetworkReply *reply)
{
    QJsonObject jsonObj;
    QJsonDocument jsonDoc;
    QJsonParseError parseError;
    auto replyText = reply->readAll();
    jsonDoc = QJsonDocument::fromJson(replyText, &parseError);
    if(parseError.error != QJsonParseError::NoError){
        qDebug() << replyText;
        qWarning() << "Json parse error: " << parseError.errorString();
    }else{
        if(jsonDoc.isObject())
            jsonObj  = jsonDoc.object();
        else if (jsonDoc.isArray())
            jsonObj["non_field_errors"] = jsonDoc.array();
    }
    return jsonObj;
}

bool RestConnector::onFinishRequest(QNetworkReply *reply)
{
    auto replyError = reply->error();
    if (replyError == QNetworkReply::NoError ) {
        int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if ((code >=200) && (code < 300)) {
            return true;
        }
    }
    return false;
}

void RestConnector::handleQtNetworkErrors(QNetworkReply *reply, QJsonObject &obj)
{
    auto replyError = reply->error();
    if (!(replyError == QNetworkReply::NoError ||
          replyError == QNetworkReply::ContentNotFoundError ||
          replyError == QNetworkReply::ContentAccessDenied ||
          replyError == QNetworkReply::ProtocolInvalidOperationError
          ) ) {
        qDebug() << reply->error();
        obj[KEY_QNETWORK_REPLY_ERROR] = reply->errorString();
    } else if (replyError == QNetworkReply::ContentNotFoundError)
        obj[KEY_CONTENT_NOT_FOUND] = reply->errorString();
}
