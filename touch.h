
// タッチ制御用　ポートの定義
// SW0 (P36)
#define SW_0_PDR      (PORT3.PDR.BIT.B6)   //  出力または入力ポートに指定
#define SW_0_PODR     (PORT3.PODR.BIT.B6)  //  出力データ

// SW1 (P31)
#define SW_1_PMR      (PORT3.PMR.BIT.B1)   //  汎用入出力ポート
#define SW_1_PDR      (PORT3.PDR.BIT.B1)   //  出力または入力ポートポートに指定
#define SW_1_PODR     (PORT3.PODR.BIT.B1)  //  出力データ

// SW2 (P30)
#define SW_2_PMR      (PORT3.PMR.BIT.B0)   //  汎用入出力ポート
#define SW_2_PDR      (PORT3.PDR.BIT.B0)   //  出力または入力ポートポートに指定
#define SW_2_PODR     (PORT3.PODR.BIT.B0)  //  出力データ


// SW3 (P37)
#define SW_3_PDR      (PORT3.PDR.BIT.B7)   //  出力または入力ポートポートに指定
#define SW_3_PODR     (PORT3.PODR.BIT.B7)  //  出力データ




extern uint16_t   ad_x_val;	// X軸データ (A/D変換データ)
extern uint16_t   ad_y_val;     // Y軸データ

extern uint16_t   ad_z1_val;	// 　(タッチ圧測定用)
extern uint16_t   ad_z2_val;


void touch_port_ini(void);

void touch_position(void);

