// Clock multiplier
// By Simon Juhl
// www.simonjuhl.net
int pulseState = LOW;
float clockSpeed = 0;
const int pulseLength = 8000; // 8ms
long lastPulse;
long lastOutPulse;
bool pulseHigh = false;
const int in = 3;
const int out = 2;
int mult; // change mult in loop
unsigned long time;

void setup() {
  pinMode(out, OUTPUT); // clock ouput
  pinMode(in, INPUT); // clock input
  pinMode(4, INPUT); // switch to 5v
  pinMode(1, INPUT); // switch to 5v
  pinMode(0, INPUT); // switch to 5v
}

void loop() {
  // multiply settings
  mult = 2; // multi base

  // multiplication - remove if not needed
  // mult could be controlled by analog input
  if (digitalRead(4) == HIGH) {
    mult = mult * 2;
  }
  if (digitalRead(1) == HIGH) {
    mult = mult * 3;
  }
  if (digitalRead(0) == HIGH) {
    mult = mult * 4;
  }

  // clock input
  time = micros(); // save time
  if (digitalRead(in) == HIGH && pulseHigh == false) {
    pulseHigh = true;
    clockSpeed = (time - lastPulse); // save clock speed
    lastPulse = time; // save time since last pulse
  }
  if (digitalRead(in) == LOW) {
    pulseHigh = false;
  }

  // clock output
  if (time >= lastOutPulse + clockSpeed / mult || time == lastPulse) {
    digitalWrite(out, HIGH);
    lastOutPulse = time; // save rising edge time
  }

  // pulse
  if (time == lastPulse + pulseLength || time >= lastOutPulse + pulseLength) { // check if x time has passed since HIGH pulse
    digitalWrite(out, LOW);
  }
}
