#ifndef MYVIDEOSURFACE_H
#define MYVIDEOSURFACE_H

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
#include <QLabel>

#include "DynamsoftCommon.h"
#include "DynamsoftBarcodeReader.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

// https://www.qtcentre.org/threads/57090-How-could-I-get-the-image-buffer-of-QCamera
class MyVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT
private:
    QList<QImage> images;
    Ui::MainWindow *ui;
    void *reader;
    bool is_detecting;

public:
    MyVideoSurface(QObject *parent, Ui::MainWindow *ui, void *reader);
    ~MyVideoSurface();

    void setUI(Ui::MainWindow *ui);
    void reset();

    QList<QVideoFrame::PixelFormat>
    supportedPixelFormats(QAbstractVideoBuffer::HandleType type) const;

    bool present(const QVideoFrame &frame);
};
#endif // MYVIDEOSURFACE_H
