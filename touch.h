
// �^�b�`����p�@�|�[�g�̒�`
// SW0 (P36)
#define SW_0_PDR      (PORT3.PDR.BIT.B6)   //  �o�͂܂��͓��̓|�[�g�Ɏw��
#define SW_0_PODR     (PORT3.PODR.BIT.B6)  //  �o�̓f�[�^

// SW1 (P31)
#define SW_1_PMR      (PORT3.PMR.BIT.B1)   //  �ėp���o�̓|�[�g
#define SW_1_PDR      (PORT3.PDR.BIT.B1)   //  �o�͂܂��͓��̓|�[�g�|�[�g�Ɏw��
#define SW_1_PODR     (PORT3.PODR.BIT.B1)  //  �o�̓f�[�^

// SW2 (P30)
#define SW_2_PMR      (PORT3.PMR.BIT.B0)   //  �ėp���o�̓|�[�g
#define SW_2_PDR      (PORT3.PDR.BIT.B0)   //  �o�͂܂��͓��̓|�[�g�|�[�g�Ɏw��
#define SW_2_PODR     (PORT3.PODR.BIT.B0)  //  �o�̓f�[�^


// SW3 (P37)
#define SW_3_PDR      (PORT3.PDR.BIT.B7)   //  �o�͂܂��͓��̓|�[�g�|�[�g�Ɏw��
#define SW_3_PODR     (PORT3.PODR.BIT.B7)  //  �o�̓f�[�^




extern uint16_t   ad_x_val;	// X���f�[�^ (A/D�ϊ��f�[�^)
extern uint16_t   ad_y_val;     // Y���f�[�^

extern uint16_t   ad_z1_val;	// �@(�^�b�`������p)
extern uint16_t   ad_z2_val;


void touch_port_ini(void);

void touch_position(void);

