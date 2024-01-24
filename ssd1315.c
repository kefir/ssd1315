#include "ssd1315.h"
#include <stdint.h>
#include <string.h>

// TODO: STB-style cat code
#define SSD1315_USE_CAT
#ifdef SSD1315_USE_CAT
#include "ssd1315_cat.h"
#endif

static SSD1315Driver *drv = NULL;

static uint8_t buffer[SSD1315_BUFF_SIZE + 1] = { 0x00 };

void ssd1315_init(SSD1315Driver *driver)
{
    if (driver) {
        drv = driver;
    }

    // Default initialization
    ssd1315_charge_pump_mode_set(SSD1315_CHARGE_PUMP_MODE7V);
    ssd1315_display_on(false);
    ssd1315_memory_mode_set(SSD1315_MEM_MODE_HORIZ);
    ssd1315_display_start_line_set(0);
    ssd1315_com_scan_invert(true);
    ssd1315_segment_remap(true);
    ssd1315_display_invert(false);
    ssd1315_display_on(true);
    ssd1315_entire_display_on(false);
    drv->delay_ms(100);

    memset(buffer, 0x00, sizeof(buffer));

#ifdef SSD1315_USE_CAT
    uint8_t* dst = &buffer[1];
    for (uint16_t y = 0; y < 64; y++) {
        for (uint16_t x = 0; x < 16; x++) {
            uint8_t src_pixel = image_data_cat[y * 16 + x];
            for (uint8_t i = 0; i < 8; i++) {
                uint16_t dst_y = y / 8;
                uint16_t dst_x = x * 8 + dst_y * 128; 
                uint16_t dst_i = dst_x + i; 
                dst[dst_i] |= ((src_pixel >> (7 - i)) & 0x01) << (y - dst_y * 8);
            }
        }
    }
#endif

    // buffer[SSD1315_COL_COUNT * 3 + SSD1315_COL_COUNT / 2] = 0xFF;
    for (uint8_t i = 0; i < SSD1315_PAGE_COUNT; i++) {
        uint16_t pos = SSD1315_COL_COUNT * i + 1;
        buffer[pos - 1] = SSD1315_DATA;
        drv->i2c_write(SSD1315_I2C_ADDR, &buffer[pos - 1], SSD1315_COL_COUNT + 1);
    }
}

void ssd1315_display_on(bool enable)
{
    uint8_t cmd[] = {
        SSD1315_CMD,
        SSD1315_CMD_DON | (uint8_t)enable
    };
    drv->i2c_write(SSD1315_I2C_ADDR, cmd, sizeof(cmd));
}

void ssd1315_entire_display_on(bool enable)
{
    uint8_t cmd[] = {
        SSD1315_CMD,
        SSD1315_CMD_EDON | (uint8_t)enable
    };
    drv->i2c_write(SSD1315_I2C_ADDR, cmd, sizeof(cmd));
}

void ssd1315_column_start_address_set(uint8_t addr)
{
    uint8_t cmd[] = {
        SSD1315_CMD,
        (SSD1315_CMD_LCS | (addr & SSD1315_CMD_LCS_MASK))
    };
    drv->i2c_write(SSD1315_I2C_ADDR, cmd, sizeof(cmd));
    cmd[1] = (SSD1315_CMD_HCS | ((addr >> 4) & SSD1315_CMD_HCS_MASK));
    drv->i2c_write(SSD1315_I2C_ADDR, cmd, sizeof(cmd));
}

void ssd1315_display_start_line_set(uint8_t line)
{
    uint8_t cmd[] = {
        SSD1315_CMD,
        (SSD1315_CMD_DSL | (line & SSD1315_CMD_DSL_MASK))
    };
    drv->i2c_write(SSD1315_I2C_ADDR, cmd, sizeof(cmd));
}

void ssd1315_memory_mode_set(SSD1315MemMode mode)
{
    uint8_t cmd[] = {
        SSD1315_CMD,
        SSD1315_CMD_MAM,
        ((uint8_t)mode & SSD1315_CMD_MAM_MASK)
    };
    drv->i2c_write(SSD1315_I2C_ADDR, cmd, sizeof(cmd));
}

void ssd1315_page_start_address_set(uint8_t addr)
{
    uint8_t cmd[] = {
        SSD1315_CMD,
        (SSD1315_CMD_PSA | (addr & SSD1315_CMD_PSA_MASK))
    };
    drv->i2c_write(SSD1315_I2C_ADDR, cmd, sizeof(cmd));
}

void ssd1315_contrast_set(uint8_t contrast)
{
    uint8_t cmd[] = {
        SSD1315_CMD,
        SSD1315_CMD_CC, 
        contrast
    };
    drv->i2c_write(SSD1315_I2C_ADDR, cmd, sizeof(cmd));
}

void ssd1315_segment_remap(bool remap)
{
    uint8_t cmd[] = {
        SSD1315_CMD,
        SSD1315_CMD_SR | (uint8_t)remap
    };
    drv->i2c_write(SSD1315_I2C_ADDR, cmd, sizeof(cmd));
}

void ssd1315_mux_ratio_set(uint8_t mux)
{
    mux = (mux < SSD1315_MUX_MIN) ? SSD1315_MUX_MIN : mux;
    mux = (mux > SSD1315_MUX_MAX) ? SSD1315_MUX_MAX : mux;
    uint8_t cmd[] = {SSD1315_CMD, SSD1315_CMD_MR, mux};
    drv->i2c_write(SSD1315_I2C_ADDR, cmd, sizeof(cmd));
}

void ssd1315_com_scan_invert(bool invert)
{
    uint8_t cmd[] = {
        SSD1315_CMD,
        SSD1315_CMD_COSD | ((uint8_t)invert << SSD1315_CMD_COSD_POS)
    };
    drv->i2c_write(SSD1315_I2C_ADDR, cmd, sizeof(cmd));
}

void ssd1315_display_offset_set(uint8_t offset)
{
    uint8_t cmd[] = {SSD1315_CMD, SSD1315_CMD_DO, offset};
    drv->i2c_write(SSD1315_I2C_ADDR, cmd, sizeof(cmd));
}

void ssd1315_clock_set(uint8_t freq, uint8_t divider)
{
    uint8_t cmd[] = {
        SSD1315_CMD, 
        SSD1315_CMD_DCDR,
        (divider & SSD1315_CMD_DCDR_DIV_MASK) |
        ((freq << SSD1315_CMD_DCDR_CLK_POS) & SSD1315_CMD_DCDR_CLK_MASK)
    };
    drv->i2c_write(SSD1315_I2C_ADDR, cmd, sizeof(cmd));
}

void ssd1315_precharge_period_set(uint8_t period)
{
    uint8_t cmd[] = {SSD1315_CMD, SSD1315_CMD_PCP, period};
    drv->i2c_write(SSD1315_I2C_ADDR, cmd, sizeof(cmd));
}

void ssd1315_com_pins_set(bool alternative, bool left_right_remap)
{
    uint8_t cmd[] = {
        SSD1315_CMD, 
        SSD1315_CMD_CPHC,
        SSD1315_CMD_CPHC_VALUE
        | (uint8_t)alternative << SSD1315_CMD_CPHC_CONF_POS
        | (uint8_t)left_right_remap << SSD1315_CMD_CPHC_MAP_POS
    };
    drv->i2c_write(SSD1315_I2C_ADDR, cmd, sizeof(cmd));
}

void ssd1315_vcomh_level_set(SSD1315VcomLevel level)
{
    uint8_t cmd[] = {SSD1315_CMD, SSD1315_CMD_VCSL, (uint8_t)level};
    drv->i2c_write(SSD1315_I2C_ADDR, cmd, sizeof(cmd));
}

void ssd1315_charge_pump_mode_set(SSD1315ChargePumpMode mode)
{
    uint8_t cmd[] = {
        SSD1315_CMD,
        SSD1315_CMD_CPS,
        (uint8_t)mode
    };
    drv->i2c_write(SSD1315_I2C_ADDR, cmd, sizeof(cmd));
}

void ssd1315_display_invert(bool invert)
{
    uint8_t cmd[] = {
        SSD1315_CMD,
        (SSD1315_CMD_DINV | (uint8_t)invert)
    };
    drv->i2c_write(SSD1315_I2C_ADDR, cmd, sizeof(cmd));
}

void ssd1315_column_address_set(uint8_t begin, uint8_t end)
{
    uint8_t cmd[] = {
        SSD1315_CMD,
        SSD1315_CMD_CA,
        begin & SSD1315_CMD_CA_MASK,
        end & SSD1315_CMD_CA_MASK
    };
    drv->i2c_write(SSD1315_I2C_ADDR, cmd, sizeof(cmd));
}

void ssd1315_page_address_set(uint8_t begin, uint8_t end)
{
    uint8_t cmd[] = {
        SSD1315_CMD,
        SSD1315_CMD_PA,
        begin & SSD1315_CMD_PA_MASK,
        end & SSD1315_CMD_PA_MASK
    };
    drv->i2c_write(SSD1315_I2C_ADDR, cmd, sizeof(cmd));
}
