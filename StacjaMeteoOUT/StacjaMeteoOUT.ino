#include<stdlib.h>
#include <VirtualWire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <dht11.h>

#define oneWirePin 11
#define DHT11Pin 10

OneWire  oneWire(oneWirePin);
DallasTemperature sensors(&oneWire);

dht11 DHT11;
float temperature;
float tmpTemperature;
float humidity;
float tmpHumidity;
char bufor[20];
char msg[27];

void setup(void) {

  Serial.begin(9600);

  vw_set_ptt_inverted(true);
  vw_setup(2000);
  vw_set_tx_pin(12);

  sensors.begin();
  sensors.setResolution(16);

  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);

  DHT11.read(DHT11Pin);
  humidity = DHT11.humidity;

}

void loop(void) {

  sensors.requestTemperatures();
  tmpTemperature = sensors.getTempCByIndex(0);

  DHT11.read(DHT11Pin);
  tmpHumidity = DHT11.humidity;

  if ( (tmpTemperature >= temperature + 0.1 || tmpTemperature <= temperature - 0.1) ||
       (tmpHumidity >= humidity + 1 || tmpHumidity <= humidity - 1)) {

    temperature = tmpTemperature;
    humidity = tmpHumidity;
    //        dtostrf(FLOAT,WIDTH,PRECSISION,BUFFER)
    String temp = dtostrf(temperature, 5, 1, bufor);
    String humi = dtostrf(humidity, 2, 0, bufor);
    String calosc = temp + "   " + humi;
    
    calosc.toCharArray(msg,calosc.length()+1);
    
    Serial.print("wysylanie  ");
    vw_send((uint8_t *)msg, strlen(msg));
    vw_wait_tx();
    Serial.println("wyslano RF");
    Serial.println(calosc);
    for (int i = 0; i <= calosc.length(); i++) {
      Serial.print(msg[i]);
    }

  }
  Serial.println();
  Serial.print("Wilgotnosc (%): ");
  Serial.print(humidity, 0);
  Serial.print("\t\t");
  Serial.print("Temperatura (C): ");
  Serial.println(temperature, 1);

  delay(500);
}
