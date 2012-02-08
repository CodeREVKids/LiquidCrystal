// ---------------------------------------------------------------------------
// Created by Francisco Malpartida on 20/08/11.
// Copyright 2011 - Under creative commons license 3.0:
//        Attribution-ShareAlike CC BY-SA
//LiquidCrystal
// This software is furnished "as is", without technical support, and with no 
// warranty, express or implied, as to its usefulness for any purpose.
//
// Thread Safe: No
// Extendable: Yes
//
// @file LiquidCrystal.cpp
// This file implements a basic liquid crystal library that comes as standard
// in the Arduino SDK.
// 
// @brief 
// This is a basic implementation of the LiquidCrystal library of the
// Arduino SDK. The original library has been reworked in such a way that 
// this class implements the all methods to command an LCD based
// on the Hitachi HD44780 and compatible chipsets using the parallel port of
// the LCD (4 bit and 8 bit).
//
// The functionality provided by this class and its base class is identical
// to the original functionality of the Arduino LiquidCrystal library.
//
//
// This library is only compatible with Arduino's SDK version 1.0
//
//
// @author F. Malpartida - fmalpartida@gmail.com
// ---------------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#if (ARDUINO <  100)
#include <WProgram.h>
#else
#include <Arduino.h>
#endif
#include <LiquidCrystal.h>

// STATIC helper functions
// ---------------------------------------------------------------------------


// CONSTRUCTORS
// ---------------------------------------------------------------------------
LiquidCrystal::LiquidCrystal(uint8_t rs, uint8_t rw, uint8_t enable,
                             uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                             uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
   init(LCD_8BIT, rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7);
}

LiquidCrystal::LiquidCrystal(uint8_t rs, uint8_t enable,
                             uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                             uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
   init(LCD_8BIT, rs, 255, enable, d0, d1, d2, d3, d4, d5, d6, d7);
}

LiquidCrystal::LiquidCrystal(uint8_t rs, uint8_t rw, uint8_t enable,
                             uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
   init(LCD_4BIT, rs, rw, enable, d0, d1, d2, d3, 0, 0, 0, 0);
}

LiquidCrystal::LiquidCrystal(uint8_t rs,  uint8_t enable,
                             uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
   init(LCD_4BIT, rs, 255, enable, d0, d1, d2, d3, 0, 0, 0, 0);
}

// PRIVATE METHODS
// ---------------------------------------------------------------------------


// init
void LiquidCrystal::init(uint8_t fourbitmode, uint8_t rs, uint8_t rw, uint8_t enable,
                         uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                         uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
   uint8_t i;
   
   // Initialize the IO pins
   // -----------------------
   
   _rs_pin = rs;
   _rw_pin = rw;
   _enable_pin = enable;
   
   _data_pins[0] = d0;
   _data_pins[1] = d1;
   _data_pins[2] = d2;
   _data_pins[3] = d3; 
   _data_pins[4] = d4;
   _data_pins[5] = d5;
   _data_pins[6] = d6;
   _data_pins[7] = d7;
   
   // Initialize the IO port direction to OUTPUT
   // ------------------------------------------
   
   for ( i = 0; i < 4; i++ )
   {
      pinMode ( _data_pins[i], OUTPUT );
   }
   
   // Initialize the rest of the ports if it is an 8bit controlled LCD
   // ----------------------------------------------------------------
   
   if ( !fourbitmode )
   {
      for ( i = 4; i < 8; i++ )
      {
         pinMode ( _data_pins[i], OUTPUT );
      }
   }
   pinMode(_rs_pin, OUTPUT);
   
   // we can save 1 pin by not using RW. Indicate by passing 255 instead of pin#
   if (_rw_pin != 255) 
   { 
      pinMode(_rw_pin, OUTPUT);
   }
   
   pinMode(_enable_pin, OUTPUT);
   
   // Initialise displaymode functions to defaults: LCD_1LINE and LCD_5x8DOTS
   // -------------------------------------------------------------------------
   if (fourbitmode)
      _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
   else 
      _displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;
   
   // Now we pull both RS and R/W low to begin commands
   digitalWrite(_rs_pin, LOW);
   digitalWrite(_enable_pin, LOW);
   
   if (_rw_pin != 255) 
   { 
      digitalWrite(_rw_pin, LOW);
   }
}

// PUBLIC METHODS
// ---------------------------------------------------------------------------

/************ low level data pushing commands **********/

// send
void LiquidCrystal::send(uint8_t value, uint8_t mode) 
{
   digitalWrite( _rs_pin, mode );
   
   // if there is a RW pin indicated, set it low to Write
   // ---------------------------------------------------
   if (_rw_pin != 255) 
   { 
      digitalWrite(_rw_pin, LOW);
   }
   
   if (_displayfunction & LCD_8BITMODE)
   {
      writeNbits(value, 8); 
   } 
   else 
   {
      writeNbits ( value >> 4, 4 );
      writeNbits ( value, 4 );
   }
   waitUsec ( EXEC_TIME ); // wait for the command to execute by the LCD
}

//
// pulseEnable
void LiquidCrystal::pulseEnable(void) 
{
   // There is no need for the delays, since the digitalWrite operation
   // takes longer.
   digitalWrite(_enable_pin, HIGH);   
   waitUsec(1);          // enable pulse must be > 450ns   
   digitalWrite(_enable_pin, LOW);
   waitUsec(20);
}

//
// write4bits
void LiquidCrystal::writeNbits(uint8_t value, uint8_t numBits) 
{
   for (uint8_t i = 0; i < numBits; i++) 
   {
      digitalWrite(_data_pins[i], (value >> i) & 0x01);
   }
   pulseEnable();
}
