// eTextile matrix sensor - E-256 (shield V2.0)
#include "E256.h"

////////////////////////////////////// SETUP
void setup() {

  pinMode(A0_PIN, INPUT);
  pinMode(A1_PIN, INPUT);

  Serial.begin(BAUD_RATE);
  // serial.setPacketHandler(&onPacket); // We must specify a packet handler method so that
  // serial.begin(BAUD_RATE);  // Start the serial module

  pinMode(SS, OUTPUT);      // Set up slave mode
  digitalWrite(SS, LOW);    // Set latchPin LOW
  digitalWrite(SS, HIGH);   // Set latchPin HIGH
  SPI.begin();              // Start the SPI module
  SPI.beginTransaction(settings);

  ///// SETUP ADC_0 ////
  adc->setAveraging(1, ADC_0);                                            // Set number of averages
  adc->setResolution(8, ADC_0);                                           // Set bits of resolution
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED, ADC_0);  // Set the conversion speed
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED, ADC_0);      // Set the sampling speed
  // always call the compare functions after changing the resolution!
  adc->enableCompare(1.0 / 3.3 * adc->getMaxValue(ADC_0), 0, ADC_0);      // Measurement will be ready if value < 1.0V
  //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_0)/3.3, 2.0*adc->getMaxValue(ADC_0)/3.3, 0, 1, ADC_0); // Ready if value lies out of [1.0,2.0] V

  ////// SETUP ADC_1 /////
  adc->setAveraging(1, ADC_1);                                            // Set number of averages
  adc->setResolution(8, ADC_1);                                           // Set bits of resolution
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_HIGH_SPEED, ADC_1);  // Set the conversion speed
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_HIGH_SPEED, ADC_1);      // Set the sampling speed
  // always call the compare functions after changing the resolution!
  adc->enableCompare(1.0 / 3.3 * adc->getMaxValue(ADC_1), 0, ADC_1);  // Measurement will be ready if value < 1.0V
  //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_1)/3.3, 2.0*adc->getMaxValue(ADC_1)/3.3, 0, 1, ADC_1); // Ready if value lies out of [1.0,2.0] V

  adc->startSynchronizedContinuous(A0_PIN, A1_PIN);

  // serial.send(myPacket, ROW_FRAME);
}

////////////////////////////////////// LOOP
void loop() {

  if ((millis() - lastFarme) >= 1000) {
    Serial.printf(F("\nFPS: %d"), fps);
    lastFarme = millis();
    fps = 0;
  }

  // Columns are digital OUTPUT PINS
  // Rows are analog INPUT PINS
  // uint16_t setCols = 0x8080; // Powering two cols at a time (NOTGOOD) -> 1000 0000 1000 0000
  uint16_t setCols = 0x8000;    // We must powering one col at a time (GOOD) -> 1000 0000 0000 0000
  uint8_t index = 0;

  for (uint8_t col = 0; col < COLS; col++) {
    for (uint8_t row = 0; row < DUAL_ROWS; row++) {

      digitalWrite(SS, LOW);              // Set latchPin LOW
      SPI.transfer(lowByte(setCols));     // Shift out the LSB byte to set up the OUTPUT shift register
      SPI.transfer(highByte(setCols));    // Shift out the MSB byte to set up the OUTPUT shift register
      SPI.transfer(dualSetRows[row]);     // Shift out one byte that setup the two 8:1 analog multiplexers
      digitalWrite(SS, HIGH);             // Set latchPin HIGH

      // result = adc->analogSynchronizedRead(A0_PIN, A1_PIN);
      result = adc->readSynchronizedContinuous();

      myPacket[index] = result.result_adc0;
      myPacket[index + DUAL_ROW_FRAME] = result.result_adc1;
      index += 1;
    }
    setCols = setCols >> 1;
  }

  for (int i = 0; i < index ; i++) {
    Serial.printf(F(" %d"), myPacket[index]);
  }
  Serial.printf(F("\n"));

  index = 0;

  // serial.update();

  fps++;
}

// This is our packet callback, the buffer is delivered already decoded.
void onPacket(const uint8_t* buffer, size_t size) {
  // The send() method will encode the buffer
  // as a packet, set packet markers, etc.
  serial.send(myPacket, ROW_FRAME);
}


