# DC Motor Control with Torque Adjustment

## Overview
This project implements DC motor control using an MP6551 motor driver. It allows speed and direction control via PWM and GPIO, and torque adjustment by modifying the SR potentiometer instead of using PWM.

## Features
- Speed control using PWM
- Direction control (Clockwise and Counter-Clockwise)
- Torque measurement via ADC
- Adjusting torque by modifying the SR potentiometer

## Hardware Requirements
- DC Motor
- MP6551 Motor Driver
- ATmega32 Microcontroller
- ADC and PWM capable pins

## Software Implementation
### 1. Initialization
- `DcMotor_Init(MotorSpecs *motor)`: Initializes motor Specs.
- `ADC_init()`: Preparing the ADC for torque measurement.
- `Timer0_init()`: Sets up PWM for speed control.

### 2. Motor Control Functions
- `DcMotorControlSpeed(RotationDirection direction, uint8 speed)`: Sets the motor speed and direction.
- `DcMotorGetTorque()`: Measures and returns the motor torque.
- `adjustSRforTorque(float32 targetTorque)`: Adjusts the SR potentiometer to achieve a specific torque.

## Usage
1. Call `DcMotor_Init()` with the motor specifications.
2. Use `DcMotorControlSpeed()` to control speed and direction.
3. Use `DcMotorGetTorque()` to monitor the torque.
4. Adjust SR using `adjustSRforTorque()` if necessary.

## Example Test Code
```c
int main() {
    ADC_init();
    Timer0_init();
    
    MotorSpecs myMotor = {3.0, 0.02, 1000, 1.3, 1094, 0.13, 0.8};
    DcMotor_Init(&myMotor);
    
    DcMotorControlSpeed(CW_ROTATION, 50);
    _delay_ms(2000);
    
    DcMotorControlSpeed(CCW_ROTATION, 75);
    _delay_ms(2000);
    
    DcMotorControlSpeed(NO_ROTATION, 0);
    
    float32 torque = DcMotorGerTorque();
    printf("Torque: %.2f kg.cm\n", torque);
    
    adjustSRforTorque(0.02);
    return 0;
}
```

## Notes
- Ensure the SR potentiometer is correctly calibrated for optimal results.
- Torque measurement accuracy depends on ADC precision.
- Speed control is PWM-based, but torque adjustment is done through SR instead of PWM to control the input current to the motor.

## Future Enhancements
- Implement closed-loop torque control.
- Add safety mechanisms for overcurrent protection.
- Integrate feedback from external sensors.

