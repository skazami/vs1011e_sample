#include <p24FJ64GA002.h>
#include <stdio.h>
#include <spi.h>
#include "vs1011e_drv.h"


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
