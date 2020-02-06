/*
  © The Regents of the University of California, Davis campus, 2019. 
  This material is available as open source for research and personal use 
  under a Creative Commons Attribution-Non-Commercial 4.0 International Public License 
  (https://creativecommons.org/licenses/by-nc/4.0/).
  */
#include <Wire.h>
#include <Servo.h>

#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>

#define  O2_SENSOR          A2
#define  CO_SENSOR          A1
#define  NO_SENSOR          A0
#define  PRESSURE_SENSOR    A3
#define  LOWFLOW_PRESSURE_SENSOR  A4



AndroidAccessory acc("Google, Inc.",
		     "DemoKitTest",
		     "DemoKitTest Arduino Board",
		     "1.0",
		     "http://www.android.com",
		     "0000000012345678");

unsigned long time = 0;
unsigned long current_time;    //Timer variable for consistent sampling intervals
unsigned long prev_time;       //Timer variable for consistent sampling intervals
unsigned long interval=4995;   //Represents timing in microseconds to enable 5 millisecond sampling

const int redPin = 3;      //Red led pin number
const int yellowPin = 5;   //Yellow led pin number
const int greenPin = 6;    //Green led pin number
const int RUN_PIN=4;      //Run switch pin number

int i,j, k, loopCounter;
int runProgram=1;         //Sets program to run once start up conditions have been met
const int oversample=16;  //Number of extra samples to collect to minimize noise
byte b1, b2, c;

void setup(){
        //Initialize serial communication and pins
	Serial.begin(115200);
	Serial.print("\r\nStart");
  pinMode(redPin, OUTPUT);\
  pinMode(yellowPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(RUN_PIN, INPUT_PULLUP);
	c = 0;
	acc.powerOn();
  //Flash lights to indicate Ardunion turned on/reset
  for (j=1;j<=3;j++){
  	digitalWrite(redPin,HIGH);
    digitalWrite(yellowPin,HIGH);
    digitalWrite(greenPin,HIGH);
    delay(500);
    digitalWrite(redPin,LOW);
    digitalWrite(yellowPin,LOW);
    digitalWrite(greenPin,LOW);
    delay(500);
  }
}

void loop(){
	static byte msg[3];
  static byte msglow[3];
  //When the run switch is off and the spirometer is connected to the tablet
  //the yellow LED is on to represent standby mode
  //And runProgram is set to 1 which enables the program to run once
  //the switch is flipped
  if(digitalRead(RUN_PIN)==HIGH && acc.isConnected()==0)  {
    digitalWrite(redPin,LOW);
    digitalWrite(yellowPin,HIGH);
    digitalWrite(greenPin,LOW);
    runProgram=1;
  }
   //The program can run once the spirometer is connected to the tablet
   //and the run program switch is flipped
   while (acc.isConnected()&&digitalRead(RUN_PIN)==LOW){
     //The program can run only if the system has been properly reset
     if(runProgram==1){
       //The program consists of 10 different steps
       //The first step is the initialization of the program
       //which sets runProgram to 0 to prevent the program from restarting
       //if left on
        for (i = 0; i <= 10; i++){
          if(i == 0){
            runProgram=0;
            Serial.print("\r\nInitial Red");
            digitalWrite(redPin, HIGH);
            digitalWrite(yellowPin, LOW);
            digitalWrite(greenPin, LOW);
            delay(2000);
          }
          //Serial.print("\r\nRead: ");
					int len = acc.read(msg, sizeof(msg), 1);
					byte b;
					uint16_t val;
        	uint16_t val1;
        	uint16_t val2;
        	uint16_t val3;
        	uint16_t val4;
        	uint16_t val5;
					int x, y;
					char c0;
          //The next part consists of collection pressure sensor data
          //to monitor the tidal flow
          //Currently, this step is skipped
          if(i==1){
            for (loopCounter=1;loopCounter<=1;loopCounter++){
  //          Serial.print("\r\nChem Sensor Pressure");
              digitalWrite(greenPin, HIGH);
              digitalWrite(redPin, LOW);
              val4 = analogRead(PRESSURE_SENSOR);
      	    	msg[0] = 0x2;
      	    	msg[1] = val4 >> 8;
      	    	msg[2] = val4 & 0xff;
      	    	acc.write(msg, 3);
              delay(10);
            }
          }
          //The next step is the 60 second delay during which the
          //patient continues to tidal breath while the chemical
          //sensors equilibrate
          else if(i==2){
            for (loopCounter=1; loopCounter<=60; loopCounter++){
              //The green LED flashes on and off to indicate that
              //the chem sensors are still equilibriating
              if (loopCounter%2==1){
                 digitalWrite(greenPin, HIGH);
              }else{
                digitalWrite(greenPin, LOW);
              }
              delay(1000);
            }
          }
          //Once the sensors have reached equilibrium, the program
          //records the NO sensor data
          else if(i==3){
            for (loopCounter=1;loopCounter<=2000;loopCounter++){
  //            Serial.print("\r\nNO Sensor");
              digitalWrite(greenPin, HIGH);
              digitalWrite(redPin, LOW);
              val1=analogRead(NO_SENSOR);
              Serial.print(val1);
              Serial.print("  ,  ");
              //The recorded value is set to the tablet via msg
              msg[0] = 0x5;
              msg[1] = val1 >> 8;
  	      		msg[2] = val1 & 0xff;
  	      		acc.write(msg, 3);
    					//A 5 millisecond delay is included to space out
              //measurements
              delayMicroseconds(5000);
            }
          }
          //After the NO sensor data is collected, the CO data is collected
          else if(i==4){
            for (loopCounter=1;loopdCounter<=1000;loopCounter++){
//            Serial.print("\r\nCO Sensor");
            	digitalWrite(greenPin, HIGH);
            	digitalWrite(redPin, LOW);
              val2= analogRead(CO_SENSOR);
            	time = millis();
            	msg[0] = 0x6;
            	msg[1] = val2 >> 8;
	    				msg[2] = val2 & 0xff;
	    				acc.write(msg, 3);
							delayMicroseconds(5000);
              }
            }
            //After the CO sensor data is recorded, the O2 sensor data is collected
            //Currently, this data is ignored
            else if(i==5){
            for (loopCounter=1;loopCounter<=1;loopCounter++){
//            Serial.print("\r\nO2 Sensor");
              digitalWrite(greenPin, HIGH);
              digitalWrite(redPin, LOW);
            	val3 = analogRead(O2_SENSOR);
              msg[0] = 0x1;
              msg[1] = val3 >> 8;
	      			msg[2] = val3 & 0xff;
	      			acc.write(msg, 3);
              delay(10);
              }
            }
            //After the chemical sensor data is collected, there is a pause
            //before spirometry
          else if(i==6){
            for (loopCounter=1;loopCounter<=5;loopCounter++) {
            Serial.print("\r\nWait");
            digitalWrite(greenPin,LOW);
            digitalWrite(yellowPin, HIGH);
            delay(500);
            digitalWrite(yellowPin, LOW);
            delay(500);
            digitalWrite(yellowPin, HIGH);
            delay(500);
            digitalWrite(yellowPin, LOW);
            delay(500);
            }
          }
           //The pause is followed by cycling the LEDs from red to yellow to green
          //which provides the timing to start the manuever
          else if(i == 7){
           	Serial.println("\r\nPressure Sensor Next");
           	Serial.println("Time [ms], Pressure [Bit], Low Pressure [Bit]");
            digitalWrite(redPin, HIGH);
            delay(5000);
            digitalWrite(redPin, LOW);
            digitalWrite(yellowPin, HIGH);
            delay(1000);
            digitalWrite(yellowPin, LOW);
            digitalWrite(greenPin, HIGH);
            current_time=micros();
            prev_time=time;
          }
          //During the spirometry manuver, data from the pressure sensors is
          //recorded
          else if(i==8){
						//2000 data points are collected in 5 millisecond intervals
          	for (loopCounter=1;loopCounter<=2000;loopCounter++){
            	while ((current_time-prev_time)<interval){
              		current_time=micros();
            	}
           		//current_time=millis();
            	prev_time=current_time;
            	val=0;
            	val5=0;
            	//The pressure data is oversampled such that
            	//16 data points are averaged for each data point sent to the tablet
            	for(k=0;k<oversample;k++){
            		val += analogRead(PRESSURE_SENSOR);
              	val5 += analogRead(LOWFLOW_PRESSURE_SENSOR);
            	}
            	val=val>>4;
            	val5=val5>>4;
            	Serial.print(current_time);
            	Serial.print(",");
            	Serial.print(val);
            	Serial.print(",");
							Serial.println(val5);
  	    			msg[0] = 0x4;
  	    			msg[1] = val >> 8;
  	    			msg[2] = val & 0xff;
  	    			acc.write(msg, 3);
  	    			msglow[0] = 0x3;
  	    			msglow[1] = val5 >> 8;
  	    			msglow[2] = val5 & 0xff;
  	    			acc.write(msglow, 3);
          	}
          }
          //After the spirometry manuever, the program is finished until it is reset
          else{
            if (i==10){
              Serial.print("\r\nDone Pressure Sensor");
            }
            delay(1000);
            digitalWrite(greenPin, LOW);
            digitalWrite(redPin, HIGH);
        	}
      	}
   		}
   }
}
