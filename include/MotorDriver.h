#ifndef MY_MOTOR_DRIVER_H
#define MY_MOTOR_DRIVER_H
#include <Arduino.h>

class MotorDriver {
  
  private:
    byte en;
    byte cw;
    byte cs;
    byte pwm;
    byte ccw;
    
  public:
    MotorDriver(byte en, byte cw, byte ccw, byte pwm,byte cs);
    void init();
    void CW();
    void CCW();
    void off();
};

#endif