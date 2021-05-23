//13052021

#include <DmxSimple.h>
//#include "simpleTestDmx.h"
#define MAX 512
#define PIN_NAME 3
#define LEVEL 255
#define DELAY_PERIOD 10
#define SELECTED_CHANN 1
#define DELIMETER '_'
#define VERSION "VER 1 REV2 DATED 23052021"

enum state_t { RX_CMD, RX_X, RX_Y, RX_WAIT_FOR_NL };  // The possible states
static state_t state; // A variable of that enumerated type (basically an integer)

enum cmd_t { CMD_MOVE, CMD_DRAW };  // The possible commands
static cmd_t cmd;                   // A variable of that type

static uint16_t channel, level;               // Parameters

//===========================
// This FSM parses commands of the form "M#,#" or "D#,#".  As each character
//   is received, it either changes the current "state", or contributes
//   to a parameter's value.
//
// No temporary buffers are used, and invalid commands are immediately
//   rejected.
#define COMMAND_START 'D'
#define COMMAND_PARAMETER 'X'
bool getCommand( char c )
{
  bool gotIt = false; // Assume we're not done.

  switch (state) {

    case RX_CMD:          //--------------------------------
      if (c == COMMAND_START) {
        cmd   = CMD_MOVE;
        state = RX_X;
      } else if (c == COMMAND_PARAMETER) {
        cmd   = CMD_DRAW;
        state = RX_X;
      } else
        state = RX_WAIT_FOR_NL;
      channel = level = 0;
      break;

    case RX_X:           //--------------------------------
      if (isdigit( c ))
        channel = channel*10 + (c - '0'); // accumulate another digit
      else if (c == DELIMETER)
        state = RX_Y;
      else
        state = RX_WAIT_FOR_NL;
      break;

    case RX_Y:           //--------------------------------
      if (isdigit( c ))
        level = level*10 + (c - '0'); // accumulate another digit
      else if (c == '\n') {
        gotIt = true;
        state = RX_CMD;
      } else
        state = RX_WAIT_FOR_NL;
      break;

    case RX_WAIT_FOR_NL:     //--------------------------------
      if (c == '\n')
        state = RX_CMD;
      break;
  }

  return gotIt;
}
void setup() {
  /* The most common pin for DMX output is pin 3, which DmxSimple
  ** uses by default. If you need to change that, do it here. */

   Serial.begin(115200); 
  DmxSimple.usePin(PIN_NAME);
 pinMode(LED_BUILTIN, OUTPUT);
 Serial.println( VERSION); 
  DmxSimple.maxChannel(MAX);
}
 int counter;
int delays;

void loop() {
  counter=10000;
  int brightness;
  while(1){
    if( Serial.available())
    {


          bool gotSome = getCommand( Serial.read() );

//    Serial.print( (uint8_t) state );  // Show the state for each char received
    if (gotSome) {
      Serial.print( F("Valid command: ") );
      Serial.print( cmd );
      Serial.print( ' ' );
      Serial.print( channel );
      Serial.print( ',' );
      Serial.println( level );
      break;
    }
    }
  }
  /* Simple loop to ramp up brightness */
  //for (brightness = 0; brightness <= LEVEL; brightness++)
  {
    
    /* Update DMX channel 1 to new brightness */
    DmxSimple.write(channel,level);//SELECTED_CHANN, brightness);
    if(delays%counter==0)
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delays++;
    /* Small delay to slow down the ramping */
   // delay(DELAY_PERIOD);
  }

}
