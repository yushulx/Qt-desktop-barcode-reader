#include "myvideosurface.h"

QList<QVideoFrame::PixelFormat> MyVideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType type) const
{
    if (type == QAbstractVideoBuffer::NoHandle)
    {
        return QList<QVideoFrame::PixelFormat>()
               << QVideoFrame::Format_RGB32;
    }
    else
    {
        return QList<QVideoFrame::PixelFormat>();
    }
}

bool MyVideoSurface::present(const QVideoFrame &frame)
{
    if (frame.isValid())
    {
        QVideoFrame cloneFrame(frame);
        cloneFrame.map(QAbstractVideoBuffer::ReadOnly);
        const QImage img(cloneFrame.bits(),
                         cloneFrame.width(),
                         cloneFrame.height(),
                         QVideoFrame::imageFormatFromPixelFormat(cloneFrame.pixelFormat()));

        QImage cp = img.copy();
        cp = cp.mirrored(false, true);
        QPixmap pm = QPixmap::fromImage(cp);
        label->setPixmap(pm.scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

        cloneFrame.unmap();

        return true;
    }
    return false;
}

void MyVideoSurface::setLabel(QLabel *label)
{
    this->label = label;
}