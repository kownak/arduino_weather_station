#include <VirtualWire.h>
#include <dht11.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <LPS331.h>
//#include <string.h>
#include <DS1340lib.h>

#define LIGHT_SWITCH 9
#define BACKLIGHT_PIN 3
#define DHT11Pin 13

LiquidCrystal_I2C lcd(0x3F, 20, 4);
LPS331 ps;
RTC_DS1340 RTC;
dht11 DHT11;
float pressure;
float temperatureIn;
float humidityIn;
boolean light = true;

void setup() {

  pinMode(LIGHT_SWITCH, INPUT_PULLUP);
  Wire.begin();

  //setup RTC
  RTC.begin(); // Init RTC
  //  RTC.adjust(DateTime(__DATE__, __TIME__)); 
  //end setup RTC

  //setup ps
  Serial.begin(9600);
  if (!ps.init())
  {
    Serial.println("Failed to autodetect pressure sensor!");
    while (1);
  }
  ps.enableDefault();
  //end setup ps

  //setup lcd
  lcd.init();
  lcd.backlight();
  lcd.setBacklight(HIGH);
  lcd.print("TEMP  WILGOT  hPa");

  vw_set_ptt_inverted(true);
  vw_setup(2000);
  vw_set_rx_pin(4);
  vw_rx_start();
}

void loop() {

  uint8_t odebrano[VW_MAX_MESSAGE_LEN];
  uint8_t dlugosc = VW_MAX_MESSAGE_LEN;

  String msg ="";
  if (vw_get_message(odebrano, &dlugosc))
  {
    Serial.println("odebrano rf");
    for (int i = 0; i < dlugosc; i++) {
      //Serial.print(odebrano[i], HEX);
      msg += char(odebrano[i]);
    }
    msg+='%';
    Serial.println(msg);
  }

  if (digitalRead(LIGHT_SWITCH) == LOW) {
    switch(light){
      case true:
      lcd.noBacklight();
      light = false;
      break;
      case false:
      lcd.backlight();
      light = true;
      break;
    }
  }
  if (digitalRead(10) == LOW) {
    lcd.backlight();
  }

  DHT11.read(DHT11Pin);
  humidityIn = DHT11.humidity;
  pressure = ps.readPressureMillibars();
  temperatureIn = ps.readTemperatureC();
  DateTime now = RTC.now();

  lcd.setCursor(8, 1);
  lcd.print(humidityIn, 0);
  lcd.print("% ");
  Serial.println(humidityIn, 0);
  lcd.setCursor(1, 1);
  lcd.print(temperatureIn, 1);
  lcd.print(' ');
  lcd.setCursor(14, 1);
  lcd.print(pressure, 1);
  
  lcd.setCursor(0, 3);

  String hour = (now.hour() > 9) ? now.hour() : ("0" + String(now.hour()));
  String minutes = (now.minute() > 9) ? now.minute() : ("0" + String(now.minute()));
  String seconds = (now.second() > 9) ? now.second() : ("0" + String(now.second()));
  String myTime = hour + ":" + minutes + ":" + seconds;
  lcd.print(myTime);

  lcd.setCursor(10, 3);
  String day = (now.day() > 9) ? now.day() : ("0" + String(now.day()));
  String month = (now.month() > 9) ? now.month() : ("0" + String(now.month()));
  String date = day + "/" + month + "/" + String(now.year());
  lcd.print(date);

  lcd.setCursor(0,2);
  lcd.print(msg);

  delay(500);
}

