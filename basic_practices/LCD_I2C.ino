#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2); /*set the LCD address to 0x27 for a 16 chars and 2 line display,if you’re not sure what your LCD’s I2C address is, you can run a simple I2C scanner sketch that scans your I2C bus and returns the address of each I2C device it finds.

You can find this sketch under File > Examples > Wire > i2c_scanner. */

void setup()
{
  lcd.init();
  lcd.clear();
  lcd.backlight();
  
  
  lcd.setCursor(1,0); //set cursor to character 1 (column 2) of line 0(row 1)
  lcd.print("Hello World!");
  
}

void loop()
{
  
}
