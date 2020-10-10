#include <SoftwareSerial.h>
#include <LBLE.h>
#include <LBLEPeriphral.h>
#include <string>

char* device_ID = "ECART001"; // device's name
SoftwareSerial mySerial(2, 3); // 設定reader的RX, TX對應的pin腳

//==================================BLE: Define a simple GATT service=====================================
LBLEService ledService("19B10010-E8F2-537E-4F6C-D104768A1214"); // 宣告Service
// 開7個characteristics分別對應七個不同的reader報的值
LBLECharacteristicString characteristic_reader_1("19B10011-E8F2-537E-4F6C-D104768A1214", LBLE_READ);  //| LBLE_WRITE
LBLECharacteristicString characteristic_reader_2("19B10012-E8F2-537E-4F6C-D104768A1214", LBLE_READ);
LBLECharacteristicString characteristic_reader_3("19B10013-E8F2-537E-4F6C-D104768A1214", LBLE_READ);
LBLECharacteristicString characteristic_reader_4("19B10014-E8F2-537E-4F6C-D104768A1214", LBLE_READ);
LBLECharacteristicString characteristic_reader_5("19B10015-E8F2-537E-4F6C-D104768A1214", LBLE_READ);
LBLECharacteristicString characteristic_reader_6("19B10016-E8F2-537E-4F6C-D104768A1214", LBLE_READ);
LBLECharacteristicString characteristic_reader_7("19B10017-E8F2-537E-4F6C-D104768A1214", LBLE_READ);
LBLECharacteristicString characteristic_reader_8("19B10018-E8F2-537E-4F6C-D104768A1214", LBLE_READ);
//========================================================================================================

//initial 時呼叫function clean serial's buffer, 似乎沒什麼用
void clear_serial(SoftwareSerial mySerial){
  while(mySerial.available() > 0)char t = mySerial.read();
}

//把讀上來的data切割
void str_split(String s, String data_list[]){ 

  int count = 0;
  char buff[s.length() + 1];
  s.toCharArray(buff, s.length() + 1);
  char *tok = strtok(buff, "~ *"); // data strcuture: ~00 EVR01 NO ABCDE*
  while(tok != NULL) // 分割字串
  {
    data_list[count++] = String(tok); // 分割完字串存在data_list
    tok = strtok(NULL, "~ *");
  } 
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // 開port
  mySerial.begin(9600); // 開port
  clear_serial(mySerial);
  
//================BLE Setting===================
  // Initialize BLE subsystem
  LBLE.begin();
  while (!LBLE.ready()) 
  {
    delay(100);
    Serial.print(".");
  } 
  Serial.println("BLE ready");

  Serial.print("Device Address = [");
  Serial.print(LBLE.getDeviceAddress());
  Serial.println("]");

  // configure our advertisement data.
  // In this case, we simply create an advertisement that represents an
  // connectable device with a device name
  LBLEAdvertisementData advertisement;
  advertisement.configAsConnectableDevice(device_ID);

  // Configure our device's Generic Access Profile's device name
  // Ususally this is the same as the name in the advertisement data.
  LBLEPeripheral.setName(device_ID);

  // 把所有characteristics加到service中
  ledService.addAttribute(characteristic_reader_1);
  ledService.addAttribute(characteristic_reader_2);
  ledService.addAttribute(characteristic_reader_3);
  ledService.addAttribute(characteristic_reader_4);
  ledService.addAttribute(characteristic_reader_5);
  ledService.addAttribute(characteristic_reader_6);
  ledService.addAttribute(characteristic_reader_7);
  ledService.addAttribute(characteristic_reader_8);
  
  // 將服務添加到 GATT 伺服器之中
  LBLEPeripheral.addService(ledService);
  LBLEPeripheral.begin();
  
  // start advertisment
  LBLEPeripheral.advertise(advertisement);
  //======================================
}

void loop() {
  // put your main code here, to run repeatedly:
  
  String data_list[4] = ""; // 存 reader報上來且分割過的data
  String tmp = ""; // tmp存抓下來的raw data
  String send_data = ""; // 最後要傳出去的data
  LBLECharacteristicString *characteristic_in; // 創一個指標指向需要input資料的characteristics
  
  if(mySerial.available() == 0) return; // 檢查看看buffer是不是空的，空的就直接return, arduino會重複呼叫loop function
  delay(5);
  
  while(true)
  {
    if(mySerial.available() > 0) // 持續從串流讀取字串，每次都要這樣判斷，不然會抓到垃圾
    {
      char c = mySerial.read();
      tmp += c;
      if(c == '*') // 遇到結尾字元 break
        break;
    }
  }

  
  str_split(tmp, data_list); //分割字串
  
  switch(data_list[0].toInt()) // 判斷是哪個reader傳來的Data string
  {
    case 0:
      data_list[0] = "01";
      characteristic_in = &characteristic_reader_1; // pointer指向需要input資料的characteristics位址
      break;
    case 1:
      data_list[0] = "02";
      characteristic_in = &characteristic_reader_2;
      break;
    case 2:
      data_list[0] = "03";
      characteristic_in = &characteristic_reader_3;
      break;
    case 3:
      data_list[0] = "04";
      characteristic_in = &characteristic_reader_4;
      break;
    case 4:
      data_list[0] = "05";
      characteristic_in = &characteristic_reader_5;
      break;
    case 5:
      data_list[0] = "06";
      characteristic_in = &characteristic_reader_6;
      break;
    case 6:
      data_list[0] = "07";
      characteristic_in = &characteristic_reader_7;
      break;
    case 7:
      data_list[0] = "08";
      characteristic_in = &characteristic_reader_8;
      break;    
  }

  if(data_list[1] == "EVR01") // 放貨訊息
  {
    if(data_list[2] == "NO")
      send_data = "PODIN "+ data_list[0] + " NO " + data_list[3] + " " + device_ID;
    else // TE
      send_data = "PODIN " + data_list[0] + " TE " + device_ID;

    Serial.println(send_data);
    characteristic_in->setValue(send_data);
  }
  else if(data_list[1] == "EVR02") // 拿貨訊息
  {
    if(data_list[2] == "NO")
      send_data = "PODOUT " + data_list[0] + " NO " + data_list[3] + " " + device_ID;
    else // TE
      send_data = "PODOUT " + data_list[0] + " TE " + device_ID;

    Serial.println(send_data);
    characteristic_in->setValue(send_data);
  }
}
