#include "themoviedb.h"

#ifdef QT_DEBUG
    #include <QDebug>
#endif

themoviedb::themoviedb(QObject *parent) : QObject(parent)
{

}

void themoviedb::searchSeries(QString text)
{
    #ifdef QT_DEBUG
        qDebug() << "Searching TMDB for: " << text;
        qDebug() << "apikey: " << apikey;
    #endif

    QNetworkRequest request(QUrl("https://api.themoviedb.org/3/search/tv?api_key=" + apikey + "&language=de-DE&page=1&include_adult=true&query=" + text));

    // request.setRawHeader("Accept", "utf-8, application/json");
    // request.setRawHeader("Accept-Language", "de");
    // request.setRawHeader("Authorization", "Bearer " + mToken.toUtf8());

    QNetworkAccessManager *manager = new QNetworkAccessManager();
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(searchSeriesFinished(QNetworkReply*)));

    manager->get(request);
}

void themoviedb::searchSeriesFinished(QNetworkReply *reply)
{
    if(reply->error())
    {
        if(reply->error() == QNetworkReply::AuthenticationRequiredError)
        {
            #ifdef QT_DEBUG
                qDebug() << "QNetworkReply error: " << reply->errorString();
                qDebug() << "Refreshing TheTVDB.com token ...";
            #endif

            // refreshToken();
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

    QJsonValue agentsArrayValue = getjson.value("results");
    QJsonArray agentsArray = agentsArrayValue.toArray();

    // update progressbar
    int totalSearchResults = agentsArray.count();
    sendProgBarUpdates(0, totalSearchResults, 0);

    int resCNT = 0;

    foreach(const QJsonValue & v, agentsArray)
    {
        QString firstAired = v.toObject().value("first_air_date").toString();
        double vDouble = v.toObject().value("id").toDouble();
        QString id = QString::number(vDouble);
        QString image = v.toObject().value("poster_path").toString();
        QString network = " ";
        QString overview = v.toObject().value("overview").toString();
        QString seriesName = v.toObject().value("name").toString();
        QString slug = v.toObject().value("original_name").toString();

        #ifdef QT_DEBUG
            qDebug() << "firstAired: " << firstAired;
            qDebug() << "id: " << id;
            qDebug() << "image: " << image;
            qDebug() << "network: " << network;
            qDebug() << "overview: " << overview;
            qDebug() << "seriesName: " << seriesName;
            qDebug() << "slug: " << slug;
            qDebug() << " ============================ ";
        #endif

        sendSearchResults(id, image, seriesName, network, overview, firstAired, slug);

        resCNT++;
        sendProgBarUpdates(0, totalSearchResults, resCNT);
    }

    sendEnableAllButtons();
}

void themoviedb::getEpisode(int row, int id, int season, int episode)
{
    mRow = row;

    // QNetworkRequest request(QUrl("https://api.thetvdb.com/series/" + QString::number(id) + "/episodes/query?dvdSeason=" + QString::number(season) + "&dvdEpisode=" + QString::number(episode)));

    QNetworkRequest request(QUrl("https://api.themoviedb.org/3/tv/" + QString::number(id) + "/season/" + QString::number(season) + "/episode/" + QString::number(episode) + "?api_key=" + apikey + "&language=de-DE"));

    // request.setRawHeader("Accept", "utf-8, application/json");
    // request.setRawHeader("Accept-Language", "de");
    // request.setRawHeader("Authorization", "Bearer " + mToken.toUtf8());

    QNetworkAccessManager *manager = new QNetworkAccessManager();
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getEpisodeFinished(QNetworkReply*)));

    // manager->get(request);

    QEventLoop loop;
    manager->connect(manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    manager->get(request);
    loop.exec();
}

void themoviedb::getEpisodeFinished(QNetworkReply *reply)
{
    if(reply->error())
    {
        if(reply->error() == QNetworkReply::AuthenticationRequiredError)
        {
            #ifdef QT_DEBUG
                qDebug() << "QNetworkReply error: " << reply->errorString();
            #endif
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

    #ifdef QT_DEBUG
        qDebug() << "json reply: " << text;
    #endif

    QJsonDocument tvdbJson = QJsonDocument::fromJson(text.toUtf8());
    QJsonObject getjson = tvdbJson.object();

    QString preview;

    QString episodeName = getjson.value("name").toString();

    QString thisSeason = QString::number(getjson.value("season_number").toDouble());
    QString thisEpisodeNumber = QString::number(getjson.value("episode_number").toDouble());

    QString SEASON = QString("%1").arg(thisSeason.toInt(), 2, 10, QChar('0'));
    QString EPISODE = QString("%1").arg(thisEpisodeNumber.toInt(), 2, 10, QChar('0'));

    preview = "S" + SEASON + "E" + EPISODE + " - " + episodeName;


    sendUpdatePreview(mRow, preview, "OK");
}
