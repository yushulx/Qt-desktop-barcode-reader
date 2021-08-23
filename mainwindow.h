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
#include <QMessageBox>
#include <QCloseEvent>
#include <QDebug>
#include <QInputDialog>
#include <QPainter>
#include <QCameraViewfinder>
#include <QCamera>
#include <QCameraInfo>
#include <QAbstractVideoSurface>
#include <QTimer>
#include <QDateTime>
#include <QThread>
#include "DynamsoftCommon.h"
#include "DynamsoftBarcodeReader.h"
#include "myvideosurface.h"
#include "work.h"

using namespace dynamsoft::dbr;

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *);

private:
    Ui::MainWindow *ui;
    void *reader;
    void showImage(const QImage &image, QString fileName);
    void showMessageBox(QString title, QString content);
    QCamera *camera;
    MyVideoSurface *surface;
    Work* worker;
    QThread* thread;

private slots:
    void openFile();
    void openFolder();
    void listWidgetClicked(QListWidgetItem *item);
    void exportTemplate();
    void about();
    void setLicense();
    void loadTemplate();
    void startCamera();
    void stopCamera();
    void updateUI(QString results);  
};
#endif // MAINWINDOW_H
