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
  digitalWrite(SS, LOW);    // set latchPin LOW
  digitalWrite(SS, HIGH);   // set latchPin HIGH

  ///// SETUP ADC_0 ////
  adc->setAveraging(1, ADC_0);                                        // Set number of averages
  adc->setResolution(8, ADC_0);                                       // Set bits of resolution
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED, ADC_0);   // Change the conversion speed
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED, ADC_0);       // Change the sampling speed
  // always call the compare functions after changing the resolution!
  adc->enableCompare(1.0 / 3.3 * adc->getMaxValue(ADC_0), 0, ADC_0);  // measurement will be ready if value < 1.0V
  //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_0)/3.3, 2.0*adc->getMaxValue(ADC_0)/3.3, 0, 1, ADC_0); // ready if value lies out of [1.0,2.0] V

  ////// SETUP ADC_1 /////
  adc->setAveraging(1, ADC_1);                                        // set number of averages
  adc->setResolution(8, ADC_1);                                       // set bits of resolution
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED, ADC_1);   // change the conversion speed
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED, ADC_1);       // change the sampling speed
  // always call the compare functions after changing the resolution!
  adc->enableCompare(1.0 / 3.3 * adc->getMaxValue(ADC_1), 0, ADC_1);  // measurement will be ready if value < 1.0V
  //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_1)/3.3, 2.0*adc->getMaxValue(ADC_1)/3.3, 0, 1, ADC_1); // ready if value lies out of [1.0,2.0] V

  adc->startSynchronizedContinuous(PIN_A9, PIN_A3);
  serial.send(myPacket, ROW_FRAME);
}

////////////////////////////////////// LOOP
void loop() {

  uint16_t setCols = 0x8080; // 1000 0000 1000 0000

  for (uint8_t row = 0; row < DUAL_ROWS; row++) {

    setCols = setCols >> 1;

    digitalWrite(SS, LOW);              // Set latchPin LOW
    SPI.transfer(lowByte(setCols));     // Shift out the terird byte
    SPI.transfer(highByte(setCols));    // Shift out the secound byte
    SPI.transfer(dualSetRows[row]);     // Shift out the first byte
    digitalWrite(SS, HIGH);             // Set latchPin HIGH

    //result = adc->analogSynchronizedRead(PIN_A9, PIN_A3);
    result = adc->readSynchronizedContinuous();

    uint8_t index_A = col * COLS + row;                // Compute the unidimensional array index for sensorA
    uint8_t index_B = col * COLS + row + ROW_FRAME_2;  // Compute the unidimensional array index for sensorB

    myPacket[index_A] = result.result_adc0;
    myPacket[index_B] = result.result_adc1;

  }
  serial.update();
}


