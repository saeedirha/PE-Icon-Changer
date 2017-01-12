/*
 * Written by SaEeD
 *
 */

#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QDebug>
#include <QPixmap>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QtEndian>
//Windows header files
#include <Windows.h>
#include <iostream>
#include <WinUser.h>
//C++ header files
#include <iostream>
#include <fstream>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void on_actionSave_to_File_triggered();

    void on_actionChoose_Icon_File_triggered();

    void on_btnLoad_clicked();

    void on_txtPEFile_textChanged(const QString &arg1);

    void on_btnUpdateIcon_clicked();

private:
    Ui::Dialog *ui;

};

#endif // DIALOG_H
