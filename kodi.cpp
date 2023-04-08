#include "kodi.h"
#include <QFile>
#include <QTextStream>

#include <QDebug>

kodi::kodi(QObject *parent) : QObject(parent)
{

}

void kodi::createNFOShow()
{
    // showTemplate();
    // episodeTemplate();
}

void kodi::createNFOEpisode()
{

}

void kodi::write2file(QString tmpl, QString txt)
{
    QFile file(tmpl);
    if(file.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        stream << txt;
        stream.flush();
    }

    file.close();
}

// https://kodi.wiki/view/NFO_files/Templates
void kodi::showTemplate()
{
    QString tmpl;

    tmpl    = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\n"
              "<tvshow>\n"
              "    <title>Das ißt ein Täst Öööö @</title>\n"
              "    <plot></plot>\n"
              "    <userrating></userrating>\n"
              "    <mpaa></mpaa>\n"
              "    <uniqueid type=\"\" default=\"true\"></uniqueid>\n"
              "    <genre></genre>\n"
              "    <premiered></premiered>\n"
              "    <status></status>\n"
              "    <studio></studio>\n"
              "    <actor>\n"
              "        <name></name>\n"
              "        <role></role>\n"
              "        <order></order>\n"
              "    </actor>\n"
              "    <namedseason number=\"1\"></namedseason>\n"
              "</tvshow>";

    // qDebug() << tmpl;

    write2file("D:/________________RENAMETEST/Die Simpsons (1989)/show.nfo", tmpl);

}

// https://kodi.wiki/view/NFO_files/Templates
void kodi::episodeTemplate()
{
    QString tmpl;
    tmpl.toUtf8();

    tmpl    = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\n"
              "<episodedetails>\n"
              "    <title>Das ißt ein Täst Öööö @</title>\n"
              "    <season>1</season>\n"
              "    <episode>2</episode>\n"
              "    <userrating></userrating>\n"
              "    <plot></plot>\n"
              "    <uniqueid type=\"\" default=\"true\"></uniqueid>\n"
              "    <credits></credits>\n"
              "    <director></director>\n"
              "    <aired></aired>\n"
              "    <actor>\n"
              "        <name></name>\n"
              "        <role></role>\n"
              "        <order></order>\n"
              "    </actor>\n"
              "</episodedetails>";

    // qDebug() << tmpl;

    write2file("D:/________________RENAMETEST/Die Simpsons (1989)/episode.nfo", tmpl);

}

