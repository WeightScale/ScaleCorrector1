/*
 * \brief Получать данные с беспроводного пульта дистанционного управления (асинхронно / синхронно)
 *
 * Эта библиотека для IC2272 / 2272 (315 МГц и 433 МГц), так называемой SC2272. 
 * но может использоваться с любым подобным пультом дистанционного управления.
 *
 * \author Konst <kreogen@email.ua>
 * \date 25.12.2017
 * \license MIT License (contact me if too restrictive)
 * \copyright Copyright (c) 2018 Konst
 * \version 1.0
 */

#ifndef WirelessRemoteController_h
#define WirelessRemoteController_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define PIN_D0 0
#define PIN_D1 1
#define PIN_D2 2
#define PIN_D3 3
#define PIN_VT 4  /*! PORD4*/

//#define PLUS_CALIBRATION	12	///< Нажаты две кнопки C and A
//#define MINUS_CALIBRATION	9	///< Нажаты две кнопки C and B
//#define OFFSET_CALIBRATION	10	///< Нажаты две кнопки C and D калибровука отклонения от реального веса

/*---A--B--
  --C--D---*/
//#define PULT_0
/*---C--A--
  --D--B---*/
#define PULT_1


#ifdef PULT_0
	#define ACTION_BUTTON_A		4	///< Нажата кнопка
	#define ACTION_BUTTON_B		1	///< Нажата кнопка
	#define ACTION_BUTTON_C		8	///< Нажата кнопка
	#define ACTION_BUTTON_D		2	///< Нажата кнопка
#elif defined(PULT_1)
	#define ACTION_BUTTON_A		1	///< Нажата кнопка
	#define ACTION_BUTTON_B		2	///< Нажата кнопка
	#define ACTION_BUTTON_C		4	///< Нажата кнопка
	#define ACTION_BUTTON_D		8	///< Нажата кнопка
#endif



/*! A reference to an IID */
#define MASK_PULT_PIN		B00001111 ///< Маска пинов данных кнопок пульта

enum button{
	NONE,
	BUTTON_A,
	BUTTON_B,
	BUTTON_C,
	BUTTON_D,
	BUTTON_A_delay,
	BUTTON_B_delay,
	BUTTON_C_delay,
	BUTTON_D_delay
	};

class WirelessRemoteController{
	public:	
    /*!
     * \brief WirelessRemoteController Инициализируйте беспроводной пульт дистанционного управления.     
     */
	WirelessRemoteController();
	~WirelessRemoteController();
    void init();
	bool readBitsFromPort();
	bool readBitsPortToTime(long t);
	bool readBitsAfterTime(long t);
	bool readBitVT();
	button getCommand(long t);
	/*! 
	* \brief Почучаем считаные биты методом readBitsFromPort.	
	*	
	* Метод используется после вызова readBitsFromPort = true.
	* \return Возвращает биты нажатых кнопок на пульте. 
	*/
	byte getBits(){return _pinsBit;};

    /*!
     * \brief Запуск функции каждый раз, когда удаленный контроллер отправляет данные.	 
     *
     * PIN_VT цифровой триггерный входной пин (должен быть Interrupt пином: https://www.arduino.cc/en/Reference/AttachInterrupt)
     * \param function Функция, которая будет запускаться каждый раз, когда данные отправляются с пульта.
     */
	void addTrigger( void (*function)());
	void setFlagVT(bool f){flagVT = f;};

	private:
		byte _pinsBit;	///< Храним пины которые получили от пульта при считывании.
		bool flagVT;		
};

extern WirelessRemoteController remoteController;

#endif //WirelessRemoteController_h

