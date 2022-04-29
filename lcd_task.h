#ifndef _LCD_TASK_H_
#define _LCD_TASK_H_

#include <LiquidCrystal.h>
#include <stdint.h>

void LCD_Init();
void LCD_ResetState();
void LCD_Task();

extern LiquidCrystal lcd;

#endif