# QuickPID   [![arduino-library-badge](https://www.ardu-badge.com/badge/QuickPID.svg?)](https://www.ardu-badge.com/QuickPID) [![PlatformIO Registry](https://badges.registry.platformio.org/packages/dlloydev/library/QuickPID.svg)](https://registry.platformio.org/packages/libraries/dlloydev/QuickPID)
QuickPID is an updated implementation of the Arduino PID library with additional features for PID control. By default, this implementation closely follows the method of processing the p,i,d terms as in the PID_v1 library except for using a more advanced anti-windup mode. Integral anti-windup can be based on conditionally using PI terms to provide some integral correction, prevent deep saturation and reduce overshoot. Anti-windup can also be based on clamping only, or it can be turned completely off. Also, the proportional term can be based on error, measurement, or both. The derivative term can be based on error or measurement.  PID controller modes include timer, which allows external timer or ISR timing control.

```
Note: You can use this library in esp-idf tool to program esp32 by cloning
this repo into your components folder, then clean the build and rebuild.
```

### Features

Development began with a fork of the Arduino PID Library. Modifications and new features have been added as described in the [releases](https://github.com/Dlloydev/QuickPID/releases).

#### New feature Summary

- [x] New functions added: `SetProportionalMode`, `SetDerivativeMode` and `SetAntiWindupMode`
- [x] `timer` mode for calling PID compute by an external timer function or ISR
- [x] Proportional on error `pOnError`, measurement `pOnMeas` or both `pOnErrorMeas` options
- [x] Derivative on error `dOnError` and measurement `dOnMeas` options
- [x] New PID Query Functions `GetPterm`, `GetIterm`, `GetDterm`, `GetPmode`, `GetDmode` and `GetAwMode`
- [x] New integral anti-windup options `iAwCondition`, `iAwClamp` and `iAwOff`

### Functions

#### QuickPID_Constructor

```c++
QuickPID::QuickPID(float* Input, float* Output, float* Setpoint, float Kp, float Ki, float Kd,
                   pMode pMode = pMode::pOnError, dMode dMode = dMode::dOnMeas,
                   iAwMode iAwMode = iAwMode::iAwCondition, Action action = Action::direct)
```

- `Input`, `Output`, and `Setpoint` are pointers to the variables holding these values.
- `Kp`, `Ki`, and `Kd` are the PID proportional, integral, and derivative gains.
- `pMode` is the proportional mode parameter with options for `pOnError` proportional on error (default), `pOnMeas`  proportional on measurement and `pOnErrorMeas` which is 0.5 `pOnError` + 0.5 `pOnMeas`.
- `dMode` is the derivative mode parameter with options for `dOnError` derivative on error, `dOnMeas` derivative on measurement (default).
- `awMode` is the integral anti-windup parameter with an option for `iAwCondition`  (default) that is based on PI terms to provide some integral correction, prevent deep saturation and reduce overshoot. The`iAwClamp` option clamps the summation of the pmTerm and iTerm. The `iAwOff` option turns off all anti-windup.
- `Action` is the controller action parameter which has `direct` (default)  and `reverse` options. These options set how the controller responds to a change in input.  `direct` action is used if the input moves in the same direction as the controller output (i.e. heating process). `reverse` action is used if the input moves in the opposite direction as the controller output (i.e. cooling process).

```c++
QuickPID::QuickPID(float* Input, float* Output, float* Setpoint,
                   float Kp, float Ki, float Kd, Action action)
```

This allows you to use Proportional on Error without explicitly saying so.

```c++
QuickPID::QuickPID(float *Input, float *Output, float *Setpoint)
```

This simplified version allows you to define the remaining parameters later via specific setter functions. By default, Kp, Ki, and Kd will be initialized to zero and should be later set via `SetTunings` to relevant values.

#### Compute

```c++
bool QuickPID::Compute();
```

This function contains the PID algorithm and it should be called once every loop(). Most of the time it will just return false without doing anything. However, at a  frequency specified by `SetSampleTime` it will calculate a new Output and return true.

#### Initialize

```c++
void QuickPID::Initialize();
```

Does all the things that need to happen to ensure a bump-less transfer from manual to automatic mode.

#### PID Query Functions

These functions query the internal state of the PID.

```c++
float GetKp();            // proportional gain
float GetKi();            // integral gain
float GetKd();            // derivative gain
float GetPterm();         // proportional component of output
float GetIterm();         // integral component of output
float GetDterm();         // derivative component of output
uint8_t GetMode();        // manual (0), automatic (1) or timer (2)
uint8_t GetDirection();   // direct (0), reverse (1)
uint8_t GetPmode();       // pOnError (0), pOnMeas (1), pOnErrorMeas (2)
uint8_t GetDmode();       // dOnError (0), dOnMeas (1)
uint8_t GetAwMode();      // iAwCondition (0, iAwClamp (1), iAwOff (2)
```

#### PID Set Functions

These functions set the internal state of the PID.

```c++
void SetMode(Control mode);                     // Set PID mode to manual, automatic or timer
void SetOutputLimits(float Min, float Max);     // Set and clamps the output to (0-255 by default)
void SetTunings(float Kp, float Ki, float Kd,   // set pid tunings and all computational modes
     pMode pMode, dMode dMode, iAwMode iAwMode);
void SetTunings(float Kp, float Ki, float Kd);  // only set pid tunings, other pid modes are unchanged
void SetControllerDirection(Action Action);     // Set controller action to direct (default) or reverse
void SetSampleTimeUs(uint32_t NewSampleTimeUs); // Set PID compute sample time, default = 100000 µs
void SetProportionalMode(pMode pMode);          // Set pTerm based on error (default), measurement, or both
void SetDerivativeMode(dMode dMode);            // Set the dTerm, based error or measurement (default).
void SetAntiWindupMode(iAwMode iAwMode);        // Set iTerm anti-windup to iAwCondition, iAwClamp or iAwOff
```

### Autotuner

#### Get  [sTune](https://github.com/Dlloydev/sTune)   [![arduino-library-badge](https://www.ardu-badge.com/badge/sTune.svg?)](https://www.ardu-badge.com/sTune)  [![PlatformIO Registry](https://badges.registry.platformio.org/packages/dlloydev/library/sTune.svg)](https://registry.platformio.org/packages/libraries/dlloydev/sTune)

A very fast autotuner capable of on-the-fly tunings and more.
