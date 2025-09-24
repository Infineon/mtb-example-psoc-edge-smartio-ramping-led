/*******************************************************************************
 * File Name     : main.c
 *
 * Description   : This is the source code for the Ramping an LED using
 * Smart I/O Example in CM33 CPU non-secure environment.
 *
 * Related Document : See README.md
 *
 *******************************************************************************
 * Copyright 2023-2025, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 ******************************************************************************/

/*******************************************************************************
 * Header Files
 ******************************************************************************/
#include "cybsp.h"
#include "retarget_io_init.h"

/*******************************************************************************
 * Macros
 ******************************************************************************/
/* The timeout value in microsecond used to wait for the CM55 core to be booted.
 * Use value 0U for infinite wait till the core is booted successfully.
 */
#define CM55_BOOT_WAIT_TIME_USEC            (10U)

/* App boot address for CM55 project */
#define CM55_APP_BOOT_ADDR                  (CYMEM_CM33_0_m55_nvm_START + \
                                                CYBSP_MCUBOOT_HEADER_SIZE)

/*******************************************************************************
 * Function Definitions
 ******************************************************************************/
/*******************************************************************************
 * Function Name: main
 *******************************************************************************
 * Summary:
 *   This is the main function of the CM33 non-secure application.
 *   It does the following -
 *    1. Initialize the board, retarget-io and PWM.
 *    2. Initialize and start the Smart IO to generate a ramping signal to drive
 *       an external LED.
 *
 * Parameters:
 *  void
 *
 * Return:
 *  int
 *
 *******************************************************************************/
int main(void)
{
    cy_rslt_t result;
    cy_en_smartio_status_t smartio_status;
    cy_en_tcpwm_status_t tcpwm_status;

    /* Initialize the device and board peripherals. */
    result = cybsp_init();

    /* Board init failed. Stop program execution. */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Initialize retarget-io middleware */
    init_retarget_io();

    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen. */
    printf("\x1b[2J\x1b[;H");

    printf("\r\n*********** PSOC Edge MCU: Ramping LED using Smart IO ***********\r\n");

    /* Enable interrupts. */
    __enable_irq();

    /* Initialize the counter in the TCPWM block for the PWM operation. */
    tcpwm_status = Cy_TCPWM_PWM_Init(CYBSP_SMARTIO_PWM_HW, CYBSP_SMARTIO_PWM_NUM,
            &CYBSP_SMARTIO_PWM_config);

    if(CY_TCPWM_SUCCESS != tcpwm_status)
    {
        printf("TCPWM init failed with error: %lu\r\n",(unsigned long) tcpwm_status);
        CY_ASSERT(0);
    }

    /* Enable the TCPWM for PWM mode of operation. */
    Cy_TCPWM_PWM_Enable(CYBSP_SMARTIO_PWM_HW, CYBSP_SMARTIO_PWM_NUM);

    /* Start the TCPWM block. */
    Cy_TCPWM_TriggerStart_Single(CYBSP_SMARTIO_PWM_HW,
            CYBSP_SMARTIO_PWM_NUM);

    /* Initialize the SmartIO block. The Smart I/O implements a sequential
     * circuit to generate a square wave signal with time varying duty cycle.
     * This signal drives an LED creating visual perception of breathing LED. */
    smartio_status = Cy_SmartIO_Init(CYBSP_SMART_IO_11_HW, 
                                    &CYBSP_SMART_IO_11_config);

    if(CY_SMARTIO_SUCCESS != smartio_status)
    {
        printf("SmartIO init failed with error: %lu\r\n",(unsigned long) smartio_status);
        CY_ASSERT(0);
    }

    /* Enable the Smart I/O. */
    Cy_SmartIO_Enable(CYBSP_SMART_IO_11_HW);

    printf("\r\nSmartIO enabled. The output can be observed on pin P%d_%d.\r\n",
            CYBSP_SMART_IO_OUTPUT_PORT_NUM, CYBSP_SMART_IO_OUTPUT_PIN);

    printf("\r\nConnect P11[3](D3) to J7.4(USER_LED1) to see ramping effect on Board LED.\r\n");

    /* LED breathing effect is generated using PWM and Smart I/O
     * hardware peripherals - no CPU is used. */

    /* Enable CM55. */
    /* CM55_APP_BOOT_ADDR must be updated if CM55 memory layout is changed.*/
    Cy_SysEnableCM55(MXCM55, CM55_APP_BOOT_ADDR, CM55_BOOT_WAIT_TIME_USEC);

    for(;;)
    {

    }
}

/* [] END OF FILE */
