/*
 * bluetooth.c
 *
 *  Created on: 26.11.2024
 *
 */

#include <bluetooth.h>

int8_t bluetoothInit(BluetoothModule_t *BluetoothModule, USART_TypeDef *usart, uint32_t baudRate) {

if (usart == USART2){
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
		BluetoothATReplyBool_t reply;
		reply = bluetoothGetStatus(BluetoothModule);
		if(reply.status == 0 && reply.boolean == true){ // We are OK
			return ++BluetoothModule->initStatus;
		} else if (reply.status < 0){ // We have steps to do
			return BluetoothModule->initStatus;
		} else{ // Something went wrong, try again
			BluetoothModule->counter++;
			return --BluetoothModule->initStatus;
		}

		/*
	case -9:
		if (BluetoothModule->ATInProgress == false) {
			BluetoothModule->ATInProgress = true;
			usartSendString(USART2, (char*) "AT");
			return ++BluetoothModule->initStatus;
		} else {
			return BluetoothModule->initStatus;
		}
	case -8:
		BluetoothModule->ATInProgress=false;
		if (BluetoothModule->counter > 10) { // after 10 attempts return negative acknowledge
			return 0x15;
		}
		if (BluetoothModule->available == 2
				&& strncmp(BluetoothModule->messageBuffer, "OK", 2) == 0) {

			return ++BluetoothModule->initStatus;
		}

		else {
			BluetoothModule->counter++;
			return BluetoothModule->initStatus;
		}*/
	default:
		 gpioTogglePin(GPIOA, PIN10);
		return 0;
	}
}else{
	// Todo
}
}

BluetoothATReplyBool_t bluetoothGetStatus(BluetoothModule_t *BluetoothModule){

	if (BluetoothModule->usart == USART2){
		static int8_t status = -10;
		BluetoothATReplyBool_t returnValue = {status, false};
		switch (status) {
		case -10:


			if (BluetoothModule->ATInProgress == false) {
				BluetoothModule->ATInProgress = true;
				usartSendString(USART2, (char*) "AT");
				returnValue.status = status + 1; // We can go to the next step
				return returnValue;
			} else { // We couldn't send the command
				 gpioTogglePin(GPIOA, PIN10);

				return returnValue;
			}

		case -9:
			status = -10; // Last step, make sure to reset the status, otherwise we can't use the funcction anymore
			BluetoothModule->ATInProgress = false;

			if (BluetoothModule->available == 2
					&& strncmp(BluetoothModule->messageBuffer, "OK", 2) == 0) {

				returnValue.status = 0;
				return returnValue;
			}

			else {
				returnValue.status = 0x15;
				return returnValue;
			}
		case -8: //MAC Anfragen
		status=-7
		case -7: //MAC auswerten
			statu = 0
		case -6: // Verbindung abfragen

		default: // Unknown status, should never be called
			returnValue.status = 0x15;
			returnValue.boolean = false;
			return returnValue;
		}

	}else{
		//Todo
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
