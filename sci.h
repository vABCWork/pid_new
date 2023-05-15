
//  ���M����LED�p�@�o�̓|�[�g PH1�@(�p�\�R���Ƃ̒ʐM�p)
#define LED_TX_PMR	(PORTH.PMR.BIT.B1)   // 0: �ėp���o�̓|�[�g�Ƃ��Ďw��
#define LED_TX_PDR      (PORTH.PDR.BIT.B1)   // 1: �o�̓|�[�g�Ɏw��
#define LED_TX_PODR     (PORTH.PODR.BIT.B1)  //   �o�̓f�[�^


//  ��M����LED�p�@�o�̓|�[�g PH0 �@(�p�\�R���Ƃ̒ʐM�p)
#define LED_RX_PMR      (PORTH.PMR.BIT.B0)   // 0: �ėp���o�̓|�[�g�Ƃ��Ďw��
#define LED_RX_PDR      (PORTH.PDR.BIT.B0)   // 1: �o�̓|�[�g�Ɏw��
#define LED_RX_PODR     (PORTH.PODR.BIT.B0)  //   �o�̓f�[�^


extern  volatile uint8_t  rcv_cnt;
extern	volatile uint8_t rcv_over;


void initSCI_1(void);

void LED_comm_port_set(void);

void comm_cmd(void);

uint32_t write_para_data(void);
uint32_t set_mode_index(void);
uint32_t set_mv_data(void);
uint32_t set_flash_write(void);
uint32_t resp_temp_para_read(void);



