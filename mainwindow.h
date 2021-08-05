#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QImage>
#include <QPixmap>
#include <QTextEdit>
#include <QFile>
#include <QTextStream>

#include "DynamsoftBarcodeReader.h"

using namespace dynamsoft::dbr;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void *reader;
    void showImage(const QImage &image, QString fileName);

private slots:
    void openFile();
    void openFolder();
    void listWidgetClicked(QListWidgetItem *item);
    void exportTemplate();
};
#endif // MAINWINDOW_H
