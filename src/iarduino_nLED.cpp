#include "iarduino_nLED.h"

//		ИНТЕРФЕЙС ПЕРЕДАЧИ ДАННЫХ:                         УСЛОВИЯ:
//		    ────┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌┐┌──┐        Запись каждого бита в сдвиговые регистры индикаторов
//		SCL     │^│^│^│^│^│^│^│^│^│^│^│^│^│^│^│^: │        осуществляется из линии SDI по фронту на линии SCL.
//			    └┘└┘└┘└┘└┘└┘└┘└┘└┘└┘└┘└┘└┘└┘└┘└┘: └────
//			    :               :               : :        Длительность тактирующих импульсов для записи в сдвиговые регистры.
//		        ┌─────┐       ┌─┐ ┌───┐   ┌───┐ : :        не должна превышать 16,5 мкс.
//		SDI     │     │       │ │ │   │   │   │ : :
//			────┘     └───────┘ └─┘   └───┘   └────────    Данные сдвиговых регистров не отображаются на индикаторах.
//			    :               :               : :
//			    :  ПЕРВЫЙ БАЙТ  :  ВТОРОЙ БАЙТ  : :        Наличие уровня логической «1» на линии SCL дольше 32,6 мкс
//			    :               :               : :        приведёт к записи данных из сдвиговых регистров в регистр защёлки.
//		        ║1│1│1│0│0│0│0│1║0│1│1│0│0│1│1│0║О║
//		        ║a│b│c│d│e│f│g│h║a│b│c│d│e│f│g│h║Т║        Данные регистра защёлки отображаются на индикаторах.
//              ║               ║               ║О║
//              ║       a       ║               ║Б║           a
//              ║      ───      ║               ║Р║          ───
//			    ║         │b    ║    f│ g │b    ║А║        f│ g │b
//			    ║               ║      ───      ║З║          ───
//			    ║         │c    ║         │c    ║И║        e│   │c
//			    ║           • h ║               ║Т║          ───  • h
//              ║               ║               ║Ь║           d
//
//		Конструктор класса:															//
		iarduino_nLED::iarduino_nLED(uint8_t a, uint8_t b, uint8_t c, uint8_t d){	//	Параметры: a - количество индикаторов (1-255), b - вывод SCL (0-254), c - вывод SDI (0-254) [, d - вывод PWM (0-254)].
			len_DAT = a;															//	Сохраняем количество подключённых индикаторов.
			pin_SCL = b;															//	Сохраняем номер вывода назначенного для линии тактирования (Serial CLock).
			pin_SDI = c;															//	Сохраняем номер вывода назначенного для линии данных       (Serial Data Input).
			pin_PWM = d;															//	Сохраняем номер вывода назначенного для установки яркости  (Pulse Width Modulation).
		//	Конфигурируем выводы:													//
			if( pin_SCL<255 ){ pinMode(pin_SCL, OUTPUT); digitalWrite(pin_SCL,0); }	//	Переводим вывод pin_SCL в режим выхода и устанавливаем уровень логического 0.
			if( pin_SDI<255 ){ pinMode(pin_SDI, OUTPUT); digitalWrite(pin_SDI,0); }	//	Переводим вывод pin_SDI в режим выхода и устанавливаем уровень логического 0.
			if( pin_PWM<255 ){ pinMode(pin_PWM, OUTPUT); digitalWrite(pin_PWM,1); }	//	Переводим вывод pin_PWM в режим выхода и устанавливаем уровень логической  1.
		//	Выделяем блок памяти под массив данных:									//
			free(pnt_DAT);															//	Освобождаем ранее созданный блок памяти под данные.
			free(flg_DAT);															//	Освобождаем ранее созданный блок памяти под флаги сокрытия данных.
			uint8_t i = len_DAT/8+(len_DAT%8?1:0);									//	Определяем количество байт требуемое для массива флагов сокрытия данных.
			pnt_DAT   = (uint8_t*) malloc(len_DAT);									//	Выделяем новый блок памяти под данные размером len_DAT байт.
			flg_DAT   = (uint8_t*) malloc(   i   );									//	Выделяем новый блок памяти под флаги  размером    i    байт.
			memset(pnt_DAT, 0, len_DAT);											//	Чистим все биты массива pnt_DAT.
			memset(flg_DAT, 0, i      );											//	Чистим все биты массива flg_DAT.
		//	Очистка всех разрядов.													//
			clear();																//	Очистка всех разрядов.
}																					//
																					//
//		Очистка экрана:																//
void	iarduino_nLED::clear(void){													//	Очистка всех разрядов.
			memset(pnt_DAT, 0, len_DAT); fnc_printDAT();							//	Чистим все биты массива pnt_DAT и выводим его на все индикаторы.
}																					//
																					//
//		Установка точек:															//
void	iarduino_nLED::point(uint8_t pos, bool data){								//	Параметры: pos - позиция (1-255 или nLED_ALL), data - значение (true/false).
		//	Проверяем параметры:													//
			if( (pos>len_DAT) && (pos!=nLED_ALL) ){ return; }						//	Позиция не должна превышать количество индикаторов.
		//	Определяем позицию крайних разрядов:									//
			uint8_t j=(pos==nLED_ALL)? 0:(pos-1);									//	Позиция левого разряда.
			uint8_t k=(pos==nLED_ALL)? len_DAT:pos;									//	Позиция за правым разрядом.
		//	Добавляем точки в массив pnt_DAT:										//
			for(uint8_t i=j; i<k; i++){												//	Проходим по разрядам.
				if( data ){ pnt_DAT[i] |= 0x80; }									//	Добавляем точку.
				else      { pnt_DAT[i] &= 0x7F; }									//	Удаляем точку.
			}																		//
			fnc_printDAT();															//	Выводим массив pnt_DAT на все индикаторы.
}																					//
																					//
//		Установка яркости:															//
void	iarduino_nLED::light( uint8_t pwm ){										//	Параметры: pwm - яркость (0-255).
			if( pin_PWM==255 ){ return; }											//	Проверяем наличие вывода pin_PWM.
			#if defined(TCCR0) || defined(TCCR1) || defined(TCCR2) || defined(TCCR0A) || defined(TCCR1A) || defined(TCCR2A) || defined(TCCR3A) || defined(TCCR4A) || defined(TCCR4C) || defined(TCCR5A)
				analogWrite( pin_PWM, pwm );										//	Устанавливаем ШИМ на выводе pin_PWM.
			#elif defined(_ESP32_HAL_LEDC_H_)										//
				ledcWrite  ( pin_PWM, pwm );										//	Устанавливаем ШИМ на канале pin_PWM.
			#endif																	//
}																					//
																					//
//		Установка светодиодов по битам:												//
void	iarduino_nLED::setLED(uint8_t pos, uint8_t data){							//	Параметры: pos - позиция (1-255 или nLED_ALL), data - значение (0-255).
		//	Проверяем параметры:													//
			if( (pos>len_DAT) && (pos!=nLED_ALL) ){ return; }						//	Позиция не должна превышать количество индикаторов.
		//	Определяем позицию крайних разрядов:									//
			uint8_t j=(pos==nLED_ALL)? 0:(pos-1);									//	Позиция левого разряда.
			uint8_t k=(pos==nLED_ALL)? len_DAT:pos;									//	Позиция за правым разрядом.
		//	Добавляем биты в массив pnt_DAT:										//
			for(uint8_t i=j; i<k; i++){												//	Проходим по разрядам.
				pnt_DAT[i] = data;													//	Устанавливаем биты разряда.
			}																		//
			fnc_printDAT();															//	Выводим массив pnt_DAT на все индикаторы.
}																					//
																					//
//		Скрыть данные в указанном месте:											//
void	iarduino_nLED::hide(uint8_t pos, uint8_t len, bool flg){					//	Параметры: pos - позиция (1-255), len - размер (1-255), flg - флаг (true/false).
			pos--;																	//
		//	Проверяем параметры:													//
			if( pos>=len_DAT      ){ return; }										//	Позиция первого скрываемого разряда не должна превышать количество индикаторов.
			if( (pos+len)>len_DAT ){ return; }										//	Позиция последнего скрываемого разряда не должна превышать количество индикаторов.
		//	Устанавливаем или сбрасываем флаги сокрытия данных:						//
			for(uint8_t i=pos; i<(pos+len); i++){									//	Проходим по разрядам.
				if(flg){ flg_DAT[i/8] |=   1<<(i%8) ;}								//	Устанавливаем бит флага.
				else   { flg_DAT[i/8] &= ~(1<<(i%8));}								//	Сбрасываем    бит флага.
			}																		//
			fnc_printDAT();															//	Выводим массив pnt_DAT на все индикаторы.
}																					//
																					//
//		Вывод строки в указанное место:												//
void	iarduino_nLED::prnSTR(char* str, uint8_t pos, uint8_t len, uint8_t alg, char sym){	//	Параметры: str - строка, pos - позиция (1-255), len - размер (1-255), alg - выравнивание (nLED_LEFT/nLED_RIGHT), sym - замещающий символ ('символ'/0)
		//	Проверяем параметры:													//
			if( pos==0              ){ return; }									//	Позиция первого разряда не может быть меньше 1.
			if( pos>len_DAT         ){ return; }									//	Позиция первого разряда не должна превышать количество индикаторов.
			if( (pos+len-1)>len_DAT ){ return; }									//	Позиция последнего разряда не должна превышать количество индикаторов.
		//	Добавляем строку str в массив pnt_DAT:									//
			uint8_t i=0, j=strlen(str);												//	i - позиция начала строки str, j - позиция конца строки str.
			bool f=0; pos--;														//	f - флаг наличия точки.
			for(int k=i; k<j; k++){ if(str[k]==','){str[k]='.';} }					//	Меняем запятые на точки.
		//	Если данные требуется выравнять по левому краю:							//
			if( alg==nLED_LEFT ){													//
			//	Проходим по сегментам слева на право:								//
				for(int k=pos; k<pos+len; k++){										//
				//	Если в строке str еще есть символы:								//
					if( i<j ){														//
					//	Добавляем код символа str[i]:								//	Симолы добавляем с начала строки str.
						if(str[i]=='.'){											//	Если текущий символ str[i] является '.', то ...
							if(!k){pnt_DAT[k]=0;}									//	Если текущий сегмент является первым слева, то стираем его.
							else{ if(f){pnt_DAT[k]=0;}else{k--;} }					//	Если предыдущим символом была '.', то стираем текущий сегмент, а если предыдущим символом была не точка, то возвращаемся к предыдущему сегменту.
							pnt_DAT[k]|=0x80; f=1;									//	Добавляем точку к текущему или предыдущему сегменту и устанавливаем флаг наличия точки.
						}else{														//	Если текущий символ str[i] не является '.', то ...
							pnt_DAT[k]=fnc_coder(str[i]); f=0;						//	То выводим его и сбрасываем флаг наличия точки.
						}															//
						i++;														//
				//	Если в строке str больше нет символов:							//
					}else if( sym ){												//
					//	Добавляем замещающий символ:								//
						pnt_DAT[k]=fnc_coder(sym);									//
					}																//
				}																	//
				if( i<j ){ if(str[i]=='.'){	pnt_DAT[pos+len-1]|=0x80; }}			//	Добавляем '.' если таковая имеется после всех выведенных символов.
		//	Если данные требуется выравнять по правому краю:						//
			}else{																	//
			//	Проходим по сегментам справа на лево:								//
				for(int k=pos+len-1; k>=pos; k--){									//
				//	Если в строке str еще есть символы:								//
					if( j ){														//
					//	Добавляем код символа str[j]:								//	Симолы добавляем начиная с конца строки str.
						j--;														//
						if(f){														//	Если предыдущим символом была '.'
							if(str[j]!='.'){f=0;}									//	Если текущий символ str[j] не является '.', то сбрасываем флаг f указывающий на то, что предыдущим символом была '.'
							pnt_DAT[k]=fnc_coder(str[j])|0x80;						//	Выводим текущий символ и добавляем к нему точку.
						}else if(str[j]=='.'){										//	Если текущим символом str[j] является '.', то ...
							f=1; k++;												//	Устанавливаем флаг наличия точки f и ничего не добавляем.
						}else{														//	Если ни предыдущий, ни текущий символ str[j] не является '.', то добавляем этот символ.
							pnt_DAT[k]=fnc_coder(str[j]);							//
						}															//
				//	Если в строке str больше нет символов:							//
					}else if( sym ){												//
					//	Добавляем замещающий символ:								//
						pnt_DAT[k]=fnc_coder(sym);									//
					}																//
				}																	//
			}																		//
			fnc_printDAT();															//	Выводим массив pnt_DAT на все индикаторы.
}																					//
																					//
//		Вывод массива pnt_DAT на все индикаторы:									//
void	iarduino_nLED::fnc_printDAT(void){											//	Параметры: отсутствуют.
			for(uint8_t i=len_DAT; i; i--){											//	Проходим по разрядам.
			//	Проверяем данные разряда на скрытность:								//
				uint8_t dat=(flg_DAT[(i-1)/8]&(1<<((i-1)%8)))?0:pnt_DAT[i-1];		//
			//	Выводим 7 сегментов без точки:										//
				for(uint8_t j=0; j<7; j++){											//	Проходим по сегментам.
					digitalWrite(pin_SDI, bitRead(dat,j) );							//	
					digitalWrite(pin_SCL, 1);										//
					digitalWrite(pin_SCL, 0);										//
				}																	//
			//	Выводим сегмент точки:												//
				digitalWrite(pin_SDI, bitRead(dat,7) );								//	
				if( i==1 ){															//
					digitalWrite(pin_SCL, 1);										//
					delayMicroseconds(50); 											//
					digitalWrite(pin_SCL, 0);										//
				}else{																//
					digitalWrite(pin_SCL, 1);										//
					digitalWrite(pin_SCL, 0);										//
				}																	//
			}																		//
}																					//
																					//
//		Kодирование символа в семисегментный вид:									//
uint8_t	iarduino_nLED::fnc_coder(char i){											//
			switch(i){																//
				case 'o': case 'O': case '0':	return 0x3F;	break;				//
				case 'i': case 'I': case '1':	return 0x06;	break;				//
									case '2':	return 0x5B;	break;				//
									case '3':	return 0x4F;	break;				//
									case '4':	return 0x66;	break;				//
				case 's': case 'S':	case '5':	return 0x6D;	break;				//
									case '6':	return 0x7D;	break;				//
									case '7':	return 0x07;	break;				//
									case '8':	return 0x7F;	break;				//
									case '9':	return 0x6F;	break;				//
				case 'a': case 'A':				return 0x77;	break;				//
				case 'b': case 'B':				return 0x7C;	break;				//
				case 'c': case 'C':				return 0x39;	break;				//
				case 'd': case 'D':				return 0x5E;	break;				//
				case 'e': case 'E':				return 0x79;	break;				//
				case 'f': case 'F':				return 0x71;	break;				//
				case 'g': case 'G':				return 0x3D;	break;				//
				case 'h': case 'H':				return 0x76;	break;				//
				case 'j': case 'J':				return 0x1E;	break;				//
				case 'l': case 'L':				return 0x38;	break;				//
				case 'n': case 'N':				return 0x54;	break;				//
				case 'p': case 'P':				return 0x73;	break;				//
				case 't': case 'T':				return 0x78;	break;				//
				case 'u': case 'U':				return 0x3E;	break;				//
				case '-':						return 0x40;	break;				//
				case '_':						return 0x08;	break;				//
				case '*':						return 0x63;	break;				//
				case '"': case '\'':			return 0x22;	break;				//
				default :						return 0x00;	break;				//
			}																		//
}																					//
																					//
//		Конструктор класса:															//
		create_place::create_place(iarduino_nLED& a, uint8_t b, uint8_t c){			//	Параметры: a - указатель на объект, b - позиция (1-255), c - размер (1-255).
			obj_LED = &a;															//	Сохраняем адрес указателя на объект работы с индикаторами.
			pos_LEF = b;															//	Сохраняем позицию первого разряда.
			pos_LEN = c;															//	Сохраняем количество разрядов.
}																					//
																					//
//		Очистка экрана:																//
void	create_place::clear(void){													//	Очистка всех разрядов.
			char i[pos_LEN+1]; memset(i, ' ', pos_LEN); i[pos_LEN]=0;				//	Создаём строку i состоящую из пробелов.
			obj_LED->prnSTR(i,pos_LEF,pos_LEN,pos_ALG,pos_SYM);						//	Выводим указанную строку.
}																					//
																					//
//		Установка точек:															//
void	create_place::point(uint8_t pos, bool data){								//	Параметры: pos - позиция (1-255 или nLED_ALL), data - значение (true/false).
		//	Проверяем параметры:													//
			if( (pos>pos_LEN) && (pos!=nLED_ALL) ){ return; }						//	Позиция не должна превышать количество индикаторов.
		//	Определяем позицию крайних разрядов:									//
			uint8_t j=(pos_LEF-1)+((pos==nLED_ALL)? 0:(pos-1));						//	Позиция левого разряда.
			uint8_t k=(pos_LEF-1)+((pos==nLED_ALL)? pos_LEN:pos);					//	Позиция за правым разрядом.
		//	Добавляем точки в массив pnt_DAT:										//
			for(uint8_t i=j; i<k; i++){												//	Проходим по разрядам.
				if( data ){ obj_LED->pnt_DAT[i] |= 0x80; }							//	Добавляем точку.
				else      { obj_LED->pnt_DAT[i] &= 0x7F; }							//	Удаляем точку.
			}																		//
			obj_LED->setLED(1, obj_LED->pnt_DAT[0]);								//	Выводим массив obj_LED->pnt_DAT на все индикаторы.
}																					//
																					//
//		Установка светодиодов по битам:												//
void	create_place::setLED(uint8_t pos, uint8_t data){							//	Параметры: pos - позиция (1-255 или nLED_ALL), data - значение (0-255).
		//	Проверяем параметры:													//
			if( (pos>pos_LEN) && (pos!=nLED_ALL) ){ return; }						//	Позиция не должна превышать количество индикаторов.
		//	Определяем позицию крайних разрядов:									//
			uint8_t j=(pos_LEF-1)+((pos==nLED_ALL)? 0:(pos-1));						//	Позиция левого разряда.
			uint8_t k=(pos_LEF-1)+((pos==nLED_ALL)? pos_LEN:pos);					//	Позиция за правым разрядом.
		//	Добавляем биты в массив pnt_DAT:										//
			for(uint8_t i=j; i<k; i++){												//	Проходим по разрядам.
				obj_LED->pnt_DAT[i] = data;											//	Устанавливаем биты разряда.
			}																		//
			obj_LED->setLED(1, obj_LED->pnt_DAT[0]);								//	Выводим массив obj_LED->pnt_DAT на все индикаторы.
}																					//
																					//
//		Скрыть данные в указанном месте:											//
void	create_place::hide(uint8_t pos, uint8_t len, bool flg){						//	Параметры: pos - позиция (1-255), len - размер (1-255), flg - флаг (true/false).
			pos--;																	//
		//	Проверяем параметры:													//
			if( pos>=pos_LEN      ){ return; }										//	Позиция первого скрываемого разряда не должна превышать количество индикаторов.
			if( (pos+len)>pos_LEN ){ return; }										//	Позиция последнего скрываемого разряда не должна превышать количество индикаторов.
		//	Скрываем данные:														//
			obj_LED->hide(pos_LEF+pos, len, flg);									//
}																					//
																					//
