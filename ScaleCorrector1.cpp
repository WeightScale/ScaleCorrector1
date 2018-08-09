/*
 * ScaleCorrector.ino
 *
 * Created: 12/25/2017 9:02:01 PM
 * Author: Kostya
 */ 
#include "Arduino.h"
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include "CAT5171.h"
#include "WirelessRemoteController.h"
#include "HX711.h"
#include "CORE.h"

CoreClass CORE;

CAT5171 POT_PLUS(CAT5171_AD0);
CAT5171 POT_MINUS(CAT5171_AD1);
Q2HX711 hx711(PIN_DATA, PIN_CLOCK);

void setup(){	
	CORE.begin();
 	//CORE.standart();
	remoteController.init();
	hx711.powerUp();
	CORE.reset();
	core_value.offset = hx711.read();
	//sei();
		
}

void loop(){
	switch (remoteController.getCommand(4000)){
		case BUTTON_C_delay:			///< Войти в процесс калибровки плюсовой коррекции.
			CORE.doCalibration();
		break;
		case BUTTON_A:			///< Включить додавление процентов.
			CORE.doPlus();
		break;
		case BUTTON_B:			///< Включить снятие процениов.
			CORE.doMinus();
		break;
		case BUTTON_C:			///< Сбросить коррекцию
			POT_PLUS.twitch(10, 1000);
			hx711.powerUp();
			CORE.reset();
		break;
		case BUTTON_D:			///< Отключится от схемы
			POT_PLUS.twitch(10, 1000);
			CORE.standart();
		break;
		case BUTTON_D_delay:
			POT_PLUS.twitch(10, 1000);
			CORE.reset();
			core_value.offset = hx711.read();
			/*POT_PLUS.setResistance(0xFF);
			while(1){
				if (remoteController.readBitsFromPort()){
					switch(remoteController.getBits()){
						case ACTION_BUTTON_C:
						case ACTION_BUTTON_A:
						case ACTION_BUTTON_B:
							return;
						break;
					}
				}
			}*/
		break;	
	}
	
	/*if (remoteController.readBitsFromPort()){		
		switch(remoteController.getBits()){			
			case PLUS_CALIBRATION:			///< Войти в процесс калибровки плюсовой коррекции.
				CORE.doPlusCalibration();
			break;			
			case ACTION_BUTTON_A:			///< Включить додавление процентов.
				CORE.doPlus();
			break;
			case ACTION_BUTTON_B:			///< Включить снятие процениов.
				CORE.doMinus();
			break;
			case ACTION_BUTTON_C:			///< Сбросить коррекцию	
				POT_PLUS.twitch(10, 1000);											
				hx711.powerUp();		
				CORE.reset();
			break;
			case ACTION_BUTTON_D:			///< Отключится от схемы
				POT_PLUS.twitch(10, 1000);
				CORE.standart();
			break;
			case OFFSET_CALIBRATION:
				POT_PLUS.setResistance(0xFF);
				while(1){		
					if (remoteController.readBitsFromPort()){
						switch(remoteController.getBits()){
							case ACTION_BUTTON_C:
							case ACTION_BUTTON_A:
							case ACTION_BUTTON_B:
								return;
							break;
						}			
					}
				}
			break;
		}
	}*/
	/*if (remoteController.readBitsAfterTime(5000)){
		switch(remoteController.getBits()){
			case ACTION_BUTTON_C:			///< Войти в процесс калибровки плюсовой коррекции									
				CORE.doPlusCalibration();
			break;
			case ACTION_BUTTON_D:
				CORE.reset();
				core_value.factorO = hx711.read();
			break;
		}
	}*/
		
	int res = float(hx711.read() - core_value.offset) * core_value.factorO;//0.00003032051;//0.00002062979;//core_value.factorO;
	//core_value.r_adc = hx711.read();
	//int res = float(core_value.r_adc - core_value.offset) / core_value.factorO;
	if (res < 0){
		res = abs(res);
		int r = constrain(res, 0, 255);
		POT_PLUS.setResistance(0);
		POT_MINUS.setResistance(r);
	}else{
		int r = constrain(res, 0, 255);
		POT_PLUS.setResistance(r);
		POT_MINUS.setResistance(0);
	}
}








