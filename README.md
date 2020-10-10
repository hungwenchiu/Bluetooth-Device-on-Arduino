# Bluetooth-Device-on-Arduino
Develop a device that can read RFID data and send it to cellphone through bluetooth

## Hardware
MTK linkit 7697/Arduino with bluetooth moudle
## IDE
BlocklyDuino
## Language
C/C++

## Description
1. 開發板需要有藍芽BLE功能
2. 腳位需要連接reader
3. linkit板子Serial通訊是TTL，而Reader是RS232，
   高低電位電壓範圍不一樣(如果直接接會收到亂碼)。
   中間需要接一個 TTL轉RS232的轉接頭。
   (板子Tx接轉接頭Tx且中間要給他一個1K電阻，否則電流太強有機會收到亂碼)。
