#ifndef DATA_H
#define DATA_H

#include <QString>
#include <QStringList>
#include <QMetaType>

struct eData
{

    QString apiKey_TMDB = "";
    QString apiKey_TVDB = "";

    QStringList list_regex = QStringList();

};

Q_DECLARE_METATYPE(eData)

#endif // DATA_H
