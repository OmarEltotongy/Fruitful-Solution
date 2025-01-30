/*
 * ExampleUsage.c
 *
 *  Created on: Jan 29, 2025
 *      Author: Omar Eltoutongy
 */
#include "dc_motor.h"
#include "adc.h"
#include "gpio.h"
#include "timer.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>


int main(void)
{
	/*Initializing with Handpiece Motor
	 *
	 *Specification:
	*rated_voltage 		3 	 	Volts
    *rated_torque 		0.02 	Kg·Cm
    *rated_speed        1000 	RPM
    *rated_current      1.3		Amperes
    *no_load_speed      1094 	RPM
    *no_load_current    0.13 	Amperes
    *stall_current 	    ≤0.8	Amperes
	  */

    // Initialize the DC motor and set it up
	MotorSpecs myMotor = {3.0, 0.02, 1000, 1.3, 1094, 0.13, 0.8};
    DcMotor_Init(&myMotor);

	/* Initialize ADC */
	    ADC_init();

	    printf("Testing DC Motor Control:\n");

	   /* Test motor at different speeds and directions */
	   printf("Running CW at 50%% speed\n");
	   DcMotorControlSpeed(CW_ROTATION, 50);
	   _delay_ms(2000);

	   printf("Running CCW at 75%% speed\n");
	   DcMotorControlSpeed(CCW_ROTATION, 75);
	   _delay_ms(2000);

	   printf("Stopping Motor\n");
	   DcMotorControlSpeed(NO_ROTATION, 100);
	   _delay_ms(2000);

	   /* Test torque control */
	   float32 torque = DcMotorGetTorque();
	   printf("Measured Torque: %.2f kg.cm\n", torque);

	   /* Adjust SR for specific torque */
	   adjustSRforTorque(0.02);

    return 0;
}


