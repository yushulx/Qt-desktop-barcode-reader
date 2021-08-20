# Qt Desktop Barcode Reader
A desktop barcode reader app built with [Dynamsoft C++ Barcode SDK](https://www.dynamsoft.com/barcode-reader/overview/) and [Qt](https://www.qt.io/).

## Trial License
Get a [trial license](https://www.dynamsoft.com/customer/license/trialLicense?product=dbr) to make the barcode recognition work.
 
## Download
- Qt
  - [Windows](https://www.qt.io/download)
  - Linux
    
    ```bash
    sudo apt-get install qt5-default
    ```
    
- [Dynamsoft C++ SDK for Windows and Linux](https://www.dynamsoft.com/barcode-reader/downloads/r)

## Build and Run

![Qt desktop barcode reader](https://www.dynamsoft.com/codepool/img/2021/08/barcode-scanner-webcam-qcamera.png)

**Windows**

Add `Qt\6.1.2\mingw81_64\bin` and `Qt\Tools\mingw810_64\bin` to system path.

Run the following commands:

```bash
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
BarcodeReader.exe
```

**Linux**

```bash
mkdir build
cd build
cmake ..
cmake --build .
./BarcodeReader
```
 
