#include "protocol.h"
#include <Arduino.h>

uint8_t recv[PROTOCOL_DATA_SIZE];
boolean flash = false;
uint32_t flash_cnt = 0;
boolean led_on = false;
uint8_t recvIndex = 0;
uint8_t recvSizeCurrent = 0;
uint32_t lastReceiveTime;

void setup() {
  // initialize serial:
  Serial.begin(PROTOCOL_BAUD_RATE, PROTOCOL_SERIAL_CONFIG);
  Serial.setTimeout(1000);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
}

void printHex(uint8_t num) {
  char hexCar[2];

  sprintf(hexCar, "%02X ", num);
  Serial.print(hexCar);
}

uint8_t getCommandLen(uint8_t command) {
  switch (command) {
  // 0 byte data commands
  case PROTOCOL_A_START_DISCONNECT_STOP:
  case PROTOCOL_A_START_NO_DISCONNECT_STOP:
  case PROTOCOL_A_STOP_BELT:
  case PROTOCOL_A_AUTO_STOP:
  case PROTOCOL_A_AUTO_COOL_DOWN:
  case PROTOCOL_A_TOGGLE_TRANSMIT_ACK:
  case PROTOCOL_C_BELT_STATUS:
  case PROTOCOL_C_CURRENT_ACTUAL_SPEED:
  case PROTOCOL_C_CURRENT_ACTUAL_ELEVATION:
  case PROTOCOL_C_CURRENT_SET_SPEED:
  case PROTOCOL_C_CURRENT_SET_ELEVATION:
    return 0;

  // 4 byte data commands
  case PROTOCOL_A_SET_SPEED:
  case PROTOCOL_A_SET_ELEVATION:
    return 4;
  }

  return 0;
}

void processCommand() {
  flash = !flash;

  switch (recv[0]) {
  case PROTOCOL_A_START_DISCONNECT_STOP:
    Serial.write(PROTOCOL_B_START_DISCONNECT_STOP);
    break;
  case PROTOCOL_A_START_NO_DISCONNECT_STOP:
    Serial.write(PROTOCOL_B_START_NO_DISCONNECT_STOP);
    break;
  case PROTOCOL_A_STOP_BELT:
    Serial.write(PROTOCOL_B_STOP_BELT);
    break;
  case PROTOCOL_A_AUTO_STOP:
    Serial.write(PROTOCOL_B_AUTO_STOP);
    break;
  case PROTOCOL_A_AUTO_COOL_DOWN:
    Serial.write(PROTOCOL_B_AUTO_COOL_DOWN);
    break;
  case PROTOCOL_A_TOGGLE_TRANSMIT_ACK:
    Serial.write(PROTOCOL_B_TOGGLE_TRANSMIT_ACK);
    break;
  case PROTOCOL_C_BELT_STATUS:
    Serial.write(PROTOCOL_C_BELT_STATUS);
    break;
  case PROTOCOL_C_CURRENT_ACTUAL_SPEED:
    Serial.write(PROTOCOL_C_CURRENT_ACTUAL_SPEED);
    break;
  case PROTOCOL_C_CURRENT_ACTUAL_ELEVATION:
    Serial.write(PROTOCOL_C_CURRENT_ACTUAL_ELEVATION);
    break;
  case PROTOCOL_C_CURRENT_SET_SPEED:
    Serial.write(PROTOCOL_C_CURRENT_SET_SPEED);
    break;
  case PROTOCOL_C_CURRENT_SET_ELEVATION:
    Serial.write(PROTOCOL_C_CURRENT_SET_ELEVATION);
    break;

  // 4 byte data commands
  case PROTOCOL_A_SET_SPEED:
    Serial.write(PROTOCOL_B_SET_SPEED);
    break;
  case PROTOCOL_A_SET_ELEVATION:
    Serial.write(PROTOCOL_B_SET_ELEVATION);
    break;
  }

  /*
  for (int i = 0; i < recvSizeCurrent + 1; i++) {
    printHex(recv[i]);
    recv[i] = 0x42;
  }
  Serial.println();
  */
}

void loop() {
  if (flash) {
    flash_cnt++;
    if (flash_cnt >= 100000) {
      digitalWrite(LED_BUILTIN, led_on ? LOW : HIGH);
      led_on = !led_on;
      flash_cnt = 0;
    }
  }

  if (Serial.available()) {
    lastReceiveTime = millis();
    uint8_t c = (uint8_t)Serial.read();
    if (recvIndex == 0) {
      recvSizeCurrent = getCommandLen(c);
    }

    recv[recvIndex] = c;

    if (recvIndex >= recvSizeCurrent) {
      processCommand();
      recvIndex = 0;
    }

    recvIndex++;
  }

  if (millis() - lastReceiveTime >= 100) {
    recvIndex = 0;
  }
}