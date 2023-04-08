#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>

namespace Ui {
class settings;
}

class settings : public QDialog
{
    Q_OBJECT

public:
    explicit settings(QWidget *parent = 0);
    ~settings();

private slots:
    bool isValidRegEX(QString regEX);
    void on_label_3_linkActivated(const QString &link);
    void on_label_4_linkActivated(const QString &link);
    void on_line_newRegEX_returnPressed();
    void on_push_newRegEX_clicked();
    void on_push_deleteRegEX_clicked();
    void on_list_RegEX_clicked(const QModelIndex &index);
    void on_push_saveRegEX_clicked();
    void on_line_editRegEX_returnPressed();

private:
    Ui::settings *ui;
};

#endif // SETTINGS_H
