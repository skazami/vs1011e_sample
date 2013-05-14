/*	Filename: lcd-sdcard.c
	Description: Application Example
	Language: C30(Evaluation Version)
	Target: PIC24FJ64GA002, LCD-SDCARD UNIT
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

#define CLOCK 32

void delay_us(int usec);
void delay_ms(int msec);
void pic_configuration_init(void);

int main(void){
	FSFILE *pFile;
	BYTE data[32];
	BYTE buf[4];
	char i;
	unsigned int d=0;

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

	VS1011E_CS    = 1;     // Initialize CS
	VS1011E_XDCS  = 1;     // Initialize XCDS

	vs1011e_soft_reset();

	vs1011e_sci_write_with_verify(0x00, 0x08, 0x20);
	vs1011e_sci_write_with_verify(0x03, 0x98, 0x00);
	vs1011e_sci_write_with_verify(0x0B, 0x60, 0x60);

//	vs1011e_test_sine(0x06, 0x0E);

/*
	VS1011E_CS = 0;
	VS1011E_XDCS  = 1;
	delay_us(2);
	while( !VS1011E_DREQ );
	VS1011E_XDCS  = 0;
	VS1011E_CS = 1;
	delay_us(2);
	do
	{
	VS1011E_CS = 0;
	VS1011E_XDCS  = 1;
	delay_us(2);
	while( !VS1011E_DREQ );
	VS1011E_XDCS  = 0;
	VS1011E_CS = 1;
	delay_us(2);
#if 1
	vs1011e_sdi_write(0x53);
	vs1011e_sdi_write(0x70);
	vs1011e_sdi_write(0xEE);
	vs1011e_sdi_write(0x00);
#else
	vs1011e_sdi_write(0x4D);
	vs1011e_sdi_write(0xEA);
	vs1011e_sdi_write(0x6D);
	vs1011e_sdi_write(0x54);
#endif
	vs1011e_sdi_write(0x00);
	vs1011e_sdi_write(0x00);
	vs1011e_sdi_write(0x00);
	vs1011e_sdi_write(0x00);
	delay_ms(10);
	vs1011e_sci_read(0x08,data);
	delay_ms(10);
	vs1011e_sci_read(0x00,buf);
	}while(0);
//	}while(data[0]!=buf[0]||data[1]!=buf[1]);

	d=1;
	while(d)
	{
	vs1011e_sdi_write(0x45);
	vs1011e_sdi_write(0x78);
	vs1011e_sdi_write(0x69);
	vs1011e_sdi_write(0x74);
	vs1011e_sdi_write(0x00);
	vs1011e_sdi_write(0x00);
	vs1011e_sdi_write(0x00);
	vs1011e_sdi_write(0x00);
	}

	d=1;
	while(d);
*/
	VS1011E_CS = 1;
	while( !VS1011E_DREQ );
	VS1011E_XDCS  = 0;
	while(FSfread(data, 32, 1, pFile) > 0)
	{
		while( !VS1011E_DREQ );

		for(i=0;i<32;i++)
		{
			WriteSPI_with_wait_interrupt(data[i]);
		}
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

	SPI2CON1 = SPICONValue1;
	SPI2CON2 = SPICONValue2;
//	SPI2CON2 = SPICONValue2 & 0xfffe;
	SPI2STAT = SPISTATValue;
//	SPI2STAT = SPISTATValue & 0xbfff;
//	SPI2STATbits.SISEL = 5;
}

void vs1011e_power_up(void)
{
	delay_ms(1);
	VS1011E_XRESET = 1;
}

void vs1011e_power_down(void)
{
	delay_ms(1);
	VS1011E_XRESET = 0;
}

void vs1011e_hard_reset(void)
{
	vs1011e_power_down();
	delay_ms(1);
	vs1011e_power_up();
	delay_ms(1);
}

void vs1011e_soft_reset(void)
{
	vs1011e_sci_write(0x00, 0x00, 0x04);
	delay_us(5);
	while( !VS1011E_DREQ );
}

BYTE WriteSPI_with_wait_interrupt(BYTE d)
{
	IFS2bits.SPI2IF = 0;

	SPI2BUF = d;
//	delay_us(2);  // 暫定処理(SPI CLK 4MHzのとき)
	delay_us(4);  // 暫定処理(SPI CLK 2MHzのとき)
//	delay_us(8);  // 暫定処理(SPI CLK 1MHzのとき)
//	while(!IFS2bits.SPI2IF);
	return SPI2BUF;
}

//
// 注意：書き込んだデータと読み込んだデータが一致しない限り、この関数からは戻らない
//
void vs1011e_sci_write_with_verify(BYTE addr, BYTE data1, BYTE data2)
{
	BYTE read_buf[2];
	do
	{
		vs1011e_sci_write(addr, data1, data2);
		vs1011e_sci_read(addr, read_buf);
//	}while(1); // 波形確認用
	}while(read_buf[0]!=data1 || read_buf[1]!= data2);
}

void vs1011e_sci_write(BYTE addr, BYTE data1, BYTE data2)
{
	VS1011E_XDCS = 1;
	while( !VS1011E_DREQ );
	VS1011E_CS = 0;

	WriteSPI_with_wait_interrupt(0x02);
	WriteSPI_with_wait_interrupt(addr);
	WriteSPI_with_wait_interrupt(data1);
	WriteSPI_with_wait_interrupt(data2);

	VS1011E_CS = 1;
	while( !VS1011E_DREQ );
}

void vs1011e_sci_read(BYTE addr, BYTE *data)
{
	VS1011E_XDCS = 1;
	while( !VS1011E_DREQ );
	VS1011E_CS = 0;

	WriteSPI_with_wait_interrupt(0x03);
	WriteSPI_with_wait_interrupt(addr);
	data[0] = WriteSPI_with_wait_interrupt(0xff);
	data[1] = WriteSPI_with_wait_interrupt(0xff);

	VS1011E_CS = 1;
	while( !VS1011E_DREQ );
}

void vs1011e_sdi_write(BYTE data)
{
	while( !VS1011E_DREQ );

	WriteSPI_with_wait_interrupt(data);
}

// Test mode:sine test
// 呼び出し前に以下の処理を実行しておくこと
// 1) vs1011eのハードリセット
// 2) CS,XDCSをHigh
// 3) CLOCKF, SCI_VOLの設定
// 4) vs1011eのソフトリセット
//
// 引数の詳細はvs1011eのデータシートを参照
// この関数は無限ループに入るため戻らない
void vs1011e_test_sine(BYTE sample_rate, BYTE skip_speed)
{
	BYTE sine_test;

	// Test mode(sine)
	vs1011e_sci_write_with_verify(0x00, 0x08, 0x20);
	VS1011E_CS = 1;
	while( !VS1011E_DREQ );
	VS1011E_XDCS  = 0;
	while( !VS1011E_DREQ );

	sine_test = (((0x0f & sample_rate) << 4) | (0x0f & skip_speed));
	WriteSPI_with_wait_interrupt(0x53);
	WriteSPI_with_wait_interrupt(0xEF);
	WriteSPI_with_wait_interrupt(0x6E);
	WriteSPI_with_wait_interrupt(sine_test);
	WriteSPI_with_wait_interrupt(0x00);
	WriteSPI_with_wait_interrupt(0x00);
	WriteSPI_with_wait_interrupt(0x00);
	WriteSPI_with_wait_interrupt(0x00);

	while(1);
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
