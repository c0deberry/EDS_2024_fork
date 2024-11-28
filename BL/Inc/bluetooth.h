/*
 * bluetooth.h
 *
 *  Created on: 26.11.2024
 *
 */

#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#include <mcalUsart.h>
#include <mcalGPIO.h>


typedef struct BluetoothModule {

	USART_TypeDef *usart;
	int8_t initStatus; //Todo: Defaultwert mitgeben
	uint32_t baudRate;

}BluetoothModule_t;

int8_t bluetoothInit(BluetoothModule_t *BluetoothModule, USART_TypeDef *USART,
		uint32_t baudRate);

extern volatile char usart2Buffer[];
extern volatile uint16_t usart2BufferIndex;

#endif /* BLUETOOTH_H_ */