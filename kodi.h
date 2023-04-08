#ifndef KODI_H
#define KODI_H

#include <QObject>

class kodi : public QObject
{
    Q_OBJECT

public:
    explicit kodi(QObject *parent = nullptr);

public slots:
    void createNFOShow();
    void createNFOEpisode();

private slots:
    void write2file(QString tmpl, QString txt);
    void showTemplate();
    void episodeTemplate();

};

#endif // KODI_H
