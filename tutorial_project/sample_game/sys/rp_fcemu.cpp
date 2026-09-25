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

#include "Arduino.h"
 
#include "rp_system.h"
#include "rp_fcemu.h"
 
 
// compose status word
#define COMPOSE_STATUS (0x20 | ((int)m_carry) | ((int)m_zero << 1) | ((int)m_intDisable << 2) | ((int)m_decimal << 3) | ((int)m_overflow << 6) | ((int) m_negative << 7))
 
 
// decompose status word
#define DECOMPOSE_STATUS(status) { \
  int s = (status); \
  m_carry      = s & 0x01; \
  m_zero       = s & 0x02; \
  m_intDisable = s & 0x04; \
  m_decimal    = s & 0x08; \
  m_overflow   = s & 0x40; \
  m_negative   = s & 0x80; \
  setADCSBC();             \
}
 
 
#define BUSREAD(addr)           ( readByte( (addr) ) )
#define BUSWRITE(addr, value)   ( writeByte( (addr), (value)))
#define PAGE0READ(addr)         ( readByteP0( (addr)))
#define PAGE0WRITE(addr, value) ( writeByteP0( (addr), (value)))
#define PAGE1READ(addr)         ( readByteP1( (addr)))
#define PAGE1WRITE(addr, value) ( writeByteP1( (addr), (value)))
 
 
#define STACKPUSHBYTE(v) PAGE1WRITE(m_SP--, (v))
 
#define STACKPUSHWORD(v) { PAGE1WRITE(m_SP--, (v) >> 8); PAGE1WRITE(m_SP--, (v) & 0xff); }
 
#define STACKPOPBYTE()   PAGE1READ(++m_SP)
 
#define STACKPOPWORD()   (m_SP += 2, (PAGE1READ(m_SP - 1) | (PAGE1READ(m_SP) << 8)))
 


unsigned char rp_fcemu::readByte( uint16_t addr ) {
	if ( addr < 0x2000 ) {
		return m_RAM[ addr & 0x7ff ];
	}

	if ( addr < 0x8000 ) {
		return 0xff;
	}

	if ( addr < 0xC000 ) {
		return m_ROM8000[ addr & 0x3fff ];
	}
	if ( addr < 0xE000 ) {
		return m_ROMC000[ addr & 0x1fff ];
	}
	return m_ROME000[ addr & 0x1fff ];
}

void rp_fcemu::writeByte( uint16_t addr, uint8_t value ) {
	if ( addr < 0x2000 ) {
		m_RAM[ addr & 0x7ff ] = value;
	}
	if ( (addr >= 0x4000) && (addr <= 0x4017) ) {
		switch( addr ) {
		case 0x4015:
			value &= 0xf;
			break;
		
		}
		sys.setAPU( addr & 0x1F, value );
//		snd.setReg( addr & 0x1F, value );
//		m_APU[ addr & 0x1f ] =  value;
//		Serial.printf("SR 0x%04x %02x\n", addr , value );
	}

}

uint8_t rp_fcemu::readByteP0( uint16_t addr ) {
	return m_RAM[ addr & 0xff ];
}

void rp_fcemu::writeByteP0( uint16_t addr, uint8_t value ) {
	m_RAM[ addr & 0xff ] = value;
}

uint8_t rp_fcemu::readByteP1( uint16_t addr ) {
	return m_RAM[ (addr & 0xff) + 0x100 ];
}

void rp_fcemu::writeByteP1( uint16_t addr, uint8_t value ) {
	m_RAM[ (addr & 0xff) + 0x100 ] = value;
}


void rp_fcemu::run(uint16_t addr, uint8_t a, uint8_t x,uint8_t y ) {
	m_SP = 0xfd;  // transistor level simulators reset SP to 0xfd
	m_A = a;
	m_X = x;
	m_Y = y;
	m_PC = addr;
	for(;;) {
//		Serial.printf("PC 0x%04x %02x\n", m_PC, (BUSREAD(m_PC) & 0xff) );
		int ret = step();
		if ( ret == 0 ) break;
	}

}

void rp_fcemu::init() {
	memset( m_RAM, 0, sizeof(m_RAM));
	reset();
}

 
int rp_fcemu::reset() {
	m_A  = m_X = m_Y = 0;
	m_SP = 0xfd;  // transistor level simulators reset SP to 0xfd
	m_PC = (BUSREAD(0xfffd) << 8) + BUSREAD(0xfffc);  // reset vector at 0xfffc
	m_decimal = false;
	setADCSBC();
	return 6;
}
 
 
int rp_fcemu::IRQ()
{
    if (!m_intDisable) {
    STACKPUSHWORD(m_PC);
        STACKPUSHBYTE(COMPOSE_STATUS);
        m_PC = (BUSREAD(0xffff) << 8) + BUSREAD(0xfffe);  // IRQ vector at 0xfffe
    m_intDisable = true;
    return 7;
    }
  return 0;
}
 
 
int rp_fcemu::NMI()
{
  STACKPUSHWORD(m_PC);
    STACKPUSHBYTE(COMPOSE_STATUS);
    m_PC = (BUSREAD(0xfffb) << 8) + BUSREAD(0xfffa);  // NMI vector at 0xfffa
  m_intDisable = true;
    return 7;
}
 
 
void rp_fcemu::setADCSBC()
{
  if (m_decimal) {
    m_OP_ADC = &rp_fcemu::OP_BCDADC;
    m_OP_SBC = &rp_fcemu::OP_BCDSBC;
  } else {
    m_OP_ADC = &rp_fcemu::OP_BINADC;
    m_OP_SBC = &rp_fcemu::OP_BINSBC;
  }
}
 
 
void rp_fcemu::OP_BINADC(uint8_t m)
{
  uint32_t t = m + m_A + (int)m_carry;
  m_zero     = !(t & 0xff);
  m_negative = t & 0x80;
  m_overflow = !((m_A ^ m) & 0x80) && ((m_A ^ t) & 0x80);
  m_carry    = t & 0x100;
  m_A        = t & 0xff;
}
 
 
void rp_fcemu::OP_BINSBC(uint8_t m)
{
  uint32_t t = m_A - m - (int)(!m_carry);
  m_zero     = !(t & 0xff);
  m_negative = t & 0x80;
  m_overflow = ((m_A ^ t) & 0x80) && ((m_A ^ m) & 0x80);
  m_carry    = !(t & 0x100);
  m_A        = t & 0xff;
}
 
 
void rp_fcemu::OP_BCDADC(uint8_t m)
{
  uint32_t t = m + m_A + (int)m_carry;
  m_zero     = !(t & 0xff);
  if (((m_A & 0x0f) + (m & 0x0f) + (int)m_carry) > 9)
    t += 6;
  m_negative = t & 0x80;
  m_overflow = !((m_A ^ m) & 0x80) && ((m_A ^ t) & 0x80);
  if (t > 0x99)
    t += 96;
  m_carry    = t > 0x99;
  m_A        = t & 0xff;
}
 
 
void rp_fcemu::OP_BCDSBC(uint8_t m)
{
  uint32_t t = m_A - m - (int)(!m_carry);
  m_zero     = !(t & 0xff);
  m_negative = t & 0x80;
  m_overflow = ((m_A ^ t) & 0x80) && ((m_A ^ m) & 0x80);
  if (((m_A & 0x0f) - (int)(!m_carry)) < (m & 0x0f))
    t -= 6;
  if (t > 0x99)
    t -= 0x60;
  m_carry    = t < 0x100;
  m_A        = t & 0xff;
}
 
 
#define OP_AND {            \
  m_A        = m & m_A;     \
  m_zero     = !m_A;        \
  m_negative = m_A & 0x80;  \
}
 
 
#define OP_ASL {                \
  m_carry    = m & 0x80;        \
  m          = (m << 1) & 0xff; \
  m_zero     = !m;              \
  m_negative = m & 0x80;        \
}
 
 
#define OP_BIT {           \
  m_zero     = !(m & m_A); \
  m_overflow = m & 0x40;   \
  m_negative = m & 0x80;   \
}
 
 
#define OP_CMP {             \
    uint32_t t = m_A - m;      \
  m_zero     = !(t & 0xff);  \
    m_carry    = !(t & 0x100); \
    m_negative = t & 0x80;     \
}
 
 
#define OP_CPX {             \
    uint32_t t = m_X - m;      \
  m_zero     = !(t & 0xff);  \
    m_carry    = !(t & 0x100); \
    m_negative = t & 0x80;     \
}
 
 
#define OP_CPY {             \
    uint32_t t = m_Y - m;      \
  m_zero     = !(t & 0xff);  \
    m_carry    = !(t & 0x100); \
    m_negative = t & 0x80;     \
}
 
 
#define OP_DEC {                \
    m          = (m - 1) & 0xff;  \
  m_zero     = !m;              \
  m_negative = m & 0x80;        \
}
 
 
#define OP_EOR {            \
    m_A       ^= m;           \
  m_zero     = !m_A;        \
    m_negative = m_A & 0x80;  \
}
 
 
#define OP_INC {                \
  m          = (m + 1) & 0xff;  \
    m_negative = m & 0x80;        \
  m_zero     = !m;              \
}
 
 
#define OP_LDA {          \
  m_zero     = !m;        \
    m_negative = m & 0x80;  \
    m_A        = m;         \
}
 
 
#define OP_LDX {          \
  m_zero     = !m;        \
    m_negative = m & 0x80;  \
    m_X        = m;         \
}
 
 
#define OP_LDY {          \
  m_zero     = !m;        \
    m_negative = m & 0x80;  \
    m_Y        = m;         \
}
 
 
#define OP_LSR {          \
  m_negative = false;     \
    m_carry    = m & 0x01;  \
    m        >>= 1;         \
    m_zero     = !m;        \
}
 
 
#define OP_ORA {             \
    m_A        |= m;           \
    m_negative  = m_A & 0x80;  \
  m_zero      = !m_A;        \
}
 
 
#define OP_ROL {                         \
  m           = (m << 1) | (int)m_carry; \
  m_carry     = m & 0x100;               \
  m          &= 0xff;                    \
  m_negative  = m & 0x80;                \
  m_zero      = !m;                      \
}
 
 
#define OP_ROR {  \
  m          |= (int)m_carry << 8; \
  m_carry     = m & 0x01;          \
  m           = (m >> 1) & 0xff;   \
  m_negative  = m & 0x80;          \
  m_zero      = !m;                \
}
 
 
#define PAGECROSS() (al >> 8)
 
 
// immediate: op #dd
#define ADR_IMM {       \
  m = BUSREAD(m_PC++);  \
}
 
 
// zero page absolute: op aa
#define ADR_ZABS_GETADDR { \
  a = BUSREAD(m_PC++);     \
}
#define ADR_ZABS {         \
  ADR_ZABS_GETADDR;        \
  m = PAGE0READ(a);        \
}
 
 
// zero page absolute + X: op aa, X
#define ADR_ZABSX_GETADDR {           \
  a = (BUSREAD(m_PC++) + m_X) & 0xff; \
}
#define ADR_ZABSX {                   \
  ADR_ZABSX_GETADDR;                  \
  m = PAGE0READ(a);                   \
}
 
 
// zero page absolute + Y: op aa, Y
#define ADR_ZABSY_GETADDR {           \
  a = (BUSREAD(m_PC++) + m_Y) & 0xff; \
}
#define ADR_ZABSY {                   \
  ADR_ZABSY_GETADDR;                  \
  m = PAGE0READ(a);                   \
}
 
 
// absolute: op aaaa
#define ADR_ABS_GETADDR {                       \
  a = BUSREAD(m_PC) | (BUSREAD(m_PC + 1) << 8); \
  m_PC += 2;                                    \
}
#define ADR_ABS {                               \
  ADR_ABS_GETADDR;                              \
  m = BUSREAD(a);                               \
}
 
 
// absolute + X: op aaaa, X
#define ADR_ABSX_GETADDR {               \
  al = BUSREAD(m_PC++) + m_X;            \
  a  = al + (BUSREAD(m_PC++) << 8);      \
}
#define ADR_ABSX {                       \
  ADR_ABSX_GETADDR;                      \
  m  = BUSREAD(a);                       \
}
 
 
// absolute + Y: op aaaa, Y
#define ADR_ABSY_GETADDR {               \
  al = BUSREAD(m_PC++) + m_Y;            \
  a  = al + (BUSREAD(m_PC++) << 8);      \
}
#define ADR_ABSY {                       \
  ADR_ABSY_GETADDR;                      \
  m  = BUSREAD(a);                       \
}
 
 
// indexed indirect: op (aa, X)
#define ADR_IIND_GETADDR {                    \
  int l = (BUSREAD(m_PC++) + m_X) & 0xff;     \
  int h = (l + 1) & 0xff;                     \
  a     = PAGE0READ(l) | (PAGE0READ(h) << 8); \
}
#define ADR_IIND {                            \
  ADR_IIND_GETADDR;                           \
  m     = BUSREAD(a);                         \
}
 
 
// indirect index: op (aa), Y
#define ADR_INDI_GETADDR {          \
  int l = BUSREAD(m_PC++);          \
  int h = (l + 1) & 0xff;           \
  al    = PAGE0READ(l) + m_Y;       \
  a     = al + (PAGE0READ(h) << 8); \
}
#define ADR_INDI {                  \
  ADR_INDI_GETADDR;                 \
  m     = BUSREAD(a);               \
}
 
 
// relative: op aa
#define ADR_REL {               \
  m  = (int8_t)BUSREAD(m_PC++); \
  a  = m_PC + m;                \
  al = (m_PC & 0xff) + m;       \
}
 
 
// indirect: op (aaaa)
#define ADR_IND {                                   \
  int l = BUSREAD(m_PC++);                          \
  int h = BUSREAD(m_PC++);                          \
  a     = l | (h << 8);                             \
  l     = BUSREAD(a);                               \
  h     = BUSREAD((a & 0xff00) | ((a + 1) & 0xff)); \
  a     = l | (h << 8);                             \
}
 
 
int rp_fcemu::step()
{
  int m;  // operand value
  int al; // address low + index
  int a;  // complete address
 
  int op = (BUSREAD(m_PC++) & 0xff);
  switch ( op ) {
 
 
    // ADC #dd
    case 0x69:
      ADR_IMM;
      (this->*m_OP_ADC)(m);
      return 2;
 
    // ADC aa
    case 0x65:
      ADR_ZABS;
      (this->*m_OP_ADC)(m);
      return 3;
 
    // ADC aa, X
    case 0x75:
      ADR_ZABSX;
      (this->*m_OP_ADC)(m);
      return 4;
 
    // ADC aaaa
    case 0x6d:
      ADR_ABS;
      (this->*m_OP_ADC)(m);
      return 4;
 
    // ADC aaaa, X
    case 0x7d:
      ADR_ABSX;
      (this->*m_OP_ADC)(m);
      return 4 + PAGECROSS();
      
    // ADC aaaa, Y
    case 0x79:
      ADR_ABSY;
      (this->*m_OP_ADC)(m);
      return 4 + PAGECROSS();
 
    // ADC (aa, X)
    case 0x61:
      ADR_IIND;
      (this->*m_OP_ADC)(m);
      return 6;
 
    // ADC (aa), Y
    case 0x71:
      ADR_INDI;
      (this->*m_OP_ADC)(m);
      return 5 + PAGECROSS();
 
 
    // AND #dd
    case 0x29:
      ADR_IMM;
      OP_AND;
      return 2;
 
    // AND aa
    case 0x25:
      ADR_ZABS;
      OP_AND;
      return 3;
 
    // AND aa, X
    case 0x35:
      ADR_ZABSX;
      OP_AND;
      return 4;
 
    // AND aaaa
    case 0x2d:
      ADR_ABS;
      OP_AND;
      return 4;
 
    // AND aaaa, X
    case 0x3d:
      ADR_ABSX;
      OP_AND;
      return 4 + PAGECROSS();
      
    // AND aaaa, Y
    case 0x39:
      ADR_ABSY;
      OP_AND;
      return 4 + PAGECROSS();
 
    // AND (aa, X)
    case 0x21:
      ADR_IIND;
      OP_AND;
      return 6;
 
    // AND (aa), Y
    case 0x31:
      ADR_INDI;
      OP_AND;
      return 5 + PAGECROSS();
 
 
    // ASL A
    case 0x0a:
      m = m_A;
      OP_ASL;
      m_A = m;
      return 2;
 
    // ASL aa
    case 0x06:
      ADR_ZABS;
      OP_ASL;
      PAGE0WRITE(a, m);
      return 5;
 
    // ASL aa, X
    case 0x16:
      ADR_ZABSX;
      OP_ASL;
      PAGE0WRITE(a, m);
      return 6;
 
    // ASL aaaa
    case 0x0e:
      ADR_ABS;
      OP_ASL;
      BUSWRITE(a, m);
      return 6;
 
    // ASL aaaa, X
    case 0x1e:
      ADR_ABSX;
      OP_ASL;
      BUSWRITE(a, m);
      return 7;
 
 
    // BCC aa
    case 0x90:
      if (m_carry) {
        ++m_PC;
        return 2;
      } else {
        ADR_REL;
        m_PC = a;
        return 3 + PAGECROSS();
      }
 
 
    // BCS aa
    case 0xb0:
      if (m_carry) {
        ADR_REL;
        m_PC = a;
        return 3 + PAGECROSS();
      } else {
        ++m_PC;
        return 2;
      }
 
 
    // BEQ aa
    case 0xf0:
      if (m_zero) {
        ADR_REL;
        m_PC = a;
        return 3 + PAGECROSS();
      } else {
        ++m_PC;
        return 2;
      }
 
 
    // BIT aa
    case 0x24:
      ADR_ZABS;
      OP_BIT;
      return 3;
 
    // BIT aaaa
    case 0x2c:
      ADR_ABS;
      OP_BIT;
      return 4;
 
 
    // BMI aa
    case 0x30:
      if (m_negative) {
        ADR_REL;
        m_PC = a;
        return 3 + PAGECROSS();
      } else {
        ++m_PC;
        return 2;
      }
 
 
    // BNE aa
    case 0xd0:
      if (m_zero) {
        ++m_PC;
        return 2;
      } else {
        ADR_REL;
        m_PC = a;
        return 3 + PAGECROSS();
      }
 
 
    // BPL aa
    case 0x10:
      if (m_negative) {
        ++m_PC;
        return 2;
      } else {
        ADR_REL;
        m_PC = a;
        return 3 + PAGECROSS();
      }
 
 
    // BRK
    case 0x00:
//      Serial.printf("6502 prog end 0x%04x\n", m_PC );
      return 0;		// prog end
/*
      ++m_PC;
      STACKPUSHWORD(m_PC);
      STACKPUSHBYTE(COMPOSE_STATUS | 0x10); // set BREAK bit
      m_intDisable = true;
      m_PC = (BUSREAD(0xffff) << 8) + BUSREAD(0xfffe);  // BRK (IRQ) vector at 0xfffe
      return 7;
*/ 
 
    // BVC aa
    case 0x50:
      if (m_overflow) {
        ++m_PC;
        return 2;
      } else {
        ADR_REL;
        m_PC = a;
        return 3 + PAGECROSS();
      }
 
 
    // BVS aa
    case 0x70:
      if (m_overflow) {
        ADR_REL;
        m_PC = a;
        return 3 + PAGECROSS();
      } else {
        ++m_PC;
        return 2;
      }
 
 
    case 0x18:
      m_carry = false;
      return 2;
 
 
    case 0xd8:
      m_decimal = false;
      setADCSBC();
      return 2;
 
 
    case 0x58:
      m_intDisable = false;
      return 2;
 
 
    case 0xb8:
      m_overflow = false;
      return 2;
 
 
    // CMP #dd
    case 0xc9:
      ADR_IMM;
      OP_CMP;
      return 2;
 
    // CMP aa
    case 0xc5:
      ADR_ZABS;
      OP_CMP;
      return 3;
 
    // CMP aa, X
    case 0xd5:
      ADR_ZABSX;
      OP_CMP;
      return 4;
 
    // CMP aaaa
    case 0xcd:
      ADR_ABS;
      OP_CMP;
      return 4;
 
    // CMP aaaa, X
    case 0xdd:
      ADR_ABSX;
      OP_CMP;
      return 4 + PAGECROSS();
 
    // CMP aaaa, Y
    case 0xd9:
      ADR_ABSY;
      OP_CMP;
      return 4 + PAGECROSS();
 
    // CMP (aa, X)
    case 0xc1:
      ADR_IIND;
      OP_CMP;
      return 6;
 
    // CMP (aa), Y
    case 0xd1:
      ADR_INDI;
      OP_CMP;
      return 5 + PAGECROSS();
 
 
    // CPX #dd
    case 0xe0:
      ADR_IMM;
      OP_CPX;
      return 2;
 
    // CPX aa
    case 0xe4:
      ADR_ZABS;
      OP_CPX;
      return 3;
 
    // CPX aaaa
    case 0xec:
      ADR_ABS;
      OP_CPX;
      return 4;
 
 
    // CPY #dd
    case 0xc0:
      ADR_IMM;
      OP_CPY;
      return 2;
 
    // CPY aa
    case 0xc4:
      ADR_ZABS;
      OP_CPY;
      return 3;
 
    // CPY aaaa
    case 0xcc:
      ADR_ABS;
      OP_CPY;
      return 4;
 
 
    // DEC aa
    case 0xc6:
      ADR_ZABS;
      OP_DEC;
      PAGE0WRITE(a, m);
      return 5;
 
    // DEC aa, X
    case 0xd6:
      ADR_ZABSX;
      OP_DEC;
      PAGE0WRITE(a, m);
      return 6;
 
    // DEC aaaa
    case 0xce:
      ADR_ABS;
      OP_DEC;
      BUSWRITE(a, m);
      return 6;
 
    // DEC aaaa, X
    case 0xde:
      ADR_ABSX;
      OP_DEC;
      BUSWRITE(a, m);
      return 7;
 
 
    case 0xca:
      m_X        = (m_X - 1) & 0xff;
        m_negative = m_X & 0x80;
      m_zero     = !m_X;
      return 2;
 
 
    case 0x88:
      m_Y        = (m_Y - 1) & 0xff;
      m_negative = m_Y & 0x80;
      m_zero     = !m_Y;
      return 2;
 
 
    // EOR #dd
    case 0x49:
      ADR_IMM;
      OP_EOR;
      return 2;
 
    // EOR aa
    case 0x45:
      ADR_ZABS;
      OP_EOR;
      return 3;
 
    // EOR aa, X
    case 0x55:
      ADR_ZABSX;
      OP_EOR;
      return 4;
 
    // EOR aaaa
    case 0x4d:
      ADR_ABS;
      OP_EOR;
      return 4;
 
    // EOR aaaa, X
    case 0x5d:
      ADR_ABSX;
      OP_EOR;
      return 4 + PAGECROSS();
 
    // EOR aaaa, Y
    case 0x59:
      ADR_ABSY;
      OP_EOR;
      return 4 + PAGECROSS();
 
    // EOR (aa, X)
    case 0x41:
      ADR_IIND;
      OP_EOR;
      return 6;
 
    // EOR (aa), Y
    case 0x51:
      ADR_INDI;
      OP_EOR;
      return 5 + PAGECROSS();
      
 
    // INC aa
    case 0xe6:
      ADR_ZABS;
      OP_INC;
      PAGE0WRITE(a, m);
      return 5;
 
    // INC aa, X
    case 0xf6:
      ADR_ZABSX;
      OP_INC;
      PAGE0WRITE(a, m);
      return 6;
 
    // INC aaaa
    case 0xee:
      ADR_ABS;
      OP_INC;
      BUSWRITE(a, m);
      return 6;
 
    // INC aaaa, X
    case 0xfe:
      ADR_ABSX;
      OP_INC;
      BUSWRITE(a, m);
      return 7;
 
 
    case 0xe8:
      m_X        = (m_X + 1) & 0xff;
        m_negative = m_X & 0x80;
      m_zero     = !m_X;
      return 2;
 
 
    case 0xc8:
      m_Y        = (m_Y + 1) & 0xff;
      m_negative = m_Y & 0x80;
      m_zero     = !m_Y;
      return 2;
 
 
    // JMP aaaa
    case 0x4c:
      ADR_ABS;
      m_PC = a;
      return 3;
 
    // JMP (aaaa)
    case 0x6c:
      ADR_IND;
      m_PC = a;
      return 5;
 
 
    // JSR aaaa
    case 0x20:
      a = (m_PC + 1) & 0xffff;
      STACKPUSHWORD(a);
      ADR_ABS;
      m_PC = a;
      return 6;
 
 
    // LDA #dd
    case 0xa9:
      ADR_IMM;
      OP_LDA;
      return 2;
 
    // LDA aa
    case 0xa5:
      ADR_ZABS;
      OP_LDA;
      return 3;
 
    // LDA aa, X
    case 0xb5:
      ADR_ZABSX;
      OP_LDA;
      return 4;
 
    // LDA aaaa
    case 0xad:
      ADR_ABS;
      OP_LDA;
      return 4;
 
    // LDA aaaa, X
    case 0xbd:
      ADR_ABSX;
      OP_LDA;
      return 4 + PAGECROSS();
 
    // LDA aaaa, Y
    case 0xb9:
      ADR_ABSY;
      OP_LDA;
      return 4 + PAGECROSS();
 
    // LDA (aa, X)
    case 0xa1:
      ADR_IIND;
      OP_LDA;
      return 6;
 
    // LDA (aa), Y
    case 0xb1:
      ADR_INDI;
      OP_LDA;
      return 5 + PAGECROSS();
 
 
    // LDX #dd
    case 0xa2:
      ADR_IMM;
      OP_LDX;
      return 2;
 
    // LDX aa
    case 0xa6:
      ADR_ZABS;
      OP_LDX;
      return 3;
 
    // LDX aa, Y
    case 0xb6:
      ADR_ZABSY;
      OP_LDX;
      return 4;
    
    // LDX aaaa
    case 0xae:
      ADR_ABS;
      OP_LDX;
      return 4;
 
    // LDX aaaa, Y
    case 0xbe:
      ADR_ABSY;
      OP_LDX;
      return 4 + PAGECROSS();
 
 
    // LDY #dd
    case 0xa0:
      ADR_IMM;
      OP_LDY;
      return 2;
 
    // LDY aa
    case 0xa4:
      ADR_ZABS;
      OP_LDY;
      return 3;
 
    // LDY aa, X
    case 0xb4:
      ADR_ZABSX;
      OP_LDY;
      return 4;
 
    // LDY aaaa
    case 0xac:
      ADR_ABS;
      OP_LDY;
      return 4;
 
    // LDY aaaa, X
    case 0xbc:
      ADR_ABSX;
      OP_LDY;
      return 4 + PAGECROSS();
 
 
    // LSR A
    case 0x4a:
      m = m_A;
      OP_LSR;
      m_A = m;
      return 2;
 
    // LSR aa
    case 0x46:
      ADR_ZABS;
      OP_LSR;
      PAGE0WRITE(a, m);
      return 5;
 
    // LSR aa, X
    case 0x56:
      ADR_ZABSX;
      OP_LSR;
      PAGE0WRITE(a, m);
      return 6;
    
    // LSR aaaa
    case 0x4e:
      ADR_ABS;
      OP_LSR;
      BUSWRITE(a, m);
      return 6;
 
    // LSR aaaa, X
    case 0x5e:
      ADR_ABSX;
      OP_LSR;
      BUSWRITE(a, m);
      return 7;
 
 
    case 0xea:
      return 2;
 
 
    // ORA #dd
    case 0x09:
      ADR_IMM;
      OP_ORA;
      return 2;
 
    // ORA aa
    case 0x05:
      ADR_ZABS;
      OP_ORA;
      return 3;
 
    // ORA aa, X
    case 0x15:
      ADR_ZABSX;
      OP_ORA;
      return 4;
 
    // ORA aaaa
    case 0x0d:
      ADR_ABS;
      OP_ORA;
      return 4;
 
    // ORA aaaa, X
    case 0x1d:
      ADR_ABSX;
      OP_ORA;
      return 4 + PAGECROSS();
 
    // ORA aaaa, Y
    case 0x19:
      ADR_ABSY;
      OP_ORA;
      return 4 + PAGECROSS();
 
    // ORA (aa, X)
    case 0x01:
      ADR_IIND;
      OP_ORA;
      return 6;
 
    // ORA (aa), Y
    case 0x11:
      ADR_INDI;
      OP_ORA;
      return 5 + PAGECROSS();
    
 
    case 0x48:
      STACKPUSHBYTE(m_A);
      return 3;
 
 
    case 0x08:
      STACKPUSHBYTE(COMPOSE_STATUS | 0x10); // set BREAK bit
      return 3;
 
 
    case 0x68:
      m_A        = STACKPOPBYTE();
      m_zero     = !m_A;
      m_negative = m_A & 0x80;
      return 4;
 
 
    case 0x28:
      DECOMPOSE_STATUS(STACKPOPBYTE());
      return 4;
 
 
    // ROL A
    case 0x2a:
      m = m_A;
      OP_ROL;
      m_A = m;
      return 2;
 
    // ROL aa
    case 0x26:
      ADR_ZABS;
      OP_ROL;
      PAGE0WRITE(a, m);
      return 5;
 
    // ROL aa, X
    case 0x36:
      ADR_ZABSX;
      OP_ROL;
      PAGE0WRITE(a, m);
      return 6;
 
    // ROL aaaa
    case 0x2e:
      ADR_ABS;
      OP_ROL;
      BUSWRITE(a, m);
      return 6;
 
    // ROL aaaa, X
    case 0x3e:
      ADR_ABSX;
      OP_ROL;
      BUSWRITE(a, m);
      return 7;
 
    
    // ROR A
    case 0x6a:
      m = m_A;
      OP_ROR;
      m_A = m;
      return 2;
 
    // ROR aa
    case 0x66:
      ADR_ZABS;
      OP_ROR;
      PAGE0WRITE(a, m);
      return 5;
 
    // ROR aa, X
    case 0x76:
      ADR_ZABSX;
      OP_ROR;
      PAGE0WRITE(a, m);
      return 6;
 
    // ROR aaaa
    case 0x6e:
      ADR_ABS;
      OP_ROR;
      BUSWRITE(a, m);
      return 6;
 
    // ROR aaaa, X
    case 0x7e:
      ADR_ABSX;
      OP_ROR;
      BUSWRITE(a, m);
      return 7;
 
 	// RTI
    case 0x40:
    {
      DECOMPOSE_STATUS(STACKPOPBYTE());
      m_PC = STACKPOPWORD();
      return 6;
    }

 
 	// RTS
    case 0x60:
	{
		if ( m_SP ==  0xfd ) {
//		 	Serial.printf("6502 prog end 0x%04x\n", m_PC );
			return 0;
		}
		m_PC = STACKPOPWORD() + 1;
		return 6;
    }
 
    // SBC #dd
    case 0xe9:
      ADR_IMM;
      (this->*m_OP_SBC)(m);
      return 2;
 
    // SBC aa
    case 0xe5:
      ADR_ZABS;
      (this->*m_OP_SBC)(m);
      return 3;
 
    // SBC aa, X
    case 0xf5:
      ADR_ZABSX;
      (this->*m_OP_SBC)(m);
      return 4;
 
    // SBC aaaa
    case 0xed:
      ADR_ABS;
      (this->*m_OP_SBC)(m);
      return 4;
 
    // SBC aaaa, X
    case 0xfd:
      ADR_ABSX;
      (this->*m_OP_SBC)(m);
      return 4 + PAGECROSS();
 
    // SBC aaaa, Y
    case 0xf9:
      ADR_ABSY;
      (this->*m_OP_SBC)(m);
      return 4 + PAGECROSS();
 
    // SBC (aa, X)
    case 0xe1:
      ADR_IIND;
      (this->*m_OP_SBC)(m);
      return 6;
 
    // SBC (aa), Y
    case 0xf1:
      ADR_INDI;
      (this->*m_OP_SBC)(m);
      return 5 + PAGECROSS();
 
 
    case 0x38:
      m_carry = true;
      return 2;
 
 
    case 0xf8:
      m_decimal = true;
      setADCSBC();
      return 2;
 
 
    case 0x78:
      m_intDisable = true;
      return 2;
 
 
    // STA aa
    case 0x85:
      ADR_ZABS_GETADDR;
      PAGE0WRITE(a, m_A);
      return 3;
 
    // STA aa, X
    case 0x95:
      ADR_ZABSX_GETADDR;
      PAGE0WRITE(a, m_A);
      return 4;
 
    // STA aaaa
    case 0x8d:
      ADR_ABS_GETADDR;
      BUSWRITE(a, m_A);
      return 4;
 
    // STA aaaa, X
    case 0x9d:
      ADR_ABSX_GETADDR;
      BUSWRITE(a, m_A);
      return 5;
 
    // STA aaaa, Y
    case 0x99:
      ADR_ABSY_GETADDR;
      BUSWRITE(a, m_A);
      return 5;
 
    // STA (aa, X)
    case 0x81:
      ADR_IIND_GETADDR;
      BUSWRITE(a, m_A);
      return 6;
 
    // STA (aa), Y
    case 0x91:
      ADR_INDI_GETADDR;
      BUSWRITE(a, m_A);
      return 6;
 
 
    // STX aa
    case 0x86:
      ADR_ZABS_GETADDR;
      PAGE0WRITE(a, m_X);
      return 3;
 
    // STX aa, Y
    case 0x96:
      ADR_ZABSY_GETADDR;
      PAGE0WRITE(a, m_X);
      return 4;
 
    // STX aaaa
    case 0x8e:
      ADR_ABS_GETADDR;
      BUSWRITE(a, m_X);
      return 4;
 
 
    // STY aa
    case 0x84:
      ADR_ZABS_GETADDR;
      PAGE0WRITE(a, m_Y);
      return 3;
 
    // STY aa, X
    case 0x94:
      ADR_ZABSX_GETADDR;
      PAGE0WRITE(a, m_Y);
      return 4;
 
    // STY aaaa
    case 0x8c:
      ADR_ABS_GETADDR;
      BUSWRITE(a, m_Y);
      return 4;
 
 
    case 0xaa:
      m_X        = m_A;
      m_zero     = !m_X;
      m_negative = m_X & 0x80;
      return 2;
 
 
    case 0xa8:
      m_Y        = m_A;
      m_zero     = !m_Y;
      m_negative = m_Y & 0x80;
      return 2;
 
 
    case 0xba:
      m_X        = m_SP;
      m_zero     = !m_X;
      m_negative = m_X & 0x80;
      return 2;
 
 
    case 0x8a:
      m_A        = m_X;
      m_zero     = !m_A;
      m_negative = m_A & 0x80;
      return 2;
 
 
    case 0x9a:
      m_SP = m_X;
      return 2;
 
 
    case 0x98:
      m_A        = m_Y;
      m_zero     = !m_A;
      m_negative = m_A & 0x80;
      return 2;
 
 
    case 0x1a:
    case 0x3a:
    case 0x5a:
    case 0x7a:
    case 0xda:
    case 0xfa:
      return 2;
 
 
    // DOP aa
    case 0x04:
    case 0x44:
    case 0x64:
      ADR_ZABS;
      return 3;
 
    // DOP aa, X
    case 0x14:
    case 0x34:
    case 0x54:
    case 0x74:
    case 0xd4:
    case 0xf4:
      ADR_ZABSX;
      return 4;
 
    // DOP #dd
    case 0x80:
    case 0x82:
    case 0x89:
    case 0xc2:
    case 0xe2:
      ADR_IMM;
      return 2;
 
 
    // ISC aa
    case 0xe7:
      ADR_ZABS;
      OP_INC;
      (this->*m_OP_SBC)(m);
      return 5;
 
    // ISC aa, X
    case 0xf7:
      ADR_ZABSX;
      OP_INC;
      (this->*m_OP_SBC)(m);
      return 6;
 
    // ISC aaaa
    case 0xef:
      ADR_ABS;
      OP_INC;
      (this->*m_OP_SBC)(m);
      return 6;
 
    // ISC aaaa, X
    case 0xff:
      ADR_ABSX;
      OP_INC;
      (this->*m_OP_SBC)(m);
      return 7;
 
    // ISC aaaa, Y
    case 0xfb:
      ADR_ABSY;
      OP_INC;
      (this->*m_OP_SBC)(m);
      return 7;
 
    // ISC (aa, X)
    case 0xe3:
      ADR_IIND;
      OP_INC;
      (this->*m_OP_SBC)(m);
      return 8;
 
    // ISC (aa), Y
    case 0xf3:
      ADR_INDI;
      OP_INC;
      (this->*m_OP_SBC)(m);
      return 8;
 
    default:
      // unsupported opcode
      Serial.printf("6502 illegal 0x%02x\n", BUSREAD(m_PC - 1));
      return 0;
 
  }
}
 


