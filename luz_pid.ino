/* CONTROL DE TEMPERATURA PID PARA GERMINADOR */

// dimmer requirements (PZC:D2, PD:D3, Vcc, Gnd)
#include <JELdimmer2.h>
ACdimmer dimmer(3); // port for dimmer in pin 3


// OLED 128x32 requirements (SDA:A4, SCK:A5, Vcc, Gnd)
#include <Adafruit_SSD1306.h>
#define OLED_RESET 13
Adafruit_SSD1306 display(OLED_RESET);
#if (SSD1306_LCDHEIGHT != 32)
#error("Altura incorrecta; cambie en la librería de Adafruit_SSD1306.h!");
#endif


// DS18B20 temperature sensor requirements (DATA:D4, Vcc, Gnd)
#include <OneWire.h>
#include <DallasTemperature.h>
OneWire one_wire(4); // OneWire is in pin 4
DallasTemperature temp_sensor(&one_wire); // inicializamos libreria del sensor
#include "src/TimerOne/TimerOne.h"
float globalCelsius;
void checkTemperature(){
  // get temperature
  temp_sensor.requestTemperatures();
  float localCelsius = temp_sensor.getTempCByIndex(0);  // could be -127ºC (is an error in DS18B20)
  if(localCelsius > -100) 
    globalCelsius = localCelsius;
  Serial.print("T:"); Serial.print(globalCelsius);
}


// PID requirements
const double set_point = 25.5; // set point
const double kp = 10;  //10*(set_point-sonda_celsius)=100; baja de 100% en los últimos 10 grados
const double ki = 0.0002;
const double kd = 100000;         // no funcionó para cambios tan lentos de temperatura
unsigned long previous_time = 0;
double rate_error, last_error, cum_error = 0;
double current_time;
double dif_time;

double compute_PID(double sonda_celsius){
  current_time = millis();
  dif_time = (double)(current_time - previous_time);

  // proportional error
  double error = set_point - sonda_celsius;
  double kp_e = constrain(kp*error, -100, 100);  // negative values help to control, too
  Serial.print(",KpE:"); Serial.print(kp_e);

  // integral error
  cum_error += error * dif_time;
  // limitemos el crecimiento indiscriminado a un máximo de +-100
  if(ki*cum_error > 100) cum_error = 100 / ki;
  if(ki*cum_error < -100) cum_error = -100 / ki;
  double ki_e = ki*cum_error;
  Serial.print(",KiE:"); Serial.print(ki_e);

  // derivative error (útil sólo cuando podemos compensar acelerones locos de incremento)
  // rate_error = (error - last_error)/dif_time;
  // last_error = error;

  // save values for next iteration
  previous_time = current_time;
  
  float pid_sum = kp_e + ki_e;
  return constrain(pid_sum, 0, 100);
}


void display_oled_numbers(float sonda_celsius, float pid_to_lamp){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  
  // display temperature
  display.setCursor(1,1);
  display.print("T: ");
  if(sonda_celsius < 10) display.print(' ');
  if(sonda_celsius < 100) display.print(' ');
  display.print(sonda_celsius, 1); // one decimal place
  display.drawCircle(105,4,2,WHITE);  // degree symbol
  display.setCursor(110,1);
  display.print("C");
  
  // display lamp power
  display.setCursor(1,18);
  display.print("L: ");
  if(pid_to_lamp < 10) display.print(' ');
  if(pid_to_lamp < 100) display.print(' ');
  display.print(pid_to_lamp, 1);  // one decimal place
  display.setCursor(110,18);
  display.print("%");
  display.display();
}


void setup() {
  // temp sensor requirements
  temp_sensor.begin();

  // dimmer requirements
  dimmer.begin(ON);
  dimmer.setPower(0.0); // setPower(0-100%);

  // OLED 128x32 requirements
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // TimerOne interrumptions
  Timer1.initialize(200000);
  Timer1.attachInterrupt(checkTemperature);
  
  Serial.begin(9600);
}


// the loop function runs over and over again forever
void loop() {
  // get lamp power
  float lampPower = compute_PID(globalCelsius);
  dimmer.setPower(lampPower); // setPower(0-100%);
  Serial.print(",LampPower:"); Serial.print(lampPower);
  Serial.println();
  
  display_oled_numbers(globalCelsius, lampPower);
  
  delay(1000);
}
