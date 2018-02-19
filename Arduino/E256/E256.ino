// eTextile matrix sensor - E-256 (shield V2.0)
#include <ADC.h>
#include "E256.h"

ADC *adc = new ADC(); // adc object
ADC::Sync_result result;

////////////////////////////////////// SETUP
void setup() {

  pinMode(PIN_A9, INPUT);
  pinMode(PIN_A3, INPUT);

  serial.setPacketHandler(&onPacket); // We must specify a packet handler method so that
  serial.begin(BAUD_RATE);  // Start the serial module

  SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0));
  SPI.begin();              // Start the SPI module
  pinMode(SS, OUTPUT);      // Set up slave mode
  digitalWrite(SS, LOW);    // Set latchPin LOW
  digitalWrite(SS, HIGH);   // Set latchPin HIGH

  ///// SETUP ADC_0 ////
  adc->setAveraging(1, ADC_0);                                        // Set number of averages
  adc->setResolution(8, ADC_0);                                       // Set bits of resolution
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED, ADC_0);   // Set the conversion speed
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED, ADC_0);       // Set the sampling speed
  // always call the compare functions after changing the resolution!
  adc->enableCompare(1.0 / 3.3 * adc->getMaxValue(ADC_0), 0, ADC_0);  // Measurement will be ready if value < 1.0V
  //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_0)/3.3, 2.0*adc->getMaxValue(ADC_0)/3.3, 0, 1, ADC_0); // Ready if value lies out of [1.0,2.0] V

  ////// SETUP ADC_1 /////
  adc->setAveraging(1, ADC_1);                                        // Set number of averages
  adc->setResolution(8, ADC_1);                                       // Set bits of resolution
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED, ADC_1);   // Set the conversion speed
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED, ADC_1);       // Set the sampling speed
  // always call the compare functions after changing the resolution!
  adc->enableCompare(1.0 / 3.3 * adc->getMaxValue(ADC_1), 0, ADC_1);  // Measurement will be ready if value < 1.0V
  //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_1)/3.3, 2.0*adc->getMaxValue(ADC_1)/3.3, 0, 1, ADC_1); // Ready if value lies out of [1.0,2.0] V

  adc->startSynchronizedContinuous(PIN_A9, PIN_A3);
  serial.send(myPacket, ROW_FRAME);
}

////////////////////////////////////// LOOP
void loop() {

  // uint16_t setCols = 0x8080; // Powering two cols at a time (NOTGOOD) -> 1000 0000 1000 0000
  uint16_t setCols = 0x8000; // Powering one cols at a time (GOOD) -> 1000 0000 0000 0000

  for (uint8_t col = 0; col < COLS; col++) {
    for (uint8_t row = 0; row < DUAL_ROWS; row++) {

      digitalWrite(SS, LOW);              // Set latchPin LOW
      SPI.transfer(lowByte(setCols));     // Shift out the terird byte
      SPI.transfer(highByte(setCols));    // Shift out the secound byte
      SPI.transfer(dualSetRows[row]);     // Shift out the first byte
      digitalWrite(SS, HIGH);             // Set latchPin HIGH

      //result = adc->analogSynchronizedRead(PIN_A9, PIN_A3);
      result = adc->readSynchronizedContinuous();

      uint8_t index_A = col * COLS + row;                      // Compute the unidimensional array index_A (FIXME)
      uint8_t index_B = col * COLS + row + DUAL_ROW_FRAME;     // Compute the unidimensional array index_B (FIXME)

      myPacket[index_A] = result.result_adc0;     // Write ADC0 value to the unidimensional array
      myPacket[index_B] = result.result_adc1;     // Write ADC1 value to the unidimensional array

    }
    setCols = setCols >> 1;
  }
  serial.update();
}


