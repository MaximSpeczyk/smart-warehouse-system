#include <LiquidCrystal_I2C.h>

#define green 7
#define yellow 6
#define blue 5

// I2C address might be 0x27 or 0x3F depending on your LCD module
LiquidCrystal_I2C lcd_1(0x27, 16, 2);

String strings[3] = {"", "", ""}; // Start empty
bool updated = false;

void setup()
{
  pinMode(green, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(blue, OUTPUT);

  digitalWrite(green, LOW);
  digitalWrite(yellow, LOW);
  digitalWrite(blue, LOW);

  Serial.begin(9600);
  lcd_1.init();
  lcd_1.backlight();
  Serial.println("Ready");

  lcd_1.setCursor(0, 0);
  lcd_1.print("Slo1  Slo2  Slo3");
}

void updateLEDs() {
  digitalWrite(green, strings[0] != "" ? HIGH : LOW);
  digitalWrite(yellow, strings[1] != "" ? HIGH : LOW);
  digitalWrite(blue, strings[2] != "" ? HIGH : LOW);
}

void updateLCD() {
  lcd_1.setCursor(0, 1);
  lcd_1.print("                "); // Clear second row

  lcd_1.setCursor(0, 1);
  lcd_1.print(strings[0]);

  lcd_1.setCursor(6, 1);
  lcd_1.print(strings[1]);

  lcd_1.setCursor(12, 1);
  lcd_1.print(strings[2]);
}

void showMessage(const String& message) {
  lcd_1.setCursor(0, 0);
  lcd_1.print("                ");  // Clear first row
  lcd_1.setCursor(0, 0);
  lcd_1.print(message);
  delay(2000); // Wait for 2 seconds
  lcd_1.setCursor(0, 0);
  lcd_1.print("                ");
  lcd_1.setCursor(0, 0);
  lcd_1.print("Slo1  Slo2  Slo3");
}

void loop()
{
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    // Handle deletion commands
    if (input == "del1") {
      strings[0] = "";
      updated = true;
    } else if (input == "del2") {
      strings[1] = "";
      updated = true;
    } else if (input == "del3") {
      strings[2] = "";
      updated = true;
    }
    // Handle adding new string
    else {
      if (input.length() > 4) {
        input = input.substring(0, 4);
      }

      // Check for duplicates
      bool duplicate = false;
      for (int i = 0; i < 3; i++) {
        if (strings[i] == input) {
          duplicate = true;
          break;
        }
      }

      if (duplicate) {
        showMessage("Already exists");
      } else {
        for (int i = 0; i < 3; i++) {
          if (strings[i] == "") {
            strings[i] = input;
            updated = true;
            showMessage("Added to Slot " + String(i + 1));
            break;
          }
        }

        if (!updated) {
          Serial.println("All slots are full! Use del1, del2, or del3 to delete.");
        }
      }
    }

    updateLCD();
  }

  updateLEDs();
}
