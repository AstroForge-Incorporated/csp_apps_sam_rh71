/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
 * Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
 * ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include <unistd.h>
/***************************************
 * Check PWM outputs on pins
 * Channel 0 PWMH - PA00
 * Channel 0 PWML - PA04
 * Channel 1 PWMH - PA01
 * Channel 1 PWML - PA05
 * Channel 2 PWMH - PC12
 * Channel 2 PWML - PA06
 ***************************************/

/* Save PWM pwm_period */
uint16_t pwm_period;
#define MAX_DUTY 95

/* This function is called after PWM0 counter event */
void PWM0_CounterEventHandler(uint32_t status, uintptr_t context) {
    /* duty cycle values */
    static uint16_t duty0 = 625U;
    static uint16_t duty1 = 2500U;
    static uint16_t duty2 = 5000U;

    //    PWM0_ChannelDutySet(PWM_CHANNEL_0, duty0);
    //    PWM0_ChannelDutySet(PWM_CHANNEL_1, duty1);
    //    PWM0_ChannelDutySet(PWM_CHANNEL_2, duty2);
    //    
    //    /* Increment duty cycle values */
    ////    duty0 += DUTY_INCREMENT;
    //    duty1 += DUTY_INCREMENT;
    //    duty2 += DUTY_INCREMENT;
    //    
    //    if (duty0 > pwm_period)
    //        duty0 = 0U;
    //    if (duty1 > pwm_period)
    //        duty1 = 0U;
    //    if (duty2 > pwm_period)
    //        duty2 = 0U;
}

void print_usage() {
    printf("> set duty cycle [0-%d]: \r\n", MAX_DUTY);
}

void clear_buffer() {
    printf("-->clear_buffer()\r\n");
    bool success = true;
    while (FLEXCOM1_USART_ReceiverIsReady()) {
        uint8_t c = 0;
        success = FLEXCOM1_USART_Read(&c, 1);
        printf("%c", c);
    }
    printf("\r\n<--clear_buffer()\r\n");

}

double solve_cdty(double duty_cycle_percent, double f_channel_x_clock, double period) {
    double duty_cycle = 50.0 / 100.0;
    if (duty_cycle >= 1.0)
        duty_cycle = 0.999999; // avoid divide by zero
    else if (duty_cycle <= 0.0)
        duty_cycle = 0.000001;

    double cdty = 2.0 / ((1.0 - duty_cycle) * f_channel_x_clock * period);
    return cdty;
}
// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main(void) {
    /* Initialize all modules */
    SYS_Initialize(NULL);

    printf(" ------------------------------ \r\n");
    printf("     Avionics PWM Utility       \r\n");
    printf(" ------------------------------ \r\n");

    /* Register callback function for Channel 0 counter event */
    PWM0_CallbackRegister(PWM0_CounterEventHandler, (uintptr_t) NULL);

    /* Read the pwm_period */
    pwm_period = PWM0_ChannelPeriodGet(PWM_CHANNEL_0);

    /* Start all synchronous channels by starting channel 0*/
    PWM0_ChannelsStart(PWM_CHANNEL_0_MASK);

    int16_t duty_cycle_percent = 0;
    print_usage();
    while (true) {
        /* User input */
        if (FLEXCOM1_USART_ReceiverIsReady() == false) {
            continue;
        }

        int count = scanf("%4u", &duty_cycle_percent);
        if (count != 1) {
            clear_buffer();
            continue;
        } else if (duty_cycle_percent > MAX_DUTY || duty_cycle_percent < 0) {
            printf("Error, invalid duty cycle: %d%%\r\n", duty_cycle_percent);
        } else {
            printf("setting duty cycle to: %u%%\r\n", duty_cycle_percent);
            double cdty = ((double) (pwm_period * (100 - duty_cycle_percent)) / 100.0);
            printf("cdty: %f\r\n", cdty);
            PWM0_ChannelDutySet(PWM_CHANNEL_0, (uint16_t) cdty);
        }
        print_usage();
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE);
}


/*******************************************************************************
 End of File
 */

