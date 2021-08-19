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

// https://www.qtcentre.org/threads/57090-How-could-I-get-the-image-buffer-of-QCamera
class MyVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT
private:
    QLabel *label;
    QList<QImage> images;

public:
    MyVideoSurface(QObject *parent = NULL) : QAbstractVideoSurface(parent)
    {
    }

    void setLabel(QLabel *label);

    QList<QVideoFrame::PixelFormat>
    supportedPixelFormats(QAbstractVideoBuffer::HandleType type) const;

    bool present(const QVideoFrame &frame);
};
#endif // MYVIDEOSURFACE_H
