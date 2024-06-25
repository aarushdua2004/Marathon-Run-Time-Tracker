#include <string>
#include <vector>
#include <SPI.h>
#include <MFRC522.h>


#include "GoogleFormPost.h"

#define FORM_ROOT_URL "https://docs.google.com/forms/d/e/1FAIpQLScIeEk0yyfLJE0MnUDdoK72y34nS4e2t_NQdtnfg7ITc_jFZQ/viewform"
#define RST_PIN  0
#define SS_PIN   5
const char *ssid = "Amitoj Singh";
const char *password = "12345678";
bool postOnce;
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;  
MFRC522::StatusCode status; 
int blockNum = 2;  
/* Create another array to read data from Block */
/* Legthn of buffer should be 2 Bytes more than the size of Block (16 Bytes) */
byte bufferLen = 18;
byte readBlockData[18];
char str16[] = "";

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
    postOnce = true;
    SPI.begin();
    mfrc522.PCD_Init(); 
}

void loop() {
      mfrc522.PCD_Init();
  /* Look for new cards */
  /* Reset the loop if no new card is present on RC522 Reader */
  if ( ! mfrc522.PICC_IsNewCardPresent()) {return;}
  /* Select one of the cards */
  if ( ! mfrc522.PICC_ReadCardSerial()) {return;}
  /* Read data from the same block */
  //--------------------------------------------------
  Serial.println();
  Serial.println(F("Reading last data from RFID..."));
  ReadDataFromBlock(blockNum, readBlockData);
  /* If you want to print the full memory dump, uncomment the next line */
  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  
  /* Print the data read from block */
  Serial.println();
  Serial.print(F("Last data in RFID:"));
  Serial.print(blockNum);
  Serial.print(F(" --> "));
  for (int j=0 ; j<16 ; j++)
  {
    str16[j] = readBlockData[j];
  }
  Serial.println(str16);



    if (postOnce) {
        GoogleFormPost gf;
        gf.setDebugMode(true);
        gf.setFormUrl(String(FORM_ROOT_URL));
        // version without knowing the field IDs
        int i = gf.readFields();
        if (i>0) {
            gf.addData(str16);
            gf.showDebug();
            gf.send();
        }
        // or if we did know them:
        else{
        gf.reset();
        gf.setFormUrl(String(FORM_ROOT_URL));
        gf.addData( str16 , String("entry.333496518"));
        gf.showDebug();
        gf.send();}
        delay(50);
    }
}

void ReadDataFromBlock(int blockNum, byte readBlockData[]) 
{ 
  //----------------------------------------------------------------------------
  /* Prepare the ksy for authentication */
  /* All keys are set to FFFFFFFFFFFFh at chip delivery from the factory */
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  //----------------------------------------------------------------------------
  /* Authenticating the desired data block for Read access using Key A */
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockNum, &key, &(mfrc522.uid));
  //----------------------------------------------------------------------------s
  if (status != MFRC522::STATUS_OK){
     Serial.print("Authentication failed for Read: ");
     Serial.println(mfrc522.GetStatusCodeName(status));
     return;
  }
  //----------------------------------------------------------------------------
  else {
    Serial.println("Authentication success");
  }
  //----------------------------------------------------------------------------
  /* Reading data from the Block */
  status = mfrc522.MIFARE_Read(blockNum, readBlockData, &bufferLen);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Reading failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  //----------------------------------------------------------------------------
  else {
    Serial.println("Block was read successfully");  
  }
  //----------------------------------------------------------------------------
}
