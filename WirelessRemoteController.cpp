/*
 * \brief Прием данных с беспроводного пульта дистанционного управления.
 *
 * \author Konst <kreogen@email.ua>
 * \date 25.12.2017
 * \license MIT License (contact me if too restrictive)
 * \copyright Copyright (c) 2018 Konst
 * \version 1.0
 */

#include "WirelessRemoteController.h"

WirelessRemoteController remoteController;

WirelessRemoteController::WirelessRemoteController(){
	pinMode(PIN_D0, INPUT);
	pinMode(PIN_D1, INPUT);
	pinMode(PIN_D2, INPUT);
	pinMode(PIN_D3, INPUT);
	
	pinMode(PIN_VT, INPUT);
}

WirelessRemoteController::~WirelessRemoteController(){};
	
void WirelessRemoteController::init(){
	PCICR |=(1<<PCIE2);
	PCMSK2 |=(1<<PCINT20);
	//addTrigger(test);
}

/*! \brief Считываем биты данных.
*
*	Метод проверяет если на пульте нажаты кнопки и мы их считываем первый раз, 
*	тогда считываем данные с порта и ставим флаг flagVT = true чтобы знать что данные  уже считаны.
*	Когда кнопка на пульте отпущена, тогда следующая проверка сбросит флаг flagVT = false.
*	
*	\return Возвращает true- если пришли новые данные с пульта. 
 */
bool WirelessRemoteController::readBitsFromPort(){	
	if(readBitVT()){
		if (flagVT){
			flagVT = false;	
			_pinsBit = PIND & MASK_PULT_PIN;
			return true;
		}						
	}else{
		flagVT = true;
	}
	return false;
}

bool WirelessRemoteController::readBitsPortToTime(long t){
	static long time = millis();
	static bool flag;
	/*if(readBitVT()){
		if (!flag){
			flag= true;
			time = millis();
			_pinsBit = PIND & MASK_PULT_PIN;
			return true;
		}else if ((time + t) < millis()){
				_pinsBit = PIND & MASK_PULT_PIN;
				delay(100);
				return true;	
		}			
	}else{
		flag = false;
	}*/
	
	if(readBitVT()){
		if (!flag){
			flag= true;
			time = millis();
			//_pinsBit = PIND & MASK_PULT_PIN;
			//return true;
		}else if ((time + t) < millis()){
				_pinsBit = PIND & MASK_PULT_PIN;
				delay(100);
				return true;	
		}			
	}else{
		_pinsBit = PIND & MASK_PULT_PIN;
		if (flag){
			flag = false;
			return true;	
		}
	}
	
	/*if(readBitVT()){
		if (flagVT){
			flagVT = false;	
			_pinsBit = PIND & MASK_PULT_PIN;
			return true;
		}
						
	}else{
		flagVT = true;
	}*/	

	return false;
}

bool WirelessRemoteController::readBitsAfterTime(long t){
	static long time = millis();
	static bool flag;
		
	if(readBitVT()){
		if (!flag){
			flag = true;
			time = millis();
			//_pinsBit = PIND & MASK_PULT_PIN;
			//return true;
		}else if ((time + t) < millis()){
				_pinsBit = PIND & MASK_PULT_PIN;
				//delay(100);
				return true;	
		}			
	}else{
		//_pinsBit = PIND & MASK_PULT_PIN;
		//if (flag){
			flag = false;
			//return true;	
		//}
	}
	
	return false;
}
 
bool WirelessRemoteController::readBitVT(){
	return (bool)digitalRead(PIN_VT);
}

void WirelessRemoteController::addTrigger(void (*function)()){
}

button WirelessRemoteController::getCommand(long t){
	static long time = millis();
	static bool flag;
	if(flagVT){
	//if(readBitVT()){
		if (!flag){
			flag= true;
			time = millis();
			_pinsBit = PIND & MASK_PULT_PIN;
			//return true;
		}else if ((time + t) < millis()){
			_pinsBit = PIND & MASK_PULT_PIN;
			flag = false;
			switch(getBits()){
				case ACTION_BUTTON_A:
					return BUTTON_A_delay;
				break;
				case ACTION_BUTTON_B:
					return BUTTON_B_delay;
				break;
				case ACTION_BUTTON_C:
					return BUTTON_C_delay;
				break;
				case ACTION_BUTTON_D:
					return BUTTON_D_delay;
				break;	
			}
			//delay(100);
			//return NONE;
		}
		return NONE;
	}else{
		//_pinsBit = PIND & MASK_PULT_PIN;
		if (flag){
			flag = false;
			switch(remoteController.getBits()){
				case ACTION_BUTTON_A:
					return BUTTON_A;
				break;
				case ACTION_BUTTON_B:
					return BUTTON_B;
				break;
				case ACTION_BUTTON_C:
					return BUTTON_C;
				break;
				case ACTION_BUTTON_D:
					return BUTTON_D;
				break;
			}
			//return true;
		}
		return NONE;
	}	
}

ISR(PCINT2_vect){
	remoteController.setFlagVT(remoteController.readBitVT());
		
}