#ifndef THETVDB_H
#define THETVDB_H

#include <QObject>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QUrl>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QEventLoop>
#include <QLabel>

class TheTVDB : public QObject
{
    Q_OBJECT

public:
    explicit TheTVDB(QObject *parent = nullptr);

public slots:
    void getTVDBAuth();
    void refreshToken();
    void searchSeries(QString text);
    void getEpisodeByDVDOrder(int row, int id, int season, int episode);
    void getEpisodeByTVOrder(int row, int id, int season, int episode);
    void getEpisodeByAbsoluteNumber(int row, int id, int episode);

private slots:
    void getTVDBAuthFinished(QNetworkReply *reply);
    void refreshTokenFinished(QNetworkReply *reply);
    void searchSeriesFinished(QNetworkReply *reply);
    void getEpisodeByDVDOrderFinished(QNetworkReply *reply);
    void getEpisodeByTVOrderFinished(QNetworkReply *reply);
    void getEpisodeByAbsoluteNumberFinished(QNetworkReply *reply);

signals:
    void sendMessageBox(QString type, QString header, QString text);
    void sendSearchResults(QString id, QString cover, QString name, QString network, QString overview, QString date, QString slug);
    void sendProgBarUpdates(int min, int max, int value);
    void sendUpdatePreview(int row, QString preview, QString status);
    void sendAddLogText(QString text);
    void sendEnableAllButtons();

private:
    QString apikey = "439DFEBA9D3059C6";
    QString mToken;
    int mRow;

};

#endif // THETVDB_H
