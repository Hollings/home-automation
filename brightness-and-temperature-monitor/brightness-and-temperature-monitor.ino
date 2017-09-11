#include <LiquidCrystal.h>
#include <SimpleDHT.h>
SimpleDHT11 dht11;

// Some defaults for the Thermistor -> Temperature conversion
#define THERMISTORPIN A7 // The pin the thermistor is on     
#define THERMISTORNOMINAL 8666 // resistance at 25 degrees C
#define TEMPERATURENOMINAL 25 // temp. for nominal resistance (almost always 25 C)
#define NUMSAMPLES 5 // how many samples to take and average, more takes longer but is more 'smooth'
#define BCOEFFICIENT 3950 // The beta coefficient of the thermistor (usually 3000-4000)
#define SERIESRESISTOR 10000  // the value of the 'other' resistor
uint16_t samples[NUMSAMPLES];
float R1 = 10000;
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;


// Set up pin inputs
int redpin = 9; 
int bluepin = 10; 
int greenpin = 11;
int temperaturePin = A13;
int humidityPin = A14;
int brightnessSensorPin = A15;  

// Inital values
int brightnessSensorValue = 0;
int brightness = 0;
int lastBrightness = 0;
int normalBrightness = 0;
int ledChoice = 0;
int lastChoice = 0;
int temperatureValue = 0;

// Timers for the loops
unsigned long minuteCheckStartTime = 0;
unsigned long minuteLoopTime = 60000;
unsigned long minuteLoopStartTime = millis();

void setup() {
  analogReference(DEFAULT);
  Serial.begin(9600);
  Serial.println("starting");
  analogWrite(bluepin, 255);
  analogWrite(greenpin, 255);
  analogWrite(redpin, 255);
}

int currentHumidity() {
  // Get Humidity from the dht11 component
  // It returns temp and humidity, but temperature isn't very accurate
  // so we dont use it
  byte temperature = 0;
  byte humidity = 0;
  byte data[40] = {0};
  dht11.read(humidityPin, &temperature, &humidity, data);
  return (int)humidity;
}

float currentTemperature() {
  // Grabs a couple samples from the thermistor and averages them
  // Outputs the temperature in fahrenheit
  // https://en.wikipedia.org/wiki/Steinhart%E2%80%93Hart_equation
  float average;
  int i;
  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
   samples[i] = analogRead(THERMISTORPIN);
   delay(10);
  }
 
  // average all the samples out
  average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
     average += samples[i];
  }
  average /= NUMSAMPLES;
  // convert the value to resistance
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;
  float steinhart;
  steinhart = average / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C
  return (steinhart  * 1.8 + 32);
}

void lightVisualization(){
  // When photoresistor reads brightness, pick a random 2 colors  and output them
  // on the RGB LED. Alternates colors when the light goes low
  lastBrightness = brightness;
  //Get Brightness from photoresistor
  brightnessSensorValue = analogRead(brightnessSensorPin);
  brightness =  constrain(map(brightnessSensorValue, 150, 800, 0, 255),0, 255);
  int ledChoices[] = {redpin,bluepin,greenpin};
  int nextChoice = 0;
  if(brightness < 50 and brightness > lastBrightness){
   
    if(ledChoice==2){
      ledChoice = 0;
    } else {
      ledChoice++;
    }    
  }
   if (ledChoice==2){
      nextChoice = 0;
    } else {
      nextChoice = ledChoice + 1;
    }
    if (ledChoice==0){
      lastChoice = 2;
    } else {
      lastChoice = ledChoice - 1;
    }
    
  analogWrite(ledChoices[lastChoice],  0);
  analogWrite(ledChoices[ledChoice],  brightness);
  analogWrite(ledChoices[nextChoice], brightness);
}

void loop() {
  float temperature;
  int humidity;
  
  // Every minute, do this loop
  unsigned long minuteLoopTime = millis() - minuteLoopStartTime;
  if (minuteLoopTime > minuteLoopStartTime)
  {   
    temperature = currentTemperature();
    humidity = currentHumidity();

    // Reset the timer
    minuteLoopStartTime = millis();
  }
  lightVisualization();
}
