#include "iodefine.h"
#include "misratypes.h"

//
//  S12AD用 アナログ入力 の設定
// 自己診断無し、断線検出アシストあり
//
void s12ad_ini(void)
{
					// A/Dコントロールレジスタ(ADCSR)
	S12AD.ADCSR.BIT.ADST = 0;	// A/D 変換停止
	
	S12AD.ADCSR.BIT.ADHSC = 0;	// 高速変換動作 
	S12AD.ADCSR.BIT.ADIE = 0;	// スキャン終了後のS12ADI0割り込み発生の禁止
	S12AD.ADCSR.BIT.ADCS = 0;	// シングルスキャン モード
	
	
					// A/Dコントロール拡張レジスタ(ADCER)
	S12AD.ADCER.BIT.ACE = 0;	// 自動クリアの禁止
	
	
	S12AD.ADCER.BIT.DIAGLD = 0;     // 自己診断　電圧ローテーションモード
	
	S12AD.ADCER.BIT.DIAGM = 0;	// 自己診断を実施しない
	
	S12AD.ADCER.BIT.ADRFMT = 0;	// 右詰めフォーマット
					// 初期化時は全て、変換対象としない。
	S12AD.ADANSA0.BIT.ANSA000 = 0;  // AN000 変換対象としない
	S12AD.ADANSA0.BIT.ANSA001 = 0;  // AN001 変換対象としない
	S12AD.ADANSA0.BIT.ANSA002 = 0;  // AN002 変換対象としない
	S12AD.ADANSA0.BIT.ANSA003 = 0;  // AN003 変換対象としない
	S12AD.ADANSA0.BIT.ANSA004 = 0;  // AN004 変換対象としない
	
	S12AD.ADANSA0.BIT.ANSA005 = 0;  // AN005 変換対象としない
	
	
        S12AD.ADHVREFCNT.BIT.HVSEL = 0;  // 基準電圧(高電位側) = AVCC0
	S12AD.ADHVREFCNT.BIT.LVSEL = 0;  // 基準電圧(低電位側) = AVSS0   
	
	S12AD.ADDISCR.BIT.ADNDIS = 0x12; // A/D断線検出アシスト　プリチャージ チャージ期間 = 2ステート (31.25nsec x 2= 62.5 nsec )
}
