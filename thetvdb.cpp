#include "thetvdb.h"

#ifdef QT_DEBUG
    #include <QDebug>
#endif

TheTVDB::TheTVDB(QObject *parent) : QObject(parent)
{

}

void TheTVDB::getTVDBAuth()
{
    QJsonObject obj;
    obj.insert("apikey", apikey);

    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    QString jsonString(data);

    QNetworkRequest request(QUrl("https://api.thetvdb.com/login"));

    sendAddLogText(tr("Get auth from TheTVDB.com API using key: ") + apikey);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "utf-8, application/json");

    QNetworkAccessManager *manager = new QNetworkAccessManager();
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getTVDBAuthFinished(QNetworkReply*)));

    QEventLoop loop;
    manager->connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    manager->post(request, jsonString.toUtf8());
    loop.exec();
}

void TheTVDB::getTVDBAuthFinished(QNetworkReply *reply)
{
    if(reply->error())
    {
        sendMessageBox("critical", tr("TheTVDB.com API Fehler"), reply->errorString());
        sendAddLogText(tr("Critical TheTVDB.com Error: ") + reply->errorString());

        #ifdef QT_DEBUG
            qDebug() << "QNetworkReply error: " << reply->errorString();
        #endif

        reply->deleteLater();
        return;
    }

    QString text = reply->readAll();

    QJsonDocument tvdbJson = QJsonDocument::fromJson(text.toUtf8());
    QJsonObject getjson = tvdbJson.object();

    QString token = getjson.value("token").toString();

    if(!token.isEmpty())
    {
        mToken = token;
    }

    sendAddLogText(tr("New TheTVDB.com token: ") + mToken);

    #ifdef QT_DEBUG
        qDebug() << "New TheTVDB.com token: " << mToken;
    #endif
}

void TheTVDB::refreshToken()
{
    if(mToken.isEmpty())
    {
        getTVDBAuth();
        return;
    }

    sendAddLogText(tr("Refreshing TheTVDB.com token ..."));

    QNetworkRequest request(QUrl("https://api.thetvdb.com/refresh_token"));

    request.setRawHeader("Accept", "utf-8, application/json");
    request.setRawHeader("Authorization", "Bearer " + mToken.toUtf8());

    QNetworkAccessManager *manager = new QNetworkAccessManager();
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(refreshTokenFinished(QNetworkReply*)));

    QEventLoop loop;
    manager->connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    manager->get(request);
    loop.exec();
}

void TheTVDB::refreshTokenFinished(QNetworkReply *reply)
{
    if(reply->error())
    {
        if(reply->error() == QNetworkReply::AuthenticationRequiredError)
        {
            #ifdef QT_DEBUG
                qDebug() << "QNetworkReply error: " << reply->errorString();
                qDebug() << "TheTVDB.com LogIn ...";
            #endif

            getTVDBAuth();
        }
        else
        {
            sendMessageBox("critical", tr("TheTVDB.com API Fehler"), reply->errorString());

            #ifdef QT_DEBUG
                qDebug() << "QNetworkReply error: " << reply->errorString();
            #endif
        }

        reply->deleteLater();
        return;
    }

    QString text = reply->readAll();

    QJsonDocument tvdbJson = QJsonDocument::fromJson(text.toUtf8());
    QJsonObject getjson = tvdbJson.object();

    QString token = getjson.value("token").toString();

    if(!token.isEmpty())
    {
        mToken = token;
    }

    sendAddLogText(tr("Refreshed TheTVDB.com token: ") + mToken);

    #ifdef QT_DEBUG
        qDebug() << "Refreshed TheTVDB.com token: " << mToken;
    #endif
}

void TheTVDB::searchSeries(QString text)
{
    if(mToken.isEmpty())
    {
        #ifdef QT_DEBUG
            qDebug() << "Empty TheTVDB.com token, starting LogIn ...";
        #endif

        getTVDBAuth();
    }

    QNetworkRequest request(QUrl("https://api.thetvdb.com/search/series?name=" + text));

    request.setRawHeader("Accept", "utf-8, application/json");
    request.setRawHeader("Accept-Language", "de");
    request.setRawHeader("Authorization", "Bearer " + mToken.toUtf8());

    QNetworkAccessManager *manager = new QNetworkAccessManager();
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(searchSeriesFinished(QNetworkReply*)));

    manager->get(request);
}

void TheTVDB::searchSeriesFinished(QNetworkReply *reply)
{
    if(reply->error())
    {
        if(reply->error() == QNetworkReply::AuthenticationRequiredError)
        {
            #ifdef QT_DEBUG
                qDebug() << "QNetworkReply error: " << reply->errorString();
                qDebug() << "Refreshing TheTVDB.com token ...";
            #endif

            refreshToken();
        }
        else
        {
            sendMessageBox("critical", tr("TheTVDB.com API Fehler"), reply->errorString());

            #ifdef QT_DEBUG
                qDebug() << "QNetworkReply error: " << reply->errorString();
            #endif
        }

        reply->deleteLater();
        return;
    }

    QString text = reply->readAll();

    QJsonDocument tvdbJson = QJsonDocument::fromJson(text.toUtf8());
    QJsonObject getjson = tvdbJson.object();

    QJsonValue agentsArrayValue = getjson.value("data");
    QJsonArray agentsArray = agentsArrayValue.toArray();

    // update progressbar
    int totalSearchResults = agentsArray.count();
    sendProgBarUpdates(0, totalSearchResults, 0);

    int resCNT = 0;

    foreach(const QJsonValue & v, agentsArray)
    {
        QStringList aliases;
        QJsonValue agentsArrayValue2 = v.toObject().value("aliases").toArray();
        QJsonArray agentsArray2 = agentsArrayValue2.toArray();
        if(!agentsArray2.isEmpty())
        {
            foreach(const QJsonValue & v2, agentsArray2)
            {
                aliases.append(v2.toString());
            }
        }

        QString banner = v.toObject().value("banner").toString();
        QString firstAired = v.toObject().value("firstAired").toString();
        double vDouble = v.toObject().value("id").toDouble();
        QString id = QString::number(vDouble);
        QString image = v.toObject().value("image").toString();
        QString network = v.toObject().value("network").toString();
        QString overview = v.toObject().value("overview").toString();
        QString poster = v.toObject().value("poster").toString();
        QString seriesName = v.toObject().value("seriesName").toString();
        QString slug = v.toObject().value("slug").toString();
        QString status = v.toObject().value("status").toString();

        #ifdef QT_DEBUG
            qDebug() << "aliases: " << aliases;
            qDebug() << "banner: " << banner;
            qDebug() << "firstAired: " << firstAired;
            qDebug() << "id: " << id;
            qDebug() << "image: " << image;
            qDebug() << "network: " << network;
            qDebug() << "overview: " << overview;
            qDebug() << "poster: " << poster;
            qDebug() << "seriesName: " << seriesName;
            qDebug() << "slug: " << slug;
            qDebug() << "status: " << status;
            qDebug() << " ============================ ";
        #endif

        sendSearchResults(id, image, seriesName, network, overview, firstAired, slug);

        resCNT++;
        sendProgBarUpdates(0, totalSearchResults, resCNT);
    }

    sendEnableAllButtons();
}

void TheTVDB::getEpisodeByDVDOrder(int row, int id, int season, int episode)
{
    if(mToken.isEmpty())
    {
        getTVDBAuth();
    }

    mRow = row;

    QNetworkRequest request(QUrl("https://api.thetvdb.com/series/" + QString::number(id) + "/episodes/query?dvdSeason=" + QString::number(season) + "&dvdEpisode=" + QString::number(episode)));

    request.setRawHeader("Accept", "utf-8, application/json");
    request.setRawHeader("Accept-Language", "de");
    request.setRawHeader("Authorization", "Bearer " + mToken.toUtf8());

    QNetworkAccessManager *manager = new QNetworkAccessManager();
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getEpisodeByDVDOrderFinished(QNetworkReply*)));

    // manager->get(request);

    QEventLoop loop;
    manager->connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    manager->get(request);
    loop.exec();
}

void TheTVDB::getEpisodeByDVDOrderFinished(QNetworkReply *reply)
{
    if(reply->error())
    {
        if(reply->error() == QNetworkReply::AuthenticationRequiredError)
        {
            #ifdef QT_DEBUG
                qDebug() << "QNetworkReply error: " << reply->errorString();
                qDebug() << "Refreshing TheTVDB.com token ...";
            #endif

            refreshToken();
        }
        else
        {
            // sendMessageBox("critical", tr("TheTVDB.com API Fehler"), reply->errorString());

            #ifdef QT_DEBUG
                qDebug() << "QNetworkReply error: " << reply->errorString();
            #endif
        }

        reply->deleteLater();

        // sendUpdatePreview(mRow, " ", reply->errorString());
        sendUpdatePreview(mRow, reply->errorString(), "ERR");

        return;
    }

    QString text = reply->readAll();

    QJsonDocument tvdbJson = QJsonDocument::fromJson(text.toUtf8());
    QJsonObject getjson = tvdbJson.object();

    QJsonValue agentsArrayValue = getjson.value("data");
    QJsonArray agentsArray = agentsArrayValue.toArray();

    QString preview;

    foreach(const QJsonValue & v, agentsArray)
    {
        QString episodeName = v.toObject().value("episodeName").toString();

        // QString airedSeason = QString::number(v.toObject().value("airedSeason").toDouble());
        // QString airedEpisodeNumber = QString::number(v.toObject().value("airedEpisodeNumber").toDouble());
        QString dvdSeason = QString::number(v.toObject().value("dvdSeason").toDouble());
        QString dvdEpisodeNumber = QString::number(v.toObject().value("dvdEpisodeNumber").toDouble());

        QString SEASON = QString("%1").arg(dvdSeason.toInt(), 2, 10, QChar('0'));
        QString EPISODE = QString("%1").arg(dvdEpisodeNumber.toInt(), 2, 10, QChar('0'));

        preview = "S" + SEASON + "E" + EPISODE + " - " + episodeName;

        /*
        #ifdef QT_DEBUG
            qDebug() << "episodeName: " << episodeName;
            qDebug() << "airedSeason: " << airedSeason;
            qDebug() << "airedEpisodeNumber: " << airedEpisodeNumber;
            qDebug() << "dvdSeason: " << dvdSeason;
            qDebug() << "dvdEpisodeNumber: " << dvdEpisodeNumber;
            qDebug() << " ============================ ";
        #endif
        */
    }

    sendUpdatePreview(mRow, preview, "OK");
}

void TheTVDB::getEpisodeByTVOrder(int row, int id, int season, int episode)
{
    if(mToken.isEmpty())
    {
        getTVDBAuth();
    }

    mRow = row;

    QNetworkRequest request(QUrl("https://api.thetvdb.com/series/" + QString::number(id) + "/episodes/query?airedSeason=" + QString::number(season) + "&airedEpisode=" + QString::number(episode)));

    request.setRawHeader("Accept", "utf-8, application/json");
    request.setRawHeader("Accept-Language", "de");
    request.setRawHeader("Authorization", "Bearer " + mToken.toUtf8());

    QNetworkAccessManager *manager = new QNetworkAccessManager();
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getEpisodeByTVOrderFinished(QNetworkReply*)));

    // manager->get(request);

    QEventLoop loop;
    manager->connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    manager->get(request);
    loop.exec();
}

void TheTVDB::getEpisodeByTVOrderFinished(QNetworkReply *reply)
{
    if(reply->error())
    {
        if(reply->error() == QNetworkReply::AuthenticationRequiredError)
        {
            #ifdef QT_DEBUG
                qDebug() << "QNetworkReply error: " << reply->errorString();
                qDebug() << "Refreshing TheTVDB.com token ...";
            #endif

            refreshToken();
        }
        else
        {
            // sendMessageBox("critical", tr("TheTVDB.com API Fehler"), reply->errorString());

            #ifdef QT_DEBUG
                qDebug() << "QNetworkReply error: " << reply->errorString();
            #endif
        }

        reply->deleteLater();

        sendUpdatePreview(mRow, reply->errorString(), "ERR");

        return;
    }

    QString text = reply->readAll();

    QJsonDocument tvdbJson = QJsonDocument::fromJson(text.toUtf8());
    QJsonObject getjson = tvdbJson.object();

    QJsonValue agentsArrayValue = getjson.value("data");
    QJsonArray agentsArray = agentsArrayValue.toArray();

    QString preview;

    foreach(const QJsonValue & v, agentsArray)
    {
        QString episodeName = v.toObject().value("episodeName").toString();

        QString airedSeason = QString::number(v.toObject().value("airedSeason").toDouble());
        QString airedEpisodeNumber = QString::number(v.toObject().value("airedEpisodeNumber").toDouble());
        // QString dvdSeason = QString::number(v.toObject().value("dvdSeason").toDouble());
        // QString dvdEpisodeNumber = QString::number(v.toObject().value("dvdEpisodeNumber").toDouble());

        QString SEASON = QString("%1").arg(airedSeason.toInt(), 2, 10, QChar('0'));
        QString EPISODE = QString("%1").arg(airedEpisodeNumber.toInt(), 2, 10, QChar('0'));

        preview = "S" + SEASON + "E" + EPISODE + " - " + episodeName;

        /*
        #ifdef QT_DEBUG
            qDebug() << "episodeName: " << episodeName;
            qDebug() << "airedSeason: " << airedSeason;
            qDebug() << "airedEpisodeNumber: " << airedEpisodeNumber;
            qDebug() << "dvdSeason: " << dvdSeason;
            qDebug() << "dvdEpisodeNumber: " << dvdEpisodeNumber;
            qDebug() << " ============================ ";
        #endif
        */
    }

    sendUpdatePreview(mRow, preview, "OK");
}

void TheTVDB::getEpisodeByAbsoluteNumber(int row, int id, int episode)
{
    if(mToken.isEmpty())
    {
        getTVDBAuth();
    }

    mRow = row;

    QNetworkRequest request(QUrl("https://api.thetvdb.com/series/" + QString::number(id) + "/episodes/query?absoluteNumber=" + QString::number(episode)));

    request.setRawHeader("Accept", "utf-8, application/json");
    request.setRawHeader("Accept-Language", "de");
    request.setRawHeader("Authorization", "Bearer " + mToken.toUtf8());

    QNetworkAccessManager *manager = new QNetworkAccessManager();
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getEpisodeByTVOrderFinished(QNetworkReply*)));

    // manager->get(request);

    QEventLoop loop;
    manager->connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    manager->get(request);
    loop.exec();
}

void TheTVDB::getEpisodeByAbsoluteNumberFinished(QNetworkReply *reply)
{
    if(reply->error())
    {
        if(reply->error() == QNetworkReply::AuthenticationRequiredError)
        {
            #ifdef QT_DEBUG
                qDebug() << "QNetworkReply error: " << reply->errorString();
                qDebug() << "Refreshing TheTVDB.com token ...";
            #endif

            refreshToken();
        }
        else
        {
            // sendMessageBox("critical", tr("TheTVDB.com API Fehler"), reply->errorString());

            #ifdef QT_DEBUG
                qDebug() << "QNetworkReply error: " << reply->errorString();
            #endif
        }

        reply->deleteLater();

        sendUpdatePreview(mRow, reply->errorString(), "ERR");

        return;
    }

    QString text = reply->readAll();

    QJsonDocument tvdbJson = QJsonDocument::fromJson(text.toUtf8());
    QJsonObject getjson = tvdbJson.object();

    QJsonValue agentsArrayValue = getjson.value("data");
    QJsonArray agentsArray = agentsArrayValue.toArray();

    QString preview;

    foreach(const QJsonValue & v, agentsArray)
    {
        QString episodeName = v.toObject().value("episodeName").toString();

        // QString airedSeason = QString::number(v.toObject().value("airedSeason").toDouble());
        // QString airedEpisodeNumber = QString::number(v.toObject().value("airedEpisodeNumber").toDouble());
        QString dvdSeason = QString::number(v.toObject().value("dvdSeason").toDouble());
        QString dvdEpisodeNumber = QString::number(v.toObject().value("dvdEpisodeNumber").toDouble());

        QString SEASON = QString("%1").arg(dvdSeason.toInt(), 2, 10, QChar('0'));
        QString EPISODE = QString("%1").arg(dvdEpisodeNumber.toInt(), 2, 10, QChar('0'));

        preview = "S" + SEASON + "E" + EPISODE + " - " + episodeName;

        /*
        #ifdef QT_DEBUG
            qDebug() << "episodeName: " << episodeName;
            qDebug() << "airedSeason: " << airedSeason;
            qDebug() << "airedEpisodeNumber: " << airedEpisodeNumber;
            qDebug() << "dvdSeason: " << dvdSeason;
            qDebug() << "dvdEpisodeNumber: " << dvdEpisodeNumber;
            qDebug() << " ============================ ";
        #endif
        */
    }

    sendUpdatePreview(mRow, preview, "OK");
}
