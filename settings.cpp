#include "settings.h"
#include "data.h"
#include "ui_settings.h"
#include <QRegularExpression>
#include <QDesktopServices>
#include <QUrl>

#ifdef QT_DEBUG
    #include <QDebug>
#endif

extern eData Data;

settings::settings(QWidget *parent) : QDialog(parent), ui(new Ui::settings)
{
    // qRegisterMetaTypeStreamOperators<eData>("eData");
    // qRegisterMetaType<eData>("eData");

    qDebug() << "apiKey_TMDB: " << Data.apiKey_TMDB;

    ui->setupUi(this);

    // themoviedb.org
    ui->label_3->setText("<a href=\"https://developers.themoviedb.org/3/getting-started/introduction\">https://developers.themoviedb.org/3/getting-started/introduction</a>");
    ui->label_3->setTextFormat(Qt::RichText);
    ui->label_3->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->label_3->setOpenExternalLinks(true);

    // thetvdb.com
    ui->label_4->setText("<a href=\"https://thetvdb.github.io/v4-api/\">https://thetvdb.github.io/v4-api/</a>");
    ui->label_4->setTextFormat(Qt::RichText);
    ui->label_4->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->label_4->setOpenExternalLinks(true);

    ui->line_editRegEX->setDisabled(true);
    ui->push_saveRegEX->setDisabled(true);
}

settings::~settings()
{
    delete ui;
}

// check valid regex
bool settings::isValidRegEX(QString regEX)
{
    QRegularExpression re(regEX);

    if(re.isValid())
    {
        return true;
    }

    return false;
}


// themoviedb.org
void settings::on_label_3_linkActivated(const QString &link)
{
    QDesktopServices::openUrl(QUrl(link));
}

// thetvdb.com
void settings::on_label_4_linkActivated(const QString &link)
{
    QDesktopServices::openUrl(QUrl(link));
}

// add new regex
void settings::on_line_newRegEX_returnPressed()
{
    on_push_newRegEX_clicked();
}

// add new regex
void settings::on_push_newRegEX_clicked()
{
    ui->line_newRegEX->setStyleSheet("");
    QString newRegEX = ui->line_newRegEX->text();

    if(!newRegEX.isEmpty())
    {
        qDebug() << newRegEX;


        if(isValidRegEX(newRegEX))
        {
            ui->list_RegEX->addItem(newRegEX);

            ui->line_newRegEX->clear();
            ui->line_newRegEX->setFocus();
        }
        else
        {
            ui->line_newRegEX->setStyleSheet("color: white; background-color: red; selection-color: white; selection-background-color: red;");
            ui->line_newRegEX->setFocus();
            ui->line_newRegEX->selectAll();
        }
    }

}

// delete regex item(s)
void settings::on_push_deleteRegEX_clicked()
{
    ui->line_editRegEX->clear();
    ui->line_editRegEX->setDisabled(true);
    ui->push_saveRegEX->setDisabled(true);

    QList<QListWidgetItem*> items = ui->list_RegEX->selectedItems();
    foreach(QListWidgetItem * item, items)
    {
        delete ui->list_RegEX->takeItem(ui->list_RegEX->row(item));
    }
}

// edit regex item
void settings::on_list_RegEX_clicked(const QModelIndex &index)
{
    Q_UNUSED(index);

    QString regexItem = ui->list_RegEX->currentItem()->text();

    if(!regexItem.isEmpty())
    {
        ui->line_editRegEX->setText(regexItem);

        ui->line_editRegEX->setEnabled(true);
        ui->push_saveRegEX->setEnabled(true);
    }
}

// save changes to regex item
void settings::on_push_saveRegEX_clicked()
{
    ui->line_editRegEX->setStyleSheet("");
    QString regexItem = ui->line_editRegEX->text();

    if(!regexItem.isEmpty())
    {
        if(isValidRegEX(regexItem))
        {
            ui->list_RegEX->currentItem()->setText(regexItem);
        }
        else
        {
            ui->line_editRegEX->setStyleSheet("color: white; background-color: red; selection-color: white; selection-background-color: red;");
            ui->line_editRegEX->setFocus();
            ui->line_editRegEX->selectAll();
        }
    }
}

void settings::on_line_editRegEX_returnPressed()
{
    on_push_saveRegEX_clicked();
}
