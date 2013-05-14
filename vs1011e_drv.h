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
void vs1011e_test_sine(BYTE sample_rate, BYTE skip_speed);
BYTE WriteSPI_with_wait_interrupt(BYTE d);

unsigned int SPICONValue1 = ENABLE_SCK_PIN & ENABLE_SDO_PIN &
							SPI_MODE8_ON & SPI_SMP_OFF &
							SPI_CKE_ON & SLAVE_ENABLE_OFF &
							CLK_POL_ACTIVE_HIGH & MASTER_ENABLE_ON &
							SEC_PRESCAL_2_1 & PRI_PRESCAL_4_1;
unsigned int SPICONValue2 = FRAME_ENABLE_OFF & FRAME_SYNC_OUTPUT &
							FRAME_POL_ACTIVE_HIGH &
							FRAME_SYNC_EDGE_COINCIDE &
							FIFO_BUFFER_ENABLE;
unsigned int SPISTATValue = SPI_ENABLE & SPI_IDLE_CON &
							SPI_RX_OVFLOW_CLR;


