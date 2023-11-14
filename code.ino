//Xylophone
//Adapted for an ArduinoMega 
//from Jenna deBoisblanc and Spiekenzie Labs initial code

//*******************************************************************************************************************
// User settable variables
//*******************************************************************************************************************
int pinRead;

char pinAssignments[8] ={'A0','A1','A2','A3','A4','A5','A6','A7'};
byte PadNote[8] =       { 74 , 22 , 38 , 36 , 52 , 25 , 47 , 48 }; // MIDI notes from 0 to 127 (Mid C = 60)
int PadCutOff[8] =      { 1000 , 1000 , 1000 , 1000 , 1000 , 1000 , 1000 , 1000 }; // Minimum Analog value to cause a drum hit
int MaxPlayTime[8] =    { 10 , 10 , 10 , 10 , 10 , 10 , 10 , 10 }; // Cycles before a 2nd hit is allowed

#define  midichannel 1;                              // MIDI channel from 0 to 15 (+1 in "real world")

boolean VelocityFlag  = false;                           // Velocity ON (true) or OFF (false)

//*******************************************************************************************************************
// Internal Use Variables
//*******************************************************************************************************************
boolean activePad[8] = {
  0,0,0,0,0,0,0,0};                   // Array of flags of pad currently playing
int PinPlayTime[8] = {
  0,0,0,0,0,0,0,0};                     // Counter since pad started to play
byte status1;

int pin = 0;     
int hitavg = 0;

//*******************************************************************************************************************
// Setup
//*******************************************************************************************************************
void setup() 
{
  Serial.begin(57600);                                  // SET HAIRLESS TO THE SAME BAUD RATE IN THE SETTINGS
}

//*******************************************************************************************************************
// Main Program
//*******************************************************************************************************************
void loop() 
{
  for(int pin=0; pin < 8; pin++) {
    
    hitavg = analogRead(pin);  

    if((hitavg > PadCutOff[pin])) {                               // If the value produced from hitting a pad is greater than its cutoff value [PadCutOff]
      if((activePad[pin] == false)) {                             // If active pad is currently false [has not been hit, e.g has been unactive for its MaxPlayTime]
        if(VelocityFlag == true) {                                // If velocity sensitive has been set to true, then calculate as velocity sensitive
          // hitavg = 127 / ((1023 - PadCutOff[pin]) / (hitavg - PadCutOff[pin])); With full range (Too sensitive ?)
          hitavg = (hitavg / 8) -1 ; 
        } else {                                                  // Else just send the max velocty value
          hitavg = 100;
        }
        MIDI_TX(144,PadNote[pin],hitavg);                         // Sending MIDI message + setting PinPlayTime to 0 (Will increment with each read) and make activePad = true
        PinPlayTime[pin] = 0;
        activePad[pin] = true;
      } else {                                                    // Else if pin is currently active (still within its PinPlayTime) increment it with 1 (splitsecond)
        PinPlayTime[pin] = PinPlayTime[pin] + 1;
      }
    } else if((activePad[pin] == true)) {                         // Same as above just is the pad is NOT hit and is currently active
      PinPlayTime[pin] = PinPlayTime[pin] + 1;
      if(PinPlayTime[pin] > MaxPlayTime[pin]) {
        activePad[pin] = false;
        MIDI_TX(144,PadNote[pin],0); 
      }
    }
  } 
}

//*******************************************************************************************************************
// Transmit MIDI Message
//*******************************************************************************************************************
void MIDI_TX(byte MESSAGE, byte PITCH, byte VELOCITY) 
{
  status1 = MESSAGE + midichannel;
  Serial.write(status1);
  Serial.write(PITCH);
  Serial.write(VELOCITY);
}

