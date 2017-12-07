/*
 Name:		ArduinoReadSDCard.ino
 Created:	12/5/2017 9:14:35 PM
 Author:	mk
*/

// the setup function runs once when you press reset or power the board

#include <SD.h>
#include <avr/pgmspace.h>

const String commandText[] PROGMEM = { "act", "msc", "val", "arr", "nxt" };
const String actionText[] PROGMEM = { "flk", "txt", "fill" };

enum Cmd { act, msc, val, arr, nxt, CommandUndefined };
enum Act { flk, txt, fill, ActionUndefined };

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("Initializing SD card...");

  if (!SD.begin(SD_CHIP_SELECT_PIN)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  File myFile = SD.open("A1_1.2", FILE_READ);
  if (myFile) {
    //char buffer[40];
    Cmd state = CommandUndefined; // start
    Act action = ActionUndefined;
    String value, next;
    int delay = 0;
    while (myFile.available())
    {
      String line = myFile.readStringUntil('\n');
      line.toLowerCase();
      if (state != arr) {
        int t = line.indexOf(':');
        String cmd = line.substring(0, t);
        if (cmd.equals(commandText[(byte)Cmd::act])) {
          state = act;
          String act = line.substring(t + 1, line.indexOf(' ', t + 2));
          act.trim();
          if (act.equals(actionText[(byte)Act::flk])) {
            action = flk;
          }
          else if (act.equals(actionText[(byte)Act::txt]))
          {
            action = txt;
          }
          else if (act.equals(actionText[(byte)Act::fill]))
          {
            action = fill;
          }
        }
        else if (cmd.equals(commandText[(byte)Cmd::msc])) {
          state = msc;
          String delayText = line.substring(t + 1, line.indexOf(' ', t + 2));
          delay = delayText.toInt();
        }
        else if (cmd.equals(commandText[(byte)Cmd::val])) {
          state = Cmd::val;
          value = line.substring(line.indexOf(':') + 1);
          int comment = value.indexOf('#');
          if (comment >= 0) {
            value = value.substring(0, comment);
          }
          value.trim();
        }
        else if (cmd.equals(commandText[(byte)Cmd::nxt])) {
          state = nxt;
          next = line.substring(t + 1, line.indexOf(' ', t + 2));
          next.trim();
        }
        else if (cmd.equals(commandText[(byte)Cmd::arr])) {
          state = arr;
        }
      }
      else {
        //TODO print values
        for (byte i = 1; i < line.lastIndexOf('|'); i++) {
          if (line[i] == ',') {
            continue;
          }
          Serial.print('-');
          Serial.print(ConvertHex((byte)line[i]));
        }

      }
    }
    if (action != ActionUndefined) {
      Serial.print("Action: ");
    }
    switch (action) {
    case flk:
      Serial.println("flicker!");
      break;
    case txt:
      Serial.print("text: ");
      Serial.println(value);
      break;
    case fill:
      Serial.print("Fill complete: ");
      Serial.println(value);
    }
    if (next.length() > 0) {
      Serial.print("Jump To afterwards: ");
      Serial.println(next);
    }
    if (delay > 0) {
      Serial.print("Wait for ");
      Serial.println(delay);
    }
    myFile.close();
  }
  else {
    Serial.println("error opening A1_1.2");
  }
}

int ConvertHex(char input) {
  byte result = 15;
  if (input <= 32) {
    result = 0;
  }
  else if (isDigit(input)) {
    result = ((byte)input) - 48;
  }
  else if (input >= 'a' && input <= 'f'){
    result = ((byte)input) - 87;
  }
  return result * 17;
}

// the loop function runs over and over again until power down or reset
void loop() {
}
