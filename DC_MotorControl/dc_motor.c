/*
 * dc_motor.c
 *
 *  Created on: Jan 29, 2025
 *      Author: Omar Eltoutongy
 */

#include "dc_motor.h"
#include "adc.h"
#include "common_macros.h" /* For GET_BIT Macro */
#include "gpio.h"
#include "std_types.h"
#include "timer.h"
#include <stdio.h>

/*******************************************************************************
 *                    		 Global Variables                                  *
 *******************************************************************************/

/* To make sure if any function needed any parameter of the motor, it can see them*/
static MotorSpecs motor_specs;


/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description :
 * Initialize the DC Motor:
 * 1. Setup the DC Motor pins directions by use the GPIO driver.
 * 2. Enable Driver.
 * 3. Initialize DC Motor State as Idle.
 */
void DcMotor_Init(const MotorSpecs *motor)
{
	/* Configure Pins as OUTPUT */
	GPIO_setupPinDirection(DC_MOTOR_INPUT1_PORT_ID,DC_MOTOR_INPUT1_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(DC_MOTOR_INPUT2_PORT_ID,DC_MOTOR_INPUT2_PIN_ID,PIN_OUTPUT);


	/* Initialize Pins as LOGIC_LOW */
	GPIO_writePin(DC_MOTOR_INPUT1_PORT_ID,DC_MOTOR_INPUT1_PIN_ID,LOGIC_LOW);
	GPIO_writePin(DC_MOTOR_INPUT2_PORT_ID,DC_MOTOR_INPUT2_PIN_ID,LOGIC_LOW);

	/*initialize specification of the motor*/

	/* Store motor specifications using arrow operator */
	motor_specs.rated_voltage    = motor->rated_voltage;
	motor_specs.rated_torque     = motor->rated_torque;
	motor_specs.rated_speed      = motor->rated_speed;
	motor_specs.rated_current    = motor->rated_current;
	motor_specs.no_load_speed    = motor->no_load_speed;
	motor_specs.no_load_current  = motor->no_load_current;
	motor_specs.stall_current    = motor->stall_current;

}

/*
 * Description :
 *	Detect which direction is used
 *	0- No Rotation
 *	1- CW Rotation
 *	2- CCW Rotation
 */
void setDirection(DcMotor_State direction)
{
	switch(direction)
		{
		case NO_ROTATION:
			GPIO_writePin(DC_MOTOR_INPUT1_PORT_ID,DC_MOTOR_INPUT1_PIN_ID,LOGIC_LOW);
			GPIO_writePin(DC_MOTOR_INPUT2_PORT_ID,DC_MOTOR_INPUT2_PIN_ID,LOGIC_LOW);
			break;
		case CW_ROTATION:
				GPIO_writePin(DC_MOTOR_INPUT1_PORT_ID,DC_MOTOR_INPUT1_PIN_ID,LOGIC_HIGH);
				GPIO_writePin(DC_MOTOR_INPUT2_PORT_ID,DC_MOTOR_INPUT2_PIN_ID,LOGIC_LOW);
				break;
		case CCW_ROTATION:
				GPIO_writePin(DC_MOTOR_INPUT1_PORT_ID,DC_MOTOR_INPUT1_PIN_ID,LOGIC_LOW);
				GPIO_writePin(DC_MOTOR_INPUT2_PORT_ID,DC_MOTOR_INPUT2_PIN_ID,LOGIC_HIGH);
				break;
		}
}

/*
 * Description :
 * Set Speed percentage of the DC Motor:
 * 1. Set Direction depending on argument.
 * 2. Set PWM using PWM Driver.
 */
void DcMotorControlSpeed(DcMotor_State direction,uint8 speed)
{
	setDirection(direction);

	// Constrain speed between 0-100%
	if (speed > 100) speed = 100;
	if (speed < 0  ) speed = 0;

	Timer0_pwmAdjust(speed); //send duty cycle to timer
}

/*
 * Description :
 * Set Torque percentage of the DC Motor:
 * 1. Set Direction depending on argument.
 * 2. Set PWM using PWM Driver.
 */
float32 DcMotorGetTorque()
{
 /*Torque is directly proportional to current.*/
	/*Read CM1 and CM2 from MP6551 driver, should be the same*/
	float32 CM1 = readMotorCurrent(CM1_ADC_CHANNEL);
	float32 CM2 = readMotorCurrent(CM2_ADC_CHANNEL);

    float32 current = (CM1 + CM2) / 2.0;

    // Map ADC value to current
	if (current < motor_specs.stall_current) {
		return 0.0; // Below stall current, assume no torque
	}
	/* Map current to torque */
	float32 torque = (current / motor_specs.rated_current) * motor_specs.rated_torque;

	/* Limit torque to rated value */
	if (torque > motor_specs.rated_torque) {
		increaseSRResistance();
	}
		return torque;

}

float32 readMotorCurrent(uint8 channel) {
    uint16 adc_value = ADC_readChannel(channel);
    float32 voltage = (adc_value / ADC_MAXIMUM_VALUE) * ADC_REF_VOLT_VALUE;

    /* Convert voltage to current using the sense resistor formula */
        float32 current = voltage / MP6551_SENSE_RESISTOR;

    return current ;
}

void adjustSRforTorque(float32 desired_torque) {
    /* Convert torque to required current */
    float32 required_current = (desired_torque / motor_specs.rated_torque) * motor_specs.rated_current;

    /* Read actual motor current from ADC */
    float32 actual_current = (readMotorCurrent(CM1_ADC_CHANNEL) + readMotorCurrent(CM2_ADC_CHANNEL)) / 2.0;

    /* If the actual current is higher than required, adjust SR potentiometer */
    if (actual_current > required_current) {
        /* Increase SR resistance to lower current */
    	increaseSRResistance();
    } else if (actual_current < required_current) {
        /*  Decrease SR resistance to allow more current */
    	decreaseSRResistance();
    }

}

void decreaseSRResistance()
{
	printf("You Should Decrease Resistance to Allow More Current\n");
}

void increaseSRResistance()
{
	printf("You Should Increase Resistance to Lower Current\n");
}

