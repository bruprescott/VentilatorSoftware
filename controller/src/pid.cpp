/* Copyright 2020, Edwin Chiu

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "pid.h"
#include "comms.h"
#include "hal.h"
#include "parameters.h"
#include "types.h"

// Define Variables we'll be connecting to
static double Setpoint, Input, Output;

// Configure the PID
// Specify the links and initial tuning parameters
static float Kp = 2, Ki = 8, Kd = 0;
static PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

// These constants won't change. They're used to give names to the pins used:
// Analog output pin that the LED is attached to
static const int analogOutPin = LED_BUILTIN;

static void send_periodicData(uint32_t delay, uint16_t pressure,
                              uint16_t volume, uint16_t flow) {
  static uint32_t time;
  static bool first_call = true;

  if (first_call == true) {
    first_call = false;
    time = Hal.millis();
  } else {
    if ((Hal.millis() - time) > delay) {
      if (parameters_getPeriodicReadings()) {
        // Send readings data
        comms_sendPeriodicReadings(pressure * 1.0, volume * 0.0, flow * 0.0);
      }

      time = Hal.millis();
    }
  }
}


enum class pid_fsm_state {
  reset = 0,
  inspire = 1,
  plateau = 2,
  expire = 3,
  expire_dwell = 4,

  count /* Sentinel */
};

void pid_init() {

  // Initialize PID
  Input = map(Hal.analogRead(DPSENSOR_PIN), 0, 1023, 0, 255);
  Setpoint = BLOWER_LOW;

  // turn the PID on
  myPID.SetMode(AUTOMATIC);
}

void pid_execute() {

  uint16_t cyclecounter = 0;
  int16_t sensorValue = 0; // value read from the pot
  enum pid_fsm_state state = pid_fsm_state::reset;

  switch (state) {
  case pid_fsm_state::reset: // reset
    cyclecounter = 0;
    Setpoint = PEEP;
    state = pid_fsm_state::inspire; // update state
    break;

  case pid_fsm_state::inspire: // Inspire
    cyclecounter++;
    // set command
    Setpoint += INSPIRE_RATE;
    if (Setpoint > PIP) {
      Setpoint = PIP;
    }
    // update state
    if (cyclecounter > INSPIRE_TIME) {
      cyclecounter = 0;
      state = pid_fsm_state::plateau;
    }
    break;

  case pid_fsm_state::plateau: // Inspiratory plateau
    cyclecounter++;
    // set command
    Setpoint = INSPIRE_DWELL_PRESSURE;
    // update state
    if (cyclecounter > INSPIRE_DWELL) {
      cyclecounter = 0;
      state = pid_fsm_state::expire;
    }
    break;

  case pid_fsm_state::expire: // Expire
    cyclecounter++;
    // set command
    Setpoint -= EXPIRE_RATE;
    if (Setpoint < PEEP) {
      Setpoint = PEEP;
    }
    // update state
    if (cyclecounter > EXPIRE_TIME) {
      cyclecounter = 0;
      state = pid_fsm_state::expire_dwell;
    }
    break;

  case pid_fsm_state::expire_dwell: // Expiratory Dwell
    cyclecounter++;
    // set command
    Setpoint = PEEP;
    // update state
    if (cyclecounter > EXPIRE_DWELL) {
      cyclecounter = 0;
      state = pid_fsm_state::reset;
    }
    break;

  default:
    state = pid_fsm_state::reset;
    break;
  }

  // Update PID Loop
  sensorValue = Hal.analogRead(DPSENSOR_PIN); // read sensor
  Input = map(sensorValue, 0, 1023, 0, 255);  // map to output scale
  myPID.Compute();                            // computer PID command
  Hal.analogWrite(BLOWERSPD_PIN, Output);     // write output
  send_periodicData(DELAY_100MS, sensorValue, 0, 0);
}
