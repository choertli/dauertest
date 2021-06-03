


#include "MotorDriver.h"

MotorDriver::MotorDriver(byte en, byte cw, byte ccw, byte pwm,byte cs)
{
    // Use 'this->' to make the difference between the
    // 'pin' attribute of the class and the
    // local variable 'pin' created from the parameter.
    this->en = en;
    this->cw = cw;
    this->ccw = ccw;
    this->pwm = pwm;
    this->cs = cs;
    init();
    CW();
    CCW();
    off();
}
void MotorDriver::init()
{
    pinMode(en, OUTPUT);
    pinMode(cw, OUTPUT);
    pinMode(ccw, OUTPUT);
    pinMode(pwm, OUTPUT);
    pinMode(cs,INPUT);

    off();
}
void MotorDriver::CW()
{
    digitalWrite(cw, HIGH);
    digitalWrite(ccw, LOW);
}
void MotorDriver::CCW()
{
    digitalWrite(ccw, HIGH);
    digitalWrite(cw, LOW);
}

void MotorDriver::off()
{
    digitalWrite(cw, LOW);
    digitalWrite(ccw, LOW);
    digitalWrite(en, HIGH);
    digitalWrite(pwm, HIGH);
}
