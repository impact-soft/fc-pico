/*
  Created by Fabrizio Di Vittorio (fdivitto2013@gmail.com) - <http://www.fabgl.com>
  Copyright (c) 2019-2022 Fabrizio Di Vittorio.
  All rights reserved.
 
 
* Please contact fdivitto2013@gmail.com if you need a commercial license.
 
 
* This library and related software is available under GPL v3.
 
  FabGL is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
 
  FabGL is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with FabGL.  If not, see <http://www.gnu.org/licenses/>.
 */
 
 
#pragma once
 
 
#include <stdint.h>
//#include "rp_sound.h"

 
 
 
class rp_fcemu {
 
public:
  void init();
  int reset();
  int IRQ();
    int NMI();

//  void setSND( rp_sound  *pSND ) { m_pSND = pSND; }

  void setPC(uint16_t addr) { m_PC = addr; }
  uint16_t getPC()          { return m_PC; }

  void run(uint16_t addr, uint8_t a, uint8_t x, uint8_t y );

  void setROM( const uint8_t *ROM ) {
  	m_ROM8000 = ROM;
  	m_ROMC000 = &ROM[0x4000];
  	m_ROME000 = &ROM[0x6000];
  }
  void setROM8000( const uint8_t *ROM ) { m_ROM8000 = ROM; }		// Sound data 16kb
  void setROMC000( const uint8_t *ROM ) { m_ROMC000 = ROM; }		// DPCM 8kb
  void setROME000( const uint8_t *ROM ) { m_ROME000 = ROM; }		// Sound Driver 8kb

  uint8_t   m_RAM[ 0x800 ];
//  uint8_t   m_APU[ 0x18 ];
  uint8_t readByte( uint16_t addr );
 
private:
  int step();

  void writeByte( uint16_t addr, uint8_t value );
  uint8_t readByteP0( uint16_t addr );
  void writeByteP0( uint16_t addr, uint8_t value );
  uint8_t readByteP1( uint16_t addr );
  void writeByteP1( uint16_t addr, uint8_t value );

  typedef void (rp_fcemu::*ADCSBC)(uint8_t);
 
  void OP_BINADC(uint8_t m);
  void OP_BINSBC(uint8_t m);
 
  void OP_BCDADC(uint8_t m);
  void OP_BCDSBC(uint8_t m);
 
  void setADCSBC();
 
 
  ADCSBC   m_OP_ADC;
  ADCSBC   m_OP_SBC;
 
  uint16_t m_PC;
  uint8_t  m_A;
  uint8_t  m_X;
  uint8_t  m_Y;
  uint8_t  m_SP;
 
  bool     m_carry;
  bool     m_zero;
  bool     m_intDisable;
  bool     m_decimal;
  bool     m_overflow;
  bool     m_negative;

  const uint8_t   *m_ROM8000;
  const uint8_t   *m_ROMC000;
  const uint8_t   *m_ROME000;
 
};
 

