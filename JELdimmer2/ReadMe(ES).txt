La siguiente librería se usa para controlar cargas AC, como lamparas atenuables (lamparas incandecentes), resistencias calefactoras, puede controlar varias 
cargas AC por medio de un tempoerizador interno y una interrupción externa a tráves de un Pin previamente definido:
Para Leonardo, el PIN D7 se usa para detectar el cruce por cero
Para otras tarjetas se usa el pin D2

Esta librería simplificad el codigo de usuario con las siguientes funciones:

	1. Función ACdimmer(pin) esta función inicializa el numero de pin utilizado para la salida de control y es definida por el usuario
	Ejemplo: ACdimmer dimmer4(4);   la salida del dimmer es inicializada a el pin 4

	2. Función begin(ON/OFF) inicialización del puerto, temporizador y interrupción externa para el cruce por cero
	La inicalización del pin de detección de cruce por cero es definida por la librería y corresponde al pin D2 en las tarjetas
	 Duemilanove, Uno, Nano, Pro mini, Mega; y pic D7 en tarjetas con mcu ATmega32U4, como la tarjeta Leonardo
				
    Ejemplo: dimmer4.begin(ON); inicialización del puerto, temporizador, interrupción externa y estado del dimmer ON. 

    Parámetros: ON/OFF.
        a. ON - estado del dimmer ON, permite el uso del dimmer
        b. OFF - estado del dimmer OFF, no permite el uso del dimmer

    3. Función setPower(value) fija el valor del dimmer de 0 a 100%
        Ejemplo: dimmer4.setPower(90);

    4. Función getPower() para desplegar el valor actual del dimmer
        Ejemplo: Serial.print(dimmer4.getPower()); Resultado 0~100 int
   
   
    5. Función setState(ON/OFF) fija el estado del dimmer ON/OFF
        Ejemplo: dimmer4.setState(ON); delay(100); dimmer4.setState(OFF);
   
    6. Función getState() para desplegar el estado actual del dimmer
        Serial.print(dimmer4.getState()); Result 0 (OFF) or 1 (ON)
   
    7. Función changeState() cambia el estado del dimmer al estado contrario
        Ejemplo: dimmer4.setState(ON); delay(100); dimmer4.changeState; delay(100);
  

