#define BLYNK_TEMPLATE_ID "TMPL6mO80FfAF"
#define BLYNK_TEMPLATE_NAME "PenyiramTanaman"
#define BLYNK_AUTH_TOKEN "Q11Iixs3H085Y8Iovo0aDLLKcgLt7CpA"

#include <Blynk.h>
#include <BlynkSimpleWifi.h>
#include <WidgetRTC.h>

#include <WiFiManager.h>
#include <strings_en.h>
#include <wm_consts_en.h>
#include <wm_strings_en.h>
#include <wm_strings_es.h>

#define TRIGGER_PIN   10
#define AP_SSID          "PenyiramTanamanAP"

#define RAIN_SENSOR_ANALOG_INPUT  34
#define MOISTURE_SENSOR_ANALOG_INPUT 32
#define PIR_SENSOR_DIGITAL_INPUT 27

#define PUMP_CONTROL_OUTPUT 17
#define UV_CONTROL_OUTPUT 16

typedef struct{
  int raw_value;
  float normalized_value;
  int max_value;
  int min_value;
} RainSensor_t;

typedef struct{
  int raw_value;
  float normalized_value;
  int max_value;
  int min_value;
} MoistureSensor_t;

char blynk_token[] = BLYNK_AUTH_TOKEN;

WiFiManager wm;
BlynkTimer btimer;
WidgetRTC wRTC;

RainSensor_t rain_sensor;
MoistureSensor_t moisture_sensor;
int pir_sensor;



void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V0, moisture_sensor.normalized_value);
  Blynk.virtualWrite(V1, rain_sensor.normalized_value);
  Blynk.virtualWrite(V2, pir_sensor);

  String currentTime = String(hour()) + ":" + minute() + ":" + second();
  Serial.print("Current time: ");
  Serial.print(currentTime);
  Serial.println();
} 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("\n Starting");

  pinMode(PIR_SENSOR_DIGITAL_INPUT, INPUT);
  pinMode(PUMP_CONTROL_OUTPUT, OUTPUT);
  pinMode(UV_CONTROL_OUTPUT, OUTPUT);

  bool res;
  res = wm.autoConnect(AP_SSID);

  if(!res){
    Serial.println("Failed to connect");
  }else{
    Serial.println("Connected !!");
  }

  // WiFiManagerParameter custom_blynk_token("Blynk", "blynk token", mqtt_server, 40);
  // wm.addParameter(&custom_blynk_token);

  Blynk.begin(BLYNK_AUTH_TOKEN, WiFi.SSID().c_str(), WiFi.psk().c_str());
  wRTC.begin();


  btimer.setInterval(1000L, myTimerEvent);
}

float normalize_sensor(int min_val, int max_val, int val){
  return (float)(val - min_val) / (float)(max_val - min_val);
}

void loop() {
  // put your main code here, to run repeatedly:
  rain_sensor.raw_value = analogRead(RAIN_SENSOR_ANALOG_INPUT);
  moisture_sensor.raw_value = analogRead(MOISTURE_SENSOR_ANALOG_INPUT);

  rain_sensor.normalized_value = 1.00 - normalize_sensor(0, 4095, rain_sensor.raw_value);
  moisture_sensor.normalized_value = 1.00 - normalize_sensor(0, 4095, moisture_sensor.raw_value);

  pir_sensor = digitalRead(PIR_SENSOR_DIGITAL_INPUT);

  Serial.print("Sensor Raw Value [Rain, Moisture, PIR] : ");
  Serial.print(rain_sensor.normalized_value);
  Serial.print(" ");
  Serial.print(moisture_sensor.normalized_value);
  Serial.print(" ");
  Serial.print(pir_sensor);
  Serial.println();

  Blynk.run();
  btimer.run(); // Initiates BlynkTimer
  delay(20);
}
