// E-256 eTextile matrix sensor shield V2.0

#include "E256.h"

////////////////////////////////////// SETUP
void setup() {

  serial.setPacketHandler(&onPacket); // We must specify a packet handler method so that
  serial.begin(BAUD_RATE);  // Start the serial module

  SPI.begin();              // Start the SPI module
  pinMode(SS, OUTPUT);      // Set up slave mode
  SPI.transfer(0xFFFFFF);   // All OFF
  digitalWrite(SS, LOW);    // set latchPin LOW
  digitalWrite(SS, HIGH);   // set latchPin HIGH
}

////////////////////////////////////// LOOP
void loop() {

  if (scan) {

    for (byte row = 0; row < ROWS; row++) {
      for (byte col = 0; col < COLS; col++) {

        if (row < 8) {
          byteA = setRows[row];
          byteB = setCols[col];
          byteC = 0x0;
          SPI.transfer(byteA);    // shift out the first byte
          SPI.transfer(byteB);    // shift out the secound byte
          SPI.transfer(byteC);    // shift out the terird byte
          digitalWrite(SS, LOW);  // set latchPin LOW
          digitalWrite(SS, HIGH); // set latchPin HIGH
        } else {
          byteA = setRows[row];
          byteB = 0x0;
          byteC = setCols[col];
          SPI.transfer(byteA);    // shift out the first byte
          SPI.transfer(byteB);    // shift out the secound byte
          SPI.transfer(byteC);    // shift out the terird byte
          digitalWrite(SS, LOW);  // set latchPin LOW
          digitalWrite(SS, HIGH); // set latchPin HIGH
        }

        int rowValue = analogRead(A0_PIN);  // Reding use to store analog inputs values
        byte sensorID = col * COLS + row;   // Calculate the unidimensional array index

        if (calibration) {
          Calibrate(sensorID, rowValue, minVals);
        } else {
          uint8_t value = map(rowValue, minVals[sensorID], 1023, 0, 255);
          myPacket[sensorID] = value;
        }

      }
    }
    scan = false;
  }

  // The update() method attempts to read in
  // any incoming serial data and emits packets via
  // the user's onPacket(const uint8_t* buffer, size_t size)
  // method registered with the setPacketHandler() method.
  // The update() method should be called at the end of the loop().
  serial.update();

}


