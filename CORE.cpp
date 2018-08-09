#include "CORE.h"
#include "WirelessRemoteController.h"
#include "HX711.h"

value_t EEMEM core_value_eep;
value_t core_value;



CoreClass::CoreClass(){	
};
	
CoreClass::~CoreClass(){};

void CoreClass::begin(){
	Wire.begin();
	Wire.setClock(400000);	
	eeprom_read_block (&core_value, &core_value_eep, sizeof(value_t));	
	/*if (core_value.factorO == 0){
		core_value.factorO = OFFSET_HX711;
	}*/
}	

/*! 
*	\brief Функция калибровки плюсового значения. 
*	
*/
void CoreClass::doCalibration(){
	unsigned char p0 = 0;					///< Для визуального определения что вошли в калибровку.
	CORE.reset();
	delay(100);	
	core_value.offset = hx711.read();
	core_value.l_adc = 0;
	for(p0 = 0; p0 < 255; p0++){
		POT_PLUS.setResistance(p0);
		delay(20);
		long a = hx711.read();
		if (a > core_value.l_adc){
			core_value.l_adc = a;
		}else if(a <= core_value.l_adc){
			p0++;
			POT_PLUS.setResistance(p0);
			a = hx711.read();
			if (a > core_value.l_adc)
				core_value.l_adc = a;
			else{
				core_value.corrMtoP = --p0;
				break;
			}				
		}else{
			core_value.corrMtoP = p0;
			break;	
		}
	}
	core_value.corrMtoP = p0;
	CORE.reset();	
	core_value.l_adc -= core_value.offset;
	float fp = 0,fm = 0, f = 0;
	for(int i=0; i<FACTOR_MAX; i++){
		switch(i){
			case 0:
				fp=0.075;
				fm=-0.075;
			break;
			case 1:
				fp=0.165;
				fm=-0.165;
			break;
			case 2:
				fp=0.21;
				fm=-0.30;
			break;
			case 3:
				fp=0.26;
				fm=-0.35;
			break;
		}		
		//fp+=0.05;
		//fm+=-0.075;
		//core_value.factorP[i] = float(float(core_value.corrMtoP)* f) / float(core_value.l_adc);
		core_value.factorP[i] = float(float(core_value.corrMtoP)* fp) / float(core_value.l_adc);
		core_value.factorM[i] = float(float(core_value.corrMtoP)* fm) / float(core_value.l_adc);
	}	
	eeprom_update_block (&core_value, &core_value_eep, sizeof(value_t));
	while(1){
		switch (remoteController.getCommand(3000)){
			case BUTTON_A_delay:				///< Добатить вес.
				while(1){
					if (remoteController.readBitsPortToTime(2000)){
						switch(remoteController.getBits()){
							case ACTION_BUTTON_A:				///< Добатить вес.
								POT_PLUS.setResistance(++p0);
							break;
							case ACTION_BUTTON_B:				///< Отнять вес.
								POT_PLUS.setResistance(--p0);
							break;
							case ACTION_BUTTON_C:
								int p = POT_PLUS.getResistance();
								core_value.r_adc = hx711.read();
								core_value.factorO = float(p) / float(core_value.r_adc - core_value.offset);
								core_value.corrMtoP = p0;
								eeprom_update_block (&core_value, &core_value_eep, sizeof(value_t));
								POT_PLUS.setResistance(0);
								POT_PLUS.twitch(10, 1000);
								POT_PLUS.setResistance(p0);
								return;
							break;
						}
					}
				}
			break;
			case BUTTON_B_delay:				///< Отнять вес.
				while(1){
					if (remoteController.readBitsPortToTime(2000)){
						switch(remoteController.getBits()){
							case ACTION_BUTTON_A:				///< Добатить вес.
								POT_MINUS.setResistance(--p0);
							break;
							case ACTION_BUTTON_B:				///< Отнять вес.
								POT_MINUS.setResistance(++p0);
							break;
							case ACTION_BUTTON_C:
								int p = (POT_MINUS.getResistance() * (-1));
								core_value.r_adc = hx711.read();
								core_value.factorO = float(p) / float(core_value.r_adc - core_value.offset);
								core_value.corrMtoP = p0;
								eeprom_update_block (&core_value, &core_value_eep, sizeof(value_t));
								POT_MINUS.setResistance(0);
								POT_MINUS.twitch(10, 1000);
								POT_MINUS.setResistance(p0);
								return;
							break;
						}
					}
				}
			break;
			case BUTTON_C:				///< Калибровка наклона сдвига отклонения от реального веса.
				//p0 = POT_PLUS.getResistance();
				p0 = 0;
				core_value.r_adc = hx711.read();
				core_value.factorO = float(p0) / float(core_value.r_adc - core_value.offset);
				core_value.corrMtoP = p0;
				eeprom_update_block (&core_value, &core_value_eep, sizeof(value_t));
				POT_PLUS.setResistance(0);
				POT_PLUS.twitch(10, 1000);
				POT_PLUS.setResistance(p0);
				goto calout;
			break;
			case BUTTON_D:				///< Выйти без сохранения.
				goto calout;
			break;	
		}
		
		/*if (remoteController.readBitsPortToTime(2000)){			
			switch(remoteController.getBits()){				
				case ACTION_BUTTON_A:				///< Добатить вес.
					POT_PLUS.setResistance(++p0);
				break;
				case ACTION_BUTTON_B:				///< Отнять вес.
					POT_PLUS.setResistance(--p0);
				break;
				case ACTION_BUTTON_C:	
				case OFFSET_CALIBRATION:			///< Калибровка наклона сдвига отклонения от реального веса.
					p0 = POT_PLUS.getResistance();
					core_value.r_adc = hx711.read();
					core_value.factorO = float(p0) / float(core_value.r_adc - core_value.offset);
					core_value.corrMtoP = p0;	
					eeprom_update_block (&core_value, &core_value_eep, sizeof(value_t));
					POT_PLUS.setResistance(0);
					POT_PLUS.twitch(10, 1000);					
					POT_PLUS.setResistance(p0);	
					goto calout;
				break;
				case ACTION_BUTTON_D:				///< Выйти без сохранения.
					goto calout;
				break;
			}
		}*/
	}	
	calout:
	{
		POT_PLUS.setResistance(0);					///< Для визуального определения что вышли в корректировку плюсования.
		POT_PLUS.twitch(10, 1000);		
	}
}

/*void CoreClass::plusCalibration(uint8_t * p){
	while(1){
		if (remoteController.readBitsPortToTime(2000)){
			switch(remoteController.getBits()){
				case ACTION_BUTTON_A:				///< Добатить вес.
					POT_PLUS.setResistance(++p);
				break;
				case ACTION_BUTTON_B:				///< Отнять вес.
					POT_PLUS.setResistance(--p);
				break;
				case ACTION_BUTTON_C:
				break;				
			}	
		}
	}
}*/

	

void CoreClass::doPlus(){
	int i = 0;
	POT_PLUS.twitch(10, 2000);																		///< Для визуального определения что вошли в корректировку плюсования.
	POT_MINUS.setResistance(0);
	while(1){			
		int res = float(hx711.read() - core_value.offset) * (core_value.factorP[i] + core_value.factorO);		///< Вычисляем значение сопротивления для корекции.
		//res = abs(res);
		//res = constrain(res, 0, 255);																///< Чтобы не вышло из диапазона.
		//POT_PLUS.setResistance(res);
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
		if (remoteController.readBitsFromPort()){
			switch(remoteController.getBits()){
				case ACTION_BUTTON_A:
					int t;
					if (++i >3){
						i = 0;
					}
					t = (i+1) * 10;
					POT_PLUS.twitch(t, 2000);														///< Для визуального определения что вышли в корректировку плюсования.					
				break;
				case ACTION_BUTTON_C:
					POT_PLUS.twitch(10, 500);														///< Для визуального определения что вышли в корректировку плюсования.
					POT_PLUS.reset();
					return;
				case ACTION_BUTTON_B:
					goto _minus;
			}			
		}				
	}
	_minus: ;
	{
		remoteController.setFlagVT(true);											///< Устанавливаем флаг чтобы кнопка минус сработала	
	}
}

void CoreClass::doMinus(){
	int i = 0;			
	POT_MINUS.twitch(10, 2000);														///< Для визуального определения что вошли в корректировку минусования.
	POT_PLUS.setResistance(0);
	while(1){
		int res = float(hx711.read() - core_value.offset) * (core_value.factorM[i] + core_value.factorO);///< Вычисляем значение сопротивления для корекции.
		//res = abs(res);			
		//res = constrain(res, 0, 255);												///< Чтобы не вышло из диапазона.
		//POT_MINUS.setResistance(res);
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
		if (remoteController.readBitsFromPort()){									///< Выходим из коректировки.
			switch(remoteController.getBits()){
				case ACTION_BUTTON_B:
				int t;
					if (++i >3){
						i = 0;
					}
					t = (i+1) * 10;
					POT_MINUS.twitch(t, 2000);										///< Для визуального определения что вышли в корректировку минусования.
				break;
				case ACTION_BUTTON_C:					
					POT_MINUS.twitch(10, 500);										///< Для визуального определения что вышли в корректировку минусования.
					POT_MINUS.reset();
				return;
				case ACTION_BUTTON_A:
					goto _plus;	
			}			
		}
	}
	_plus: ;
	{
		remoteController.setFlagVT(true);											///< Устанавливаем флаг чтобы кнопка плюс сработала	
	}
}

void CoreClass::standart(){
	hx711.powerDown();
	//hx711.powerUp();
	CORE.disconnect();
	while(1){		
		if (remoteController.readBitsFromPort()){
			switch(remoteController.getBits()){
				case ACTION_BUTTON_C:
				case ACTION_BUTTON_A:
				case ACTION_BUTTON_B:
					goto _exit;
				break;
			}			
		}
	}
	{
		_exit: ;
		hx711.powerUp();
		remoteController.setFlagVT(true);
	}
}

void CoreClass::disconnect(){
	POT_PLUS.shutdown();
	POT_MINUS.shutdown();
}

void CoreClass::reset(){
	POT_PLUS.setResistance(0);
	POT_MINUS.setResistance(0);
}


	
