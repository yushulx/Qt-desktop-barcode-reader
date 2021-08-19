#include "myvideosurface.h"
#include "./ui_mainwindow.h"

MyVideoSurface::MyVideoSurface(QObject *parent, Ui::MainWindow *ui, void *reader) : QAbstractVideoSurface(parent)
{
    this->ui = ui;
    this->reader = reader;
    this->is_detecting = true;
}

MyVideoSurface::~MyVideoSurface()
{

}

void MyVideoSurface::reset()
{
    this->is_detecting = true;
}

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
    if (frame.isValid() && is_detecting)
    {
        QVideoFrame cloneFrame(frame);
        cloneFrame.map(QAbstractVideoBuffer::ReadOnly);
        const QImage img(cloneFrame.bits(),
                         cloneFrame.width(),
                         cloneFrame.height(),
                         QVideoFrame::imageFormatFromPixelFormat(cloneFrame.pixelFormat()));

        QImage cp = img.copy();

        cloneFrame.unmap();

        cp = cp.mirrored(false, true);
        QPixmap pm = QPixmap::fromImage(cp);
        QPainter painter(&pm);
        painter.setPen(Qt::red);

        // Decode barcode and draw results
        int ret = DBR_DecodeBuffer(reader, (unsigned char *)cp.bits(), cp.width(), cp.height(), cp.bytesPerLine(), IPF_ARGB_8888, "");
        TextResultArray *handler = NULL;
        DBR_GetAllTextResults(reader, &handler);

        if (handler->resultsCount == 0)
        {
            ui->textEdit_results->setText("No barcode found.\n");
            DBR_FreeTextResults(&handler);
            ui->label->setPixmap(pm.scaled(ui->label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            return true;
        }

        if (ui->checkBox_autostop->isChecked()) is_detecting = false;

        QString out = "";
        TextResult **results = handler->results;
        for (int index = 0; index < handler->resultsCount; index++)
        {
            LocalizationResult* localizationResult = results[index]->localizationResult;
            out += "Index: " + QString::number(index) + "\n";
            out += "Barcode format: " + QString(results[index]->barcodeFormatString) + "\n";
            out += "Barcode value: " + QString(results[index]->barcodeText) + "\n";
            out += "Bounding box: (" + QString::number(localizationResult->x1) + ", " + QString::number(localizationResult->y1) + ") "
            + "(" + QString::number(localizationResult->x2) + ", " + QString::number(localizationResult->y2) + ") "
            + "(" + QString::number(localizationResult->x3) + ", " + QString::number(localizationResult->y3) + ") "
            + "(" + QString::number(localizationResult->x4) + ", " + QString::number(localizationResult->y4) + ")\n";
            out += "----------------------------------------------------------------------------------------\n";

            painter.drawLine(localizationResult->x1, localizationResult->y1, localizationResult->x2, localizationResult->y2);
            painter.drawLine(localizationResult->x2, localizationResult->y2, localizationResult->x3, localizationResult->y3);
            painter.drawLine(localizationResult->x3, localizationResult->y3, localizationResult->x4, localizationResult->y4);
            painter.drawLine(localizationResult->x4, localizationResult->y4, localizationResult->x1, localizationResult->y1);
        }

        DBR_FreeTextResults(&handler);

        painter.end();
        ui->label->setPixmap(pm.scaled(ui->label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

        ui->textEdit_results->setText(out);

        return true;
    }
    return false;
}

void MyVideoSurface::setUI(Ui::MainWindow *ui)
{
    this->ui = ui;
}
