#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/** General */
#define SSD1315_I2C_ADDR          0x3C
#define SSD1315_CMD               0x80
#define SSD1315_DATA              0x40
#define SSD1315_PAGE_COUNT        8
#define SSD1315_COL_COUNT         128
#define SSD1315_BUFF_SIZE         (SSD1315_COL_COUNT * SSD1315_PAGE_COUNT)

/** Commmand list */
#define SSD1315_CMD_CC            0x81 /** Set Contrast Control */
#define SSD1315_CMD_SR            0xA0 /** Set Segment Remap */
#define SSD1315_CMD_EDON          0xA4 /** Entire Display On */
#define SSD1315_CMD_DINV          0xA6 /** Set Normal/Inverse Display */
#define SSD1315_CMD_IREF          0xAD /** Internal Iref Setting */
#define SSD1315_CMD_DON           0xAE /** Set Display On/Off */
#define SSD1315_CMD_DO            0xD3 /** Set Display Offset */
#define SSD1315_CMD_PCP           0xD9 /** Set Pre-charge Period */
#define SSD1315_CMD_VCSL          0xDB /** Set Vcomh Select Level */
#define SSD1315_CMD_NOP           0xE3 /** NOP */
#define SSD1315_CMD_CPS           0x8D /** Charge Pump Setting */
#define SSD1315_CMD_CHSS          0x26 /** Continuous Horizontal Scroll Setup */
#define SSD1315_CMD_CVHSS         0x29 /** Continuous Vertical and Horizontal Scroll Setup */
#define SSD1315_CMD_DS            0x2E /** Deactivate Scroll */
#define SSD1315_CMD_AS            0x2F /** Activate Scroll */
#define SSD1315_CMD_VSA           0xA3 /** Set Vertical Scroll Area */
#define SSD1315_CMD_CSS           0x2C /** Content Scroll Setup */
#define SSD1315_CMD_FOB           0x23 /** Set Fade Out and Blinking */
#define SSD1315_CMD_ZI            0xD6 /** Set Zoom In */

#define SSD1315_CMD_LCS           0x00 /** Set Lower Column Start Address for Page Addressing Mode */
#define SSD1315_CMD_LCS_MASK      0x0F

#define SSD1315_CMD_HCS           0x10 /** Set Higher Column Start Address for Page Addressing Mode */
#define SSD1315_CMD_HCS_MASK      0x07

#define SSD1315_CMD_MAM           0x20 /** Set Memory Addressing Mode */
#define SSD1315_CMD_MAM_MASK      0x03

#define SSD1315_CMD_CA            0x21 /** Set Column Address */
#define SSD1315_CMD_CA_MASK       0x7F

#define SSD1315_CMD_PA            0x22 /** Set Page Address */
#define SSD1315_CMD_PA_MASK       0x07

#define SSD1315_CMD_DSL           0x40 /** Set Display Start Line */
#define SSD1315_CMD_DSL_MASK      0x3F

#define SSD1315_CMD_MR            0xA8 /** Set Multiplex Ratio */
#define SSD1315_MUX_MIN           15
#define SSD1315_MUX_MAX           63

#define SSD1315_CMD_PSA           0xB0 /** Set Page Start Address for Page Addressing Mode */
#define SSD1315_CMD_PSA_MASK      0x03

#define SSD1315_CMD_COSD          0xC0 /** Set COM Output Scan Direction */
#define SSD1315_CMD_COSD_POS      3

#define SSD1315_CMD_DCDR          0xD5 /** Set Display Clock Divide Ratio/Oscillator Frequency */
#define SSD1315_CMD_DCDR_CLK_POS  4
#define SSD1315_CMD_DCDR_CLK_MASK 0xF0
#define SSD1315_CMD_DCDR_DIV_MASK 0x0F

#define SSD1315_CMD_CPHC          0xDA /** Set COM Pins Hardware Configuration */
#define SSD1315_CMD_CPHC_VALUE    0x02
#define SSD1315_CMD_CPHC_CONF_POS 4
#define SSD1315_CMD_CPHC_MAP_POS  5



typedef void (*SSD1315I2cWrite)(uint8_t addr, uint8_t data[], uint32_t len);
typedef void (*SSD1315Delay)(uint32_t delay);

typedef enum {
    SSD1315_MEM_MODE_HORIZ = 0x00,
    SSD1315_MEM_MODE_VERT  = 0x01,
    SSD1315_MEM_MODE_PAGE  = 0x02,
} SSD1315MemMode;

typedef enum {
    SSD1315_VCOM_LEVEL065 = 0x00,
    SSD1315_VCOM_LEVEL071 = 0x10,
    SSD1315_VCOM_LEVEL077 = 0x20,
    SSD1315_VCOM_LEVEL083 = 0x30,
} SSD1315VcomLevel;
typedef enum {
    SSD1315_CHARGE_PUMP_DISABLE  = 0x10,
    SSD1315_CHARGE_PUMP_MODE7V   = 0x14,
    SSD1315_CHARGE_PUMP_MODE8_5V = 0x94,
    SSD1315_CHARGE_PUMP_MODE9V   = 0x95,
} SSD1315ChargePumpMode;
typedef struct {
    SSD1315I2cWrite i2c_write;
    SSD1315Delay delay_ms;
} SSD1315Driver;

void ssd1315_init(SSD1315Driver *driver);
void ssd1315_display_on(bool enable);
void ssd1315_entire_display_on(bool enable);
void ssd1315_column_start_address_set(uint8_t addr);
void ssd1315_page_start_address_set(uint8_t addr);
void ssd1315_display_start_line_set(uint8_t line);
void ssd1315_memory_mode_set(SSD1315MemMode mode);
void ssd1315_contrast_set(uint8_t contrast);
void ssd1315_segment_remap(bool remap);
void ssd1315_mux_ratio_set(uint8_t mux);
void ssd1315_com_scan_invert(bool invert);
void ssd1315_display_offset_set(uint8_t offset);
void ssd1315_clock_set(uint8_t freq, uint8_t divider);
void ssd1315_precharge_period_set(uint8_t period);
void ssd1315_com_pins_set(bool alternative, bool left_right_remap);
void ssd1315_vcomh_level_set(SSD1315VcomLevel level);
void ssd1315_charge_pump_mode_set(SSD1315ChargePumpMode mode);
void ssd1315_display_invert(bool invet);
void ssd1315_column_address_set(uint8_t begin, uint8_t end);
void ssd1315_page_address_set(uint8_t begin, uint8_t end);
