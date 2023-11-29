/* Countdown Timer */

void setup() {
  Serial.begin(9600);
}

void loop() {
  // Check if 60 seconds have elapsed
  if (millis() >= 60000) {
    // If 60 seconds have passed, print "0"
    Serial.print("0");
  } else {
    // If less than 60 seconds have passed, print the countdown timer
    Serial.print("There are ");
    Serial.print((60000 - millis()) / 1000);
    Serial.println(" seconds to go");
    delay(1000);
  }
}
