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
  adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_0), 0, ADC_0);      // measurement will be ready if value < 1.0V
  //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_0)/3.3, 2.0*adc->getMaxValue(ADC_0)/3.3, 0, 1, ADC_0); // ready if value lies out of [1.0,2.0] V

  ////// SETUP ADC_1 /////
  adc->setAveraging(1, ADC_1);                                        // set number of averages
  adc->setResolution(8, ADC_1);                                       // set bits of resolution
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED, ADC_1);   // change the conversion speed
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED, ADC_1);       // change the sampling speed
  // always call the compare functions after changing the resolution!
  adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_1), 0, ADC_1);      // measurement will be ready if value < 1.0V
  //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_1)/3.3, 2.0*adc->getMaxValue(ADC_1)/3.3, 0, 1, ADC_1); // ready if value lies out of [1.0,2.0] V

  adc->startSynchronizedContinuous(PIN_A9, PIN_A3);
  serial.send(myPacket, ROW_FRAME);
}

////////////////////////////////////// LOOP
void loop() {

  if (scan) {
    for (uint8_t row = 0; row < DUAL_ROWS; row++) {

      byteA = dualSetRows[row];

      for (uint8_t col = 0; col < COLS; col++) {

        if (col < 8) {
          byteB = setCols[col];
          byteC = 0x0;
        } else {
          byteB = 0x0;
          byteC = setCols[col];
        }

        digitalWrite(SS, LOW);  // Set latchPin LOW
        SPI.transfer(byteC);    // Shift out the terird byte
        SPI.transfer(byteB);    // Shift out the secound byte
        SPI.transfer(byteA);    // Shift out the first byte
        digitalWrite(SS, HIGH); // Set latchPin HIGH

        //result = adc->analogSynchronizedRead(PIN_A9, PIN_A3);
        result = adc->readSynchronizedContinuous();

        // if using 16 bits and single-ended is necessary to typecast to unsigned,
        // otherwise values larger than 3.3/2 will be interpreted as negative
        result.result_adc0 = (uint16_t)result.result_adc0;
        result.result_adc1 = (uint16_t)result.result_adc1;

        byte sensorID_A = col * COLS + row;                // Compute the unidimensional array index for sensorA
        byte sensorID_B = col * COLS + row + ROW_FRAME_2;  // Compute the unidimensional array index for sensorB

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


