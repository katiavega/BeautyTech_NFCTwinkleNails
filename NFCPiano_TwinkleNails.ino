
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

#include "pitches.h"

int notes[] = {
  NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5
};

// If using the breakout with SPI, define the pins for SPI communication.
#define PN532_SCK  (2)
#define PN532_MOSI (3)
#define PN532_SS   (4)
#define PN532_MISO (5)

// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.  Use the values below (2, 3) for the shield!
#define PN532_IRQ   (2)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

#if defined(ARDUINO_ARCH_SAMD)
// for Zero, output on USB Serial console, remove line below if using programming port to program the Zero!
// also change #define in Adafruit_PN532.cpp library file
   #define Serial SerialUSB
#endif

const int numCards = 10;
uint32_t cards[numCards][7]= { 
{0x04, 0x74, 0xF9, 0x02, 0xF1, 0x4A, 0x80}, 
{0x04, 0x64, 0xF7, 0x02, 0xF1, 0x4A, 0x80}, 
{0x04, 0xB2, 0xFF, 0x02, 0xF1, 0x4A, 0x80}, 
{0x04, 0x3A, 0xDB, 0x92, 0x8F, 0x49, 0x81},
{0x04, 0x5F, 0xDE, 0x92, 0x8F, 0x49, 0x81}, 
{0x04, 0x3D, 0xD8, 0x92, 0x8F, 0x49, 0x81},
{0x04, 0xB2, 0xFF, 0x02, 0xF1, 0x4A, 0x80}, 
{0x04, 0x64, 0xF7, 0x02, 0xF1, 0x4A, 0x11},
{0x04, 0x74, 0xF9, 0x02, 0xF1, 0x4A, 0x11}, 
{0x04, 0x64, 0xF7, 0x02, 0xF1, 0x4A, 0x11}};

void setup(void) {
  #ifndef ESP8266
    while (!Serial); // for Leonardo/Micro/Zero
  #endif
  Serial.begin(115200);
  Serial.println("Hello!");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
  
  Serial.println("Waiting for an ISO14443A Card ...");
}


void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
   
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    
    lookForCard (uid);
  }
  
  
}

void lookForCard(uint8_t cardID[7])
{
  for (int i =0; i<numCards;i++)
  {
    int count =0;
    for (int j =0; j<7;j++)
      {
        //Serial.print("cardid: ");Serial.print(cardID[j],HEX);
        //Serial.print(" cards: ");Serial.println(cards[i][j],HEX);
          if (cardID[j]== cards[i][j])
          {
            count++;
            
          }
      }
      Serial.print("count: ");Serial.println(count);
      
      if (count == 7) 
      {
        Serial.print("i: ");Serial.println(i);
        if (i==0 || i==5) {tone(8, notes[0], 325); Serial.println("DO");return;}
        if (i==1 || i==6) {tone(8, notes[1], 325); Serial.println("RE");return;}
        if (i==2 || i==7) {tone(8, notes[2], 325); Serial.println("MI");return;}
        if (i==3 || i==8) {tone(8, notes[3], 325); Serial.println("FA");return;}
        if (i==4 || i==9) {tone(8, notes[4], 325); Serial.println("SOL");return;}
      }
  }  
}

