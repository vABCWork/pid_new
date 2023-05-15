
// SW�̌�
#define KEY_SW_NUM	6	// SW��6��  STOP/RUN(SW0),Auto/Manual(SW1),Para(SW2),��(Down)(SW3),��(Up)(SW4), Save(SW5)

// Up �L�[,Down�L�[�̒��������́A������
#define LONG_PUSH_CNT   5	// �������́A1�񉟂��̏�����5��J��Ԃ�

// �X�C�b�`���͊֌W�̍\����
struct SW_info
{
	uint8_t status;		// ����� �^�b�`(Low=0),��^�b�`(High=1) ��� (10msec��)
	uint8_t pre_status;	// �O���   :
	uint8_t low_cnt;	// �^�b�`(ON)�̉�
	uint8_t one_push;	// 0:�L�[���͏����v���Ȃ� 1:�L�[���͏����v��(1�x����)�@�i�L�[���͏����A�\�������I�����0�N���A�j
        uint8_t long_push;      // 0:�L�[���͏����v���Ȃ� 1:�L�[���͏����v��(������)�@ ( Low��High �̗��オ�茟�o���� 0�N���A)
	
};


#define PARA_MAX_NUMBER  9	// �p�����[�^�̌� 9 (0:SV, 1:P, 2:I, 3:D, 4:MR, 5:Hys, 6:H/C, 7:Ch2, 8:CJT)
// ���d�l �p�����[�^�̌� 12 (0:SV, 1:P, 2:I, 3:D, 4:MR, 5:Hys, 6:H/C, 7:Ch1, 8:Ch2, 9:Ch3, 10:Ch4, 11:CJT)

void touch_pre_status_ini(void);

void touch_xyz_press(void);
void touch_key_status_check(void);
void switch_input_check( uint8_t id ); 

void key_input(void);	

void key_up(void);
void key_down(void);

extern	uint8_t  para_index;

extern volatile struct  SW_info  Key_sw[KEY_SW_NUM];	// �X�C�b�`�@5���̏��i�[�̈�
