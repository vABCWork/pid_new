
#include "typedefine.h"
#include  "iodefine.h"
#include "misratypes.h"
#include "timer.h"
#include "key.h"
#include "crc_16.h"

extern  uint8_t alm_4;

extern  volatile uint8_t  rcv_cnt;
extern	volatile uint8_t rcv_over;

// Timer 
volatile uint8_t flg_100msec_interval;	// 100msec����ON

volatile uint8_t timer_a_cnt;          // �@(10msec���ɃJ�E���g�A�b�v)



//  �R���y�A�}�b�`�^�C�} CMT0
//   10msec���̊��荞��
// �@�� 22[usec]������B22[usec]�͑��̊��荞�݂͓���Ȃ��B
//
#pragma interrupt (Excep_CMT0_CMI0(vect=28))

void Excep_CMT0_CMI0(void){
	
	uint32_t i;
	
	
	
	timer_a_cnt++;	       // �J�E���g�̃C���N�������g
	
	if ( timer_a_cnt > 9 ) {	   // 100msec�o��
		
		flg_100msec_interval = 1;  // 100msec�t���O ON
		
		timer_a_cnt = 0;	  //  �J�E���^�[�̃N���A
	}
	
	
	
	touch_position();		// �^�b�`�ʒu�̓ǂݏo�� (A/D�ϊ��f�[�^�𓾂�)
	  	
	touch_xyz_press();		// X,Y,Z�̒l�𓾂�B�^�b�`�����v�Z�B
	 
	touch_key_status_check();   // �^�b�`�L�[�̏�Ԃ𓾂�
	 
	for( i = 0; i < KEY_SW_NUM; i++ ) {   // �^�b�`����^�b�`��ԂƂȂ����L�[�������āA�L�[���͏����v���t���O���Z�b�g����B
		     switch_input_check(i);           //  (�O�񉟂��ꂽ�L�[���������Ă���)
	}
	
}


//
//    10msec �^�C�}(CMT0)
//    CMT���j�b�g0��CMT0���g�p�B 
//
//  PCLKB(=32MHz)��128�����ŃJ�E���g 32/128 = 1/4 MHz
//      1�J�E���g = 4/1 = 4 [usec]  
//    1*10,000 usec =  N * 4 usec 
//      N = 2500


void Timer10msec_Set(void)
{	
	IPR(CMT0,CMI0) = 3;		// ���荞�݃��x�� = 3�@�@�i15���ō����x��)
	IEN(CMT0,CMI0) = 1;		// CMT0 �����݋���
		
	CMT0.CMCR.BIT.CKS = 0x2;        // PCLKB/128       
	CMT0.CMCOR = 2499;		// CMCNT��0����J�E���g 	


}


//   CMT0 �^�C�}�J�n�@
//  ���荞�݋����ăJ�E���g�J�n

void Timer10msec_Start(void)
{	
	CMT0.CMCR.BIT.CMIE = 1;		// �R���y�A�}�b�`�����݁@����
		
	CMT.CMSTR0.BIT.STR0 = 1;	// CMT0 �J�E���g�J�n
	
	timer_a_cnt = 0;		//  �^�C�}�̃J�E���g�l�N���A
}

