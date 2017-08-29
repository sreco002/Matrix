// E-256 eTextile matrix sensor shield V2.0

#include "E256.h"

////////////////////////////////////// SETUP
void setup() {

  serial.setPacketHandler(&onPacket); // We must specify a packet handler method so that
  serial.begin(BAUD_RATE);  // Start the serial module

  SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0));
  SPI.begin();              // Start the SPI module

  pinMode(SS, OUTPUT);      // Set up slave mode
  digitalWrite(SS, LOW);    // set latchPin LOW
  digitalWrite(SS, HIGH);   // set latchPin HIGH
}

////////////////////////////////////// LOOP
void loop() {

  if (scan) {

    for (uint8_t row = 0; row < ROWS; row++) {
      for (uint8_t col = 0; col < COLS; col++) {

        if (col < 8) {
          byteA = setRows[row];
          byteB = setCols[col];
          byteC = 0x0;     // B00000000
        } else {
          byteA = setRows[row];
          byteB = 0x0;     // B00000000
          byteC = setCols[col];
        }

        digitalWrite(SS, LOW);  // Set latchPin LOW
        SPI.transfer(byteC);    // Shift out the terird byte
        SPI.transfer(byteB);    // Shift out the secound byte
        SPI.transfer(byteA);    // Shift out the first byte
        digitalWrite(SS, HIGH); // Set latchPin HIGH

        int rowValue = analogRead(A0_PIN);  // Reding use to store analog inputs values
        // if (DEBUG) Serial.print(rowValue), Serial.print(" ");
        byte sensorID = col * COLS + row;   // Calculate the unidimensional array index

        if (calibration) {
          Calibrate(sensorID, rowValue, minVals);
        } else {
          uint8_t value = map(rowValue, minVals[sensorID], 1023, 0, 255);
          myPacket[sensorID] = value;
        }

      }
      // if (DEBUG) Serial.println();
    }
    // if (DEBUG) Serial.println();

    scan = false;
  }

  // The update() method attempts to read in
  // any incoming serial data and emits packets via
  // the user's onPacket(const uint8_t* buffer, size_t size)
  // method registered with the setPacketHandler() method.
  // The update() method should be called at the end of the loop().
  serial.update();

}


