# Bar code scanner emulator
### Scanner work at COM-port emulation. 
### Characteristics
* Speed: 19200 (Bit Rate)
* VID: 1155 | PID: 22336
* Name: Flipper {Your Flipper name}
* Manufacturer Name: Flipper Devices Inc.
* SerialNumber=flip_{Your Flipper name}

## Installation
1. Copy bc_scanner.fap file in Flipper SDCard (apps folder)
2. Make "/bcscanner" folder in Flipper SDCard 
3. Add bar code files (.bc) in "bcscanner" folder

## GUI
### App icon and name
![Icon](https://gitlab.idcloud.space/flipper-zero/bc_scanner_e/-/wikis/uploads/4c4fabf431947ef31a6e4e8103154b0f/icon.png)
### File Select
After open app - select bar code to be emulated

![Catalog](https://gitlab.idcloud.space/flipper-zero/bc_scanner_e/-/wikis/uploads/a03a1fa3e8bea95510eca7e10af5b5cd/catalog.png)
### Emulation
1. Click "Run" Button

![Run](https://gitlab.idcloud.space/flipper-zero/bc_scanner_e/-/wikis/uploads/2b6144eebd5e88caa8adc5e0c7514342/Runing.png)

When sending completed You will see 100% on the display:

![End](https://gitlab.idcloud.space/flipper-zero/bc_scanner_e/-/wikis/uploads/676d868d7c377ef9d0f42706bcd83913/EndRun.png)

2. If you need to resend, click "Run" again 
3. To exit - press the "Back" button on the flipper

## Create Bar Code File
1. Open the .txt file in an editor that recognizes GS special characters, such as sublime text
2. Add a barcode in any format, such as GS1 format
3. Save file and change extension on .bc
4. Copy file in Flipper SDCard, to "/bcscanner" folder