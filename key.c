#include "iodefine.h"
#include  "misratypes.h"
#include  "timer.h"
#include  "key.h"
#include  "pid.h"
#include  "touch.h"
#include  "alarm.h"

uint8_t  para_index;	// 0:SV, 1:P, 2:I, 3:D, 4:MR, 5:Hys, 6:H/C , 7:Ch2, 8:CJT)


volatile struct  SW_info  Key_sw[KEY_SW_NUM];	// �X�C�b�`�@6���̏��i�[�̈�


// X���AY��k�̑���l�̕��Ϗ����p
// ����l��12bit�f�[�^�����A��� 8bit�������Ă���

uint8_t   touch_x_val;		// X���f�[�^
uint8_t   touch_y_val;          // Y���f�[�^

uint8_t   touch_z1_val;		// Z�����@(�^�b�`������p)
uint8_t   touch_z2_val;

uint8_t   tc_val_pt;		// ����l�̊i�[�ʒu

uint8_t   touch_x[8];		// 8�񕪂̑���l
uint8_t   touch_y[8];


uint16_t   touch_x_average;	// ���ϒl
uint16_t   touch_y_average;


float	touch_resistance;	// �^�b�`��R

float   fl_touch_z1;		// touch_z1_val�� float�`��
float   fl_touch_z2;            // touch_z2_val�� float�`��
float   touch_z1_z2;		// z2/z1

// 
// �^�b�`��Ԃ̏����� (  �d��ON�����1����{ )
//  
//   �S�Ĕ�^�b�`(SW OFF: High = 1 )�Ƃ���
//   ����l�̊i�[�ʒu�̏�����
//
void touch_pre_status_ini(void)
{
	uint32_t i;
		
	for ( i = 0 ; i < KEY_SW_NUM; i++ ) {		//  �S�Ĕ�^�b�`(SW OFF: High = 1 )�Ƃ���
	
	     Key_sw[i].pre_status = 1;
	
	}
	
 	tc_val_pt = 0;		// �i�[�ʒu�̃N���A	
}


// X���AY��k�̑��茋�ʂ̕��ϒl�𓾂� (�f�o�b�N�p) (�L�[���͔��菈���ɕ��ϒl�͎g�p���Ă��Ȃ�)
// 12bit�f�[�^�̏�� 8bit���g�p tc_val_pt
void touch_cal_average(void)
{
	uint32_t  i;
	uint32_t  x_avg;
	uint32_t  y_avg;
	
	
	touch_x_val =  ( ad_x_val >> 4 );	// X������f�[�^�� b11-b4
	touch_y_val =  ( ad_y_val >> 4 );  	// Y������f�[�^�� b11-b4

	touch_x[tc_val_pt] = touch_x_val;   // ����ʒu�̊i�[
	touch_y[tc_val_pt] = touch_y_val;
	
	x_avg = 0;			// ���ϒl�̌v�Z
	y_avg = 0;
	
	for ( i = 0 ; i < 8 ; i++ ) {	// 8�񕪂̑��a�𓾂� 
	      x_avg = x_avg + touch_x[i];
	      y_avg = y_avg + touch_y[i];
	 }
	   
	touch_x_average =  x_avg >> 3;   // ���� 8
	touch_y_average =  y_avg >> 3;   // ���� 8
	

	if ( tc_val_pt < 7 ) {		// 
	
	   tc_val_pt = tc_val_pt + 1;	// �i�[�ʒu�̃C���N�������g
	}
	else  {				
	  
	   tc_val_pt = 0;		// �i�[�ʒu�̃N���A	
	}
	 
}



//
//   X,Y,Z1,Z2�𓾂�܂��A��R�l���v�Z����B
//
// �^�b�`�p�l����Z����(�t�B�����Ƃ��̉��ɂ���LCD�K���X�Ƃ̏㉺����)�̒�R�l(Rz)�𓾂�
//  �^�b�`����Ă��Ȃ���Ԃł́A�傫���l�ɂȂ�B
//
//     Rz = Rx_plate * ( X������l /4096 )* ( ( Z2 / Z1 ) - 1 )
//
//    Rx_plate: �^�b�`�X�N���[����X�v���[�g(�t�B������)�̒�R�l �� 270[��]�B

void touch_xyz_press(void)
{
	touch_x_val = ( ad_x_val >> 4 );  // X������f�[�^�� b11-b4
	touch_y_val = ( ad_y_val >> 4 );  // Y������f�[�^�� b11-b4
	
	touch_z1_val = ( ad_z1_val >> 4);  // Z1 ����f�[�^�� b11-b4
	touch_z2_val = ( ad_z2_val >> 4 );  // Z2 ����f�[�^�� b11-b4
	  
	if ( touch_z1_val > 0 ) {
	  
	 fl_touch_z1 = touch_z1_val;
	 fl_touch_z2 = touch_z2_val;
	 
	 touch_z1_z2 = fl_touch_z2 / fl_touch_z1;
	 
	 touch_resistance = 270.0 * ( touch_x_val / 4096.0 ) * ( touch_z1_z2 - 1.0 );
        }
	else{
	   touch_resistance = 999.9;
	}		
}



// �@
// �^�b�`�L�[�̏�Ԃ𓾂�
//  ��R�l�� 40���𒴂���ꍇ�́A��^�b�`�Ƃ���B
//  40���ȉ��̏ꍇ�A�^�b�`����Ă���Ɣ��f����B
// 
// X����Y���̓ǂݏo���f�[�^�ɂ��A�����ꂽ�L�[�𔻒f�B
//
//
// �L�[�Ɖ����ꂽ���̓ǂݏo���f�[�^
//  
// �@�@�@�@�@�@    �^�b�`���� STOP/RUN(SW0)  Auto/Manual(SW1)  Para(SW2)    ��(Down)(SW3)   ��(Up)(SW4)    Save(SW5)     �@    
//  touch_x_val:�@�@ 0xff      0x40�`0x5f     0x40�`0x5f       0x70�`0x90   0xa2�`0xc0      0xa2�`0xc0    0x70�`0x90 
//  touch_y_val:     0x76      0x20�`0x38     0x40�`0x59       0x40�`0x59   0x20�`0x38      0x40�`0x59    0x20�`0x38
//
///
//  �L�[�z�u
//        Auto/Manual   Para        ��
//        Stop/RUN   �@ Save     �@ ��
//
//
//
//


#define TC_Y_VAL_L_0	0x20	// Y������ (���ɂ���L�[ STOP/RUN(SW0), ��(Down)(SW3), Save(SW5) �p)�@,
#define TC_Y_VAL_H_0    0x38    // Y�����

#define TC_Y_VAL_L_1	0x40	// Y������ (��ɂ���L�[ Auto/Manual(SW1), Para(SW2), ��(Up)(SW4)  �p)
#define TC_Y_VAL_H_1    0x59    // Y�����


void touch_key_status_check(void)
{
	uint32_t i;
	
							// �L�[�^�b�`���y������ꍇ�ɂ́A�傫�Ȓl�Ƃ���B
	if ( touch_resistance > 40.0 ) {		// ��R�l��40���𒴂���ꍇ�A
	  for ( i = 0 ; i < KEY_SW_NUM; i++ ) {		//  �S�Ĕ�^�b�`(SW OFF: High = 1 )�Ƃ���
	     Key_sw[i].status = 1;
	  }
	  
	  return;  
	}
	
							// �����ꂽ�L�[�̔���
		
	if (( touch_y_val >= TC_Y_VAL_L_0 ) && (  touch_y_val <= TC_Y_VAL_H_0 )){        //   STOP/RUN(SW0), ��(Down)(SW3), Save(SW5) �p
	
	   if (( touch_x_val >= 0x40 ) && (  touch_x_val <= 0x5f )){   //   STOP/RUN(SW0) �^�b�`
	  	Key_sw[0].status = 0;				       // �����ꂽ�L�[���^�b�`����(SW ON: Low = 0 )�Ƃ���
	    }
	   
	    else if (( touch_x_val >= 0x70 ) && (  touch_x_val <= 0x90 )){     //  Save(SW5) �^�b�`
	  	Key_sw[5].status = 0;						
	    }
	  
	    else if  (( touch_x_val >= 0xa2 ) && (  touch_x_val <= 0xc0 )){   //  ��(Down)(SW3) �^�b�`
	         Key_sw[3].status = 0;
	    }
	    
	}
	
	else if (( touch_y_val >= TC_Y_VAL_L_1 ) && (  touch_y_val <= TC_Y_VAL_H_1 )){     //   Auto/Manual(SW1), Para(SW2), ��(Up)(SW4)  �p
	
	    if (( touch_x_val >= 0x40 ) && (  touch_x_val <= 0x5f )){     //   Auto/Manual(SW1) �^�b�`
	  	Key_sw[1].status = 0;						
	    }
	    
	    else if  (( touch_x_val >= 0x70 ) && (  touch_x_val <= 0x90 )){   //  Para(SW2)   �^�b�`
	         Key_sw[2].status = 0;
	    }
	    else if  (( touch_x_val >= 0xa2 ) && (  touch_x_val <= 0xc0 )){   //  ��(Up)(SW4) �^�b�`
	         Key_sw[4].status = 0;
	    }	
	 	
	}						
		
}

//
// �@�X�C�b�`�̓��͔��� (10msec��1��@���s)
//
// �T�v:
//   �^�b�`��Ԃ�4��p����A��^�b�`�ɂ�΃L�[���͂��ꂽ�Ƃ���B
//   �������L���L�[���A250msec�ȏ�A�^�b�`��Ԃ��p������΁A�L�[������������Ă���Ƃ���B
//
//    ���肷��X�C�b�`:
//                  0 = SW0 , STOP/RUN
//                  1 = SW1 , Auto/Manual
//                  2 = SW2 , Para
//                  3 = SW3 , ��(Down)
//                  4 = SW4 , ��(Up)
//             
//  
void switch_input_check( uint8_t id ) 
{
	
         if ( Key_sw[id].status == 0 ) {     // ���� �^�b�`���
	
	     if ( Key_sw[id].pre_status == 1 ) {  // �O�� ��^�b�`��ԁ@�@(�������茟�o)
	           Key_sw[id].low_cnt =  1;       // Low�J�E���g = 1 �Z�b�g
	     
	     }
	     else{				// �O�� �^�b�`���
	      	  if ( Key_sw[id].low_cnt > 24 ) {   // �@�J�E���g��25��ȏ�̏ꍇ�@(250msec�ȏ�)
		     
		      if (( id == 3 ) || ( id == 4 )){     // �������L���L�[ SW4,SW5  (��(Up)�Ɓ�(Down))�̏ꍇ
	                  Key_sw[id].long_push = 1;	    // �L�[���͏����v��(������)�̃Z�b�g
	               }
		      
		   }
		   else{
		           Key_sw[id].low_cnt = Key_sw[id].low_cnt + 1; // Low�J�E���g�̃C���N�������g  
		   }
	     }
	  }      // ����^�b�`���

	  else{				// ����@��^�b�`���
	   
	      if ( Key_sw[id].pre_status == 1 ) {  // �O��@��^�b�`���
	   	
	      }
	      
	      else{				// �O��@�^�b�`��� (���オ�茟�o)
	          if ( Key_sw[id].low_cnt > 3 ) {   // 4��ȏ�@�^�b�`���o�̏ꍇ
	             if ( Key_sw[id].long_push == 1 ) {   //   �L�[���͏����v��(������)�Z�b�g�ς݂̏ꍇ
		      
		         Key_sw[id].long_push = 0;	// �L�[���͏����v��(������)�N���A
		     }
		     else {
			  
		         Key_sw[id].one_push = 1;	 // �L�[���͏����v��(1�񉟂�)�Z�b�g
		     }
		     
		     Key_sw[id].low_cnt = 0;	//  Low�J�E���g�̃N���A  
	          }
	      }
	  }	  
	  	
	  Key_sw[id].pre_status = Key_sw[id].status;   // ���݂̏�Ԃ��A��O�̏�ԂփR�s�[
	  
}
   


//
// �L�[���͏���
//   0 = SW0 , STOP/RUN
//   1 = SW1 , Auto/Manual
//   2 = SW2 , Para
//   3 = SW3 , ��(Down)
//   4 = SW4 , ��(Up)
//   5 = SW5,  Save
//
void key_input(void)	
{
	uint32_t    i;
	uint32_t err_fg;
	
	if (  Key_sw[0].one_push == 1 ) {	    // Run/Stop key
		pre_mode_stop_run = mode_stop_run;  // ���݂̃��[�h��ޔ�
	
		if ( mode_stop_run == 0  ) {	    // ���݁AStop�̏ꍇ�A
		      mode_stop_run = 1;	    // Run �ɂ���B
		}
		else if ( mode_stop_run == 1) {	    // ���݁ARun�̏ꍇ�A
		      mode_stop_run = 0;	    // Stop �ɂ���B
		}
	}
	
	if ( Key_sw[1].one_push == 1 ) {		// Auto/Manual key
		pre_mode_auto_manual = mode_auto_manual; // ���݂̃��[�h��ޔ�
	
		if ( mode_auto_manual == 0  ) {	    // ���݁AAuto�̏ꍇ�A
		      mode_auto_manual = 1;	    // Manual �ɂ���B
		}
		else if ( mode_auto_manual == 1) {  // ���݁AManual�̏ꍇ�A
		      mode_auto_manual = 0;	    // Auto �ɂ���B
		}
	}
	
	
	if (  Key_sw[2].one_push == 1 ) {		// Para key
		if ( para_index  < ( PARA_MAX_NUMBER - 1 )) {
			para_index = para_index + 1;
		}
		else {
			para_index = 0;
		}
	}
	
	if ( Key_sw[3].one_push == 1 ) {		// Down
	 
		key_down();
	}
	if ( Key_sw[3].long_push == 1 ) {		// Down (������)
	   
	   for ( i = 0 ; i < LONG_PUSH_CNT ; i++ ) {	
		 key_down();
	   }
	}
	
	
	if (  Key_sw[4].one_push == 1 ) {		// Up key
		
		key_up();
	}
	if (  Key_sw[4].long_push == 1 ) {		// Up key (������)
	   
	   for ( i = 0 ; i < LONG_PUSH_CNT; i++ ) {
		key_up();
	   }
	}
	
	
	if (  Key_sw[5].one_push == 1 ) {		// Save key
		
	    err_fg = pid_para_flash_write();       //  �p�����[�^���A�f�[�^�t���b�V���֏�������
	    
	    if ( err_fg > 0 ) {
		    alm_2 = 1;
	    }
	    else{
	    	alm_2 = 0;
	    }
		
	}
	
	
	for ( i = 0 ; i < KEY_SW_NUM ; i++ ) {	// �X�C�b�` ��x�����̏����N���A
		Key_sw[i].one_push = 0;
	}
	
	
}



//	Up key����
//  manual ���[�h�̏ꍇ: pid_mv�̒l��1���₷�B (max = 99�@% )
//  auto ���[�h�̏ꍇ: 
//     SV(SP)�\�����@: pid_sv�̒l��1���₷�B�@(max =100.0 ��)
//         P �\����  : pid_p �̒l��1���₷�B  (max =100.0 %) (0�`100���ɑ΂��� %)
//         I �\����  ; pid_i �̒l��1���₷�B  (max = 360.0 �b)
//         D �\����  : pid_d �̒l��1���₷�B  (max = 360.0 �b)

void key_up(void)
{
		
	if ( mode_auto_manual == 1 ) {	// manual ���[�h�̏ꍇ
		
		if ( pid_mv < 100.0) {
			pid_mv = pid_mv + 1.0;
		}
	}
	
	
	else{				// auto ���[�h�̏ꍇ
	
		if ( para_index == 0 ) {	//  SV(SP)�\����
			if ( pid_sv < 100.0) {
				pid_sv = pid_sv + 0.1;
			}
		}
		else if ( para_index == 1 ) {	//  P �\����
			if ( pid_p < 100.0) {
				pid_p = pid_p + 0.1;
			}
		}
		else if ( para_index == 2 ) {	//  I �\����
			if ( pid_i < 3600 ) {
				pid_i = pid_i + 1.0;
			}
		}
		else if ( para_index == 3 ) {	//  D �\����
			if ( pid_d < 3600 ) {
				pid_d = pid_d + 1.0;
			}
		}
		else if ( para_index == 4 ) {	//  MR �\����
			if ( pid_mr < 100.0 ) {
				pid_mr = pid_mr + 0.1;
			}
		}
		else if ( para_index == 5 ) {	//  Hys �\����
			if ( pid_hys < 100.0 ) {
				pid_hys = pid_hys + 0.1;
			}
		}
		else if ( para_index == 6 ) {	//  heat_cool �\����
			if ( heat_cool < 1) {
				 heat_cool = heat_cool + 1;
			}
			
		}
		
		
		
		
	
	}
	
}


//	Down key���� 
//  manual ���[�h�̏ꍇ: pid_mv�̒l��1����B (min = 0�@% )
//  auto   ���[�h�̏ꍇ: 
//     SV(SP)�\�����@: pid_sv�̒l��1���炷�B�@(min = 0.0 ��)
//         P �\����  : pid_p �̒l��1���炷�B  (min =0 %) (0�`100���ɑ΂��� %)
//         I �\����  ; pid_i �̒l��1���炷�B  (min = 0 �b)
//         D �\����  : pid_d �̒l��1���炷�B  (min = 0 �b)
void key_down(void)
{

	if ( mode_auto_manual == 1 ) {	// manual ���[�h�̏ꍇ
		if ( pid_mv > 0.0) {
			pid_mv = pid_mv - 1.0;
		}
	
	}
	else{				// auto ���[�h�̏ꍇ
		if ( para_index == 0 ) {	//  SV(SP)�\����
			if ( pid_sv >= 0.1) {
				pid_sv = pid_sv - 0.1;
			}
		}
		else if ( para_index == 1 ) {	//  P �\����
			if ( pid_p >= 0.1) {
				pid_p = pid_p - 0.1;
			}
		}
		else if ( para_index == 2 ) {	//  I �\����
			if ( pid_i >= 1.0 ) {
				pid_i = pid_i - 1.0;
			}
		}
		else if ( para_index == 3 ) {	//  D �\����
			if ( pid_d >= 1.0 ) {
				pid_d = pid_d - 1.0;
			}
		}
		else if ( para_index == 4 ) {	//  MR �\����
			if ( pid_mr >= 0.1 ) {
				pid_mr = pid_mr - 0.1;
			}
		}
		else if ( para_index == 5 ) {	//  Hys �\����
			if ( pid_hys >= 0.1 ) {
				pid_hys = pid_hys - 0.1;
			}
		}
		else if ( para_index == 6 ) {	//  heat_cool �\����
			if ( heat_cool > 0 ) {
				 heat_cool = heat_cool - 1;
			}
		
		}
	
	}	
	
}
