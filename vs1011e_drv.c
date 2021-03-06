#include <p24FJ64GA002.h>
#include <stdio.h>
#include <spi.h>
#include "vs1011e_drv.h"

// at return:
//   CS,XDCS: High
void vs1011e_power_up(void)
{
	delay_ms(1);
	VS1011E_XRESET = 1;

	VS1011E_CS    = 1;     // Initialize CS
	VS1011E_XDCS  = 1;     // Initialize XCDS
}

// at return:
//   CS,XDCS: unknown ( must be initialized at power up sequence)
void vs1011e_power_down(void)
{
	delay_ms(1);
	VS1011E_XRESET = 0;
}

// at return:
//   CS,XDCS: High
void vs1011e_hard_reset(void)
{
	vs1011e_power_down();
	delay_ms(1);
	vs1011e_power_up();
	delay_ms(1);
}

// at return:
//   CS,XDCS: High (It depends on vs1011e_sci_write())
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
//	while(!IFS2bits.SPI2IF);
	while(SPI2STATbits.SRXMPT); //暫定処理
	return SPI2BUF;
}

//
// 注意：書き込んだデータと読み込んだデータが一致しない限り、この関数からは戻らない
//
// at return:
//   CS,XDCS: High (It depends on vs1011e_sci_read())
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

// at return:
//   CS,XDCS: High
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

// at return:
//   CS,XDCS: High
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

// at return:
//   CS   : High
//   XDCS : Low
void vs1011e_sdi_write(BYTE data)
{
	VS1011E_CS = 1;
	VS1011E_XDCS  = 0;
	while( !VS1011E_DREQ );

	WriteSPI_with_wait_interrupt(data);
}

// at return:
//   CS   : High
//   XDCS : Low
void vs1011e_ram_clear(void)
{
	int i;

	for(i=0;i<2048;i++)
		vs1011e_sdi_write(0x00);
}

// Test mode implementation
// Prerequisites:
//   Test modeの関数を呼び出す前に初期化関数（vs1011e_init_for_test_mode()）を実行すること
//   vs1011e_init_for_test_mode()初期化関数の処理内容:
//       1) vs1011eのハードリセット
//       2) CS,XDCSをHigh
//       3) SCI_MODE, CLOCKF, SCI_VOLの設定(SCI_MODEはSM_SDINEWとSM_TESTSをセットしておくこと)
//       4) vs1011eのソフトリセット
//
// 引数の詳細はvs1011eのデータシートを参照
//
// Support status:
//   1) Sine Test  : applicable
//   2) Pin Test   : not applicable
//   3) Memory Test: applicable
//   4) SCI Test   : under developing

void vs1011e_init_for_test_mode(void)
{
	vs1011e_hard_reset();
	vs1011e_soft_reset();
	vs1011e_sci_write_with_verify(0x00, 0x08, 0x20);
	vs1011e_sci_write_with_verify(0x03, 0x98, 0x00);
	vs1011e_sci_write_with_verify(0x0B, 0x60, 0x60);
}


// Test mode:sine test
void vs1011e_test_sine_start(BYTE sample_rate, BYTE skip_speed)
{
	BYTE sine_test;

	sine_test = (((0x0f & sample_rate) << 4) | (0x0f & skip_speed));
	vs1011e_sdi_write(0x53);
	vs1011e_sdi_write(0xEF);
	vs1011e_sdi_write(0x6E);
	vs1011e_sdi_write(sine_test);
	vs1011e_sdi_write(0x00);
	vs1011e_sdi_write(0x00);
	vs1011e_sdi_write(0x00);
	vs1011e_sdi_write(0x00);
}

void vs1011e_test_sine_end(void)
{
	vs1011e_sdi_write(0x45); // E
	vs1011e_sdi_write(0x78); // x
	vs1011e_sdi_write(0x69); // i
	vs1011e_sdi_write(0x74); // t
	vs1011e_sdi_write(0x00);
	vs1011e_sdi_write(0x00);
	vs1011e_sdi_write(0x00);
	vs1011e_sdi_write(0x00);
}

// Test mode:memory test
BOOL vs1011e_test_memory(void)
{
	BYTE result[2];

	VS1011E_CS = 1;
	while( !VS1011E_DREQ );
	VS1011E_XDCS  = 0;
	while( !VS1011E_DREQ );

	vs1011e_sdi_write(0x4D);
	vs1011e_sdi_write(0xEA);
	vs1011e_sdi_write(0x6D);
	vs1011e_sdi_write(0x54);
	vs1011e_sdi_write(0x00);
	vs1011e_sdi_write(0x00);
	vs1011e_sdi_write(0x00);
	vs1011e_sdi_write(0x00);

	delay_ms(150); 

	vs1011e_sci_read(0x08, result);

	if( result[0] == 0x80 && result[1] == 0x7F )
		return TRUE;
	else
		return FALSE;
}

// Test mode:SCI test
// SCI_HDAT0の値と指定したアドレスのレジスタ値が一致しない限り、この関数は戻らない
// 
// register data doesn't be copied to hdat0... why?????
void vs1011e_test_sci(BYTE addr)
{
	BYTE hdat[2], read_buf[2];
	BYTE result_register_addr = 0x08;

	if( addr == 0x08 )
		result_register_addr = 0x09;

	vs1011e_sdi_write(0x53);
	vs1011e_sdi_write(0x70);
	vs1011e_sdi_write(0xEE);
	vs1011e_sdi_write(addr);
	vs1011e_sdi_write(0x00);
	vs1011e_sdi_write(0x00);
	vs1011e_sdi_write(0x00);
	vs1011e_sdi_write(0x00);

	do
	{
		vs1011e_sci_read(addr, read_buf);
		vs1011e_sci_read(result_register_addr, hdat);
	}while(read_buf[0]!=hdat[0]||read_buf[1]!=hdat[1]);
}
