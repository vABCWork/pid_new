#include "iodefine.h"
#include  "misratypes.h"
#include  "thermocouple.h"
#include  "pid.h"

uint8_t   mode_stop_run;	// ���݂̏�� (0:Stop, 1:Run)
uint8_t	  pre_mode_stop_run;    // �P��������O�̏��
uint8_t   mode_auto_manual;     // ���݂̏�� (0:Auto, 1:Manual)
uint8_t	  pre_mode_auto_manual; // �P��������O�̏��

uint8_t heat_cool;             // 0:heater(�t����), 1:cooler(������)
uint8_t pid_type; 	       // ���g�p


const float  pid_sampling_time = 0.2;	// ������� (0.2 sec )
const float  pid_d_eta = 0.125;		// �s���S�����@�萔

float   pid_pv;			// ���ݒl�@PV�@ �@�P��=��

				// �p�����[�^
float   pid_sv;			// �ݒ�l(�ڕW�l) SP �@�P��=��
float	pid_p;			// ���� (0.0-100.0)
float	pid_i;			// �ϕ����� (0-3600 sec)
float   pid_d;			// �������ԎZ (0-1800 sec) 
float   pid_mr;			// �}�j���A�����Z�b�g�� (0.0-100.0)
float   pid_hys;		// ON/OFF����(P=0)�ł̃q�X�e���V�X�@�P��=��

// PID�v�Z�p
float	pid_kp;			// �Q�C��= 100 / ����
float   pid_ts_ti;		// �������/�ϕ����� (Ts/Ti)

float   pid_en;			// �΍� (����)
float   pid_en1;		// �΍� (�O��)(1��������O)(200msec�O)

float   pid_out;                // �o��( 0.0 -100.0 �ɐ������ꂽ�l)
float   pid_mv;			// PID���Z��̏o��(����)  
float   pid_mvn1;		// PID���Z��̏o��(1�����O)  

float   pid_p_mv;		// P����ɂ��o��
float   pid_i_mv;		// I����ɂ�鐧��o��(����) 
float   pid_d_mv;               // D����ɂ�鐧��o��(����)

float   pid_i_mvn1;		// I����ɂ�鐧��o��(�O��(1��������O))

float	pid_lagged;		// �s���S���� �o��(����)
float   pid_lagged1;		// �s���S���� �o��(�O��(1��������O))

float  pid_eta_td;		// ��Td
float  pid_ts_plus_eta_td;	// Ts + ��Td

//
//  ���䓮��
//
// ���[�h�@�@�@�@�@�@�@�@  ����o��
//  stop  -------------  ����o�͂́A0%
//  run   --+- auto ---  P=0�̏ꍇ�AON/OFF����(�o�́@100%/ 0% )(�q�X�e���V�X 1��)
//          |            P>0�̏ꍇ�APID���Z�ɂ�鐧��o��
//          +- manual--- ����o�͂͊O��(�ʐM��)�ɂ��ύX   
//
//
// ����
//   �΍�(en) :  en = SV -PV (���M���䎞)(�t����)
//               en = PV -SV (��p���䎞)(������)
//
//   ON/OFF ����(P=0�̏ꍇ)
//   PID����(P> 0�̏ꍇ)
//       P����o�� P_MVn = Kp * en ,   Kp = 100/P
//       I����o�� I_MVn = Kp * (( en / Ti )*Ts ) + I_MVn  
//       D����o�� lagged = (��Td / ( Ts +��Td)) * lagged1 +  ( Td / (Ts +��Td)) * (en - en1) 
//                  D_MVn = Kp * lagged
//
void control(void)
{
					// ���݂̒l��1��������O�̒l�Ƃ��ĕۑ�
	
	pid_en1 = pid_en;		//  �΍�
	pid_i_mvn1 = pid_i_mv;          // I����̒l( Kg ���|����O�̒l)				
	pid_mvn1 = pid_mv;		// PID���Z��̏o��
	pid_lagged1 = pid_lagged;	// �s���S�����p�o��
	
 				   //�@�΍� en �̌v�Z
	if ( heat_cool == 0 ) {    //�@�t����(���M����)
	
		pid_en = pid_sv - pid_pv;    // 
	}
					            
        else if ( heat_cool == 1 ) {	     //�@������(��p����)
		pid_en = pid_pv - pid_sv;    // 
	}
	
	if ( pid_p > 0 ){
		pid_kp = 100.0 / pid_p;      // P����@�Q�C�� = 100/����
	}
	else {
		pid_kp = 0.0;
	}
	
	if ( mode_stop_run == 0 ) {	// stop���[�h
	
		pid_i_mv = 0;
		pid_i_mvn1 = 0;
		pid_mv = 0;
		pid_out = 0;
		
		pid_p_mv = 0;
		pid_i_mv = 0;
		pid_d_mv = 0;
		
		pid_lagged1 = 0;
	}
	else {				// run���[�h
	
	   if ( mode_auto_manual == 0 ) {	// auto ���[�h��
	   	if ( pid_p == 0.0 ) {   // P =0 �̏ꍇ
		    on_off_control();	// ON/OFF ����
		    
		    pid_p_mv = 0;
		    pid_i_mv = 0;
		    pid_d_mv = 0;
		}
		else {                  // P > 0�@�̏ꍇ
		
		  if ( pid_d  == 0.0 ) {		// ��������Ȃ�
	    		pid_d_mv = 0.0;		// �����o�� = 0
		  }
		  else {			// �������삠�� (�s���S�����Ōv�Z)
		  
		  	pid_eta_td = pid_d_eta * pid_d;     //�@��Td
			pid_ts_plus_eta_td = pid_sampling_time + pid_eta_td; // Ts + ��Td
						// �s���S�����̌v�Z		       			
			pid_lagged = ( pid_eta_td / pid_ts_plus_eta_td) * pid_lagged1 + ( pid_d /  pid_ts_plus_eta_td) * ( pid_en - pid_en1);
		        pid_d_mv = pid_kp *  pid_lagged;
			
			                        // ���S�����̌v�Z(�Q�l)
			// pid_d_mv = pid_kp *  ( pid_d / pid_sampling_time ) * (pid_en - pid_en1);   // ( Td/Ts ) * (En - En1)
		  }
		
		  if ( pid_i == 0.0 ) {    // I���얳���̏ꍇ
		  	pid_p_mv =  pid_kp * pid_en;
		   	pid_mv = pid_p_mv + pid_d_mv + pid_mr;
		   	pid_i_mv = 0;
		  }
		  else {                  // I���삠��̏ꍇ
		       pid_p_mv =  pid_kp * pid_en;

	     	       pid_i_mv = pid_kp * (  pid_sampling_time / pid_i ) * pid_en + pid_i_mvn1;           // I���� �o��
	              
		       pid_mv =   pid_p_mv + pid_i_mv + pid_d_mv ;   // PID����@�o��
		   
		  }
		   
		} // P > 0
	   }   
	   else if ( mode_auto_manual == 1 ){	// manual�@���[�h
 						// �ʐM�� pid_mv����
		pid_p_mv = 0;
		pid_i_mv = 0;
		pid_d_mv = 0;				
	   }
	}
	 
	 	                         // �o�̓��~�b�g 
					 // ���x�^PID���Z�̏ꍇ�APID���Z����(pid_mv)�����~�b�g����Ə�񂪎����邽�߁Apid_mv�͐������Ȃ��B
	if ( pid_mv > 100.0 ) {          // ���Z��̏o�͂�100�ȏ�̏ꍇ�A
		pid_out = 100.0;         // �o�� 100%�Ƀ��~�b�g
	}
	else if ( pid_mv < 0.0 ) {        // ���Z��̏o�͂�0�����̏ꍇ
		pid_out = 0.0;            // �o�� 0%�Ƀ��~�b�g
	}
	else {                            //�@PID���Z��̒l��0����100%�ȓ��̏ꍇ�A���̂܂܏o��
		pid_out = pid_mv;
	}
	
	
	Set_PWM_duty(pid_out);		// PWM ON/OFF���Ԑݒ�
	 
	 
}



//
//  ON-OFF ����@
// pid_p = 0��ON-OFF����ƂȂ�B 
//
// ���M����(�t����j(heat_cool = 0) �̏ꍇ�A
//   PV <= (SV - �q�X�e���V�X) �Ȃ��ON (pid_mv = 100%) 
//   PV >= SP �Ȃ�΁AOFF (pid_mv= 0%)
//   (SP-�q�X�e���V�X) < PV < SV �Ȃ�΁A�o�͈ێ�(1�����O��pid_mv)
//
// ��p����(������j(heat_cool = 1) �̏ꍇ�A
//   PV >= (SV + �q�X�e���V�X) �Ȃ�΁AON(pid_mv = 100%) 
//   PV <= SP �Ȃ�΁AOFF(pid_mv= 0%)
//
void on_off_control(void)
{
	
	if ( heat_cool == 0 ) {	//�@�t����(���M����)
		if (  pid_pv >=  pid_sv  ) {    

			pid_mv = 0.0;		// �o�� OFF
		}
		else if ( pid_pv <= (pid_sv - pid_hys )) {  
		
			pid_mv = 100.0;		// �o�� ON 
		}
		else {				
			pid_mv = pid_mvn1;	// �O��̏o��
		}
	}
	
	else if ( heat_cool == 1 ) {		//�@������(��p����)
		if (  pid_pv <=  pid_sv  ) {  

			pid_mv = 0.0;		// �o�� OFF
		}
		else if ( pid_pv >= (pid_sv + pid_hys )) { 
		
			pid_mv = 100.0;		// �o�� ON 
		}
		else {			
			pid_mv = pid_mvn1;	// �O��̏o��
		}
	}
}



