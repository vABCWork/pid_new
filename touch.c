#include "iodefine.h"
#include "misratypes.h"

#include "touch.h"


uint16_t   ad_x_val;		// X軸データ (A/D変換データ)
uint16_t   ad_y_val;            // Y軸データ

uint16_t   ad_z1_val;		// 　(タッチ圧測定用)
uint16_t   ad_z2_val;


//
//  抵抗膜式タッチパネル端子へ電圧を印加するための制御信号
//　SW0～SW3は、74HC125への出力
//
void touch_port_ini(void)
{
	SW_0_PDR = 1;	// 出力ポートに指定
	
	SW_1_PMR = 0;	// 汎用入出力ポート
	SW_1_PDR = 1;	// 出力ポートに指定
	
	SW_2_PMR = 0;	// 汎用入出力ポート
	SW_2_PDR = 1;	// 出力ポートに指定
	
	SW_3_PDR = 1;	// 出力ポートに指定
	
}


//
//   タッチ位置を得る
//　　74HC125への制御線 SW_0,SW_1,SW_2,SW_3は Low enable.
//  a) X測定時: 
//     SW_0 = Low, SW_1 = Low , SW_2 = High, SW_3 = High により XP=Vcc ,XN=GNDとなる。YP(AN004)のA/D値が X位置となる。
//  b) Y測定時:
//      SW_2 = Low, SW_3 = Low, SW_0 = High, SW_1 = High により YP=Vcc ,YN=GNDとなり、XP(AN003)のA/D値が Y位置となる。  
//  c) Z1,Z2測定時:
//      SW_0 = Low, SW_3 =  Low, SW_1 = High, SW_2 = Highにより、YP=Vcc, XN=GNDとなり、XP(AN003)のA/D値が、Z1になる。YN(AN002)のA/D値が、Z2になる。
//  
//   A/D変換時間 : 1チャネル当たり1.4 [μs[ (A/D変換クロックADCLK = 32 MHz動作時)
//
void touch_position(void)
{
			// X測定
	SW_0_PODR = 0;			// X-(GND)有効 
	SW_1_PODR = 0;                  // X+(Vcc)有効
	SW_2_PODR = 1;			// Y- ハイインピーダンス  
	SW_3_PODR = 1;			// Y+ ハイインピーダンス

	
	S12AD.ADANSA0.BIT.ANSA004 = 1;  // AN004 変換対象とする

	S12AD.ADCSR.BIT.ADST = 1;	// A/D 変換開始
	while( S12AD.ADCSR.BIT.ADST == 1){  // A/D 変換終了待ち 
	}
	
	ad_x_val = S12AD.ADDR4;		// AN004のA/D変換値 (0～4095)　
	S12AD.ADANSA0.BIT.ANSA004 = 0;  // AN004 変換対象としない
	
	
			// Y測定
	SW_0_PODR = 1;			// X- ハイインピーダンス 
	SW_1_PODR = 1;			// X+ ハイインピーダンス 		
	SW_2_PODR = 0;  		// Y-(GND)有効 
	SW_3_PODR = 0;  		// Y+(Vcc)有効
			
	
	S12AD.ADANSA0.BIT.ANSA003 = 1;  // AN003 変換対象とする

	S12AD.ADCSR.BIT.ADST = 1;	// A/D 変換開始
	while( S12AD.ADCSR.BIT.ADST == 1){  // A/D 変換終了待ち 
	}
	
	ad_y_val = S12AD.ADDR3;		// AN003のA/D変換値 (0～4095)　
	S12AD.ADANSA0.BIT.ANSA003 = 0;  // AN003 変換対象としない
	
	
			// Z1,Z2 測定
        SW_1_PODR = 1;			// X+ ハイインピーダンス
	SW_2_PODR = 1;			// Y-　ハイインピーダンス
	SW_0_PODR = 0;			// X- (GND)有効
	SW_3_PODR = 0;			// Y+ (VCC 有効)
	
	S12AD.ADANSA0.BIT.ANSA003 = 1;  // AN003 変換対象とする

	S12AD.ADCSR.BIT.ADST = 1;	// A/D 変換開始
	while( S12AD.ADCSR.BIT.ADST == 1){  // A/D 変換終了待ち 
	}
	
	ad_z1_val = S12AD.ADDR3;	// AN003のA/D変換値 (0～4095)　
	S12AD.ADANSA0.BIT.ANSA003 = 0;  // AN003 変換対象としない
	

	
	S12AD.ADANSA0.BIT.ANSA002 = 1;  // AN002 変換対象とする

	S12AD.ADCSR.BIT.ADST = 1;	// A/D 変換開始
	while( S12AD.ADCSR.BIT.ADST == 1){  // A/D 変換終了待ち 
	}
	
	ad_z2_val = S12AD.ADDR2;	// AN002のA/D変換値 (0～4095)　
	S12AD.ADANSA0.BIT.ANSA002 = 0;  // AN002 変換対象としない
	

					// 測定終了後　全てハイインピーダンス
	SW_0_PODR = 1;			// X- ハイインピーダンス
	SW_1_PODR = 1;                  // X+ ハイインピーダンス
	SW_2_PODR = 1;			// Y- ハイインピーダンス  
	SW_3_PODR = 1;			// Y+ ハイインピーダンス
	
}
