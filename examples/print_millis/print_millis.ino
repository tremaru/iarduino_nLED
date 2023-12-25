#include <iarduino_nLED.h>             // Подключаем библиотеку для работы с последовательными 7-сегментными индикаторами.
iarduino_nLED disp(3, 6, 7);           // Создаём объект disp для работы с функциями и методами библиотеки iarduino_nLED, указав количество индикаторов и номера выводов: COUNT, SCL, SDI [,PWM].
                                       //
void setup(){                          //
     disp.clear();                     // Чистим дисплеи.
}                                      //
                                       //
void loop(){                           //
     float i=(float) millis()/1000;    // Получаем время в секундах с момента старта скетча в виде числа с плавающей точкой.
     disp.print( i, 1 );               // Выводим полученное число с одним знаком после запятой.
}                                      //