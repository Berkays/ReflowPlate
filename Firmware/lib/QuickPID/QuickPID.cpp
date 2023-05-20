/**********************************************************************************
   QuickPID Library for Arduino - Version 3.1.7
   by dlloydev https://github.com/Dlloydev/QuickPID
   Based on the Arduino PID_v1 Library. Licensed under the MIT License.
 **********************************************************************************/

#include "QuickPID.h"
#include "constrain.h"

QuickPID::QuickPID() {}

/* Constructor ********************************************************************
   The parameters specified here are those for for which we can't set up
   reliable defaults, so we need to have the user set them.
 **********************************************************************************/

QuickPID::QuickPID(double *Input, double *Output, double *Setpoint,
                   double Kp = 0, double Ki = 0, double Kd = 0,
                   pMode pMode = pMode::pOnError,
                   dMode dMode = dMode::dOnMeas,
                   iAwMode iAwMode = iAwMode::iAwCondition,
                   Action Action = Action::direct)
{

    myOutput = Output;
    myInput = Input;
    mySetpoint = Setpoint;
    mode = Control::manual;

    QuickPID::SetOutputLimits(0, 255); // same default as Arduino PWM limit
    sampleTimeUs = 100000;             // 0.1 sec default
    QuickPID::SetControllerDirection(Action);
    QuickPID::SetTunings(Kp, Ki, Kd, pMode, dMode, iAwMode);

    lastTime = HAL_GetTick() - sampleTimeUs;
}

/* Constructor *********************************************************************
   To allow using pOnError, dOnMeas and iAwCondition without explicitly saying so.
 **********************************************************************************/
QuickPID::QuickPID(double *Input, double *Output, double *Setpoint,
                   double Kp, double Ki, double Kd, Action Action)
    : QuickPID::QuickPID(Input, Output, Setpoint, Kp, Ki, Kd,
                         pmode = pMode::pOnError,
                         dmode = dMode::dOnMeas,
                         iawmode = iAwMode::iAwCondition,
                         action = Action)
{
}

/* Constructor *********************************************************************
   Simplified constructor which uses defaults for remaining parameters.
 **********************************************************************************/
QuickPID::QuickPID(double *Input, double *Output, double *Setpoint)
    : QuickPID::QuickPID(Input, Output, Setpoint,
                         dispKp = 0,
                         dispKi = 0,
                         dispKd = 0,
                         pmode = pMode::pOnError,
                         dmode = dMode::dOnMeas,
                         iawmode = iAwMode::iAwCondition,
                         action = Action::direct)
{
}

/* Compute() ***********************************************************************
   This function should be called every time "void loop()" executes. The function
   will decide whether a new PID Output needs to be computed. Returns true
   when the output is computed, false when nothing has been done.
 **********************************************************************************/
bool QuickPID::Compute()
{
    if (mode == Control::manual)
        return false;
    uint32_t now = HAL_GetTick();
    uint32_t timeChange = (now - lastTime);
    if (mode == Control::timer || timeChange >= sampleTimeUs)
    {

        double input = *myInput;
        double dInput = input - lastInput;
        if (action == Action::reverse)
            dInput = -dInput;

        error = *mySetpoint - input;
        if (action == Action::reverse)
            error = -error;
        double dError = error - lastError;

        double peTerm = kp * error;
        double pmTerm = kp * dInput;
        if (pmode == pMode::pOnError)
            pmTerm = 0;
        else if (pmode == pMode::pOnMeas)
            peTerm = 0;
        else
        { // pOnErrorMeas
            peTerm *= 0.5f;
            pmTerm *= 0.5f;
        }
        pTerm = peTerm - pmTerm; // used by GetDterm()
        iTerm = ki * error;
        if (dmode == dMode::dOnError)
            dTerm = kd * dError;
        else
            dTerm = -kd * dInput; // dOnMeas

        // condition anti-windup (default)
        if (iawmode == iAwMode::iAwCondition)
        {
            bool aw = false;
            double iTermOut = (peTerm - pmTerm) + ki * (iTerm + error);
            if (iTermOut > outMax && dError > 0)
                aw = true;
            else if (iTermOut < outMin && dError < 0)
                aw = true;
            if (aw && ki)
                iTerm = constrain(iTermOut, -outMax, outMax);
        }

        // by default, compute output as per PID_v1
        outputSum += iTerm; // include integral amount
        if (iawmode == iAwMode::iAwOff)
            outputSum -= pmTerm; // include pmTerm (no anti-windup)
        else
            outputSum = constrain(outputSum - pmTerm, outMin, outMax);     // include pmTerm and clamp
        *myOutput = constrain(outputSum + peTerm + dTerm, outMin, outMax); // include dTerm, clamp and drive output

        lastError = error;
        lastInput = input;
        lastTime = now;
        return true;
    }
    else
        return false;
}

/* SetTunings(....)************************************************************
  This function allows the controller's dynamic performance to be adjusted.
  it's called automatically from the constructor, but tunings can also
  be adjusted on the fly during normal operation.
******************************************************************************/
void QuickPID::SetTunings(double Kp, double Ki, double Kd,
                          pMode pMode = pMode::pOnError,
                          dMode dMode = dMode::dOnMeas,
                          iAwMode iAwMode = iAwMode::iAwCondition)
{

    if (Kp < 0 || Ki < 0 || Kd < 0)
        return;
    if (Ki == 0)
        outputSum = 0;
    pmode = pMode;
    dmode = dMode;
    iawmode = iAwMode;
    dispKp = Kp;
    dispKi = Ki;
    dispKd = Kd;
    double SampleTimeSec = (double)sampleTimeUs / 1000000;
    kp = Kp;
    ki = Ki * SampleTimeSec;
    kd = Kd / SampleTimeSec;
}

/* SetTunings(...)************************************************************
  Set Tunings using the last remembered pMode, dMode and iAwMode settings.
******************************************************************************/
void QuickPID::SetTunings(double Kp, double Ki, double Kd)
{
    SetTunings(Kp, Ki, Kd, pmode, dmode, iawmode);
}

/* SetSampleTime(.)***********************************************************
  Sets the period, in microseconds, at which the calculation is performed.
******************************************************************************/
void QuickPID::SetSampleTimeUs(uint32_t NewSampleTimeUs)
{
    if (NewSampleTimeUs > 0)
    {
        double ratio = (double)NewSampleTimeUs / (double)sampleTimeUs;
        ki *= ratio;
        kd /= ratio;
        sampleTimeUs = NewSampleTimeUs;
    }
}

/* SetOutputLimits(..)********************************************************
  The PID controller is designed to vary its output within a given range.
  By default this range is 0-255, the Arduino PWM range.
******************************************************************************/
void QuickPID::SetOutputLimits(double Min, double Max)
{
    if (Min >= Max)
        return;
    outMin = Min;
    outMax = Max;

    if (mode != Control::manual)
    {
        *myOutput = constrain(*myOutput, outMin, outMax);
        outputSum = constrain(outputSum, outMin, outMax);
    }
}

/* SetMode(.)*****************************************************************
  Sets the controller mode to manual (0), automatic (1) or timer (2)
  when the transition from manual to automatic or timer occurs, the
  controller is automatically initialized.
******************************************************************************/
void QuickPID::SetMode(Control Mode)
{
    if (mode == Control::manual && Mode != Control::manual)
    { // just went from manual to automatic, timer or toggle
        QuickPID::Initialize();
    }
    if (Mode == Control::toggle)
    {
        mode = (mode == Control::manual) ? Control::automatic : Control::manual;
    }
    else
        mode = Mode;
}
void QuickPID::SetMode(uint8_t Mode)
{
    if (mode == Control::manual && Mode != 0)
    { // just went from manual to automatic or timer
        QuickPID::Initialize();
    }
    if (Mode == 3)
    { // toggle
        mode = (mode == Control::manual) ? Control::automatic : Control::manual;
    }
    else
        mode = (Control)Mode;
}

/* Initialize()****************************************************************
  Does all the things that need to happen to ensure a bumpless transfer
  from manual to automatic mode.
******************************************************************************/
void QuickPID::Initialize()
{
    outputSum = *myOutput;
    lastInput = *myInput;
    outputSum = constrain(outputSum, outMin, outMax);
}

/* SetControllerDirection(.)**************************************************
  The PID will either be connected to a direct acting process (+Output leads
  to +Input) or a reverse acting process(+Output leads to -Input).
******************************************************************************/
void QuickPID::SetControllerDirection(Action Action)
{
    action = Action;
}
void QuickPID::SetControllerDirection(uint8_t Direction)
{
    action = (Action)Direction;
}

/* SetProportionalMode(.)*****************************************************
  Sets the computation method for the proportional term, to compute based
  either on error (default), on measurement, or the average of both.
******************************************************************************/
void QuickPID::SetProportionalMode(pMode pMode)
{
    pmode = pMode;
}
void QuickPID::SetProportionalMode(uint8_t Pmode)
{
    pmode = (pMode)Pmode;
}

/* SetDerivativeMode(.)*******************************************************
  Sets the computation method for the derivative term, to compute based
  either on error or on measurement (default).
******************************************************************************/
void QuickPID::SetDerivativeMode(dMode dMode)
{
    dmode = dMode;
}
void QuickPID::SetDerivativeMode(uint8_t Dmode)
{
    dmode = (dMode)Dmode;
}

/* SetAntiWindupMode(.)*******************************************************
  Sets the integral anti-windup mode to one of iAwClamp, which clamps
  the output after adding integral and proportional (on measurement) terms,
  or iAwCondition (default), which provides some integral correction, prevents
  deep saturation and reduces overshoot.
  Option iAwOff disables anti-windup altogether.
******************************************************************************/
void QuickPID::SetAntiWindupMode(iAwMode iAwMode)
{
    iawmode = iAwMode;
}
void QuickPID::SetAntiWindupMode(uint8_t IawMode)
{
    iawmode = (iAwMode)IawMode;
}

/* Status Functions************************************************************
  These functions query the internal state of the PID.
******************************************************************************/
double QuickPID::GetKp()
{
    return dispKp;
}
double QuickPID::GetKi()
{
    return dispKi;
}
double QuickPID::GetKd()
{
    return dispKd;
}
double QuickPID::GetPterm()
{
    return pTerm;
}
double QuickPID::GetIterm()
{
    return iTerm;
}
double QuickPID::GetDterm()
{
    return dTerm;
}
uint8_t QuickPID::GetMode()
{
    return static_cast<uint8_t>(mode);
}
uint8_t QuickPID::GetDirection()
{
    return static_cast<uint8_t>(action);
}
uint8_t QuickPID::GetPmode()
{
    return static_cast<uint8_t>(pmode);
}
uint8_t QuickPID::GetDmode()
{
    return static_cast<uint8_t>(dmode);
}
uint8_t QuickPID::GetAwMode()
{
    return static_cast<uint8_t>(iawmode);
}
