const byte ledPin = LED_BUILTIN;
const byte interruptPin = 4;
volatile byte state = LOW;

void setup() {

  Serial.begin(9600);
  while (!Serial) ;
  
  pinMode(ledPin, OUTPUT);
  //pinMode(interruptPin, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(interruptPin), blink, RISING);
}

void loop() {
  state = !state;
  digitalWrite(ledPin, state);
  Serial.println("Check");
  delay(1500);
}


