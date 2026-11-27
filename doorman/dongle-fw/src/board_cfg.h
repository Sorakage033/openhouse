#pragma once
/* Runtime board descriptor for generic community nRF52840/nRF52833 receivers.
 *
 * Embedded in .rodata (flash) with magic "OHBC" + CRC32 so host tools / openhouse-installer can
 * patch a single generic SoC+layout image for any compatible PCB (SKY66112 / RFX2401C / AT2401C
 * two-pin FEM, GPIO or WS2812B LED, REGOUT0, DFU magic, USB product string) without building a
 * separate binary per board.
 */
#include <stddef.h>
#include <stdint.h>
#include "banks.h"

#define BOARD_CFG_MAGIC   0x4342484Fu  /* "OHBC" */
#define BOARD_CFG_VERSION 1u

enum {
    FEM_NONE    = 0,
    FEM_TWO_PIN = 1,   /* SKY66112 / RFX2401C / AT2401C: CTX + CRX */
};

enum {
    LED_NONE       = 0,
    LED_GPIO_HIGH  = 1,
    LED_GPIO_LOW   = 2,
    LED_WS2812_SPI = 3,   /* SPIM2 4 MHz: led_pin = MOSI, led_pin2 = SCK */
};

enum {
    BOARD_FLAG_REGOUT0_3V0 = 1u << 0,
    BOARD_FLAG_NORDIC_DFU  = 1u << 1,
};

#define BOARD_PIN_NONE       0xFFu
#define BOARD_PIN(port, pin) ((uint8_t)(((port) << 5) | ((pin) & 31u)))

typedef struct __attribute__((packed, aligned(4))) {
    uint32_t magic;        /* 0..3:   BOARD_CFG_MAGIC ("OHBC") */
    uint16_t version;      /* 4..5:   BOARD_CFG_VERSION (1) */
    uint8_t  board_id;     /* 6:      CMD_INFO byte 57 */
    uint8_t  fem_type;     /* 7:      FEM_NONE / FEM_TWO_PIN */
    uint8_t  fem_ctx;      /* 8:      BOARD_PIN(port, pin) or BOARD_PIN_NONE */
    uint8_t  fem_crx;      /* 9:      BOARD_PIN(port, pin) or BOARD_PIN_NONE */
    uint8_t  fem_aux0;     /* 10:     reserved (e.g. nRF21540 PDN) */
    uint8_t  fem_aux1;     /* 11:     reserved (e.g. nRF21540 MODE) */
    int8_t   tx_power;     /* 12:     radio TX power in dBm (+8, 0, -4, -8, ...) */
    uint8_t  led_type;     /* 13:     LED_* */
    uint8_t  led_pin;      /* 14:     GPIO pin or WS2812 MOSI */
    uint8_t  led_pin2;     /* 15:     WS2812 SCK or BOARD_PIN_NONE */
    uint8_t  flags;        /* 16:     BOARD_FLAG_* */
    uint8_t  reserved[3];  /* 17..19: zero */
    char     name[40];     /* 20..59: NUL-terminated USB product string ("Doorman ...") */
    uint32_t crc32;        /* 60..63: CRC32(bytes 0..59); 0 = unpatched compile-time default */
} board_cfg_t;

_Static_assert(sizeof(board_cfg_t) == 64, "board_cfg_t must be 64 bytes");

#if BOARD_SMOL_CFG
extern const board_cfg_t g_board_cfg;

static inline const board_cfg_t *board_cfg_get(void)
{
    const board_cfg_t *p = &g_board_cfg;
    /* Prevent GCC from constant-folding fields that a host patcher overwrites in flash. */
    __asm__ volatile("" : "+r"(p));
    return p;
}

static inline int board_cfg_valid(const board_cfg_t *c)
{
    if (c->magic != BOARD_CFG_MAGIC || c->version != BOARD_CFG_VERSION) return 0;
    if (c->crc32 == 0) return 1;
    return bank_crc32((const uint8_t *)c, offsetof(board_cfg_t, crc32)) == c->crc32;
}

static inline const char *board_cfg_product(void)
{
    const board_cfg_t *c = board_cfg_get();
    if (board_cfg_valid(c) && c->name[0] != '\0') return c->name;
    return DONGLE_PRODUCT;
}

static inline uint8_t board_cfg_id(void)
{
    const board_cfg_t *c = board_cfg_get();
    return board_cfg_valid(c) ? c->board_id : (uint8_t)BOARD_ID;
}

static inline uint8_t board_cfg_dfu_uf2_magic(void)
{
    const board_cfg_t *c = board_cfg_get();
    return (board_cfg_valid(c) && (c->flags & BOARD_FLAG_NORDIC_DFU)) ? 0xB1u : 0x57u;
}

static inline uint8_t board_cfg_dfu_serial_magic(void)
{
    const board_cfg_t *c = board_cfg_get();
    return (board_cfg_valid(c) && (c->flags & BOARD_FLAG_NORDIC_DFU)) ? 0xB1u : 0x4Eu;
}
#endif
