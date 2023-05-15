#include	<machine.h>
#include	 "iodefine.h"
#include	 "misratypes.h"
#include	"delay.h"
#include 	"dma.h"
#include	"sci.h"
#include	"rspi_9bit.h"
#include	 "ILI9488_9bit_dma.h"
#include	"timer.h"
#include	"lcd_num_disp.h"
#include	"lcd_disp.h"
#include        "touch.h"
#include	"s12ad.h"

#include	"dsad.h"
#include        "pid.h"
#include	"mtu.h"
#include        "thermocouple.h"
#include	"dataflash.h"
#include 	"key.h"

#include	"crc_16.h"
#include       "rom_crc_data.h"
#include	"alarm.h"


extern volatile uint8_t  rcv_data[128];

void clear_module_stop(void);

uint8_t flg_control_interval;    // 1 = 制御タイミング 

void main(void)
{
	
	clear_module_stop();	//  モジュールストップの解除
	
	alarm_port_ini();	// アラームLED用ポート設定
	
	ROM_CRC_Err_Check();	// ROM CRCの確認
	
	if ( rom_crc_16_err == 1 ) {    // ROM CRC異常の場合
	
	   while(1){		        // 無限ループ
	        alm_1 = 1;
	    	ALM_1_PODR = 1; 	//  High(ALM1_LED点灯)
		delay_msec(100);   	// 100[msec]待ち
		ALM_1_PODR = 0; 	//  Low(ALM1_LED消灯)
		delay_msec(100); 	// 100[msec]待ち
	   }
	}
	
	
	data_flash_enable();	//  E2データフラッシュへのアクセス許可
	
	pid_para_flash_read();	 // PIDパラメータ (SV, PID等)をデータフラッシュから読み出し
	
	DMA0_ini();		//  PCへのシリアルデータ受信用のDMA処理　初期設定
	
	DMAC0.DMSAR = (void *)&SCI1.RDR;	 // 転送元アドレス SCI1 受信データレジスタ		
	DMAC0.DMDAR = (void *)&rcv_data[0];	 // 転送先アドレス 受信バッファ
	DMAC0.DMCRA = 8; 	 	// 転送回数 (受信バイト数　8byte固定)	
	DMAC0.DMCNT.BIT.DTE = 1;    // DMAC0 (DMAC チャンネル0) 転送許可
	
	
	DMA1_ini();           	// PCへのシリアルデータ送信用のDMA処理　初期設定	
	initSCI_1();		// SCI1(パソコンとの通信用)  初期設定 76.8K

	
	LED_comm_port_set();	// 送信時のLEDポート設定
	
	DMA2_ini();           	// DMA チャンネル0( LCDへのデータ送信用)　初期設定
	RSPI_Init_Port();	// RSPI ポートの初期化  (LCDコントローラ用)   
     	RSPI_Init_Reg();        // SPI レジスタの設定  

     	RSPI_SPCMD_0();	        // SPI 転送フォーマットを設定, SSLA0使用	
	
	ILI9488_Reset();	// LCD のリセット	
	 
	ILI9488_Init();		// LCDの初期化
	
	delay_msec(10);		// LCD(ILI9488)初期化完了待ち
	
	IWDT_Refresh();		// ウオッチドックタイマリフレッシュ
	
	disp_black();		// 画面　黒  ( 106 [msec] at SPI Clock=16[MHz] )
	
	IWDT_Refresh();		// ウオッチドックタイマリフレッシュ
	
	disp_name();		// PV等の表示
	
	disp_switch();		// スイッチの表示
	
	s12ad_ini();		// A/Dコンバータ S12AD初期設定
	
	touch_port_ini();	// タッチパネル制御用ポート設定
	
	afe_ini();		// AFE(アナログフロントエンド)設定
	
	dsad0_ini();		// DASD0の設定　(熱電対用 2チャンネル)
	dsad1_ini();            // DASD1の設定 (基準接点補償 RTD 100 ohm)
	
	
	ad_index = 0;		// 各チャンネルのデータ格納位置の初期化
	ad1_index = 0;
	
	PWM_Init_Reg();		// PWM用の設定 周期 200msec(=制御周期)

	PWM_Port_Set();		// Heat用,Cool用出力ポートの設定
	
	 
	Timer10msec_Set();      // タイマ(10msec)作成(CMT0)
     	Timer10msec_Start();    // タイマ(10msec)開始　
	
	touch_pre_status_ini();		// 全て非タッチ状態とする。
	
	IWDT_Refresh();		// ウオッチドックタイマリフレッシュ
	
	while(1){
           if ( flg_100msec_interval == 1 ) {  // 100msec経過
	      
	       flg_100msec_interval = 0;	// 100msec経過フラグのクリア
	   	   
	        if ( flg_control_interval  == 0 ) { // 非制御タイミング(=A/D変換開始)	
		   
		
		   
		  dsad0_start();		// DSAD0開始
		  dsad1_start();		// DSAD1開始
		  
		  dsad0_scan_over = 0;
		  dsad1_scan_over = 0;

		 flg_control_interval = 1;	// 次回は制御タイミング
		 
		 
	      }
	      else  {	// 制御タイミング
                 	  
		while ( (dsad0_scan_over == 0 )||( dsad1_scan_over == 0) ) {  // 66.6 msec, dsad0 (ch0～ch3) とdsad1(ch0)のスキャン完了確認待ち
		}    

		dsad0_stop();		 // DSAD0 停止 (dsad0とdsad1の停止　1.3 usec)
		dsad1_stop();		 // DSAD1 停止
		
		Cal_ad_avg();		 // dsad0 各チャンネルの平均値を得る
	   	Cal_ad1_avg();		 // dsad1 各チャンネルの平均値を得る
		
		tc_temp_cal();		 // 温度計算
		
		if (( tc_temp[0] > 110.0 ) || ( tc_temp[1] > 110.0 )){ //  110℃以上で断線エラーとする
			alm_3 = 1;		// 熱電対　断線エラー
		}
		else{
			alm_3 = 0;
		}
		
		pid_pv = tc_temp[0];	  // Ch1を PV値とする。
		
		control();		  // 制御処理
		  
		flg_control_interval = 0; // 次回は非制御タイミング
		
	     }       // 制御タイミング	
	       
	       
	                                        // 100mse毎の共通処理
						
	     if ( rcv_over == 1 ) {		// 通信処理 コマンド受信の場合
	     	   LED_RX_PODR = 0;		// 受信 LEDの消灯
  		    
		    comm_cmd();			// レスポンス作成、割り込みによる送信開始
		   
	   	    rcv_over = 0;		// コマンド受信フラグのクリア
	       }				
			
	       
	       key_input();			// キー入力処理
	       
	                                        // 以下の表示 35[msec]ぐらいかかる 
	     
	       disp_symbol_para(para_index);	// 項目名 SV,P,I,D　を表示
	       disp_para_value(para_index);	// SP,P,I,D等のパラメータの値表示
	       
	       disp_mv_bar_data();		// MV 値の表示（棒グラフとデータ) 6.8 msec
	       
	       disp_float_data(pid_pv, 80, 32, 1, COLOR_WHITE );   // PV 表示 サイズ(48x96) 

	       disp_symbol_stop_run(mode_stop_run); // 状態 stop/run の表示

	       disp_symbol_auto_manual(mode_auto_manual); // 状態 auto/manual の表示
	   
	       disp_symbol_heat_cool(heat_cool);  // 制御モード(heater,cooler,heat-cool)
	       
	       alarm_led();			  // アラーム発生時のLED点灯処理
	       
	       IWDT_Refresh();			// ウオッチドックタイマリフレッシュ
	      
	   }  // 100msec経過

	}   // whiel(1)
	
}





// モジュールストップの解除
//
//   コンペアマッチタイマ(CMT) ユニット0(CMT0, CMT1) 
//   マルチファンクションタイマパルスユニット（MTU0 ～ MTU5）
//   アナログフロントエンド(AFE)
//   24ビットΔ-Σ A/D コンバータ(DSAD0) ユニット0
//   24ビットΔ-Σ A/D コンバータ(DSAD1) ユニット1
//   12 ビットA/D コンバータ (S12ADE)
//   シリアルペリフェラルインタフェース0(RSPI)
//   DMA コントローラ(DMACA)
//  シリアルコミュニケーションインタフェース1(SCI1)(パソコンとの通信用)
//  CRC 演算器（CRC）
//

void clear_module_stop(void)
{
	SYSTEM.PRCR.WORD = 0xA50F;	// クロック発生、消費電力低減機能関連レジスタの書き込み許可	
	
	MSTP(CMT0) = 0;			// コンペアマッチタイマ(CMT) ユニット0(CMT0, CMT1) モジュールストップの解除
	
	MSTP(MTU) = 0;			// マルチファンクションタイマパルスユニット モジュールストップの解除
	MSTP(AFE) = 0;			// アナログフロントエンド(AFE) モジュールストップの解除
	MSTP(DSAD0) = 0;		// 24 ビットΔ-Σ A/D コンバータ(DSAD0) ユニット0 モジュールストップの解除
	MSTP(DSAD1) = 0;		//             :                        ユニット1 
	
	MSTP(S12AD) = 0;               //  A/Dコンバータのモジュールストップ解除
	
	MSTP(RSPI0) = 0;		// シリアルペリフェラルインタフェース0 モジュールストップの解除
	MSTP(DMAC) = 0;                //  DMA モジュールストップ解除
	
	MSTP(SCI1) = 0;	        	// SCI1 モジュールストップの解除
	MSTP(CRC) = 0;			// CRC モジュールストップの解除
	
	SYSTEM.PRCR.WORD = 0xA500;	// クロック発生、消費電力低減機能関連レジスタ書き込み禁止
}

