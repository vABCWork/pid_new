
void disp_name(void);
void disp_symbol_para(uint8_t id);

void disp_mv_bar_data(void);
void disp_mv_data(uint32_t mv_val );
void disp_mv_bar(uint32_t cnt);

void disp_symbol_stop_run(uint8_t id);
void disp_symbol_auto_manual(uint8_t id);
void disp_symbol_heat_cool(uint32_t id);


void disp_s16_font( uint8_t *pt_str, uint8_t num, uint32_t st_col, uint32_t st_row, uint8_t color);
uint8_t  s16_font_index( uint8_t cd );

void disp_s24_24_font( uint8_t dt_index,  uint32_t st_col, uint32_t st_row ,uint8_t color);

void disp_s48_24_font( uint8_t dt_index,  uint32_t st_col, uint32_t st_row ,uint8_t color);


void disp_switch(void);

void disp_switch_square_id( uint8_t index, uint16_t st_col, uint16_t st_row);


