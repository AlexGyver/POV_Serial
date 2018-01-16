/*
   POV Serial by AlexGyver
   https://github.com/AlexGyver/POV_Serial
   Сообщество ВК: https://vk.com/pov_light
*/

// ------------------ НАСТРОЙКИ ------------------
#define TRIGGER 12        // пин кнопки запуска анимации
#define SPACE 2           // интервал между буквами
#define CUSTOM_AMOUNT 8   // число спецсимволов
#define BLUETOOTH_MODE 0  // если схема с bluetooth модулем
// ------------------ НАСТРОЙКИ ------------------

// ------------------ ДЛЯ РАЗРАБОТЧИКОВ ------------------
#include "fonts.h"
#include "EEPROMex.h"

#if (BLUETOOTH_MODE == 1)    // если режим блютуса
#include <SoftwareSerial.h>
SoftwareSerial btSerial(10, 11); // RX, TX
#define Serial btSerial      // заменить Serial на btSerial во всём коде ниже
#endif

byte availableBytes, frames;
boolean recievedFlag;
String frameString = "";
String strData = "";
String speedSet = ".speed";
String customSet = ".custom";
String reverseSet = ".reverse";
String loopSet = ".loop";
byte Speed;
boolean loopMode, reverseMode, customMode;
boolean anim_flag, trigFlag, offlineMode;
// ------------------ ДЛЯ РАЗРАБОТЧИКОВ ------------------

void setup() {
  // ------ ПЕРВЫЙ ЗАПУСК -----
  if (EEPROM.read(100) != 1) {   // в ячейке 100 должен быть замисан флажок 1, если его нет - делаем
    EEPROM.writeByte(100, 1);
    EEPROM.updateByte(101, 40);
    EEPROM.updateByte(102, 0);
    EEPROM.updateByte(103, 0);
    EEPROM.updateByte(104, 5);
    EEPROM.updateByte(105, 0);
  }

  pinMode(TRIGGER, INPUT_PULLUP);            // пин кнопки подтянуть

  if (!digitalRead(TRIGGER)) {               // если при запуске нажата кнопка
    offlineMode = true;                      // запускаемся в оффлайн режиме
    DDRD = B11111111;                        // пины 0-7 как выходы
    PORTD = 0xff;                            // включить все
    while (!digitalRead(TRIGGER));           // ждём отпускания кнопки
    delay(100);
    PORTD = 0x00;                            // выключить все
  }
  while (!digitalRead(TRIGGER));             // ждём отпускания кнопки

  if (BLUETOOTH_MODE) {
    offlineMode = true;
    DDRD = B11111111;                        // пины 0-7 как выходы
    PORTD = 0x00;
  }
  /*
     EEPROM MAP
     0-64 - буфер
     100 - первый запуск
     101 - скорость
     102 - реверс
     103 - луп
     104 - длина буфера
     105 - загружен ли кастом
  */
  // загружаем настройки из памяти
  Speed = EEPROM.read(101);
  reverseMode = EEPROM.read(102);
  loopMode = EEPROM.read(103);
  frames = EEPROM.read(104);
  customMode = EEPROM.read(105);

  // загружаем буфер анимации из памяти
  for (byte i = 0; i < frames; i++) {
    frameString += (char)EEPROM.read(i);
  }

  if (!offlineMode || BLUETOOTH_MODE) {  // если не оффлайн режим ИЛИ блютус режим
    Serial.begin(9600);
    Serial.println(F("Welcome to GyverPOV!"));
    Serial.println(F("You can use some serial commands:"));
    Serial.println(F("Send any text то put in in the animation buffer"));
    Serial.println(F("Send .customNUM with NUM 0-7 to load custom symbol (example .custom3)"));
    Serial.println(F("Send .speedVALUE with VALUE 0-100 to set speed (example .speed40)"));
    Serial.println(F("Send .reverse0 to animation left-to-right or .reverse1 to right-to-left"));
    Serial.println(F("Send .loop0 to disable animation repeating or .loop1 to enable it"));
    Serial.println(F("Any changes will be instantly saved in EEPROM"));
    Serial.println(F("Hold trigger button before power up to load system in offline mode"));

    Serial.print(F("Loaded preset is "));
    if (customMode) Serial.print(F("custom symbol"));
    else {
      Serial.print(F("text <"));
      Serial.print(frameString);
      Serial.print(F(">"));
    }
    Serial.print(F(" with ")); Serial.print(frames);
    if (reverseMode) Serial.print(F(" reversed "));
    else Serial.print(F(" not reversed "));
    Serial.print(F("frames, "));
    Serial.print(Speed);
    Serial.print(F(" ms interval, "));
    if (loopMode) Serial.print(F("looped. "));
    else Serial.print(F("not looped. "));
    Serial.println(F("Enjoy!"));
    Serial.println();
  }
}

void loop() {
  //------------------------------------------- SERIAL LOOP ------------------------------------------
  for (;;) {         // главный подцикл, ожидает данные в Serial и отрабатывает кнопку
    triggerTick();
    availableBytes = Serial.available();         // считаем, сколько байт в буфере
    if (availableBytes > 0 && !anim_flag) {      // если есть что то на вход и НЕ показывается анимация
      delay(200);                                // ждём, пока придут остальные символы
      availableBytes = Serial.available();       // обновляем число байт в буфере
      strData = "";                              // очистить строку
      for (int i = 0; i < availableBytes; i++) {
        char newByte = Serial.read();
        strData += newByte;                      // забиваем строку принятыми данными
      }
      recievedFlag = true;                       // поднять флаг что получили данные
      break;                                     // выходим из цикла
    }
  }
  //------------------------------------------- SERIAL LOOP ------------------------------------------


  //----------------------------------------- ОБРАБОТКА ДАННЫХ ----------------------------------------
  if (recievedFlag) {
    // ----- НАСТРОЙКА СКОРОСТИ АНИМАЦИИ -----
    if (strData.startsWith(speedSet)) {         // если строка начинается с
      strData.remove(0, 6);                     // обрезать строку, оставить только значение
      byte newSpeed = strData.toInt();          // перевести в int
      if (newSpeed > 0 && newSpeed < 100) {     // если лежит в диапазоне
        Serial.print(F("Speed set: "));
        Speed = newSpeed;
        Serial.println(Speed);
        updateEEPROM();
      }
      else Serial.println(F("Wrong speed!"));   // если не лежит

      // ----- НАСТРОЙКА ЦИКЛИЧНОСТИ -----
    } else if (strData.startsWith(loopSet)) {
      strData.remove(0, 5);
      byte newLoop = strData.toInt();
      if (newLoop == 1) {
        loopMode = true;
        Serial.println(F("Loop mode on"));
        updateEEPROM();
      } else if (newLoop == 0) {
        loopMode = false;
        Serial.println(F("Loop mode off"));
      }
      else Serial.println(F("Wrong loop!"));

      // ----- НАСТРОЙКА РЕВЕРСА -----
    } else if (strData.startsWith(reverseSet)) {
      strData.remove(0, 8);
      byte newReverse = strData.toInt();
      if (newReverse == 1) {
        reverseMode = true;
        updateEEPROM();
        Serial.println(F("Reverse on"));
      } else if (newReverse == 0) {
        reverseMode = false;
        Serial.println(F("Reverse off"));
      }
      else Serial.println(F("Wrong reverse!"));

      // ----- СВОЙ СИМВОЛ -----
    } else if (strData.startsWith(customSet)) {
      strData.remove(0, 7);
      byte newCustom = strData.toInt();
      if (newCustom >= 0 && newCustom < CUSTOM_AMOUNT) {
        loadCustom(newCustom);
        Serial.print(F("Custom #"));
        Serial.print(newCustom);
        Serial.println(F(" is loaded"));
      }
      else Serial.println(F("Wrong custom number!"));
      customMode = true;
      updateEEPROM();

    } else {
      frameString = strData;
      frames = availableBytes;
      Serial.print(F("Animation text: "));
      Serial.println(frameString);
      customMode = false;
      updateEEPROM();
    }
    recievedFlag = false;
    availableBytes = 0;
  }
  //----------------------------------------- ОБРАБОТКА ДАННЫХ ----------------------------------------

}

// функция загрузки своего символа в строку (можно было сделать красивее, но мне было лень)
void loadCustom(byte newCustom) {
  switch (newCustom) {
    case 0: fillCustom(custom0, sizeof(custom0)); break;
    case 1: fillCustom(custom1, sizeof(custom1)); break;
    case 2: fillCustom(custom2, sizeof(custom2)); break;
    case 3: fillCustom(custom3, sizeof(custom3)); break;
    case 4: fillCustom(custom4, sizeof(custom4)); break;
    case 5: fillCustom(custom5, sizeof(custom5)); break;
    case 6: fillCustom(custom6, sizeof(custom6)); break;
    case 7: fillCustom(custom7, sizeof(custom7)); break;
  }
}

// забивка строки байтами символа
void fillCustom(uint8_t thisCustom[], byte frames_amount) {
  frames = frames_amount;
  frameString = "";
  for (int i = 0; i < frames; i++) {
    char newByte = thisCustom[i];
    frameString += newByte;                      // забиваем строку принятыми данными
  }
}

// ----------------------- ОТРАБОТКА КНОПКИ ---------------------
void triggerTick() {
  if (!digitalRead(TRIGGER) && !trigFlag) {
    trigFlag = true;
    anim_flag = true;  // бесполезный флаг, запрещающий общение с портом. Но бережёного бох бережёт
    if (!offlineMode)Serial.println(F("Animation start"));
    delay(20);
    if (!offlineMode)Serial.end();
    if (!offlineMode)DDRD = B11111111;  // пины 0-7 как выходы
    showAnimation();
    delay(10);
  }
  if (digitalRead(TRIGGER) && trigFlag) {
    if (!offlineMode)DDRD = B00000000;  // пины 0-7 как входы
    if (!offlineMode)Serial.begin(9600);
    delay(10);
    trigFlag = false;
    anim_flag = false;
    if (!offlineMode)Serial.println();
    if (!offlineMode)Serial.println(F("Animation end"));
    delay(100);
  }
}
// ----------------------- ОТРАБОТКА КНОПКИ ---------------------

void updateEEPROM() {
  // ----- СОХРАНЕНИЕ -----
  /*
    EEPROM MAP
    0-64 - буфер
    100 - первый запуск
    101 - скорость
    102 - реверс
    103 - луп
    104 - длина буфера
    105 - загружен ли кастом
  */

  for (byte i = 0; i < frames; i++) {
    EEPROM.updateByte(i, frameString[i]);
  }
  EEPROM.updateByte(101, Speed);
  EEPROM.updateByte(102, reverseMode);
  EEPROM.updateByte(103, loopMode);
  EEPROM.updateByte(104, frames);
  if (customMode) EEPROM.updateByte(105, 1);
  else EEPROM.updateByte(105, 0);
}

// -------------------------------- АНИМАЦИЯ ------------------------------
void showAnimation() {
  for (;;) {                        // бесконечный цикл отрисовки
    if (reverseMode) {              // если реверс
      if (!customMode) {            // если ТЕКСТ
        for (int8_t j = (frames - 1); j >= 0; j--) {
          char newRow = frameString[j];          // взять букву из строки
          for (int8_t i = 4; i >= 0; i--) {      // для пяти столбцов буквы
            PORTD = getFont(newRow, i);          // получить её битмап и отправить на светодиоды
            delay(Speed);                        // задержка кадра
          }
          for (byte i = 0; i < SPACE; i++) {     // буква написана, теперь вставляем пробелы
            PORTD = 0x00;                        // рисуем пустой столбец
            delay(Speed);                        // ждём
          }
        }
      } else {                      // если СИМВОЛ
        for (int8_t j = (frames - 1); j >= 0; j--) {
          PORTD = frameString[j];
          delay(Speed);
        }
        PORTD = 0x00;
      }
    } else {
      if (!customMode) {
        for (byte j = 0; j < frames; j++) {
          char newRow = frameString[j];
          for (byte i = 0; i < 5; i++) {
            PORTD = getFont(newRow, i);
            delay(Speed);
          }
          for (byte i = 0; i < SPACE; i++) {
            PORTD = 0x00;
            delay(Speed);
          }
        }
      } else {
        for (byte j = 0; j < frames; j++) {
          PORTD = frameString[j];
          delay(Speed);
        }
        PORTD = 0x00;
      }
    }


    if (!loopMode) {                      // если нарисовали всё что хотели, и режим зацикливания НЕ ВКЛЮЧЕН
      break;                              // покинуть цикл отрисовки
    }
    else if (digitalRead(TRIGGER)) {      // если цикл ВКЛЮЧЕН, но ОТПУЩЕНА кнопка, покинуть цикл отрисовки
      if (!offlineMode)DDRD = B00000000;  // пины как входы
      if (!offlineMode)Serial.begin(9600);  // запустить связь
      delay(10);
      trigFlag = false;
      anim_flag = false;
      if (!offlineMode)Serial.println();
      if (!offlineMode)Serial.println(F("Animation end"));
      delay(400);
      break;
    }
  }
}
// -------------------------------- АНИМАЦИЯ ------------------------------
