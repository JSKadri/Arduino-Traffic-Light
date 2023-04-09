#include "SPI.h"
void SPI_init(void)
{
    // SPI settings
    // Master operation, clock mode = 3, f_sck = f_osc / 4
    // (disabled SPI interrupt, data order = MSB first)

    // You should set up AVR(ATmega) SPI like,
    // Clock mode: Mode 3 (leading edge is falling: latch, trailing edge is rising: shift)
    // SPI master operation, /SS as output
    // Data order: MSB first (this depends on your situation. either order will work)

    SPCR = (1 << SPE) | (1 << MSTR) | (1 << CPOL) | (1 << CPHA);
    // Setup SPI
    SPCR = (1 << SPE) | (1 << MSTR); // Start SPI as Master
}

void SPI_begin()
{
    // set /SS = low to select SPI slave device.
    PORTB &= ~(1 << latchPin);
}

void SPI_end()
{
    // switch off LED by /OE = high,
    // set new data by /SS = high,
    // and then turn on LED by /OE = low
    PORTB |= (1 << latchPin);
}

void SPI_send(uint8_t cData1, uint8_t cData2)
{
    // shift out the first byte (data) (second shift register)
    SPDR = cData1;
    while (!(SPSR & (1 << SPIF)))
        ; // wait for SPIF flag to be set

    // shift out the second byte (data) (first shift register)
    SPDR = cData2;
    while (!(SPSR & (1 << SPIF)))
        ; // wait for SPIF flag to be set
}
