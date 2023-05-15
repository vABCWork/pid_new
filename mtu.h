

// COOL PWM (P26) MTIOC4C
#define COOL_PWM_PMR      (PORT2.PMR.BIT.B6)   // 0:�ėp���o�̓|�[�g, 1:���Ӌ@�\�Ƃ��Ďg�p

// HEAT PWM (P27) MTIOC4A
#define HEAT_PWM_PMR      (PORT2.PMR.BIT.B7)   // 0:�ėp���o�̓|�[�g, 1:���Ӌ@�\�Ƃ��Ďg�p



void PWM_Init_Reg(void);

void PWM_Port_Set(void);

void PWM_Stop(void);

void Set_PWM_duty(float out_mv);
