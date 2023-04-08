#ifndef EPISODIAC_H
#define EPISODIAC_H

#include <QMainWindow>
#include <QMessageBox>
#include <QUrl>
#include <QProgressBar>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDirIterator>
#include <QDir>
#include <QRegExp>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QFileSystemModel>
#include <QBuffer>
#include <QToolTip>
#include "thetvdb.h"
#include "themoviedb.h"
#include "rename.h"
#include "settings.h"

#include <QDropEvent>
#include <QMimeData>
#include <QScrollBar>

namespace Ui
{
    class Episodiac;
}

class Episodiac : public QMainWindow
{
    Q_OBJECT

public:
    explicit Episodiac(QWidget *parent = 0);
    ~Episodiac();
    void dropEvent(QDropEvent *e);
    void dragEnterEvent(QDragEnterEvent *e);

private slots:
    void saveSettings();
    void loadSettings();
    void addLogText(QString txt);
    void setupSearchResultsTable();
    void setupFileListTable();
    void MessageBox(QString type, QString header, QString text);
    QStringList returnTVShowInfo();
    QString cleanSearch(QString suche);
    QString removeLeadingZero(QString text);
    QString removeCharactersForWindowsFileSys(QString text);
    void addSearchResults(QString id, QString cover, QString name, QString network, QString overview, QString date, QString slug);
    void addFileList(QString path, QString file);
    void updatePreview(int row, QString preview, QString status);
    QByteArray loadImage(QUrl url);
    void getFilesListFromRoot(QString path, bool subdirectories);
    QStringList getRegex(QString filePath, QString fileName);
    void progBarUpdate(int min, int max, int value);
    void on_searchSeries_returnPressed();
    void on_searchSeries_btn_pressed();
    void on_buttonPreview_clicked();
    void on_fileSystem_clicked(const QModelIndex &index);
    void on_chkSubfolders_stateChanged(int arg1);
    void disableAllButtons();
    void enableAllButtons();
    void on_currentPathEdit_returnPressed();
    void setCurrentFilePath(QString cPath);
    void on_buttonRename_clicked();
    void renameFiles();
    void on_checkLogs_clicked();
    void searchResultsClick(int row, int column);

    void on_selectAPI_currentIndexChanged(const QString &arg1);

    void on_actionSettings_triggered();

    void on_actionExit_triggered();

private:
    Ui::Episodiac *ui;
    settings *s;
    QFileSystemModel *dirModel;
    QProgressBar *progBar;
    TheTVDB *tvdb;
    themoviedb *tmdb;
    Rename *rename;
    QString rootPath = "";
    QStringList VideoFilters = (QStringList() << "*.avi" << "*.mp4" << "*.mkv" << "*.wmv" << "*.mov" << "*.m4v" << "*.mpg" << "*.mpeg" << "*.flv" << "*.divx" << "*.xvid");
    QStringList regExExcludeList = (QStringList() << "sample" << "vorschau" << "preview");
    QStringList NotAllowedWindowsCharacters = (QStringList() << "\\" << "/" << ":" << "*" << "?" << "<" << ">" << "|");
    QStringList NotAllowedWindowsFileNames = (QStringList() << "CON" << "PRN" << "AUX" << "NUL" << "COM1" << "COM2" << "COM3" << "COM4" << "COM5" << "COM6" << "COM7" << "COM8" << "COM9" << "LPT1" << "LPT2" << "LPT3" << "LPT4" << "LPT5" << "LPT6" << "LPT7" << "LPT8" << "LPT9");
};

#endif // EPISODIAC_H
