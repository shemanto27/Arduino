#include <SPI.h>
#include <SD.h>
#include <DHT.h>
#include <Keypad.h>

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

void setup() {
  Serial.begin(9600);
  Serial.print("Initializing SD card...");

  if (!SD.begin()) {
    Serial.println("Initialization failed! Insert SD Card");
    while (1);
  }

  Serial.println("Initialization done. Collecting Data!");
  delay(2000);
  dht.begin();

  // Ask user for the unit of time for frequency
  Serial.println("Choose the unit of time for data collection frequency:");
  Serial.println("Press 1 for minutes, 2 for seconds, 3 for hours");

  char unitKey = waitForValidKey('1', '3');

  int multiplier = 60000; // Default unit is minutes

  if (unitKey == '2') {
    multiplier = 1000; // If user chooses seconds, change the multiplier
  } else if (unitKey == '3') {
    multiplier = 3600000; // If user chooses hours, change the multiplier
  }

  // Ask user for data collection frequency
  Serial.println("Enter the data collection frequency and press '#':");
  String frequencyStr = "";
  
  while (true) {
    char key = keypad.getKey();
    
    if (key >= '0' && key <= '9') {
      frequencyStr += key;
      Serial.print(key);
    } else if (key == '#') {
      break;
    }
  }

  int frequency = frequencyStr.toInt() * multiplier; // Adjust the frequency based on the unit

  Serial.print("\nData will be collected every ");
  Serial.print(frequency / multiplier); // Print the frequency without the multiplier
  Serial.print(" ");
  Serial.print((unitKey == '1') ? "minutes" : (unitKey == '2') ? "seconds" : "hours");
  Serial.println(".");

  // Ask user for CSV file name
  Serial.println("Enter the CSV file name and press '#':");
  String fileName = "";
  
  while (true) {
    char key = keypad.getKey();
    
    if (key != NO_KEY) {
      if (key == '#') {
        break;
      }
      Serial.print(key);
      fileName += key;
    }
  }

  // Remove trailing '#' character if it exists
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
  const int buzzerPin = 7; // Adjust the pin number if necessary

  Serial.println("Data collecting has started.");
  Serial.print("Data will be written to file: ");
  Serial.println(fileName);

  dataFile = SD.open((fileName + ".csv").c_str(), FILE_WRITE);

  if (!dataFile) {
    Serial.println("Error opening file for writing");
    return;
  }

  while (true) {
    delay(frequency);

    byte RH = dht.readHumidity();
    byte Temp = dht.readTemperature();

    Serial.print(line);
    Serial.print(": Temperature = ");
    Serial.print(Temp);
    Serial.print("°C, Humidity = ");
    Serial.print(RH);
    Serial.println("%");

    dataFile.print(line++);
    dataFile.print(": Temperature = ");
    dataFile.print(Temp);
    dataFile.print("°C, Humidity = ");
    dataFile.print(RH);
    dataFile.println("%");

    dataFile.flush(); // Ensure data is written to the file

    // Beep sound
    tone(buzzerPin, 1000, 1000); // 1000 Hz for 1000 ms (1 second)
  }
}

void loop() {
  // Not used in this example as data collection is handled in the setup function
}
