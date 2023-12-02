#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <DHT.h>
#include <Keypad.h>

const int buzzerPin = 7; // Declare buzzerPin as a global variable

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
File dataFile;

#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {22, 24, 26, 28};
byte colPins[COLS] = {30, 32, 34, 36};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void playIntroSound() {
  tone(buzzerPin, 880); // A4
  delay(200);
  noTone(buzzerPin);
  delay(50);
  
  tone(buzzerPin, 988);  // B4
  delay(200);
  noTone(buzzerPin);
  delay(50);
  
  tone(buzzerPin, 1109);  // D5
  delay(200);
  noTone(buzzerPin);
  delay(50);
  
  tone(buzzerPin, 1318); // E5
  delay(400);
  noTone(buzzerPin);
}

void playOKSound() {
  tone(buzzerPin, 880); // A4
  delay(200);
  noTone(buzzerPin);
  delay(50);
  
  tone(buzzerPin, 987);  // B4
  delay(200);
  noTone(buzzerPin);
  delay(50);
  
  tone(buzzerPin, 1046);  // C5
  delay(200);
  noTone(buzzerPin);
  delay(50);
}

void playBipSound() {
  tone(buzzerPin, 1000, 1000); // 1000 Hz for 1000 ms (1 second)
  delay(1000);
  noTone(buzzerPin);
}

void setup() {
  lcd.init(); // initialize the LCD
  lcd.backlight();
  lcd.begin(16, 2); // Added to initialize the LCD size

  pinMode(buzzerPin, OUTPUT);

  lcd.setCursor(0, 0);
  lcd.print("Initializing SD");
  lcd.setCursor(0, 1);
  lcd.print("card...");

  if (!SD.begin()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SD Card Error");
    playOKSound();
    while (1);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Initializing");
  lcd.setCursor(0, 1);
  lcd.print("  Data Logger");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" System Ready!");
  playIntroSound();

  delay(2000);
  dht.begin();

  // Ask user for the unit of time for frequency
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Choose unit");
  lcd.setCursor(0, 1);
  lcd.print("of time:");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press 1-Minute");
  lcd.setCursor(0, 1);
  lcd.print("2-Second,3-Hour");
  playBipSound();

  char unitKey = waitForValidKey('1', '3');

  int multiplier = 60000; // Default unit is minutes

  if (unitKey == '2') {
    multiplier = 1000; // If the user chooses seconds, change the multiplier
  } else if (unitKey == '3') {
    multiplier = 3600000; // If the user chooses hours, change the multiplier
  }

  // Ask the user for data collection frequency
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter frequency");
  lcd.setCursor(0, 1);
  lcd.print("and press '#'");
  String frequencyStr = "";
  
  playBipSound();

  while (true) {
    char key = keypad.getKey();
    
    if (key >= '0' && key <= '9') {
      frequencyStr += key;
      lcd.print(key);
    } else if (key == '#') {
      break;
    }
  }

  int frequency = frequencyStr.toInt() * multiplier; // Adjust the frequency based on the unit

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Data will be");
  lcd.setCursor(0, 1);
  lcd.print("collected every");

  lcd.print(frequency / multiplier);
  lcd.print((unitKey == '1') ? " Min" : (unitKey == '2') ? " Sec" : " Hour");
  lcd.print(".");

  // Ask the user for the CSV file name
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter CSV file");
  lcd.setCursor(0, 1);
  lcd.print("name & press '#'");
  String fileName = "";
  
  playBipSound();

  while (true) {
    char key = keypad.getKey();
    
    if (key != NO_KEY) {
      if (key == '#') {
        break;
      }
      lcd.print(key);
      fileName += key;
    }
  }

  // Remove the trailing '#' character if it exists
  if (fileName.endsWith("#")) {
    fileName.remove(fileName.length() - 1);
  }
  
  // Start collecting data
  collectData(frequency, fileName);
}

char waitForValidKey(char start, char end) {
  char key;
  while (true) {
    key = keypad.getKey();
    if (key >= start && key <= end) {
      return key;
    }
  }
}

void collectData(int frequency, String fileName) {
  uint16_t line = 1;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Data collection");
  lcd.setCursor(0, 1);
  lcd.print("has started.");
  playOKSound();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Data saving In");
  lcd.setCursor(0, 1);
  lcd.print(fileName);

  dataFile = SD.open((fileName + ".csv").c_str(), FILE_WRITE);

  if (!dataFile) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error opening file");
    lcd.setCursor(0, 1);
    lcd.print("for writing");
    playOKSound();
    return;
  }

  while (true) {
    delay(frequency);

    byte RH = dht.readHumidity();
    byte Temp = dht.readTemperature();

    // Wind speed calculation
    float sensorValue = analogRead(A0);
    float voltage = (sensorValue / 1023.0) * 5.0;
    float wind_speed = map(voltage, 0.01, 5, 0, 70);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Readings No: ");
    lcd.print(line);

    lcd.setCursor(0, 1);
    lcd.print("T:");
    lcd.print(Temp);
    lcd.print("C H:");
    lcd.print(RH);
    lcd.print("% W:");
    lcd.print(wind_speed, 2); // Display wind speed with 2 decimal places
    lcd.print(" m/s");

    dataFile.print(line++);
    dataFile.print(": Temperature = ");
    dataFile.print(Temp);
    dataFile.print("°C, Humidity = ");
    dataFile.print(RH);
    dataFile.print("% Wind Speed = ");
    dataFile.print(wind_speed, 2);
    dataFile.println(" m/s");

    dataFile.flush(); // Ensure data is written to the file
  }
}

void loop() {
  // Not used in this example as data collection is handled in the setup function
}
