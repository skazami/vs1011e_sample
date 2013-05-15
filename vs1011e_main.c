/*	Filename: vs1011e_main.c
	Description: Sample project for vs1011e
	Language: C30(Evaluation Version)
	Target: PIC24FJ64GA002
*/

#include <p24FJ64GA002.h>
#include <stdio.h>
#include "FSIO.h"
#include <spi.h>
#include "vs1011e_drv.h"

_CONFIG1(JTAGEN_OFF & GCP_OFF & GWRP_OFF & BKBUG_OFF
         & COE_OFF & ICS_PGx2 & FWDTEN_OFF) 

_CONFIG2(IESO_OFF & FNOSC_FRCPLL & FCKSM_CSDCMD
         & OSCIOFNC_ON & IOL1WAY_OFF & I2C1SEL_PRI
         & POSCMOD_NONE)

void pic_configuration_init(void);


int main(void){
	FSFILE *pFile;
	BYTE data[32];
	unsigned int i;

	CLKDIV = 0;
	pic_configuration_init();

	if(!FSInit()){
		while(1);
	}

//	pFile = FSfopen ("test.wav", READ);
	pFile = FSfopen ("test.mp3", READ);
	if(pFile == NULL){
		while(1);
	}

	vs1011e_hard_reset();
	vs1011e_soft_reset();
	vs1011e_sci_write_with_verify(0x00, 0x08, 0x20);
	vs1011e_sci_write_with_verify(0x03, 0x98, 0x00);
	vs1011e_sci_write_with_verify(0x0B, 0x60, 0x60);

#if 0 // test code for test mode
	vs1011e_init_for_test_mode();
	vs1011e_test_sine_start(0x06, 0x0E);
	delay_ms(3000);
	vs1011e_test_sine_end();
//	vs1011e_test_sci(0x00);
//	vs1011e_test_memory();
#endif

	while(1)
	{
		while(FSfread(data, 32, 1, pFile) > 0)
		{
			while( !VS1011E_DREQ );
	
			for(i=0;i<32;i++)
				vs1011e_sdi_write(data[i]);
		}

		FSrewind(pFile);
	}

	vs1011e_power_down();
	FSfclose(pFile);
}

void pic_configuration_init(void)
{
	// Initialize pin configuration for vs1011e
	// Pin assign
	AD1PCFG = 0xFFFF;

//	CNPU1 = 0x0040;//0x0011,1000,0011,0010
//	CNPU2 = 0x09E0;//0x?000,1001,1110,0000		RPINR22bits.SDI2R = 4;  //  4 = RP4(SDI2)
//	CNPU1bits.CN1PUE  = 1;
//	CNPU1bits.CN11PUE = 1;
//	CNPU2bits.CN22PUE = 1;
	RPINR22bits.SDI2R = 4;  // 4 = RP4(SDI2)
	RPOR7bits.RP15R   = 10; // 10 = SDO2
	RPOR7bits.RP14R   = 11; // 11 = SCK2OUT

	TRISBbits.TRISB0  = 1;  // RB0  = INPUT(DREQ)
	TRISBbits.TRISB1  = 0;  // RB1  = OUTPUT(RESET)
	TRISBbits.TRISB4  = 1;  // RB4  = INPUT(SDI2)
	TRISBbits.TRISB8  = 0;  // RB8  = OUTPUT(XDCS)
	TRISBbits.TRISB13 = 0;  // RB13 = OUTPUT(ChipSelect)
	TRISBbits.TRISB14 = 0;  // RB13 = OUTPUT(SCK2)
	TRISBbits.TRISB15 = 0;  // RB15 = OUTPUT(SDO2)

	IFS2bits.SPI2IF = 0;
	IEC2bits.SPI2IE = 1;
	IPC8bits.SPI2IP = 6;
	SRbits.IPL = 0;
	CORCONbits.IPL3 = 0;
//	CORCON = 0x0000;
//	CNEN1bits.CN1IE = 1;
//	CNEN1bits.CN11IE = 1;

	SPI2CON1 = ENABLE_SCK_PIN & ENABLE_SDO_PIN &
				SPI_MODE8_ON & SPI_SMP_OFF &
				SPI_CKE_ON & SLAVE_ENABLE_OFF &
				CLK_POL_ACTIVE_HIGH & MASTER_ENABLE_ON &
				SEC_PRESCAL_2_1 & PRI_PRESCAL_4_1;

	SPI2CON2 = FRAME_ENABLE_OFF & FRAME_SYNC_OUTPUT &
				FRAME_POL_ACTIVE_HIGH &
				FRAME_SYNC_EDGE_COINCIDE &
				FIFO_BUFFER_ENABLE;
	SPI2STAT = SPI_ENABLE & SPI_IDLE_CON & SPI_RX_OVFLOW_CLR;

}

//----- usec_delay
void delay_us(int usec){
	usec = (int)(CLOCK*usec)/40;
	while(usec){
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		usec--;
	}
}
//----- msec delay
void delay_ms(int msec){
	int i;
	for(i=0;i<msec;i++)
		delay_us(1000);
}
