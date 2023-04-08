#ifndef RENAME_H
#define RENAME_H

#include <QObject>
#include <QDir>
#include <QFile>
#include <QFileInfo>

class Rename : public QObject
{
    Q_OBJECT

public:
    explicit Rename(QObject *parent = nullptr);

public slots:
    void renameFile(QString source, QString target, QString targetPath);

private slots:
    bool chkSource(QString source);
    bool chkTarget(QString targetPath);

};

#endif // RENAME_H
