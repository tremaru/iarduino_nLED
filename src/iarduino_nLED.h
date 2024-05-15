//	Библиотека для работы с последовательными LED индикаторами: https://iarduino.ru/shop/Expansion-payments/7-segmentnyy-displey-86-65mm-krasnyy.html
//  Версия: 1.0.2
//  Последнюю версию библиотеки Вы можете скачать по ссылке: https://iarduino.ru/file/567.html
//  Подробное описание функции бибилиотеки доступно по ссылке: https://wiki.iarduino.ru/page/7-segment-display/
//  Библиотека является собственностью интернет магазина iarduino.ru и может свободно использоваться и распространяться!
//  При публикации устройств или скетчей с использованием данной библиотеки, как целиком, так и её частей,
//  в том числе и в некоммерческих целях, просим Вас опубликовать ссылку: http://iarduino.ru
//  Автор библиотеки: Панькин Павел
//  Если у Вас возникли технические вопросы, напишите нам: shop@iarduino.ru

#ifndef iarduino_nLED_h
#define iarduino_nLED_h

#if defined(ARDUINO) && (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#define nLED_ALL	0
#define nLED_LEFT	0
#define nLED_RIGHT	1
//      BIN			2
//      OCT			8
//      DEC			10
//      HEX			16

class iarduino_nLED{																//
																					//
	public:				iarduino_nLED( uint8_t, uint8_t, uint8_t, uint8_t=255 );	//	Подключение индикаторов.           ( количество, SCL, SDI [,PWM] ).                             ( 1-255, 0-255, 0-255 [,0-255] );
	/**	функции доступные пользователю **/											//
		void			clear ( void );												//	Очистка экрана.
		void			light ( uint8_t );											//	Установка яркости.                 ( яркость ).                                                 ( 0-255 ).
		void			point ( uint8_t, bool );									//	Установка точек.                   ( позиция, значение ).                                       ( 1-255/nLED_ALL, true/false ).
		void			setLED( uint8_t, uint8_t );									//	Установка светодиодов по битам.    ( позиция, значение ).                                       ( 1-255/nLED_ALL, byte ).
		void			align ( uint8_t i, char j=' '){ pos_ALG=i; pos_SYM=j; }		//	Установка выравнивания данных.     ( выравнивание [, замещающий символ] ).                      ( nLED_LEFT/nLED_RIGHT [,'символ'/0] ).
		void			hide  ( bool f ){ hide( 1, len_DAT, f ); }					//	Скрыть все данные.                 ( флаг ).                                                    ( true/false ).
		void			hide  ( uint8_t, uint8_t, bool );							//	Скрыть данные в указанном месте.   ( позиция, размер, флаг ).                                   ( 1-255, 1-255, true/false ).
		void			prnSTR( char*, uint8_t, uint8_t, uint8_t, char );			//	Вывод строки.                      ( строка, начало, размер, выравнивание, символ ).            ( число/символ/строка/массив, система счисления/количество знаков после запятой )
#ifdef RENESAS_CORTEX_M4															//
		void			print ( unsigned int num,     int param=255)				{	char i[11]; utoa (num,i,param==BIN?BIN:(param==OCT?OCT:(param==HEX?HEX:DEC))); prnSTR(i,1,len_DAT,pos_ALG,pos_SYM); } // 0...+65535
		void 			print ( int          num,     int param=255)				{	char i[12]; itoa (num,i,param==BIN?BIN:(param==OCT?OCT:(param==HEX?HEX:DEC))); prnSTR(i,1,len_DAT,pos_ALG,pos_SYM); } // 0...±32768
#else																				//
		void			print ( uint16_t     num, uint8_t param=255)				{	char i[ 6]; utoa (num,i,param==BIN?BIN:(param==OCT?OCT:(param==HEX?HEX:DEC))); prnSTR(i,1,len_DAT,pos_ALG,pos_SYM); } // 0...+65535
		void 			print ( int16_t      num, uint8_t param=255)				{	char i[ 7]; itoa (num,i,param==BIN?BIN:(param==OCT?OCT:(param==HEX?HEX:DEC))); prnSTR(i,1,len_DAT,pos_ALG,pos_SYM); } // 0...±32768
#endif																				//
		void			print ( uint32_t     num,     int param=255)				{	char i[11]; ultoa(num,i,param==BIN?BIN:(param==OCT?OCT:(param==HEX?HEX:DEC))); prnSTR(i,1,len_DAT,pos_ALG,pos_SYM); } // 0...+4294967295
		void			print ( int32_t      num,     int param=255)				{	char i[12]; ltoa (num,i,param==BIN?BIN:(param==OCT?OCT:(param==HEX?HEX:DEC))); prnSTR(i,1,len_DAT,pos_ALG,pos_SYM); } // 0...±2147483648
		void			print ( double       num,     int param=255)				{	char i[21]; if(param==255){param=2;} if(param>9){param=9;} int32_t j=num; uint32_t k=(fabs(num)-fabs(j))*1000000000; ltoa(j,i,10); if(param){uint8_t f=strlen(i); i[f]='.'; ltoa(k,&i[f+1],10); i[f+param+1]=0;} prnSTR(i,1,len_DAT,pos_ALG,pos_SYM);}
		void			print ( char*        str,     int param=255)				{	prnSTR(str,1,len_DAT,pos_ALG,pos_SYM);}
		void			print ( const char*  str,     int param=255)				{	prnSTR((char*)str,1,len_DAT,pos_ALG,pos_SYM);}
		void			print ( String       str,     int param=255)				{	uint8_t j=str.length()+1; char i[j]; str.toCharArray(i, j); prnSTR(i,1,len_DAT,pos_ALG,pos_SYM); }
																					//
	/**	переменные доступные пользователю **/										//
		uint8_t*		pnt_DAT = (uint8_t*) malloc(1);								//	Указатель на массив выводимых данных             (старший разряд данных в нулевом элементе массива).
																					//
	private:																		//
	/**	внутренние функции **/														//
		void			fnc_printDAT(void         );								//	Вывод массива pnt_DAT на все индикаторы.
		uint8_t			fnc_coder   (char         );								//	Kодирование символа в семисегментный вид.
																					//
	/**	внутренние переменные **/													//
		uint8_t			pin_SCL = 255;												//	Номер вывода назначенного для линии тактирования (Serial CLock)
		uint8_t			pin_SDI = 255;												//	Номер вывода назначенного для линии данных       (Serial Data Input)
		uint8_t			pin_PWM = 255;												//	Номер вывода назначенного для установки яркости  (Pulse Width Modulation)
		uint8_t*		flg_DAT = (uint8_t*) malloc(1);								//	Указатель на массив флагов сокрытия данных       (каждый бит является флагом указывающим не отображать данные на индикаторе).
		uint8_t 		len_DAT = 0;												//	Размер массива выводимых данных.
		uint8_t			pos_ALG = nLED_RIGHT;										//	Выравнивание данных.
		char			pos_SYM = ' ';												//	Замещающий символ.
};																					//
																					//
class create_place{																	//
																					//
	public:				create_place( iarduino_nLED&, uint8_t, uint8_t );			//	Подключение индикаторов.           ( указатель на объект, позиция, размер ).                    ( *, 1-255, 1-255 );
	/**	функции доступные пользователю **/											//
		void			clear ( void );												//	Очистка экрана.
		void			light ( uint8_t i ){ obj_LED->light(i); }					//	Установка яркости.                 ( яркость ).                                                 ( 0-255 ).
		void			point ( uint8_t, bool );									//	Установка точек.                   ( позиция, значение ).                                       ( 1-255/nLED_ALL, true/false ).
		void			setLED( uint8_t, uint8_t );									//	Установка светодиодов по битам.    ( позиция, значение ).                                       ( 1-255/nLED_ALL, byte ).
		void			align ( uint8_t i, char j=' '){pos_ALG=i; pos_SYM=j; }		//	Установка выравнивания данных.     ( выравнивание [, замещающий символ] ).                      ( nLED_LEFT/nLED_RIGHT [,'символ'/0] ).
		void			hide  ( bool f ){ hide( 1, pos_LEN, f ); }					//	Скрыть все данные.                 ( флаг ).                                                    ( true/false ).
		void			hide  ( uint8_t, uint8_t, bool );							//	Скрыть данные в указанном месте.   ( позиция, размер, флаг ).                                   ( 1-255, 1-255, true/false ).
#ifdef RENESAS_CORTEX_M4															//
		void			print ( unsigned int num, uint8_t param=255)				{	char i[11]; ultoa(num,i,param==BIN?BIN:(param==OCT?OCT:(param==HEX?HEX:DEC))); obj_LED->prnSTR(i,pos_LEF,pos_LEN,pos_ALG,pos_SYM); } // 0...+4294967295
		void			print ( int          num, uint8_t param=255)				{	char i[12]; ltoa (num,i,param==BIN?BIN:(param==OCT?OCT:(param==HEX?HEX:DEC))); obj_LED->prnSTR(i,pos_LEF,pos_LEN,pos_ALG,pos_SYM); } // 0...±2147483648
#else																				//
		void			print ( uint16_t     num, uint8_t param=255)				{	char i[ 6]; utoa (num,i,param==BIN?BIN:(param==OCT?OCT:(param==HEX?HEX:DEC))); obj_LED->prnSTR(i,pos_LEF,pos_LEN,pos_ALG,pos_SYM); } // 0...+65535
		void 			print ( int16_t      num, uint8_t param=255)				{	char i[ 7]; itoa (num,i,param==BIN?BIN:(param==OCT?OCT:(param==HEX?HEX:DEC))); obj_LED->prnSTR(i,pos_LEF,pos_LEN,pos_ALG,pos_SYM); } // 0...±32768
		void			print ( uint32_t     num, uint8_t param=255)				{	char i[11]; ultoa(num,i,param==BIN?BIN:(param==OCT?OCT:(param==HEX?HEX:DEC))); obj_LED->prnSTR(i,pos_LEF,pos_LEN,pos_ALG,pos_SYM); } // 0...+4294967295
		void			print ( int32_t      num, uint8_t param=255)				{	char i[12]; ltoa (num,i,param==BIN?BIN:(param==OCT?OCT:(param==HEX?HEX:DEC))); obj_LED->prnSTR(i,pos_LEF,pos_LEN,pos_ALG,pos_SYM); } // 0...±2147483648
#endif																				//
		void			print ( double       num, uint8_t param=255)				{	char i[21]; if(param==255){param=2;} if(param>9){param=9;} int32_t j=num; uint32_t k=(fabs(num)-fabs(j))*1000000000; ltoa(j,i,10); if(param){uint8_t f=strlen(i); i[f]='.'; ltoa(k,&i[f+1],10); i[f+param+1]=0;} obj_LED->prnSTR(i,pos_LEF,pos_LEN,pos_ALG,pos_SYM);}
		void			print ( char*        str, uint8_t param=255)				{	obj_LED->prnSTR(str,pos_LEF,pos_LEN,pos_ALG,pos_SYM);}
		void			print ( const char*  str, uint8_t param=255)				{	obj_LED->prnSTR((char*)str,pos_LEF,pos_LEN,pos_ALG,pos_SYM);}
		void			print ( String       str, uint8_t param=255)				{	uint8_t j=str.length()+1; char i[j]; str.toCharArray(i, j); obj_LED->prnSTR(i,pos_LEF,pos_LEN,pos_ALG,pos_SYM); }
																					//
	private:																		//
	/**	внутренние переменные **/													//
		iarduino_nLED*	obj_LED;													//	Указатель на объект работы с индикаторами.
		uint8_t			pos_ALG = nLED_RIGHT;										//	Выравнивание данных.
		char			pos_SYM = ' ';												//	Замещающий символ.
		uint8_t			pos_LEF = 1;												//	Позиция первого разряда.
		uint8_t			pos_LEN = 1;												//	Количество разрядов.
};																					//
																					//
#endif