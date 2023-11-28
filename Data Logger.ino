#include <SPI.h>
#include <SD.h>
#include <DHT.h>
#include <Keypad.h>

File dataFile;
bool dataCollectionStarted = false;

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

  // Ask user for data collection frequency
  Serial.println("Enter the data collection frequency in minutes and press '#':");
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

  int frequency = frequencyStr.toInt(); // Read the entered number
  Serial.print("\nData will be collected every ");
  Serial.print(frequency);
  Serial.println(" minutes.");

  // Ask user for CSV file name
  Serial.println("Enter the CSV file name and press '#':");
  String fileName = "";
  
  while (true) {
    char key = keypad.getKey();
    
    if (key != NO_KEY) {
      Serial.print(key);
      fileName += key;
    }
    
    if (key == '#') {
      break;
    }
  }
  
  // Start collecting data
  collectData(frequency, fileName);
}

void collectData(int frequency, String fileName) {
  uint16_t line = 1;

  Serial.println("Data collecting has started.");
  Serial.print("Data will be written to file: ");
  Serial.println(fileName);

  dataFile = SD.open(fileName.c_str(), FILE_WRITE);

  if (!dataFile) {
    Serial.println("Error opening file for writing");
    return;
  }

  while (true) {
    delay(frequency * 60000); // Convert minutes to milliseconds

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

    // Check for commands to stop or restart data collection
    if (keypad.isPressed('#')) {
      char command = keypad.getKey();
      
      if (command == 'B') {
        stopDataCollection();
      } else if (command == 'C') {
        restartDataCollection();
      }
    }
  }
}

void stopDataCollection() {
  dataFile.close();
  Serial.println("Data logger has stopped. Press #C to start the process from the beginning.");
  while (!keypad.isPressed('C')) {
    // Wait for user to press #C
  }
  restartDataCollection();
}

void restartDataCollection() {
  dataFile = SD.open("DHT22Log.csv", FILE_WRITE); // Change the file name if needed
  dataCollectionStarted = false;
  setup();
}

void loop() {
  // Not used in this example as data collection is handled in the setup function
}
