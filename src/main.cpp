/*
 * Temp-Hum_meter.cpp
 *
 * Created: 17. 2. 2020 8:51:00
 * Author : Tekl7
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include "lib_files/USIWire/USIWire.h"
#include "lib_files/AVR-SSD1306-Library/SSD1306.h"
#include "lib_files/AVR-SSD1306-Library/Tahoma15x16.h"
#include "lib_files/HTU21D/HTU21D.h"


void init_wdt();
void print_title();
void print_TEMP();
void print_HUM();
void draw_battery(bool charged);



/* ---------------------------------------------------------------------- */



int main(void)
{
	// Wait for HTU21D to get stabilized
	_delay_ms(100);
	
	// Init display
	GLCD_Setup();
	GLCD_SetFont(Tahoma15x16, 15, 16);

	// Print permanent text
	GLCD_GotoXY(0, 0);
	print_title();
	GLCD_GotoX(0);
	GLCD_GotoLine(2);
	print_TEMP();
	GLCD_PrintChar(35);  // ":"
	GLCD_PrintChar(32);  // " "
	GLCD_GotoX(0);
	GLCD_GotoLine(5);
	print_HUM();
	GLCD_PrintChar(35);  // ":"
	GLCD_PrintChar(32);  // " "
	
	// Init HTU21D sensor
	HTU21D myHTU21D;
	myHTU21D.begin();
	
	init_wdt();
	
    while (1)
    {
		GLCD_FillRectangle(126, 62, 127, 63, GLCD_Black);  // Indication dot on

		bool batCharged = myHTU21D.batteryStatus();  // Get battery status
		float temp = myHTU21D.readTemperature();  // Get temperature
		float hum = myHTU21D.readCompensatedHumidity();  // Get humidity
		
		draw_battery(batCharged);

		// Print temperature
		GLCD_GotoX(50);
		GLCD_GotoLine(2);
		GLCD_PrintDouble(temp, 1);
		GLCD_PrintChar(32);  // " "
		GLCD_PrintChar(37);  // "C"
		
		// Print humidity
		GLCD_GotoX(50);
		GLCD_GotoLine(5);
		GLCD_PrintDouble(hum, 1);
		GLCD_PrintChar(32);  // " "
		GLCD_PrintChar(36);  // "%"
				
		GLCD_FillRectangle(126, 62, 127, 63, GLCD_White);  // Indication dot off

		// Sleep until watchdog interrupt
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_mode();
    }
}



/* ---------------------------------------------------------------------- */



ISR(WDT_vect)
{}

/* ---------------------------------------------------------------------- */

void init_wdt()
{
	/* https://electronics.stackexchange.com/questions/197615/avr-watchdog-interrupt-mode-and-reset-mode */

	// Just to be safe since we can not clear WDE if WDRF is set
	MCUSR &= ~_BV(WDRF);

	// Disable interrupts so we do not get interrupted while doing timed sequence
	cli();

	// First step of timed sequence, we have 4 cycles after this to make changes to WDE and WD timeout
	WDTCR |= _BV(WDCE) | _BV(WDE);

	// Timeout in 8 second, disable reset mode. Must be done in one operation
	WDTCR = _BV(WDP0) | _BV(WDP3);

	// Enable global interrupts
	sei();

	// Enable watchdog interrupt only mode
	WDTCR |= _BV(WDIE);
}

/* ---------------------------------------------------------------------- */

void print_title()
{
	print_TEMP();
	GLCD_PrintChar(33);  // "-"
	print_HUM();
	GLCD_PrintChar(32);  // " "
	GLCD_PrintChar(40);GLCD_PrintChar(43);GLCD_PrintChar(42);  // "MTR"
}

/* ---------------------------------------------------------------------- */

void print_TEMP()
{
	GLCD_PrintChar(43);GLCD_PrintChar(38);GLCD_PrintChar(40);GLCD_PrintChar(41);
}

/* ---------------------------------------------------------------------- */

void print_HUM()
{
	GLCD_PrintChar(39);GLCD_PrintChar(44);GLCD_PrintChar(40);
}

/* ---------------------------------------------------------------------- */

void draw_battery(bool charged)
{
	if (charged)
	{
		// Charged
		GLCD_FillRectangle(124, 6, 125, 7, GLCD_Black);
		GLCD_FillRectangle(122, 8, 127, 12, GLCD_Black);
	}
	else
	{
		// Discharged
		GLCD_FillRectangle(124, 6, 125, 7, GLCD_Black);
		GLCD_DrawRectangle(122, 8, 127, 12);
	}
}
