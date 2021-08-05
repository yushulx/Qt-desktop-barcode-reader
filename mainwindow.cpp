#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Open a file.
    connect(ui->actionOpen_File, SIGNAL(triggered()), this, SLOT(openFile()));

    // Open a folder.
    connect(ui->actionOpen_Folder, SIGNAL(triggered()), this, SLOT(openFolder()));

    // List widget event.
    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(listWidgetClicked(QListWidgetItem*)));

    // Export template.
    connect(ui->actionExport_template, SIGNAL(triggered()), this, SLOT(exportTemplate()));

    // Dynamsoft Barcode Reader
    reader = DBR_CreateInstance();
}

MainWindow::~MainWindow()
{
    delete ui;
    DBR_DestroyInstance(reader);
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
    if (!image.isNull()) {
        ui->label->setPixmap(QPixmap::fromImage(image).scaled(ui->label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

        int errorCode = DBR_DecodeFile(reader, fileName.toStdString().c_str(), "");
        TextResultArray *handler = NULL;
        DBR_GetAllTextResults(reader, &handler);
            
        if (handler->resultsCount == 0)
        {
            printf("No barcode found.\n");
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
        }

        DBR_FreeTextResults(&handler);

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