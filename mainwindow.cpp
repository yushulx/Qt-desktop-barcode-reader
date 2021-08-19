#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Dynamsoft Barcode Reader
    reader = DBR_CreateInstance();

    // Open a file.
    connect(ui->actionOpen_File, SIGNAL(triggered()), this, SLOT(openFile()));

    // Open a folder.
    connect(ui->actionOpen_Folder, SIGNAL(triggered()), this, SLOT(openFolder()));

    // List widget event.
    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(listWidgetClicked(QListWidgetItem*)));

    // Export template.
    connect(ui->actionExport_template, SIGNAL(triggered()), this, SLOT(exportTemplate()));

    // About dialog.
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));

    // Set license.
    connect(ui->actionEnter_License_Key, SIGNAL(triggered()), this, SLOT(setLicense()));

    // Template load button
    connect(ui->pushButton_template, SIGNAL(clicked()), this, SLOT(loadTemplate()));

    // Template export button
    connect(ui->pushButton_export_template, SIGNAL(clicked()), this, SLOT(exportTemplate()));

    // Camera button
    connect(ui->pushButton_open, SIGNAL(clicked()), this, SLOT(startCamera()));
    connect(ui->pushButton_stop, SIGNAL(clicked()), this, SLOT(stopCamera()));

    // Cameras
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    if (cameras.size() > 0) 
    {
        for( int i = 0; i < cameras.count(); ++i )
        { 
            QCameraInfo cameraInfo = cameras.at(i);
            qDebug() << cameraInfo.deviceName();
            qDebug() << cameraInfo.description();
            camera = new QCamera(cameraInfo);
            surface = new MyVideoSurface(this, ui, reader);
            camera->setViewfinder(surface);
            break;
        }
    }
    else {
        ui->pushButton_open->setEnabled(false);
        ui->pushButton_stop->setEnabled(false);
    }

    
}

MainWindow::~MainWindow()
{
    camera->stop();
    delete ui;
    DBR_DestroyInstance(reader);
    delete camera;
    delete surface;
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Barcode images (*)"));
    if (!fileName.isEmpty()) {
        // Add to list
        ui->listWidget->addItem(fileName);
        ui->statusbar->showMessage(fileName);

        // Load the image file to QImage
        QImage image(fileName);
        showImage(image, fileName);
    }
}

void MainWindow::openFolder()
{
    QString folderName = QFileDialog::getExistingDirectory(this, tr("Open Folder"), "", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!folderName.isEmpty()) {
        // Get all files in the folder
        QStringList fileNames = QDir(folderName).entryList(QDir::Files | QDir::NoDotAndDotDot);
        // Add to list
        for (int i = 0; i < fileNames.size(); i++) {
            ui->listWidget->addItem(QDir::cleanPath(folderName + QDir::separator() + fileNames.at(i)));
        }

        ui->statusbar->showMessage(folderName);
    }
}

void MainWindow::listWidgetClicked(QListWidgetItem *item)
{
    ui->statusbar->showMessage(QString(item->text()));

    // Load the image file to QImage
    QImage image(item->text());
    showImage(image, item->text());
}

void MainWindow::showImage(const QImage &image, QString fileName)
{
    ui->textEdit_results->setText("");
    if (!image.isNull()) {
        QPixmap pm = QPixmap::fromImage(image);
        QPainter painter(&pm);
        painter.setPen(Qt::red);

        /************************
         * Barcode detection.
         ************************/ 
        // Get the template content and initialize the runtime settings.
        QString content = ui->textEdit_results->toPlainText();
        char errorMessage[256];
        DBR_InitRuntimeSettingsWithString(reader, content.toStdString().c_str(), CM_OVERWRITE, errorMessage, 256);

        // Set barcode types.
        int types = 0, types2 = 0;
        if (ui->checkBox_code39->isChecked()) {types |= BF_CODE_39;}
        if (ui->checkBox_code93->isChecked()) {types |= BF_CODE_93;}
        if (ui->checkBox_code128->isChecked()){ types |= BF_CODE_128;}
        if (ui->checkBox_codabar->isChecked()){ types |= BF_CODABAR;}
        if (ui->checkBox_itf->isChecked()){ types |= BF_ITF;}
        if (ui->checkBox_ean13->isChecked()){ types |= BF_EAN_13;}
        if (ui->checkBox_ean8->isChecked()){ types |= BF_EAN_8;}
        if (ui->checkBox_upca->isChecked()){ types |= BF_UPC_A;}
        if (ui->checkBox_upce->isChecked()){ types |= BF_UPC_E;}
        if (ui->checkBox_industrial25->isChecked()){ types |= BF_INDUSTRIAL_25;}
        if (ui->checkBox_qrcode->isChecked()){ types |= BF_QR_CODE;}
        if (ui->checkBox_pdf417->isChecked()){ types |= BF_PDF417;}
        if (ui->checkBox_aztec->isChecked()){ types |= BF_AZTEC;}
        if (ui->checkBox_maxicode->isChecked()){ types |= BF_MAXICODE;}
        if (ui->checkBox_datamatrix->isChecked()){ types |= BF_DATAMATRIX;}
        if (ui->checkBox_gs1->isChecked()){ types |= BF_GS1_COMPOSITE;}
        if (ui->checkBox_patchcode->isChecked()){ types |= BF_PATCHCODE;}
        if (ui->checkBox_dotcode->isChecked()){ types2 |= BF2_DOTCODE;}
        if (ui->checkBox_postalcode->isChecked()){ types2 |= BF2_POSTALCODE;}
        
        PublicRuntimeSettings settings;
        DBR_GetRuntimeSettings(reader, &settings);
        settings.barcodeFormatIds = types;
        settings.barcodeFormatIds_2 = types2;
        DBR_UpdateRuntimeSettings(reader, &settings, errorMessage, 256);

        int errorCode = DBR_DecodeFile(reader, fileName.toStdString().c_str(), "");
        TextResultArray *handler = NULL;
        DBR_GetAllTextResults(reader, &handler);
            
        if (handler->resultsCount == 0)
        {
            ui->textEdit_results->setText("No barcode found.\n");
            DBR_FreeTextResults(&handler);
            return;
        }

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
    }
}

void MainWindow::exportTemplate()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Barcode Template (*.json)"));
    QFile file(fileName);
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);

        char* pContent = NULL;
        DBR_OutputSettingsToStringPtr(reader, &pContent, "currentRuntimeSettings");
        stream << QString(pContent);
        DBR_FreeSettingsString(&pContent);
    }
}

void MainWindow::about()
{
    showMessageBox(tr("About"), tr("<a href='https://www.dynamsoft.com/barcode-reader/overview/'>Dynamsoft Barcode Reader v8.6</a>"));
}

void MainWindow::showMessageBox(QString title, QString content)
{
    QMessageBox::information(this, title, content);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Exit"), tr("Are you sure to quit?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::setLicense()
{
    QString licenseKey = QInputDialog::getText(this, tr("Enter License Key"), tr("License Key:"));
    int ret = DBR_InitLicense(reader, licenseKey.toStdString().c_str());
    if (ret != 0) {
        showMessageBox(tr("License Key"), tr("License key is invalid."));
    }
    else {
        showMessageBox(tr("License Key"), tr("Dynamsoft Barcode Reader is activated successfully!"));
    }
}

void MainWindow::loadTemplate()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Barcode Template (*.json)"));
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        QString content = stream.readAll();
        // DBR_LoadSettingsFromStringPtr(reader, content.toStdString().c_str());
        ui->textEdit_template->setText(content);
    }
}

void MainWindow::startCamera()
{
    surface->reset();
    camera->start();
}

void MainWindow::stopCamera()
{
    camera->stop();
}