// Arduino Prueba RFM9x LoRa Radio 
// Rover TECSpace


#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 35
#define RFM95_RST 37
#define RFM95_INT 3

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 434.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Blinky on receipt
#define LED 8

void setup() 
{
  pinMode(LED, OUTPUT);     
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  while (!Serial);
  Serial.begin(9600);
  delay(100);

  Serial.println("Arduino LoRa RX Test!");
  
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}

void loop()
{
  if (rf95.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    
    if (rf95.recv(buf, &len))
    {
      digitalWrite(LED, HIGH);
      //RH_RF95::printBuffer("Received: ", buf, len);
      Serial.print("Got: ");
      Serial.println((char*)buf);
      //Serial.print("RSSI: ");
      //Serial.println(rf95.lastRssi(), DEC);
      
      // Send a reply
      uint8_t data[] = "quiero comer";
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
      //Serial.println("Sent a reply");
      digitalWrite(LED, LOW);
    }
    else
    {
      Serial.println("Receive failed");
    }


    char encoded_signal[8]; 
    strcpy(encoded_signal, buf);
    //Serial.println( encoded_signal );

    bool signal0 = ('1' == encoded_signal[7]) ? true : false ;
    bool signal1 = ('1' == encoded_signal[6]) ? true : false ;
    bool signal2 = ('1' == encoded_signal[5]) ? true : false ;
    bool signal3 = ('1' == encoded_signal[4]) ? true : false ;

    //Serial.println(signal0);
    //Serial.println(signal1);

    if (signal0) {digitalWrite(12, HIGH);} 
    else { digitalWrite(12, LOW); }

    if (signal1) {digitalWrite(11, HIGH);} 
    else { digitalWrite(11, LOW); }

    if (signal2) {digitalWrite(10, HIGH);} 
    else { digitalWrite(10, LOW); }

    if (signal3) {digitalWrite(9, HIGH);} 
    else { digitalWrite(9, LOW); }

  }

}