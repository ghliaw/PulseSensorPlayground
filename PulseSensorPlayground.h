/*
 * A central Playground object to exercise the PulseSensor.
 * See https://www.pulsesensor.com to get started.
 * 
 * Portions Copyright (c) 2016, 2017 Bradford Needham, North Plains, Oregon, USA
 * @bneedhamia, https://bluepapertech.com
 * 
 * Licensed under the MIT License, a copy of which
 * should have been included with this software.
 * 
 * This software is not intended for medical use.
 */
#ifndef PULSE_SENSOR_PLAYGROUND_H
#define PULSE_SENSOR_PLAYGROUND_H

#include <Arduino.h>
#include "utility/PulseSensorBeatDetector.h"
#include "utility/PulseSensorSerialOutput.h"

class PulseSensorPlayground {
  public:
    // We create these in begin() functions to minimize our memory footprint.
    PulseSensorBeatDetector *pBeat;
    PulseSensorSerialOutput *pSerial;

    /*
     * Beat detection functions
     */

    void beginBeatDetection() {
      pBeat = new PulseSensorBeatDetector();
    }
    void setBeatSampleIntervalMs(long sampleIntervalMs) {
      pBeat->setSampleIntervalMs(sampleIntervalMs);
	}
    int getBeatsPerMinute() { return pBeat->getBPM(); }
    int getInterBeatIntervalMs() { return pBeat->getIBI(); }
    boolean isBeat() { return pBeat->isBeat(); }
    boolean addBeatValue(int analogValue) {
      return pBeat->addBeatValue(analogValue);
    }

    /*
     * Serial Output functions
     */

    void beginSerialOutput(int outputType) {
      pSerial = new PulseSensorSerialOutput(outputType);
    }
    void output(int signal, int bpm, int ibi) {
      pSerial->output(signal, bpm, ibi);
    }
    void outputBeat(int bpm, int ibi) {
      pSerial->outputBeat(bpm, ibi);
    }
};

#endif // PULSE_SENSOR_PLAYGROUND_H