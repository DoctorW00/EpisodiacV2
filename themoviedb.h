#ifndef THEMOVIEDB_H
#define THEMOVIEDB_H

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

class themoviedb : public QObject
{
    Q_OBJECT

public:
    explicit themoviedb(QObject *parent = nullptr);

public slots:
    void searchSeries(QString text);
    void getEpisode(int row, int id, int season, int episode);

private slots:
    void searchSeriesFinished(QNetworkReply *reply);
    void getEpisodeFinished(QNetworkReply *reply);

signals:
    void sendMessageBox(QString type, QString header, QString text);
    void sendSearchResults(QString id, QString cover, QString name, QString network, QString overview, QString date, QString slug);
    void sendProgBarUpdates(int min, int max, int value);
    void sendUpdatePreview(int row, QString preview, QString status);
    void sendAddLogText(QString text);
    void sendEnableAllButtons();

private:
    QString apikey = "f090bb54758cabf231fb605d3e3e0468";
    int mRow;

};

#endif // THEMOVIEDB_H
