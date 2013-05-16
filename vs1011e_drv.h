#ifndef VS1011E_DRV_H
#define VS1011E_DRV_H

#include <p24FJ64GA002.h>
#include "vs1011e_main.h"

#define VS1011E_XRESET LATBbits.LATB1
#define VS1011E_XDCS   LATBbits.LATB8
#define VS1011E_CS     LATBbits.LATB13
#define VS1011E_DREQ   PORTBbits.RB0

void vs1011e_power_up(void);
void vs1011e_power_down(void);
void vs1011e_hard_reset(void);
void vs1011e_soft_reset(void);
void vs1011e_sci_write(BYTE addr, BYTE data1, BYTE data2);
void vs1011e_sdi_write(BYTE data);
void vs1011e_sci_write_with_verify(BYTE addr, BYTE data1, BYTE data2);
void vs1011e_sci_read(BYTE addr, BYTE *data);
void vs1011e_ram_clear(void);

void vs1011e_init_for_test_mode(void);
void vs1011e_test_sine_start(BYTE sample_rate, BYTE skip_speed);
void vs1011e_test_sine_end(void);
BOOL vs1011e_test_memory(void);
void vs1011e_test_sci(BYTE addr);
BYTE WriteSPI_with_wait_interrupt(BYTE d);

#endif /* VS1011E_DRV_H */
