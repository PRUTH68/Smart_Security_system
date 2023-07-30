#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <MFRC522.h>

#define MQ2_PIN A0
#define MQ9_PIN A1
#define LED_PIN 8
#define BUZZER_PIN 7
#define GSM_TX 2
#define GSM_RX 3
#define RST_PIN 9
#define SS_PIN 10

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial gsm(GSM_TX, GSM_RX);
MFRC522 rfid(SS_PIN, RST_PIN);

int mq2_value;
int mq9_value;
int sensitivity = 5;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  lcd.begin();
  gsm.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
}

void loop() {
  mq2_value = analogRead(MQ2_PIN);
  mq9_value = analogRead(MQ9_PIN);

  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    String uid = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
      uid += rfid.uid.uidByte[i] < 0x10 ? "0" : "";
      uid += String(rfid.uid.uidByte[i], HEX);
    }
    uid.toUpperCase();

    if (uid == "21E76F1D") {
      sensitivity = 5;
    } else if (uid == "E2D4C019") {
      sensitivity = 1;
    } else if (uid == "CARD_UID_3") {
      sensitivity = 3;
    }
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

  if (mq2_value > sensitivity * 100 || mq9_value > sensitivity * 100) {
    digitalWrite(LED_PIN, HIGH);
    tone(BUZZER_PIN, 1000);
    gsm.println("AT+CMGF=1");
    gsm.println("AT+CMGS=\"+916353543373\"");
    gsm.println("Gas detected!");
    gsm.write(26);
    lcd.setCursor(0, 0);
    lcd.print("MQ2: ");
    lcd.print(mq2_value);
    lcd.setCursor(0, 1);
    lcd.print("MQ9: ");
    lcd.print(mq9_value);
  } else {
    digitalWrite(LED_PIN, LOW);
    noTone(BUZZER_PIN);
    lcd.clear();
  }
}
