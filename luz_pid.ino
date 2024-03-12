/* CONTROL DE TEMPERATURA PID PARA GERMINADOR */
// SET POINT -> 25.5ºC/78ºF
//
// D2: PZC - DIMMER
// D3: PD  - DIMMER
// Vcc + GND
// 
// D4: DATA - DS18B20
// Vcc + GND
//
// A4: SDA - OLED 128x32
// A5: SCK - OLED 128x32
// Vcc + GND

// temp sensor requirements
#include <OneWire.h>
#include <DallasTemperature.h>
OneWire one_wire(4); // OneWire is in pin 4
DallasTemperature temp_sensor(&one_wire); // inicializamos libreria del sensor


// dimmer requirements
#include <JELdimmer2.h>
ACdimmer dimmer(3); // port for dimmer in pin 3


// OLED 128x32 requirements
#include <Adafruit_SSD1306.h>
#define OLED_RESET 13
Adafruit_SSD1306 display(OLED_RESET);
#if (SSD1306_LCDHEIGHT != 32)
#error("Altura incorrecta; cambie en la librería de Adafruit_SSD1306.h!");
#endif

void setup() {
  // temp sensor requirements
  temp_sensor.begin();

  // dimmer requirements
  dimmer.begin(ON);
  dimmer.setPower(0.0); // setPower(0-100%);

  // OLED 128x32 requirements
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  
  Serial.begin(9600);
}


// the loop function runs over and over again forever
void loop() {  
  // get and print sonda_celsius
  float sonda_celsius;
  do{
    temp_sensor.requestTemperatures();
    sonda_celsius = temp_sensor.getTempCByIndex(0);
  } while(sonda_celsius < -100);   // catch the sensor bug! (redings of -127.0 ºC)
  // print temperature and references
  Serial.print("Min:"); Serial.print(25.0);
  Serial.print(",Temp:"); Serial.print(sonda_celsius);
  Serial.print(",Max:"); Serial.print(26.0);

  // get and print PID lamp_percentage (and blow it)
  float lamp_power = compute_PID(sonda_celsius);
  dimmer.setPower(lamp_power); // setPower(0-100%);
  Serial.print(",LampPower:"); Serial.print(lamp_power);
  Serial.println();
  
  display_oled_numbers(sonda_celsius, lamp_power);
  
  delay(100);
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
void display_oled_text(String msg1, String msg2){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  
  // display msg1
  display.setCursor(4,1);
  display.print(msg1);
  // display msg2
  display.setCursor(4,18);
  display.print(msg2);
  display.display();
}
