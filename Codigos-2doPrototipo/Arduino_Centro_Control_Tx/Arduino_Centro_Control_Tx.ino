// Arduino situado en el centro de control Tx
// Rover TECSPACE

#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 35
#define RFM95_RST 37
#define RFM95_INT 3

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 434.0


//Setup Joystick

#define VRX_PIN  A0 // Arduino pin connected to VRX pin
#define VRY_PIN  A1 // Arduino pin connected to VRY pin

#define LEFT_THRESHOLD  400
#define RIGHT_THRESHOLD 800
#define UP_THRESHOLD    400
#define DOWN_THRESHOLD  800

#define COMMAND_NO     0x00
#define COMMAND_LEFT   0x01
#define COMMAND_RIGHT  0x02
#define COMMAND_UP     0x04
#define COMMAND_DOWN   0x08

int xValue = 0 ; // To store value of the X axis
int yValue = 0 ; // To store value of the Y axis
int command = COMMAND_NO;

bool motor1a = 0;
bool motor1b = 0;
bool motor2a = 0;
bool motor2b = 0;

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);


// Setup Radio


void setup() 
{
  int pinsignal0 = 13;
  int pinsignal1 = 12;


  pinMode(RFM95_RST, OUTPUT);
  pinMode(pinsignal0, INPUT) ;
  pinMode(pinsignal1, INPUT) ;

  digitalWrite(RFM95_RST, HIGH);

  while (!Serial);
  Serial.begin(9600);
  delay(100);

  Serial.println("Arduino LoRa TX Test!");

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
  

// Convert int to binary
void byteToBinary(byte valor, char *cadena) {
  for (int i = 7; i >= 0; --i) {
    cadena[7 - i] = ((valor >> i) & 1) ? '1' : '0';
  }
  cadena[8] = '\0'; // Null at last character
}


int16_t packetnum = 0;  // packet counter, we increment per xmission



void loop()
{


    char radiopacket[20];

    // Joystick Read

    xValue = analogRead(VRX_PIN);
    yValue = analogRead(VRY_PIN);

    // converts the analog value to commands
    // reset commands
    command = COMMAND_NO;

    // check left/right commands
    if (xValue < LEFT_THRESHOLD)
      command = command | COMMAND_LEFT;
    else if (xValue > RIGHT_THRESHOLD)
      command = command | COMMAND_RIGHT;

    // check up/down commands
    if (yValue < UP_THRESHOLD)
      command = command | COMMAND_UP;
    else if (yValue > DOWN_THRESHOLD)
      command = command | COMMAND_DOWN;


    if (command & COMMAND_LEFT) {
      motor1a = 1;
      motor1b = 0;

      motor2a = 0;
      motor2b = 1;
      // TODO: add your task here
    }

     else if (command & COMMAND_RIGHT) {
      motor1a = 0;
      motor1b = 1;

      motor2a = 1;
      motor2b = 0;
    }

    else if (command & COMMAND_UP) {
      motor1a = 1;
      motor1b = 0;

      motor2a = 1;
      motor2b = 0;
    }

    else if (command & COMMAND_DOWN) {
      motor1a = 0;
      motor1b = 1;

      motor2a = 0;
      motor2b = 1;
    }
    
    else {
      motor1a = 0;
      motor1b = 0;

      motor2a = 0;
      motor2b = 0;
    }

    //Encode control signals into radiopacket

    byte encoded_signal = (motor2b << 3) | (motor2a << 2) | (motor1b << 1) | motor1a;
    byteToBinary(encoded_signal, radiopacket);
    //Serial.print("Sending "); Serial.println(radiopacket);   
    rf95.send(&encoded_signal, 1);
    rf95.waitPacketSent();    

  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  //Serial.println("Waiting for reply...");
  delay(10);

  if (rf95.waitAvailableTimeout(100))
  { 
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len)) {
      Serial.println((char*)buf);
    }
    else {
      Serial.println("Receive failed");
    }
  }
  else
  {
    //Serial.println("No reply, is there a listener around?");
  }
  
  delay(100);
}