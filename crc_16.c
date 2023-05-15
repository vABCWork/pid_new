
#include	"iodefine.h"
#include	"misratypes.h"
#include	"crc_16.h"
#include	"rom_crc_data.h"

uint8_t rom_crc_16_err;  // ROMのCRCが不一致の場合=1


uint16_t	calculated_crc; // CRC演算器による計算値(データだけCRC計算)
uint16_t	invert_crc;	// 反転値

// CRC計算のテスト
//  ビルド時リンカーから出力されたCRC値(rom_crc_data)とCRC演算器で計算してビット反転した値(inver_crc)が一致すれば、
//  CRCが一致している。
//  CRC計算範囲は、 0xfffc0100～0xffffff7f
//
//
// CRC演算器：
// 生成多項式 (CRC-CCITT XModem (h'1021) ( X^16 + X^12 + X^5 + 1))
// 初期値: 0xffff
//  MSBファースト
//
void ROM_CRC_Err_Check(void)
{
	uint8_t *pt;
	
	uint32_t i;
	
	uint32_t start_adrs;
	uint32_t end_adrs;
	
	
	Init_CRC();			// CRC演算器の初期化
	
	
	start_adrs = 0xfffc0100;	     // プログラム先頭アドレス
	end_adrs = 0xffffff7f;	             // 最終アドレス
	
	
	pt = (uint8_t *)start_adrs;
	
	for ( i = 0 ; i < (end_adrs - start_adrs + 1) ; i++ ) {	     // CRCの計算

		CRC.CRCDIR = *pt;
		
		pt++;
	}
	
	       
	calculated_crc = CRC.CRCDOR;        // CRCの演算結果
	
	invert_crc = ~calculated_crc;	    // ビット反転
	
	if ( rom_crc_data != invert_crc ) { // CRCの不一致の場合	
		rom_crc_16_err = 1;
	}
	else{				    // CRC一致の場合
		rom_crc_16_err = 0;
	}
	
}






// CRC演算器の初期化
// CRC-CCITT
// 生成多項式 : X^16 + X^12 + X^5 + 1
// 初期値: 0xffff
//  MSBファースト
//
void Init_CRC(void)
{
	
	CRC.CRCCR.BYTE = 0x87;		     // CRCDORレジスタをクリア, MSBファースト, 16ビットCRC（X^16 + X^12 + X^5 + 1 ）
	CRC.CRCDOR = 0xffff;		     // 初期値

}

