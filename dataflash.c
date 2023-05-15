#include	 "iodefine.h"
#include	 "misratypes.h"
#include	 "delay.h"
#include         "pid.h"
#include	 "dataflash.h"
#include	 "crc_16.h"
#include 	 "alarm.h"


uint8_t flash_wr_buf[32];	// データフラッシュ書き込み用バッファ
uint8_t flash_rd_buf[32];       // データフラッシュ読み出し用バッファ

uint8_t  crc_x8_x5_x4_1;	// CRC-8 (x8 + x5 + X4 + 1)

//
// PID用パラメータを、データフラッシュ ブロック0へ書込み (合計 26 byte)
// 約 7msec (erase 後 28byte書込み)
// 書込みデータ(合計 28 byte)
//     No.     item     data type
//	0 	SV	float (4byte)
//	1	P	float (4byte)
//	2	I	float (4byte)
//	3	D	float (4byte)
//	4	MR	float (4byte)
//	5	Hys	float (4byte)
//      6    heat/cool  byte  (1byte)
//      7    pid_type   byte  (1byte)
//      8      CRC      word  (2byte)
//
//  出力: err_fg: = 0 成功
//              : >0  失敗  

uint32_t  pid_para_flash_write(void)
{
	uint16_t crc_cd;
	
	uint32_t i;
	uint32_t blk_index;
	uint32_t wr_num;
	uint32_t err_fg;
 
	memcpy( &flash_wr_buf[0], &pid_sv, 4);	// SV値 をflash_data_buf[0]へ
	
	memcpy( &flash_wr_buf[4], &pid_p, 4);		// P をflash_data_buf[4]へ
	
	memcpy( &flash_wr_buf[8], &pid_i, 4);		// I をflash_data_buf[8]へ
	
	memcpy( &flash_wr_buf[12], &pid_d, 4);	// D をflash_data_buf[12]へ
	
	memcpy( &flash_wr_buf[16], &pid_mr, 4);	// Mr をflash_data_buf[16]へ
	
	memcpy( &flash_wr_buf[20], &pid_hys, 4);	// Hys をflash_data_buf[20]へ
	
	memcpy( &flash_wr_buf[24], &heat_cool, 1);	// heat_cool をflash_data_buf[24]へ
		
	memcpy( &flash_wr_buf[25], &pid_type, 1);	// pid_type をflash_data_buf[25]へ
	
	
	crc_x8_x5_x4_1 = Calc_crc_x8_x5_x4_1(&flash_wr_buf[0],26);   // CRC-8(X8+X5+X4+1)の計算

	
	flash_wr_buf[26] = crc_x8_x5_x4_1; // CRC-8
	
	
	
	data_flash_enter_pe_mode();	// E2データフラッシュをリードモードからP/E モードにする	
	blk_index = 0;			// 書き込み対象ブロック
	
	err_fg = data_flash_erase( blk_index );	// イレーズ
	if ( err_fg > 0 ) {			// エラー発生の場合
		data_flash_exit_pe_mode();	//  E2データフラッシュをP/E モードからリードモードにする
		return err_fg;
	}
	
	wr_num = 27;		// 書き込みバイト数
  
	err_fg = data_flash_write(blk_index, &flash_wr_buf[0], wr_num);  // 書込み
	if ( err_fg > 0 ) {			// エラー発生の場合
		data_flash_exit_pe_mode();	//  E2データフラッシュをP/E モードからリードモードにする
		return err_fg;
	}
	
        data_flash_exit_pe_mode();		//  E2データフラッシュをP/E モードからリードモードにする
	
	return 0;
	
}


//
// パラメータのデータフラッシュからの読み出しと、CRCチェック
//
//     BLOCK_0:
//     No.     item     data type
//	0 	SV	float (4byte)
//	1	P	float (4byte)
//	2	I	float (4byte)
//	3	D	float (4byte)
//	4	MR	float (4byte)
//	5	Hys	float (4byte)
//      6    heat/cool  byte (1byte)
//      7    pid_type   byte (1byte)
//      8      CRC-8    byte (1byte)
//
// 注)E2データフラッシュがイレーズされると、データは0xffとなる。
//   　浮動小数点データが 0xffff fffffの場合、Nan(Not a number)となり、
//  　 SV値等のパラメータの値もNanとなってしまうので、この場合には、0に初期化している。
//   

void pid_para_flash_read(void)
{
	uint8_t  crc;
	uint32_t i;
	uint32_t blk_index;
	uint32_t blk_start;
	uint32_t rd_num;
	
	uint32_t u_sv;
	
	
	blk_index = 0;			// 読出し対象ブロック
	blk_start = 0x400 * ( blk_index ) +  DATA_FLASH_READ_BASE_ADRS;   // ブロック開始アドレス（読出し用)
	rd_num = 27;					        // 読み出しバイト数
	
	memcpy( &flash_rd_buf[0], blk_start, rd_num);	//  データフラッシュ block0(0x00100000) からflash_data_buf[0]へ 26byte分コピー

	memcpy( &u_sv, &flash_rd_buf[0], 4);		// flash_data_buf[0]から、仮のSV値へ読み出し
	
	if (  u_sv == 0xffffffff ) {			// SV値の浮動小数データ 0xffffffff (Nan)の場合、全てのパラメータを、0にする。
	    pid_sv = 0;
	    pid_p = 0;
	    pid_i = 0;
	    pid_d = 0;
	    pid_mr = 0;
	    pid_hys = 0;
	    heat_cool = 0;
	    pid_type = 0;
	    
	    return;
	}
	
	crc = Calc_crc_x8_x5_x4_1(&flash_rd_buf[0],27);   // CRC-8(X8+X5+X4+1)の計算
	
	
	if ( crc != 0 ) {   // CRCの演算結果 NGの場合
	
		alm_2 = 1;	   // E2データフラッシュ　CRCエラー有り
	}
	else{
		alm_2 = 0;	 // E2データフラッシュ　CRCエラー無し
	}
				 // CRCエラーが発生しても読み出す。	
	memcpy( &pid_sv, &flash_rd_buf[0], 4);		// flash_data_buf[0]から、SV値へ
	
	memcpy( &pid_p, &flash_rd_buf[4], 4);		// flash_data_buf[4]から Pへ
	
	memcpy( &pid_i, &flash_rd_buf[8], 4);		// flash_data_buf[8]から Iへ
	
	memcpy( &pid_d, &flash_rd_buf[12], 4);		// flash_data_buf[12]から Dへ
	
	memcpy( &pid_mr,&flash_rd_buf[16], 4);		// flash_data_buf[16]からMrへ
	
	memcpy( &pid_hys,&flash_rd_buf[20], 4);		// flash_data_buf[20]からHysへ
	
	memcpy( &heat_cool,&flash_rd_buf[24],1 );	// flash_data_buf[24]からheat_coolへ
	
	memcpy( &pid_type,&flash_rd_buf[25], 1);	// flash_data_buf[28]からpid_typeへ (未使用)
	
	
}




// CRC-8の計算 
// CRC-8-Maxim: X8+X5+X4+1 (0x31) 初期値=0xff
//
// 下記サンプルプログラムより引用
// STM32 の AHT20 ルーチン (aht20_stm32 demo v1_4)」 (http://www.aosong.com/class-36.html)
// 
//
uint8_t Calc_crc_x8_x5_x4_1(volatile uint8_t *data, uint8_t num)
{
        uint8_t i;
        uint8_t pt;
        uint8_t crc;
	
	crc = 0xff;

	for ( pt = 0; pt < num; pt++ ) {
  
         crc ^= data[pt];
    
	 for ( i = 8 ;i >0 ; --i)  {
    
           if ( crc & 0x80 ) {
               crc = ( crc << 1 ) ^ 0x31;
	   }
           else{
	       crc = ( crc << 1 );
	   }
	 }
       }
 
       return crc;
}





//
// E2データフラッシュへのアクセス許可
// (E2 データフラッシュアクセス禁止モードからリードモードへ) 
//
void data_flash_enable(void)
{
	FLASH.DFLCTL.BIT.DFLEN = 1;		// E2データフラッシュへのアクセス許可

	delay_5usec();	  // E2 データフラッシュSTOP 解除時間(tDSTOP)待ち　(5[usec]以上)
}



//
//  E2データフラッシュへの書込み
//
// 入力: block_index:  ブロックインデックス (0～7) 
// 出力: err_fg = 0 :書込み成功
//              = 1 :書き込み失敗
//  		= 2 :ブロック範囲外または、書込みバイト数が1Kbyteを超えている
//
//
//  E2データフラッシュ領域: 0x0010 0000から0x0010 1FFFまで 8Kbyte
//                          1ブロック(1Kbyte)の8ブロックで構成。
//
// ブロックindex :   読出しアドレス,     　　 :　書込み(プログラム)、イレーズ用アドレス
//     0         : 0x0010 0000 - 0x0010 03FF  :  0xFE00 0000 - 0xFE00 03FF　 
//     1         : 0x0010 0400 - 0x0010 07FF  :  0xFE00 0400 - 0xFE00 07FF　 
//     2         : 0x0010 0800 - 0x0010 0BFF  :  0xFE00 0800 - 0xFE00 0BFF　 
//     3         : 0x0010 0C00 - 0x0010 0FFF  :  0xFE00 0C00 - 0xFE00 0FFF　 
//     4         : 0x0010 1000 - 0x0010 13FF　:  0xFE00 1000 - 0xFE00 13FF
//     5         : 0x0010 1400 - 0x0010 17FF  :  0xFE00 1400 - 0xFE00 17FF
//     6         : 0x0010 1800 - 0x0010 1BFF  :  0xFE00 1800 - 0xFE00 1BFF
//     7         : 0x0010 1C00 - 0x0010 1FFF  :  0xFE00 1C00 - 0xFE00 1FFF
//
//　注意:
//  書込み用アドレスと読出しアドレスが異なります。
//  データを書き込む前にイレーズが必要です。
//  イレーズは1ブロック(1 Kbyte)単位です。
//  データフラッシュへの書き込みは1バイト単位で行います。
//

uint32_t  data_flash_write(uint32_t block_index, uint8_t *wr_buf, uint32_t wr_num) 
{
	uint32_t 	i;
	uint32_t	block_start;	
	uint32_t	block_end;

	
	if ( ( block_index > 7 ) || ( wr_num > 1024 ))  {  // ブロック番号が8以上または書込みバイト数が1Kbyteを超えている場合
		return 2;
	}
	
	block_start = 0x400 * ( block_index ) +  DATA_FLASH_WRITE_BASE_ADRS;   // ブロック開始アドレス 書込み用)
	block_end = block_start + 0x3ff;				   // ブロック終了アドレス 
	
	FLASH.FASR.BIT.EXS = 0;				// フラッシュ領域はデータ領域
	
	FLASH.FSARH = block_start >> 16;		// フラッシュ処理開始アドレスの設定
	FLASH.FSARL = block_start & 0xFFFF;
	
	
	for ( i = 0; i < wr_num; i++ ) {		// 書き込み処理
	
		FLASH.FWB0 = wr_buf[i];		// 書き込みデータ(1byte)セット
	
		FLASH.FCR.BYTE = 0x81;			// 書き込み実行
	
		while( FLASH.FSTATR1.BIT.FRDY == 0 ) {	// 処理完了待ち　(処理完了でFRDY = 1)
		}
	
		FLASH.FCR.BYTE = 0x00;			// 処理停止
		while( FLASH.FSTATR1.BIT.FRDY == 1 ) {	// FRDY = 0の確認待ち (FRDY=0後、次の処理実行可能)
		}
	
	
		if (( FLASH.FSTATR0.BIT.PRGERR == 1 ) || ( FLASH.FSTATR0.BIT.ILGLERR == 1 ))  // プログラム中にエラー発生またはイリーガルなコマンド発行
		{
			FLASH.FRESETR.BIT.FRESET = 1;	// フラッシュのリセット
    			FLASH.FRESETR.BIT.FRESET = 0;	// リセット解除
		
			return	0x01;			// 書き込み失敗
		}
	}
	
	return 0;			// 書き込み成功	
	
}



//
// E2データフラッシュ ブロック単位のイレーズ
// 入力: block_index:  ブロックインデックス (0～7) 
//
// 出力: err_fg = 0 :イレーズ成功
//              = 1 :イレーズ失敗
//              = 2 :ブロックインデックス範囲外
//

uint32_t  data_flash_erase(uint32_t block_index )
{
	uint32_t	block_start;	
	uint32_t	block_end;

	if  ( block_index > 7 )  {  		// ブロックインデックスが8以上の場合
		return 2;
	}
	
	block_start = 0x400 * ( block_index ) +  DATA_FLASH_WRITE_BASE_ADRS;   // ブロック開始アドレス (書込み用)
	block_end = block_start + 0x3ff;			           // ブロック終了アドレス 
	
	FLASH.FASR.BIT.EXS = 0;			// フラッシュ領域はデータ領域
	
	FLASH.FSARH = block_start >> 16;	// フラッシュ処理開始アドレスの設定
	FLASH.FSARL = block_start & 0xFFFF;
	
	FLASH.FEARH = block_end >> 16;		// フラッシュ処理終了アドレスの設定
	FLASH.FEARL = block_end  & 0xFFFF;
	
	FLASH.FCR.BYTE = 0x84;					// ブロックのイレーズ開始
	
	while( FLASH.FSTATR1.BIT.FRDY == 0 ) {			// 処理完了待ち　(処理完了でFRDY = 1)
	}
	
	FLASH.FCR.BYTE = 0x00;					// 処理停止
	while( FLASH.FSTATR1.BIT.FRDY == 1 ) {			// FRDY = 0の確認待ち (FRDY=0後、次の処理実行可能)
	}
	
	
	if (( FLASH.FSTATR0.BIT.ERERR == 1 ) || ( FLASH.FSTATR0.BIT.ILGLERR == 1 ))  // イレーズ中にエラー発生またはイリーガルなコマンド発行
	{
		FLASH.FRESETR.BIT.FRESET = 1;			// フラッシュのリセット
    		FLASH.FRESETR.BIT.FRESET = 0;			// リセット解除
		
		return	0x01;		// イレーズ失敗
	}
	
	return 0;			// イレーズ成功
	
}




//
// E2データフラッシュ 全領域のイレーズ 
//
// 出力: err_fg = 0 :イレーズ成功
//              = 1 :イレーズ失敗
//

uint32_t  data_flash_erase_all( void )
{
	uint32_t	block_start;	
	uint32_t	block_end;

	block_start = DATA_FLASH_WRITE_BASE_ADRS;    			// 先頭ブロック開始アドレス (書込み用)
	block_end =   0x400 *7 + DATA_FLASH_WRITE_BASE_ADRS +  0x3ff;   // 最終ブロック終了アドレス 
	
	FLASH.FASR.BIT.EXS = 0;			// フラッシュ領域はデータ領域
	
	FLASH.FSARH = block_start >> 16;	// フラッシュ処理開始アドレスの設定
	FLASH.FSARL = block_start & 0xFFFF;
	
	FLASH.FEARH = block_end >> 16;		// フラッシュ処理終了アドレスの設定
	FLASH.FEARL = block_end  & 0xFFFF;
	
	FLASH.FCR.BYTE = 0x86;					// 全ブロックのイレーズ開始
	
	while( FLASH.FSTATR1.BIT.FRDY == 0 ) {			// 処理完了待ち　(処理完了でFRDY = 1)
	}
	
	FLASH.FCR.BYTE = 0x00;					// 処理停止
	while( FLASH.FSTATR1.BIT.FRDY == 1 ) {			// FRDY = 0の確認待ち (FRDY=0後、次の処理実行可能)
	}
	
	
	if (( FLASH.FSTATR0.BIT.ERERR == 1 ) || ( FLASH.FSTATR0.BIT.ILGLERR == 1 ))  // イレーズ中にエラー発生またはイリーガルなコマンド発行
	{
		FLASH.FRESETR.BIT.FRESET = 1;			// フラッシュのリセット
    		FLASH.FRESETR.BIT.FRESET = 0;			// リセット解除
		
		return	0x01;		// イレーズ失敗
	}
	
	return 0;			// イレーズ成功
	
}



//
// E2データフラッシュをリードモードからP/E モードにする
//
void data_flash_enter_pe_mode(void)
{
		
	FLASH.FENTRYR.WORD =0xAA80;	// E2データフラッシュP/Eモードに設定
	delay_5usec();	  		// E2 データフラッシュSTOP 解除時間(tDSTOP)待ち　(5[usec]以上)
	
					// 高速動作モードで動作(SYSTEM.OPCCR.BIT.OPCM = 0)
	FLASH.FPR = 0xA5;		// プロテクト解除
	FLASH.FPMCR.BYTE = 0x10;	// リードモードからE2 データフラッシュP/E モードに遷移させる
        FLASH.FPMCR.BYTE = 0xEF;
	FLASH.FPMCR.BYTE = 0x10;
	
	FLASH.FISR.BIT.PCKA = 0x1F;	// FCLK=32 MHz
	
}


//
// E2データフラッシュをP/E モードからリードモードにする
//
void data_flash_exit_pe_mode(void)
{
	FLASH.FPR = 0xA5;		// プロテクト解除
	FLASH.FPMCR.BYTE = 0x08;	// P/E モードからリードモードに遷移させる
        FLASH.FPMCR.BYTE = 0xF7;
	FLASH.FPMCR.BYTE = 0x08;
	
	delay_5usec();	  		// E2 データフラッシュSTOP 解除時間(tDSTOP)待ち　(5[usec]以上)
	
	FLASH.FENTRYR.WORD =0xAA00;	// リードモードに設定
	
	while(0x0000 != FLASH.FENTRYR.WORD) // 設定の確認
	{
	}
	
}