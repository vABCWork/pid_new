#include "machine.h"
#include "misratypes.h"
#include "rom_crc_data.h"


// #pragma sectionでセクション名を変更する。
// 生成されたセクション名の開始アドレスは、リンクオプションのセクション開始アドレスで設定する。
//
// 例: セクション CROM_CRC_CODE_2を FFFC 0000 に割り付け
//   B_1,R_1,B_2,R_2,B,R,SU,SI/00000004,CROM_CRC_CODE_2/0FFFC0000,PResetPRG,C_1,C_2,C,C$DSEC,C$BSEC,C$INIT,C$VTBL,C$VECT,D_1,D_2,D,P,PIntPRG,W_1,W_2,W,L/0FFFC0100,EXCEPTVECT/0FFFFFF80,RESETVECT/0FFFFFFFC
//
// 参考:
//（4.2.4 拡張仕様の使用方法　(1) セクション切り替え記述） より、
//（ CC-RX コンパイラ ユーザーズマニュアル (R20UT3248JJ0111 Rev.1.11 )
//


#pragma section   ROM_CRC_CODE

const uint16_t  rom_crc_data = 0xd2b0;     // CS+により出力された、CRC値を記入する。
				      // セクション名は、CROM_CRC_CODE_2　(2は 2byteのため) 



