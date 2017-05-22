/*
   A central Playground object to manage a set of PulseSensors.
   See https://www.pulsesensor.com to get started.

   Copyright World Famous Electronics LLC - see LICENSE
   Contributors:
     Joel Murphy, https://pulsesensor.com
     Yury Gitman, https://pulsesensor.com
     Bradford Needham, @bneedhamia, https://bluepapertech.com

   Licensed under the MIT License, a copy of which
   should have been included with this software.

   This software is not intended for medical use.
*/
#ifndef PULSE_SENSOR_PLAYGROUND_H
#define PULSE_SENSOR_PLAYGROUND_H

#include <Arduino.h>
#include "utility/Interrupts.h"
#include "utility/PulseSensor.h"
#include "utility/PulseSensorSerialOutput.h"
#include "utility/PulseSensorTimingStatistics.h" // makes TimingStatistics class visible.

class PulseSensorPlayground {
  public:
    static const unsigned long MICROS_PER_READ = (2 * 1000L); // usecs per sample.

    //---------- PulseSensor Manager functions

    /*
       Construct the one PulseSensor Playground manager,
       that manages the given number of PulseSensors.
       Your Sketch should declare either PulseSensorPlayground() for one sensor
       or PulseSensorPlayground(n) for n PulseSensors.

       For example:
         PulseSensorPlayground pulse();
    */
    PulseSensorPlayground(int numberOfSensors = 1);

    /*
       By default the library attempts to use interrupts
       to read and process the PulseSensor analog signal.
       To not use interrupts, call useInterrtups(false) sometime before begin().
       Useful if PulseSensor doesn't yet support interrupts on your Arduino.
    */
    void useInterrupts(boolean useInterrupts) {
      UsingInterrupts = useInterrupts;
    }

    /*
       Start reading and processing data from the PulseSensors.

       Your Sketch should make all necessary PulseSensor configuration calls
       before calling begin().

       If useInterrupt(false) hasn't been called, this function
       sets up and turns on interrupts for the PulseSensor.
       Otherwise it initializes what's necessary for the Sketch to process
       PulsSensor signals. See sawNewSample(), below.

       Returns true if successful, false if unsuccessful.
       Returns false if PulseSensorPlayground doesn't yet support
       interrupts on this Arduino and useInterrupts(false) was not called
       before begin().
    */
    boolean begin();

    /*
       Returns true if a new sample has been read from each PulseSensor.
       You'll likely want to add this call to your Sketch's loop()
       only if you either 1) want to do something with each sample of the
       PulseSensor signals, or 2) your Sketch doesn't use interrupts
       to read from the PulseSensors.

       If your Sketch called useInterrupts(false), you must call
       pulse.sawNewSample() frequently (at least once every 2 milliseconds)
       to assure that PulseSensor signals are read accurately.
       A typical non-interrupt loop() will contain:
         if (pulse.sawNewSample()) {
           int latest = pulse.getLatestSample();
           ...do whatever you want with the sample read from the PulseSensor.
         }
    */
    boolean sawNewSample();

    //---------- Per-PulseSensor functions

    /*
       By default, the Playground assumes the input pin = A0.
       If your PulseSensor is connected to a different analog input pin,
       call pulse.analogInput(pin) or pulse.analogInput(pin, sensorIndex).

       inputPin = the analog input this PulseSensor is connected to.
       sensorIndex = optional, index (0..numberOfSensors - 1)
         of the PulseSensor to configure.
    */
    void analogInput(int inputPin, int sensorIndex = 0);

    /*
       By default, the Playground doesn't blink LEDs automatically.

       If you wish the Playground to automatically blink an LED
       during each detected pulse,
       call pulse.blinkOnPulse(blinkPin) or
       pulse.blinkOnPulse(blinkPin, sensorIndex).

       blinkPin = the pin to blink on each pulse, which you've connected
         to an LED and 220 ohm resistor, or the built in LED pin
         on your Arduino (for example, pin 13 on Arduino Uno).
       sensorIndex = optional, index (0..numberOfSensors - 1)
         of the PulseSensor to configure.
    */
    void blinkOnPulse(int blinkPin, int sensorIndex = 0);

    /*
       By default, the Playground doesn't blink LEDs automatically.

       If you wish the Playground to automatically blink a fading LED
       during each detected pulse,
       call fadeOnPulse(fadePin) or fadeOnPulse(fadePin, sensorIndex).

       NOTE: the fade pin must be a PWM (Pulse-Width Modulation) pin.

       fadePin = the PWM pin to blink and fade on each pulse,
         which is connected to an LED and a current-limit resistor.
       sensorIndex = optional, index (0..numberOfSensors - 1)
         of the PulseSensor to configure.
    */
    void fadeOnPulse(int fadePin, int sensorIndex = 0);
    
    /*
       (Internal to library - do not call from a Sketch)
       Perform all the processing necessary when it's time to
       read from all the PulseSensors and process their signals.
    */
    void onSampleTime();

    /*
       Returns the most recently read analog value from the given PulseSensor
       (range: 0..1023).

       sensorIndex = optional, index (0..numberOfSensors - 1)
         of the PulseSensor of interest.
    */
    int getLatestSample(int sensorIndex = 0);

    /*
       Returns the latest beats-per-minute measure for the given PulseSensor.

       The internal beats-per-minute measure is updated per-PulseSensor,
       when a beat is detected from that PulseSensor.

       sensorIndex = optional, index (0..numberOfSensors - 1)
         of the PulseSensor of interest.
    */
    int getBeatsPerMinute(int sensorIndex = 0);

    /*
       Returns the latest IBI (inter-beat interval, in milliseconds) measure
       for the given PulseSensor.

       The internal IBI measure is updated per-PulseSensor,
       when a beat is detected from that PulseSensor.

       sensorIndex = optional, index (0..numberOfSensors - 1)
         of the PulseSensor of interest.
    */
    int getInterBeatIntervalMs(int sensorIndex = 0);

    /*
       Returns true if a new heartbeat (pulse) has been detected
       from the given PulseSensor since the last call to sawStartOfBeat()
       on this PulseSensor.

       Typical use in loop():
         if (pulse.sawStartOfBeat()) {
           ...do what you want to do per-heartbeat.
         }

       sensorIndex = optional, index (0..numberOfSensors - 1)
         of the PulseSensor of interest.
    */
    boolean sawStartOfBeat(int sensorIndex = 0);

    /*
       Returns true if the given PulseSensor signal is currently
       inside a heartbeat. That is, returns true if the signal is above
       the automatically-set threshold of a beat, false otherwise.

       Typical use in loop():
         if (pulse.isInsideBeat()) {
           ...do what you want while in the beat.
         } else {
           ...do what you want while between beats.
         }

       sensorIndex = optional, index (0..numberOfSensors - 1)
         of the PulseSensor of interest.
    */
    boolean isInsideBeat(int sensorIndex = 0);

    //---------- Serial Output functions
    
    /*
       By default, the Playround doesn't output serial data automatically.
       
       If you want to output serial pulse data, call pulse.setSerial(Serial),
       pulse.setSerial(Serial1), or whatever Serial stream you like.
       
       output = the Stream to write data to. Serial, Serial1, Serial2,
       etc., and a SoftwareSerial are valid parameters to pass.
    */
    void setSerial(Stream &output);

    /*
       By default, Playground output is in SERIAL_PLOTTER format.
       
       If you want output in a different format, call this function once
       sometime before calling pulse.begin().
       
       Remember to call pulse.setSerial() if you want serial output.

       outputType = SERIAL_PLOTTER to output to the Arduino Serial Plotter,
       PROCESSSING_VISUALIZER to output to the Processing Sketch
       that draws the PulseSensor output.
    */
    void setOutputType(byte outputType);
    
    /*
       Output the current signal information for each PulseSensor,
       in the previously-set outputType.
       
       If your Sketch wants to plot samples, it should call this function
       every so often.
    */
    void outputSample();
    
    /*
       Output the current per-beat information for each PulseSensor,
       in the previously-set outputType.
       
       If your Sketch wants to plot beat information, it should call this
       function every time a beat is detected.
       
       Typical use:
         if (pulse.sawStartOfBeat()) {
           pulse.outputBeat();
         }
    */
    void outputBeat();
    
    // (internal to the library) "this" pointer for the ISR.
    static PulseSensorPlayground *OurThis;
    
  private:
  
    /*
       Configure and enable interrupts to read samples.
       Call only if UsingInterrupts is true.
    */
    void setupInterrupt();
    
    byte SensorCount;              // number of PulseSensors in Sensors[].
    PulseSensor *Sensors;          // use Sensors[idx] to access a sensor.
    volatile unsigned long NextSampleMicros; // Desired time to sample next.
    volatile boolean SawNewSample; // "A sample has arrived from the ISR"
    PulseSensorSerialOutput SerialOutput; // Serial Output manager.
    boolean UsingInterrupts;          // sample with interrupts or not.
};
#endif // PULSE_SENSOR_PLAYGROUND_H