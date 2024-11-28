/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include <stdbool.h>

#include <mcalSysTick.h>
#include <mcalUsart.h>
#include <mcalGPIO.h>

#include <bluetooth.h>

#define BLUETOOTH_SETUP_TIME 100 //ms
#define USART2_BUFFER_SIZE 1200

typedef enum{
	MAIN_INIT=0,
	MAIN_LOOP
}MAIN_MODE;



bool timerTrigger;

volatile char usart2Buffer[USART2_BUFFER_SIZE];
volatile uint16_t usart2BufferIndex = 0;

int main(void)
{

	/*
	 * Für Testzwecke
	 */
	gpioSelectPort(GPIOA);
	gpioSelectPinMode(GPIOA, PIN10, OUTPUT);

	uint32_t BluetoothTimer = 0UL; //Unsigned Long to set all bits to zero
	uint32_t *timerList[] = { &BluetoothTimer };
	uint8_t arraySize = sizeof(timerList) / sizeof(timerList[0]);

	BluetoothModule_t HM17_1;

	MAIN_MODE mode = MAIN_INIT;
	for (;;) {
		switch (mode) {
		// Initialize
		case MAIN_INIT:
			systickInit(SYSTICK_1MS);
			bool setupFinished = false;

			int8_t init1Status = -127;
			HM17_1.initStatus=-10;
			while (setupFinished == false) {
				if (timerTrigger == true) {
					systickUpdateTimerList((uint32_t*) timerList, arraySize);

				}
				if (isSystickExpired(BluetoothTimer)) {
					init1Status = bluetoothInit(&HM17_1, USART2, 9600);
					gpioTogglePin(GPIOA, PIN10);
					systickSetTicktime(&BluetoothTimer, BLUETOOTH_SETUP_TIME);
				}
				if (init1Status == 0) {
					setupFinished = true; // If all finished
				}
			}
			gpioTogglePin(GPIOA, PIN10);
			mode=MAIN_LOOP; // After setup switch to main loop
			break; //case MAIN_INIT

		// Loop forever
		case MAIN_LOOP:
			break; // case MAIN_LOOP

		}
	}
}
