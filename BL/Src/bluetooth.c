/*
 * bluetooth.c
 *
 *  Created on: 26.11.2024
 *
 */

#include <bluetooth.h>

int8_t bluetoothInit(BluetoothModule_t *BluetoothModule, USART_TypeDef *usart, uint32_t baudRate) {


	switch (BluetoothModule->initStatus) {
	case -10:
		BluetoothModule->usart = usart;
		BluetoothModule->baudRate = baudRate;
		BluetoothModule->available=0;
		BluetoothModule->counter = 0;
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // GPIOA :Bustakt aktivieren
		gpioSelectPinMode(GPIOA, PIN3, ALTFUNC); // PA2 :Modus = Alt. Funktion
		gpioSelectAltFunc(GPIOA, PIN3, AF7); // PA2 :AF7 = USART2 Rx
		gpioSelectPinMode(GPIOA, PIN2, ALTFUNC); // PA3 :Modus = Alt. Funktion
		gpioSelectAltFunc(GPIOA, PIN2, AF7); // PA3 :AF7 = USART2 Tx
		usartSelectUsart(usart);
		usartEnableUsart(usart);
		usartSetCommParams(usart, 9600, NO_PARITY, LEN_8BIT, ONE_BIT);
		usartEnableIrq(usart, USART_IRQ_RXNEIE);

		if (BluetoothModule->usart == USART2) {
			NVIC_EnableIRQ(USART2_IRQn);
		} else {
			//Todo
		}
		__enable_irq();
		return ++BluetoothModule->initStatus;

	case -9:
		if (BluetoothModule->counter > 10) { // after 10 attempts return negative acknowledge
			return 0x15;
		}
		bool reply;
		int16_t status;
		status = bluetoothGetStatus(BluetoothModule, &reply);
		if (status) { // We are OK
			return ++BluetoothModule->initStatus;
		} else if (status){ // We have steps to do
			return BluetoothModule->initStatus;
		} else{ // Something went wrong, try again
			BluetoothModule->counter++;
			return --BluetoothModule->initStatus;
		}


	default:
		 gpioTogglePin(GPIOA, PIN10);
		return 0;
	}

}

int16_t bluetoothStateHandler(BluetoothModule_t *BluetoothModule, int16_t state) {

	if (BluetoothModule->state == 0) {
		BluetoothModule->state = state;
	} else if (BluetoothModule->state % 10 != 0) { // We don't start at the beginning of a procedure
		return BluetoothWrongParameter;
	}
	if (!(BluetoothModule->state - state < 10)) { // We request a procedure, but are still in another one
		return BluetoothBusy;
	}
	switch (BluetoothModule->state) {
		case getStatus:
			usartSendString(USART2, (char*) "AT");
			return ++BluetoothModule->state;
			break;
		case getStatus_2:
			break;
		}

}
int16_t bluetoothGetStatus(BluetoothModule_t *BluetoothModule, bool * isOK){






			status = -10; // Last step, make sure to reset the status, otherwise we can't use the funcction anymore


			if (BluetoothModule->available == 2
					&& strncmp(BluetoothModule->messageBuffer, "OK", 2) == 0) {


				return returnValue;
			}

			else {

				return returnValue;
			}





}

bool bluetoothFetchBuffer(BluetoothModule_t *BluetoothModule) {
	if (BluetoothModule->usart == USART2) {
		static uint16_t lastIndex;
		if (usart2BufferIndex == lastIndex && usart2BufferIndex != 0) { // No new characters received, message is finished
			NVIC_DisableIRQ(USART2_IRQn); // Disable the IRQ while copying
			for (uint16_t x = 0; x < usart2BufferIndex; x++) {

				BluetoothModule->messageBuffer[BluetoothModule->available] =
						usart2Buffer[x];
				BluetoothModule->available++;
			}
			BluetoothModule->messageBuffer[BluetoothModule->available] = '\0';
			usart2BufferIndex = 0;
			NVIC_EnableIRQ(USART2_IRQn);
			usartSendString(USART2, &BluetoothModule->messageBuffer);
			return true;
		} else {
			lastIndex = usart2BufferIndex;
			return false;
		}

	} else {
		//Todo: Other USARTs
		return false;
	}
}

void USART2_IRQHandler(void) {
	if (USART2->SR & USART_SR_RXNE) {
#ifndef USART2_BUFFER_SIZE
#warning USART2_BUFFER_SIZE not defined, using 1200 Bytes buffer. This may result in lost characters
#define USART2_BUFFER_SIZE 1200 // One second at 9600 BAUD
#endif
		 usart2Buffer[usart2BufferIndex++]=USART2->DR & 0xFF;
		// gpioTogglePin(GPIOA, PIN10);

	}

}