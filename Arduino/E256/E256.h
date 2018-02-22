#ifndef __E256_H__
#define __E256_H__

#include <SPI.h>
#include <PacketSerial.h>

/*
  PACKET SERIAL : Copyright (c) 2012-2014 Christopher Baker <http://christopherbaker.net>
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/
PacketSerial serial;

#define  BAUD_RATE            230400
#define  COLS                 16
#define  ROWS                 16
#define  DUAL_COLS            8
#define  DUAL_ROWS            8
#define  ROW_FRAME            COLS*ROWS
#define  DUAL_ROW_FRAME       (COLS*ROWS)/2
#define  CALIBRATION_CYCLES   4

#define  A0_PIN               A9  // The output of multiplexerA (SIG pin) is connected to Arduino Analog pin 0
#define  A1_PIN               A3  // The output of multiplexerB (SIG pin) is connected to Arduino Analog pin 1

uint8_t myPacket[ROW_FRAME] = {0}; // Array to store values to transmit

void onPacket(const uint8_t* buffer, size_t size);

unsigned long lastFarme = 0;
uint16_t fps = 0;

// Array to store all parameters used to configure the two analog multiplexeurs
const uint8_t dualSetRows[ROWS] = {
  0x55, 0x77, 0x33, 0x11, 0x22, 0x44, 0x00, 0x66
};

#endif // __E256_H__
