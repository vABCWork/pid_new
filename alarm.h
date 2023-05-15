
// アラーム用　LED出力ポート
// ALM1 : ROM-CRCエラー
// ALM1: P17
#define ALM_1_PMR      (PORT1.PMR.BIT.B7)   //  汎用入出力ポート
#define ALM_1_PDR      (PORT1.PDR.BIT.B7)   //  出力または入力ポートポートに指定
#define ALM_1_PODR     (PORT1.PODR.BIT.B7)  //  出力データ

// ALM2: 熱電対の断線エラー　
// ALM2: P14
#define ALM_2_PMR      (PORT1.PMR.BIT.B4)   //  汎用入出力ポート
#define ALM_2_PDR      (PORT1.PDR.BIT.B4)   //  出力または入力ポートポートに指定
#define ALM_2_PODR     (PORT1.PODR.BIT.B4)  //  出力データ

// ALM3: パソコンとの通信 CRCエラー
// ALM3: PH3
#define ALM_3_PMR      (PORTH.PMR.BIT.B3)   //  汎用入出力ポート
#define ALM_3_PDR      (PORTH.PDR.BIT.B3)   //  出力または入力ポートポートに指定
#define ALM_3_PODR     (PORTH.PODR.BIT.B3)  //  出力データ

// ALM4: (未実施)
// ALM4: PH2
#define ALM_4_PMR      (PORTH.PMR.BIT.B2)   //  汎用入出力ポート
#define ALM_4_PDR      (PORTH.PDR.BIT.B2)   //  出力または入力ポートポートに指定
#define ALM_4_PODR     (PORTH.PODR.BIT.B2)  //  出力データ


extern	uint8_t	alm_1;
extern	uint8_t alm_2;
extern	uint8_t alm_3;
extern  uint8_t alm_4;


void alarm_led(void);
void alarm_port_ini(void);
