
// �A���[���p�@LED�o�̓|�[�g
// ALM1 : ROM-CRC�G���[
// ALM1: P17
#define ALM_1_PMR      (PORT1.PMR.BIT.B7)   //  �ėp���o�̓|�[�g
#define ALM_1_PDR      (PORT1.PDR.BIT.B7)   //  �o�͂܂��͓��̓|�[�g�|�[�g�Ɏw��
#define ALM_1_PODR     (PORT1.PODR.BIT.B7)  //  �o�̓f�[�^

// ALM2: �M�d�΂̒f���G���[�@
// ALM2: P14
#define ALM_2_PMR      (PORT1.PMR.BIT.B4)   //  �ėp���o�̓|�[�g
#define ALM_2_PDR      (PORT1.PDR.BIT.B4)   //  �o�͂܂��͓��̓|�[�g�|�[�g�Ɏw��
#define ALM_2_PODR     (PORT1.PODR.BIT.B4)  //  �o�̓f�[�^

// ALM3: �p�\�R���Ƃ̒ʐM CRC�G���[
// ALM3: PH3
#define ALM_3_PMR      (PORTH.PMR.BIT.B3)   //  �ėp���o�̓|�[�g
#define ALM_3_PDR      (PORTH.PDR.BIT.B3)   //  �o�͂܂��͓��̓|�[�g�|�[�g�Ɏw��
#define ALM_3_PODR     (PORTH.PODR.BIT.B3)  //  �o�̓f�[�^

// ALM4: (�����{)
// ALM4: PH2
#define ALM_4_PMR      (PORTH.PMR.BIT.B2)   //  �ėp���o�̓|�[�g
#define ALM_4_PDR      (PORTH.PDR.BIT.B2)   //  �o�͂܂��͓��̓|�[�g�|�[�g�Ɏw��
#define ALM_4_PODR     (PORTH.PODR.BIT.B2)  //  �o�̓f�[�^


extern	uint8_t	alm_1;
extern	uint8_t alm_2;
extern	uint8_t alm_3;
extern  uint8_t alm_4;


void alarm_led(void);
void alarm_port_ini(void);
