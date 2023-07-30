#include "episodiac.h"
#include "ui_episodiac.h"

#include <QDesktopWidget>
#include <QDesktopServices>
#include <QDateTime>
#include <QSettings>

#include "kodi.h"

#include "data.h"

#ifdef QT_DEBUG
    #include <QDebug>
#endif

eData Data;

Episodiac::Episodiac(QWidget *parent) : QMainWindow(parent), ui(new Ui::Episodiac)
{
    ui->setupUi(this);
    setAcceptDrops(true);

    loadSettings();


    // kodi *k = new kodi(this);
    // k->createNFOShow();

    ui->textEdit->setTextInteractionFlags(Qt::TextBrowserInteraction | Qt::LinksAccessibleByMouse);
    ui->textEdit->setProperty("openExternalLinks", true);

    QString mPath = QDir::homePath();

    dirModel = new QFileSystemModel(this);
    dirModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::NoSymLinks);
    dirModel->setRootPath(mPath);

    ui->fileSystem->setModel(dirModel);
    ui->fileSystem->hideColumn(1);
    ui->fileSystem->hideColumn(2);
    ui->fileSystem->hideColumn(3);

    ui->fileSystem->expand(dirModel->index(mPath));
    ui->fileSystem->scrollTo(dirModel->index(mPath));

    ui->currentPathEdit->setText(QDir::toNativeSeparators(mPath));

    setupSearchResultsTable();
    // ui->searchResults->setVisible(false);

    setupFileListTable();

    // ui->filePathButton->setIcon(QIcon(":/icons/selectpath.png"));
    // ui->filePathButton->setIconSize(QSize(25,25));

    ui->searchResults->setStyleSheet("QTableWidget { show-decoration-selected: 1; }");
    ui->searchResults->setStyleSheet("QTableWidget::item:selected { border: 1px solid #ff0000; background-color: yellow; color: black; }");
    connect(ui->searchResults, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(searchResultsClick(int,int)));

    ui->fileSystem->setStyleSheet("QTreeView { show-decoration-selected: 1; }");
    ui->fileSystem->setStyleSheet("QTreeView::item:selected { border: 1px solid #ff0000; background-color: yellow; color: black; }");

    // progBar = new QProgressBar(ui->statusBar);
    progBar = new QProgressBar(this);
    progBar->setVisible(false);
    progBar->setAlignment(Qt::AlignRight);
    progBar->setMinimumWidth(100);
    progBar->setMinimumHeight(10);
    progBar->setAlignment(Qt::AlignCenter);
    // progBar->setStyleSheet("padding: 1px;");

    // ui->statusBar->addWidget(progBar);
    ui->statusBar->addPermanentWidget(progBar);

    // TheTVDB.com API
    tvdb = new TheTVDB(this);
    connect(tvdb, SIGNAL(sendMessageBox(QString,QString,QString)), this, SLOT(MessageBox(QString,QString,QString)));
    connect(tvdb, SIGNAL(sendSearchResults(QString,QString,QString,QString,QString,QString,QString)), this, SLOT(addSearchResults(QString,QString,QString,QString,QString,QString,QString)));
    connect(tvdb, SIGNAL(sendProgBarUpdates(int,int,int)), this, SLOT(progBarUpdate(int,int,int)));
    connect(tvdb, SIGNAL(sendUpdatePreview(int,QString,QString)), this, SLOT(updatePreview(int,QString,QString)));
    connect(tvdb, SIGNAL(sendAddLogText(QString)), this, SLOT(addLogText(QString)));
    connect(tvdb, SIGNAL(sendEnableAllButtons()), this, SLOT(enableAllButtons()));

    // themoviedb.org API
    tmdb = new themoviedb(this);
    connect(tmdb, SIGNAL(sendMessageBox(QString,QString,QString)), this, SLOT(MessageBox(QString,QString,QString)));
    connect(tmdb, SIGNAL(sendSearchResults(QString,QString,QString,QString,QString,QString,QString)), this, SLOT(addSearchResults(QString,QString,QString,QString,QString,QString,QString)));
    connect(tmdb, SIGNAL(sendProgBarUpdates(int,int,int)), this, SLOT(progBarUpdate(int,int,int)));
    connect(tmdb, SIGNAL(sendUpdatePreview(int,QString,QString)), this, SLOT(updatePreview(int,QString,QString)));
    connect(tmdb, SIGNAL(sendAddLogText(QString)), this, SLOT(addLogText(QString)));
    connect(tmdb, SIGNAL(sendEnableAllButtons()), this, SLOT(enableAllButtons()));

    // disable visibility by default
    ui->searchSeriesProgress->setVisible(false);

    rename = new Rename(this);

    Data.apiKey_TMDB = "test12345";
    qDebug() << "main: " << Data.apiKey_TMDB;

    s = new settings(this);

}

Episodiac::~Episodiac()
{
    saveSettings();

    delete ui;
    delete rename;
    delete dirModel;
    delete progBar;
    delete s;
}

void Episodiac::saveSettings()
{
    QSettings config(QSettings::IniFormat, QSettings::UserScope, "EpisodiacV2", "config");

    config.beginGroup("window");
    config.setValue("position", this->geometry());
    config.setValue("maximized", this->isMaximized());
    config.setValue("apiSelector", ui->selectAPI->currentIndex());
    config.endGroup();

    config.beginGroup("splitterLayout");
    config.setValue("splitter1", ui->splitter->saveState());
    config.setValue("splitter2", ui->splitter_2->saveState());
    config.setValue("splitter3", ui->splitter_3->saveState());
    config.setValue("viewLogs", ui->checkLogs->isChecked());
    config.endGroup();
}

void Episodiac::loadSettings()
{
    QSettings config(QSettings::IniFormat, QSettings::UserScope, "EpisodiacV2", "config");

    QDesktopWidget *desktop = QApplication::desktop();

    int screenWidth, width;
    int screenHeight, height;
    int x, y;
    QSize windowSize;

    screenWidth = desktop->width();
    screenHeight = desktop->height();

    windowSize = size();
    width = windowSize.width();
    height = windowSize.height();

    x = (screenWidth - width) / 2;
    y = (screenHeight - height) / 2;
    y -= 30;

    config.beginGroup("window");
    ui->selectAPI->setCurrentIndex(config.value("apiSelector").toInt());

    if(ui->selectAPI->currentText() == "TheTVDB.com")
    {
        ui->tvdbapiSorting->setVisible(true);
    }
    else
    {
        ui->tvdbapiSorting->setVisible(false);
    }

    QVariant maximized = config.value("maximized").value<QString >();

    if(maximized.toString() == "true")
    {
        this->showMaximized();
    }
    else
    {
        QRect thisRect = config.value("position", QRect(QPoint(x,y),QSize(1024,768))).toRect();
        setGeometry(thisRect);
    }
    config.endGroup();

    bool chkLogs;
    config.beginGroup("splitterLayout");
    ui->splitter->restoreState(config.value("splitter1").toByteArray());
    ui->splitter_2->restoreState(config.value("splitter2").toByteArray());
    ui->splitter_3->restoreState(config.value("splitter3").toByteArray());
    chkLogs = config.value("viewLogs").toBool();
    config.endGroup();

    ui->checkLogs->setChecked(chkLogs);
    ui->textEdit->setVisible(chkLogs);
}

void Episodiac::addLogText(QString txt)
{
    QString URLPattern = "[(http(s)?):\\/\\/(www\\.)?a-zA-Z0-9@:%._\\+~#=]{2,256}\\.[a-z]{2,6}\\b([-a-zA-Z0-9@:%_\\+.~#?&//=]*)";
    QRegularExpression re(URLPattern, QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatchIterator i = re.globalMatch(txt);
    while(i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        if(match.hasMatch())
        {  
            QString captured = match.captured(0);
            txt.replace(captured, "<a href=\"" + captured + "\">" + captured + "</a>");
        }
    }

    QString datum = QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm:ss");

    ui->textEdit->moveCursor(QTextCursor::End);
    ui->textEdit->insertHtml("<font color=\"grey\">[" + datum + "]</font> " + txt + "<br />");
    ui->textEdit->verticalScrollBar()->setValue(ui->textEdit->verticalScrollBar()->maximum());
}

void Episodiac::progBarUpdate(int min, int max, int value)
{
    /*
    progBar->setVisible(true);
    progBar->setMaximum(min);
    progBar->setMaximum(max);
    progBar->setValue(value);
    */

    ui->searchSeriesProgress->setVisible(true);
    ui->searchSeriesProgress->setMaximum(min);
    ui->searchSeriesProgress->setMaximum(max);
    ui->searchSeriesProgress->setValue(value);

    if(value == max)
    {
        ui->searchSeriesProgress->setVisible(false);
        ui->searchSeriesProgress->setValue(0);
    }
}

void Episodiac::setupSearchResultsTable()
{
    ui->searchResults->clearContents();
    ui->searchResults->clear();
    ui->searchResults->reset();
    ui->searchResults->setRowCount(0);

    QHeaderView *verticalHeader = ui->searchResults->verticalHeader();
    verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader->setDefaultSectionSize(153);

    ui->searchResults->setColumnCount(6);
    ui->searchResults->setHorizontalHeaderLabels(QStringList()
                                                 << tr("ID")
                                                 << tr("Cover")
                                                 << tr("Info")
                                                 << tr("Name")
                                                 << tr("Year")
                                                 << tr("Slug"));

    ui->searchResults->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    ui->searchResults->setColumnWidth(1, 100);

    ui->searchResults->setColumnHidden(0, true);
    ui->searchResults->setColumnHidden(3, true);
    ui->searchResults->setColumnHidden(4, true);
    ui->searchResults->setColumnHidden(5, true);
}

void Episodiac::setupFileListTable()
{

    ui->fileList->clearContents();
    ui->fileList->clear();
    ui->fileList->reset();
    ui->fileList->setRowCount(0);

    ui->fileList->setColumnCount(7);
    ui->fileList->setHorizontalHeaderLabels(QStringList()
                                            << tr("Path")
                                            << tr("File")
                                            << tr("Season")
                                            << tr("Epsiode")
                                            << tr("Preview")
                                            << tr("Status")
                                            << tr("File Extension"));

    ui->fileList->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->fileList->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->fileList->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    ui->fileList->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    ui->fileList->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    ui->fileList->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
    ui->fileList->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Fixed);

    ui->fileList->setColumnHidden(0, true);
    ui->fileList->setColumnWidth(2, 70);
    ui->fileList->setColumnWidth(3, 70);
    // ui->fileList->setColumnWidth(5, 50);
    ui->fileList->setColumnHidden(5, true);
    ui->fileList->setColumnHidden(6, true);


}

void Episodiac::MessageBox(QString type, QString header, QString text)
{
    if(type == "critical" && !header.isEmpty() && !text.isEmpty())
    {
        QMessageBox::critical(this, header, text, QMessageBox::Ok);

        ui->searchSeriesProgress->setVisible(false);
        ui->searchSeriesProgress->setValue(0);
    }
}

/**
 * @brief Episodiac::removeLeadingZero
 * @param text
 * @return
 *
 * Helper function to remove leading zero
 */
QString Episodiac::removeLeadingZero(QString text)
{
    text.remove(QRegExp("^[0]*"));
    return text;
}

QString Episodiac::returnCleanName(QString text)
{
    QStringList badChars = { "<", ">", ":", "\"", "/", "\\", "|", "?", "*" };

    if(!text.isEmpty())
    {
        for(auto i : badChars)
        {
            if(text.contains(i))
            {
                text.replace(i, "");
            }
        }
    }

    return text;
}

/**
 * @brief Episodiac::removeCharactersForWindowsFileSys
 * @param text
 * @return
 *
 * Helper function to remove all not allowed characters for Windows files
 */
QString Episodiac::removeCharactersForWindowsFileSys(QString text)
{
    foreach(QString c, NotAllowedWindowsCharacters)
    {
        text.replace(c, ""); // remove character from string
    }

    return text;
}

QString Episodiac::cleanSearch(QString suche)
{
    QString badChars = suche;
    badChars.remove(QRegularExpression("[^A-Za-z0-9 _]"));
    return badChars;
}

void Episodiac::disableAllButtons()
{
    ui->centralWidget->setDisabled(true);
}

void Episodiac::enableAllButtons()
{
    ui->centralWidget->setEnabled(true);
}

QStringList Episodiac::returnTVShowInfo()
{
    QStringList showInfo;

    QModelIndexList list = ui->searchResults->selectionModel()->selectedIndexes();

    foreach(const QModelIndex &index, list)
    {
        showInfo.append(index.data(Qt::DisplayRole).toString());
    }

    return showInfo;
}

/**
 * @brief Episodiac::loadImage
 * @param url
 * @return
 *
 * Loader preview cover image from API provider
 */
QByteArray Episodiac::loadImage(QUrl url)
{
    addLogText(tr("Loading image from ") + url.toString());

    QByteArray rResult;

    QEventLoop eventLoop;
    QNetworkAccessManager mgr;

    connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    QNetworkRequest req(QUrl(url.toEncoded(QUrl::FullyEncoded)));
    req.setRawHeader("User-Agent", "Mozilla/5.0 (Windows NT 6.3; Win64; x64; rv:59.0) Gecko/20100101 Firefox/59.0");
    req.setRawHeader("Accept-Language", "de-DE, de;q=0.7, en;q=0.8, fr;q=0.9, *;q=0.5");
    req.setRawHeader("Accept-Charset", "utf-8, iso-8859-1;q=0.5");

    QNetworkReply *reply = mgr.get(req);
    reply->header(QNetworkRequest::ContentLengthHeader).toULongLong();
    eventLoop.exec();

    if(reply->error() == QNetworkReply::NoError)
    {
        rResult = reply->readAll();
    }
    else
    {
        addLogText(tr("Critical download error for ") + url.toString() + "[" + reply->errorString() +"]");
        MessageBox("critical", "Download Error", reply->errorString());

        rResult = QByteArray();
    }

    delete reply;
    return rResult;
}

/**
 * @brief Episodiac::addSearchResults
 * @param id
 * @param cover
 * @param name
 * @param network
 * @param overview
 * @param date
 *
 * Add tv series search results from API provider
 */
void Episodiac::addSearchResults(QString id, QString cover, QString name, QString network, QString overview, QString date, QString slug)
{
    int nRow = ui->searchResults->rowCount();
    ui->searchResults->insertRow(nRow);

    QString year = "0000";
    if(date.split("-").count())
    {
        year = date.split("-").at(0);
    }

    // QTableWidgetItem *ID = new QTableWidgetItem();
    // ID->setData(0, QVariant::fromValue(dataList));

    QTableWidgetItem *ID = new QTableWidgetItem(id);
    ui->searchResults->setItem(nRow, 0, ID);

    // load poster
    if(!cover.isEmpty())
    {
        QString imgPath;

        if(ui->selectAPI->currentText() == "TheTVDB.com")
        {
            imgPath = "https://artworks.thetvdb.com" + cover;
        }

        if(ui->selectAPI->currentText() == "TheMovieDB.org")
        {
            imgPath = "https://image.tmdb.org/t/p/w300" + cover;
        }

        QTableWidgetItem *POSTER = new QTableWidgetItem;

        // load and resize image
        QByteArray imageData = loadImage(QUrl(imgPath));

        // small cover image for table
        QImage CoverImageLoaded = QImage::fromData(imageData);
        QImage CoverImageResized = CoverImageLoaded.scaled(100, 150, Qt::IgnoreAspectRatio);

        // large tooltip cover image
        QImage icon = CoverImageLoaded.scaled(300, 450, Qt::IgnoreAspectRatio);
        QByteArray data;
        QBuffer buffer(&data);
        icon.save(&buffer, "PNG", 100);
        QString toolTipCover;

        if(year.isEmpty())
        {
            toolTipCover = QString(name + "[ID: " + id + "]" + "<br /><img src='data:image/png;base64, %0'>").arg(QString(data.toBase64()));
        }
        else
        {
            toolTipCover = QString(name + " (" + year + ") " + "[ID: " + id + "]" + "<br /><img src='data:image/png;base64, %0'>").arg(QString(data.toBase64()));
        }

        if(CoverImageLoaded.width() && CoverImageLoaded.height())
        {
            POSTER->setData(Qt::DecorationRole, QPixmap::fromImage(CoverImageResized));
            POSTER->setToolTip(toolTipCover);
            ui->searchResults->setItem(nRow, 1, POSTER);
        }
    }
    else
    {
        QTableWidgetItem *POSTER = new QTableWidgetItem(QString());
        ui->searchResults->setItem(nRow, 1, POSTER);
    }

    // QTableWidgetItem *NAME = new QTableWidgetItem(name);
    // ui->searchResults->setItem(nRow, 2, NAME);

    QString text = "<b>" + name + "</b>";

    if(!date.isEmpty())
    {
        text.append(" (" + year + ")");
    }

    // text.append(" [ID: " + id + "]");

    if(!network.isEmpty())
    {
        text.append("<br /><small><b>TV Network: " + network + "</b></small>");
    }

    if(!overview.isEmpty())
    {
        text.append("<br /><small>" + overview + "</small>");
    }

    QLabel *textItem = new QLabel();
    textItem->setText(text);
    textItem->setTextFormat(Qt::RichText);
    textItem->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    textItem->setWordWrap(true);
    textItem->setStyleSheet("padding: 3px; font-size: 16px;");
    // textItem->setStyleSheet("padding: 3px;");
    textItem->adjustSize();
    ui->searchResults->setCellWidget(nRow, 2, textItem);

    // QTableWidgetItem *NAME = new QTableWidgetItem(name + "\n\nRelease date: " + date + "\n\n" + overview);
    // NAME->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
    // ui->searchResults->setItem(nRow, 2, NAME);

    QTableWidgetItem *NAME = new QTableWidgetItem(name);
    ui->searchResults->setItem(nRow, 3, NAME);

    QTableWidgetItem *YEAR = new QTableWidgetItem(year);
    ui->searchResults->setItem(nRow, 4, YEAR);

    QTableWidgetItem *SLUG = new QTableWidgetItem(slug);
    ui->searchResults->setItem(nRow, 5, SLUG);
}

/**
 * @brief Episodiac::addFileList
 * @param path
 * @param file
 *
 * Add tv series files from drive to the table
 */
void Episodiac::addFileList(QString path, QString file)
{
    addLogText(tr("File listing: ") + path + file);

    QString preview;


    int nRow = ui->fileList->rowCount();
    ui->fileList->insertRow(nRow);

    QTableWidgetItem *tPATH = new QTableWidgetItem(path);
    tPATH->setFlags(tPATH->flags() & ~Qt::ItemIsEditable);
    ui->fileList->setItem(nRow, 0, tPATH);

    QTableWidgetItem *tFILE = new QTableWidgetItem(file);
    tFILE->setFlags(tFILE->flags() & ~Qt::ItemIsEditable);
    ui->fileList->setItem(nRow, 1, tFILE);

    QStringList epData = getRegex(path, file);

    QString epSeason;
    QString epEpisode;
    QString epFileExtension;

    if(!epData.isEmpty() && epData.count() == 3)
    {
        epSeason = removeLeadingZero(epData.at(0));
        epEpisode = removeLeadingZero(epData.at(1));
        epFileExtension = epData.at(2);
    }
    else
    {
        addLogText(tr("RegEX error: ") + file);
        preview = tr("RegEX error!");
    }

    QTableWidgetItem *tSEASON = new QTableWidgetItem(epSeason);
    tSEASON->setFlags(tSEASON->flags() & ~Qt::ItemIsEditable);
    tSEASON->setTextAlignment(Qt::AlignCenter);
    ui->fileList->setItem(nRow, 2, tSEASON);

    QTableWidgetItem *tEPISODE = new QTableWidgetItem(epEpisode);
    tEPISODE->setFlags(tEPISODE->flags() & ~Qt::ItemIsEditable);
    tEPISODE->setTextAlignment(Qt::AlignCenter);
    ui->fileList->setItem(nRow, 3, tEPISODE);

    QTableWidgetItem *tPREVIEW = new QTableWidgetItem(preview);
    ui->fileList->setItem(nRow, 4, tPREVIEW);

    QTableWidgetItem *tSTATUS = new QTableWidgetItem(QString());
    tSTATUS->setFlags(tSTATUS->flags() & ~Qt::ItemIsEditable);
    ui->fileList->setItem(nRow, 5, tSTATUS);

    QTableWidgetItem *tFILEEXTENSION = new QTableWidgetItem(epFileExtension);
    tFILEEXTENSION->setFlags(tFILEEXTENSION->flags() & ~Qt::ItemIsEditable);
    ui->fileList->setItem(nRow, 6, tFILEEXTENSION);

}

/**
 * @brief Episodiac::updatePreview
 * @param row
 * @param preview
 *
 * Update file rename preview
 */
void Episodiac::updatePreview(int row, QString preview, QString status)
{
    // QTableWidgetItem *tPREVIEW = new QTableWidgetItem(preview);
    // ui->fileList->setItem(row, 4, tPREVIEW);

    /*
    if(preview.isEmpty() || preview == " " || status != "OK")
    {
        int season = ui->fileList->item(row, 2)->text().toInt();
        int episode = ui->fileList->item(row, 3)->text().toInt();

        tvdb->getEpisodeByTVOrder(row, 73255, season, episode);

        return;
    }
    */

    if(status == "OK")
    {
        ui->fileList->item(row, 1)->setData(Qt::BackgroundRole, QColor(215,255,175));
        ui->fileList->item(row, 2)->setData(Qt::BackgroundRole, QColor(215,255,175));
        ui->fileList->item(row, 3)->setData(Qt::BackgroundRole, QColor(215,255,175));
        ui->fileList->item(row, 4)->setData(Qt::BackgroundRole, QColor(215,255,175));
        ui->fileList->item(row, 5)->setData(Qt::BackgroundRole, QColor(215,255,175));
    }
    else
    {
        ui->fileList->item(row, 1)->setData(Qt::BackgroundRole, QColor (250,150,150));
        ui->fileList->item(row, 2)->setData(Qt::BackgroundRole, QColor (250,150,150));
        ui->fileList->item(row, 3)->setData(Qt::BackgroundRole, QColor (250,150,150));
        ui->fileList->item(row, 4)->setData(Qt::BackgroundRole, QColor (250,150,150));
        ui->fileList->item(row, 5)->setData(Qt::BackgroundRole, QColor (250,150,150));
    }

    // add file extension to the preview
    // 6 = hidden table file extension

    // get file extension for sure this time
    QString fileExt = ui->fileList->item(row, 6)->text().split('.').last();

    preview = preview + "." + fileExt;

    ui->fileList->item(row, 4)->setText(removeCharactersForWindowsFileSys(preview));
    ui->fileList->item(row, 5)->setText(status);

    ui->fileList->scrollToItem(ui->fileList->item(row, 1));
}

void Episodiac::getFilesListFromRoot(QString path, bool subdirectories = false)
{
    if(subdirectories)
    {
        QString regExExclude = regExExcludeList.join("|");

        int cnt = 0;
        QDirIterator sub(path, VideoFilters, QDir::Files, QDirIterator::Subdirectories);
        while(sub.hasNext())
        {
            addLogText(tr("Recursive paths: ") + sub.next());

            QRegularExpression de(regExExclude, QRegularExpression::OptimizeOnFirstUsageOption);
            QRegularExpressionMatch match = de.match(sub.fileName());
            if(!match.hasMatch())
            {
                // addFileList(sub.fileInfo().absoluteFilePath(), sub.fileName());
                addFileList(sub.fileInfo().path(), sub.fileName());
                addLogText(tr("Recursive paths file: ") + sub.fileInfo().absoluteFilePath());

                cnt++;
            }
        }

        return;
    }


    QDir root(path);
    root.setFilter(QDir::Files | QDir::NoSymLinks);
    root.setNameFilters(VideoFilters);

    QFileInfoList list = root.entryInfoList();

    foreach (QFileInfo finfo, list)
    {
        // qDebug() << path << finfo.fileName();

        addFileList(path, finfo.fileName());
    }
}

/**
 * @brief Episodiac::getRegex
 * @param filePath
 * @param fileName
 * @return
 *
 * Try to find season and episode numbers in a filename
 */
QStringList Episodiac::getRegex(QString filePath, QString fileName)
{
    QStringList rReturn;

    // fileName.replace("_", "");

    QFileInfo file(filePath + "/" + fileName);
    QString ext = file.completeSuffix();

    #ifdef QT_DEBUG
        qDebug() << "filePath: " << filePath;
        qDebug() << "fileName: " << fileName;
        qDebug() << "ext: " << ext;
    #endif

    QString eRegEx = "\\b(?!\\d{3,4}p)(?:[a-zA-Z_]*)(?:[sS])?(\\d{1,2})(?:[xX]|[eE])?(\\d{1,2})(?:[a-zA-Z_]*)\\b";

    QRegularExpression re(eRegEx, QRegularExpression::MultilineOption | QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatch match = re.match(fileName);
    if(!re.isValid())
    {
        addLogText(tr("RegEX is invalid for: ") + fileName);
    }

    if(match.hasMatch())
    {
        QString s_staffel = match.captured(1);
        QString s_episode = match.captured(2);

        #ifdef QT_DEBUG
            qDebug() << "s_staffel: " << s_staffel;
            qDebug() << "s_episode: " << s_episode;
        #endif

        addLogText(tr("RegEX has match for: ") + fileName + s_staffel + s_episode);

        rReturn.append(s_staffel);
        rReturn.append(s_episode);
        rReturn.append(ext);

        return rReturn;
    }

    return QStringList();
}

/**
 * @brief Episodiac::on_searchSeries_returnPressed
 * Search for tv series using the selected api provider
 */
void Episodiac::on_searchSeries_returnPressed()
{
    QString text = cleanSearch(ui->searchSeries->text());

    if(text.isEmpty())
    {
        return;
    }

    setupSearchResultsTable();
    ui->searchResults->setVisible(true);
    ui->searchSeriesProgress->setVisible(true);
    ui->searchSeries->selectAll();
    ui->searchSeries->setFocus();

    disableAllButtons();

    if(ui->selectAPI->currentText() == "TheTVDB.com")
    {
        tvdb->searchSeries(text);
    }

    if(ui->selectAPI->currentText() == "TheMovieDB.org")
    {
        tmdb->searchSeries(text);
    }
}

void Episodiac::on_searchSeries_btn_pressed()
{
    on_searchSeries_returnPressed();
}

void Episodiac::on_buttonPreview_clicked()
{
    int rowCount = ui->fileList->rowCount();

    QStringList showInfo = returnTVShowInfo();
    if(!showInfo.count())
    {
        MessageBox("critical", tr("Select Show"), tr("Search and select a show first!"));
        return;
    }

    int showID = showInfo.at(0).toInt();

    disableAllButtons();

    for(int i = 0; i < rowCount; i++)
    {
        progBarUpdate(0, rowCount, i + 1);

        int season = ui->fileList->item(i, 2)->text().toInt();
        int episode = ui->fileList->item(i, 3)->text().toInt();

        if(ui->selectAPI->currentText() == "TheTVDB.com")
        {
            tvdb->getEpisodeByDVDOrder(i, showID, season, episode);
        }

        if(ui->selectAPI->currentText() == "TheMovieDB.org")
        {
            tmdb->getEpisode(i, showID, season, episode);
        }
    }

    enableAllButtons();
}

void Episodiac::on_fileSystem_clicked(const QModelIndex &index)
{
    setupFileListTable();

    QString mPath = dirModel->fileInfo(index).absoluteFilePath();
    rootPath = mPath;

    addLogText(tr("Root path selected: ") + rootPath);

    ui->currentPathEdit->setText(QDir::toNativeSeparators(rootPath));

    bool subfolders = ui->chkSubfolders->isChecked();
    getFilesListFromRoot(rootPath, subfolders);
}

void Episodiac::on_chkSubfolders_stateChanged(int arg1)
{
    Q_UNUSED(arg1);

    setupFileListTable();

    bool subfolders = ui->chkSubfolders->isChecked();
    getFilesListFromRoot(rootPath, subfolders);
}


void Episodiac::on_currentPathEdit_returnPressed()
{
    QString pathFromText = QDir::fromNativeSeparators(ui->currentPathEdit->text());

    if(!pathFromText.isEmpty())
    {
        setCurrentFilePath(pathFromText);
    }
}

void Episodiac::setCurrentFilePath(QString cPath)
{
    setupFileListTable();

    QString pathFromText = QDir::fromNativeSeparators(cPath);

    if(!pathFromText.isEmpty())
    {
        QDir dir(pathFromText);
        if(dir.exists() && dir.isReadable())
        {
            rootPath = pathFromText;
            dirModel->setRootPath(rootPath);

            ui->currentPathEdit->setText(QDir::toNativeSeparators(rootPath));

            ui->fileSystem->collapseAll();
            ui->fileSystem->setCurrentIndex(dirModel->index(rootPath));
            ui->fileSystem->expand(dirModel->index(rootPath));
            ui->fileSystem->scrollTo(dirModel->index(rootPath));

            bool subfolders = ui->chkSubfolders->isChecked();
            getFilesListFromRoot(rootPath, subfolders);
        }
    }
}


void Episodiac::dragEnterEvent(QDragEnterEvent *e)
{
    e->acceptProposedAction();
}

void Episodiac::dropEvent(QDropEvent *e)
{
    // QString pwd = e->mimeData()->text();
    // qDebug() << "Drop Event: " << pwd;

    if(!e->mimeData()->text().isEmpty())
    {
        QDir dir(e->mimeData()->urls().at(0).toLocalFile());
        if(dir.exists() && dir.isReadable())
        {
            QString p = dir.absoluteFilePath(e->mimeData()->urls().at(0).toLocalFile());
            setCurrentFilePath(p);
        }
    }

    e->acceptProposedAction();
}

void Episodiac::on_buttonRename_clicked()
{
    renameFiles();
}

void Episodiac::renameFiles()
{
    int rowCount = ui->fileList->rowCount();

    QStringList showInfo = returnTVShowInfo();
    if(!showInfo.count())
    {
        MessageBox("critical", tr("Select Show"), tr("Search and select a show first!"));
        return;
    }

    QString showName = returnCleanName(showInfo.at(3));
    QString showYear = showInfo.at(4);

    disableAllButtons();

    for(int i = 0; i < rowCount; i++)
    {
        progBarUpdate(0, rowCount, i + 1);

        QString season = ui->fileList->item(i, 2)->text();
        QString path = ui->fileList->item(i, 0)->text();
        QString file = ui->fileList->item(i, 1)->text();
        QString preview = returnCleanName(ui->fileList->item(i, 4)->text());

        if(!path.endsWith("/"))
        {
            path.append("/");
        }

        QString source = path + file;
        QString target = path + showName + " (" + showYear + ")/Season " + season + "/" + preview;
        QString targetPath = path + showName + " (" + showYear + ")/Season " + season + "/";

        addLogText(tr("Renaming file from [") + source + tr("] to [") + target + tr("] in ") + targetPath);

        rename->renameFile(source, target, targetPath);
    }

    enableAllButtons();
}

// view/hide logs
void Episodiac::on_checkLogs_clicked()
{
    if(ui->checkLogs->isChecked())
    {
        ui->textEdit->setVisible(true);
    }
    else
    {
        ui->textEdit->setVisible(false);
    }
}

// open tv show in desktop browser
void Episodiac::searchResultsClick(int row, int column)
{
    Q_UNUSED(column);

    QString slug;
    QString link;

    if(ui->selectAPI->currentText() == "TheTVDB.com")
    {
        slug = ui->searchResults->item(row, 5)->text();
        link = "https://thetvdb.com/series/" + slug;
    }

    if(ui->selectAPI->currentText() == "TheMovieDB.org")
    {
        slug = ui->searchResults->item(row, 0)->text();
        link = "https://www.themoviedb.org/tv/" + slug;
    }

    QDesktopServices::openUrl(QUrl(link));

}

void Episodiac::on_selectAPI_currentIndexChanged(const QString &arg1)
{
    if(arg1 == "TheTVDB.com")
    {
        setupSearchResultsTable();
        ui->tvdbapiSorting->setVisible(true);
        ui->searchSeries->selectAll();
        ui->searchSeries->setFocus();
    }

    if(arg1 == "TheMovieDB.org")
    {
        setupSearchResultsTable();
        ui->tvdbapiSorting->setVisible(false);
        ui->searchSeries->selectAll();
        ui->searchSeries->setFocus();
    }
}

// open settings
void Episodiac::on_actionSettings_triggered()
{
    // auto *s = new settings(this);
    s->setWindowTitle("EpisodiacV2 Settings");
    s->setWindowFlags(s->windowFlags() ^ Qt::WindowContextHelpButtonHint);
    s->show();
}

// quit app
void Episodiac::on_actionExit_triggered()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "EpisodiacV2", tr("Are you sure you want to quit?"), QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        qApp->quit();
    }
}
