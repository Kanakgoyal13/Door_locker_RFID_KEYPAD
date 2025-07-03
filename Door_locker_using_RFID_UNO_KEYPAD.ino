#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// I2C LCD setup (address 0x27 or 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2); // Try 0x3F if 0x27 doesn't work

// RFID setup
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Servo motor setup
Servo doorServo;  // Servo connected to pin D6

// Authorized RFID UID
byte authorizedUID[4] = {0xAF, 0x16, 0xC9, 0xC4}; // Change this to your card's UID
byte readUID[4];

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  lcd.init();        // Initialize the LCD
  lcd.backlight();   // Turn on the backlight
  lcd.setCursor(0, 0);
  lcd.print("Scan RFID Card");

  doorServo.attach(6);   // Servo signal wire to D6
  doorServo.write(0);    // Start in locked position
}

void loop() {
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 4; i++) {
      readUID[i] = mfrc522.uid.uidByte[i];
    }

    bool match = true;
    for (byte i = 0; i < 4; i++) {
      if (readUID[i] != authorizedUID[i]) {
        match = false;
        break;
      }
    }

    if (match) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Card Accepted");
      delay(1000);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Access Granted");
      unlockDoor();
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Invalid Card");
      delay(2000);
      lcd.clear();
      lcd.print("Scan RFID Card");
    }

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }
}

void unlockDoor() {
  lcd.setCursor(0, 1);
  lcd.print("Unlocking...");

  doorServo.write(90);    // Move servo to open position
  delay(3000);            // Hold open for 3 seconds

  doorServo.write(0);     // Return to locked position
  lcd.clear();
  lcd.print("Door Locked");
  delay(2000);

  lcd.clear();
  lcd.print("Scan RFID Card");
}