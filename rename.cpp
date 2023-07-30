#include "rename.h"

Rename::Rename(QObject *parent) : QObject(parent)
{
}

bool Rename::chkSource(QString source)
{
    QFileInfo s(source);

    if(s.exists() && s.isFile())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Rename::chkTarget(QString targetPath)
{
    QFileInfo t(targetPath);

    if(t.isReadable() && t.isWritable())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Rename::renameFile(QString source, QString target, QString targetPath)
{
    bool doRename = false;

    source = QDir::toNativeSeparators(source);
    target = QDir::toNativeSeparators(target);
    targetPath = QDir::toNativeSeparators(targetPath);

    if(chkSource(source))
    {
        if(chkTarget(targetPath))
        {
            doRename = true;
        }
        else
        {
            QDir t(targetPath);
            if(t.mkpath(targetPath))
            {
                doRename = true;
            }
        }
    }

    if(doRename)
    {
        QFile s(target);

        if(!s.exists())
        {
            QFile::rename(source, target);
        }
    }
}

