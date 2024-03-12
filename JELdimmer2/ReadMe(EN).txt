The following library is used for control AC Load as dimmer lamps (incandescent lamps), heating resistors, it can control several AC loads by means of an internal timer and an external interruption through a previously defined pin:
For Leonardo, D7 is zero-cross used pin 
For other boards is used D2 pin (MEGA, UNO, PRO MINI, NANO)

This library can simplify user code with following functions:

    1. Function  ACdimmer(pin) - this function initializes the number of operating pin and is defined by the user
    Example: ACdimmer dimmer4(4);   dimmer output is initialized on the pin 4
     
    2. Function begin(ON/OFF) port initialization, timer and external interrupt from zero-cross.
        Initialization of zero-cross is set in the library and corresponds to pin D2 on the board
        Duemilanove, Uno, Nano, Pro mini, Leonardo, Mega; and pin D7 an board with mcu ATmega32U4 as Leonardo board
				
    Example: dimmer4.begin(ON); port initialization, state dimmer ON. 

    Parameter: ON/OFF.
        a. ON - turns timer ON, allows to use dimmer.
        b. OFF - turns timer parameters OFF, prevents the use of dimmer.

    3. Function setPower(value) sets dimming value from 0 to 100%
        Example: dimmer4.setPower(90);

    4. Function getPower() to display current dimming value
        Example: Serial.print(dimmer4.getPower()); Result 0~100 int
   
   
    5. Function setState(ON/OFF) sets dimming state ON/OFF
        Example: dimmer4.setState(ON); delay(100); dimmer4.setState(OFF);
   
    6. Function getState() displays current state of dimmer
        Serial.print(dimmer4.getState()); Result 0 (OFF) or 1 (ON)
   
    7. Function changeState() changes dimmer state to the opposite one 
        Example: dimmer4.setState(ON); delay(100); dimmer4.changeState; delay(100);
  

