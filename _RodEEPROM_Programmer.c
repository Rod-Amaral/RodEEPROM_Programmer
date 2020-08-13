#include "_RodEEPROM_Programmer.h"

void set_CE(uint8_t state)
{
	state ? (PORTA |= (1<<PA7)) : (PORTA &= ~(1<<PA7));
}

void set_OE(uint8_t state)
{
	state ? (PORTA |= (1<<PA5)) : (PORTA &= ~(1<<PA5));
}

void set_WE(uint8_t state)
{
	state ? (PORTA |= (1<<PA0)) : (PORTA &= ~(1<<PA0));
}

void _RodEEPROM_ProgrammerSetup()
{
	/* Outputs */
	DDRD |= (1<<PD7); //Status LED
	DDRA |= (1<<PA7) | (1<<PA5) | (1<<PA0);

	DDRD |= ((1<<PD6) | (1<<PD5) | (1<<PD4) | (1<<PD3) | (1<<PD2));
	DDRB |= ((1<<PB4) | (1<<PB3) | (1<<PB2) | (1<<PB1) | (1<<PB0));
	DDRA |= ((1<<PA2) | (1<<PA3) | (1<<PA6) | (1<<PA4) | (1<<PA1));

	set_CE(1);
	set_OE(1);
	set_WE(1);
	set_CE(0);
}

uint8_t _RodEEPROM_ReadData()
{
	DDRC = 0x00;
	_delay_ms(DELAY);
	return ( ((PINC & 0b11111010) | ((PINC & 0b100)?1:0) | ((PINC & 0b1)?4:0)) );
}

uint8_t _RodEEPROM_ReadContinousData()
{
	return ( ((PINC & 0b11111010) | ((PINC & 0b100) >> 2) | ((PINC & 0b1) << 2)) );
}

void _RodEEPROM_OutputData(uint8_t data)
{
	DDRC = 0xFF;
	_delay_ms(DELAY);
	PORTC = ( (data & 0b11111010) | ((data & 0b100) >> 2) | ((data & 0b1) << 2) );
}

void _RodEEPROM_OutputContinousData(uint8_t data)
{
	_delay_us(3000);
	PORTC = ( (data & 0b11111010) | ((data & 0b100) >> 2) | ((data & 0b1) << 2) );
}

void _RodEEPROM_OutputAddress(uint16_t address)
{
	(address&0x1) ? (PORTD |= (1<<PD6)) : (PORTD &= ~(1<<PD6));
	(address&0x2) ? (PORTD |= (1<<PD5)) : (PORTD &= ~(1<<PD5));
	(address&0x4) ? (PORTD |= (1<<PD4)) : (PORTD &= ~(1<<PD4));
	(address&0x8) ? (PORTD |= (1<<PD3)) : (PORTD &= ~(1<<PD3));
	(address&0x10) ? (PORTD |= (1<<PD2)) : (PORTD &= ~(1<<PD2));

	(address&0x20) ? (PORTB |= (1<<PB4)) : (PORTB &= ~(1<<PB4));
	(address&0x40) ? (PORTB |= (1<<PB3)) : (PORTB &= ~(1<<PB3));
	(address&0x80) ? (PORTB |= (1<<PB2)) : (PORTB &= ~(1<<PB2));

	(address&0x100) ? (PORTA |= (1<<PA2)) : (PORTA &= ~(1<<PA2));
	(address&0x200) ? (PORTA |= (1<<PA3)) : (PORTA &= ~(1<<PA3));
	(address&0x400) ? (PORTA |= (1<<PA6)) : (PORTA &= ~(1<<PA6));
	(address&0x800) ? (PORTA |= (1<<PA4)) : (PORTA &= ~(1<<PA4));

	(address&0x1000) ? (PORTB |= (1<<PB1)) : (PORTB &= ~(1<<PB1));

	(address&0x2000) ? (PORTA |= (1<<PA1)) : (PORTA &= ~(1<<PA1));

	(address&0x4000) ? (PORTB |= (1<<PB0)) : (PORTB &= ~(1<<PB0));
}

void _RodEEPROM_WriteByte(uint16_t address, uint8_t data)
{
	_RodEEPROM_OutputAddress(address);
	_RodEEPROM_OutputData(data);

	_delay_us(1);
	set_WE(0);
	_delay_us(1);
	set_WE(1);
	_delay_us(1);

	_RodEEPROM_OutputData(0);
}

void _RodEEPROM_WriteContinousByte(uint16_t address, uint8_t data)
{
	_RodEEPROM_OutputAddress(address);
	_RodEEPROM_OutputContinousData(data);

	_delay_us(1);
	set_WE(0);
	_delay_us(1);
	set_WE(1);
	_delay_us(1);

	_RodEEPROM_OutputContinousData(0);
}

void _RodEEPROM_Set_WriteContinousByte()
{
	DDRC = 0xff;
	_delay_ms(DELAY);
}

uint8_t _RodEEPROM_ReadByte(uint16_t address)
{
	static uint8_t data;

	_RodEEPROM_OutputAddress(address);

	_delay_us(1);
	set_OE(0);
	_delay_us(1);

	data = _RodEEPROM_ReadData();
	set_OE(1);

	return data;
}

uint8_t _RodEEPROM_ReadContinousByte(uint16_t address)
{
	static uint8_t data;

	_RodEEPROM_OutputAddress(address);

	_delay_us(1);
	set_OE(0);
	_delay_us(1);

	//DDRC = 0x00;
	//_delay_ms(DELAY);
	data = _RodEEPROM_ReadContinousData();
	set_OE(1);

	return data;
}

void _RodEEPROM_Set_ReadContinousByte()
{
	DDRC = 0x00;
	_delay_ms(DELAY);
}

void _RodEEPROM_ClearEEPROM()
{
	uint16_t address = 0;

	_RodEEPROM_Set_WriteContinousByte();

	PORTD |= (1<<PD7);
	for(uint16_t address = 0; address <= 0x7fff; address++)
	{
		_RodEEPROM_WriteContinousByte(address, 0xff);

		if(! (address % 10))
			putchar('\n');
		printf(" %4.4x ", address);
	}
	PORTD &= ~(1<<PD7);
}

//Returns uint64_t MAX if succsseful
uint64_t _RodEEPROM_ClearCheck()
{
	uint16_t address = 0;
	uint8_t byte = 0;

	_RodEEPROM_Set_ReadContinousByte();

	PORTD |= (1<<PD7);
	for(uint16_t address = 0; address <= 0x7fff; address++)
	{
		byte = _RodEEPROM_ReadContinousByte(address);

		if(! (address % 32))
		{
			printf("\n0x%4.4x:   ", address);
		}

		if(! ((address + 16) % 32))
		{
			putchar('\t');
		}

		printf(" %2.2x ", byte);

		if(byte != 0xff)
		{
			puts("\n\n");
			return address;
			break;
		}
	}
	PORTD &= ~(1<<PD7);
	return 0xffffffffffffffff;
}

void _RodEEPROM_ReadEEPROM()
{
	uint16_t address = 0;
	uint8_t byte = 0;

	_RodEEPROM_Set_ReadContinousByte();

	PORTD |= (1<<PD7);
	for(uint16_t address = 0; address <= 0x7fff; address++)
	{
		byte = _RodEEPROM_ReadContinousByte(address);

		if(! (address % 32))
		{
			printf("\n0x%4.4x:   ", address);
		}

		if(! ((address + 16) % 32))
		{
			putchar('\t');
		}

		printf(" %2.2x ", byte);

	}
	PORTD &= ~(1<<PD7);
}