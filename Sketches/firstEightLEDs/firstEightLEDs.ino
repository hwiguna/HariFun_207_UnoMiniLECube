byte seq[] = {0, 1, 2, 3, 2, 1};

const byte layerPins[] = {7,8,9,10};

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  for (byte i = 0; i < sizeof(layerPins); i++) {
    pinMode(layerPins[i], INPUT);
  }
}

void ChooseLayer(byte lyr)
{
  for (byte i = 0; i < sizeof(layerPins); i++) {
    pinMode(layerPins[i], INPUT);
  }

  pinMode(layerPins[lyr], OUTPUT);
  digitalWrite(layerPins[lyr], LOW);
}

// the loop function runs over and over again forever
void loop() {
  for (int d = 250; d > -500; d -= 50) {
    for (byte lyr = 0; lyr < sizeof(layerPins); lyr++) {
      ChooseLayer(lyr);
      for (byte i = 0; i < sizeof(seq); i++) {
        byte pin = 2 + seq[i];
        digitalWrite(pin, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(max(d, 50));
        digitalWrite(pin, LOW);
      }
    }
  }
}
