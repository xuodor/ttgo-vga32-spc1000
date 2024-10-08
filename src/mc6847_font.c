#include "common.h"

/**
 * MC6847 Character Generator Internal ROM Data. (provided by Zanny)
 */
byte mc6847_font[] =
{
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //32
 0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x08, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x14, 0x14, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x14, 0x14, 0x3E, 0x14, 0x3E, 0x14, 0x14, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x08, 0x1E, 0x28, 0x1C, 0x0A, 0x3C, 0x08, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x32, 0x32, 0x04, 0x08, 0x10, 0x26, 0x26, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x10, 0x28, 0x10, 0x28, 0x26, 0x24, 0x1A, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x04, 0x08, 0x10, 0x10, 0x10, 0x08, 0x04, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x10, 0x08, 0x04, 0x04, 0x04, 0x08, 0x10, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x08, 0x1C, 0x3E, 0x1C, 0x08, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x3E, 0x08, 0x08, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x08, 0x10, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x02, 0x02, 0x04, 0x08, 0x10, 0x20, 0x20, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x0C, 0x12, 0x12, 0x12, 0x12, 0x12, 0x0C, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x08, 0x18, 0x08, 0x08, 0x08, 0x08, 0x1C, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x1C, 0x22, 0x02, 0x1C, 0x20, 0x20, 0x3E, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x1C, 0x22, 0x02, 0x0C, 0x02, 0x22, 0x1C, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x04, 0x0C, 0x14, 0x24, 0x3E, 0x04, 0x04, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x3E, 0x20, 0x3C, 0x02, 0x02, 0x22, 0x1C, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x1C, 0x22, 0x20, 0x3C, 0x22, 0x22, 0x1C, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x3E, 0x02, 0x02, 0x04, 0x08, 0x08, 0x08, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x1C, 0x22, 0x22, 0x1C, 0x22, 0x22, 0x1C, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x1C, 0x22, 0x22, 0x1E, 0x02, 0x22, 0x1C, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x08, 0x10, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x04, 0x08, 0x10, 0x20, 0x10, 0x08, 0x04, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x10, 0x08, 0x04, 0x02, 0x04, 0x08, 0x10, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x1C, 0x22, 0x02, 0x04, 0x08, 0x00, 0x08, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x1C, 0x22, 0x22, 0x06, 0x0A, 0x0A, 0x06, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x08, 0x14, 0x22, 0x3E, 0x22, 0x22, 0x22, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x3C, 0x22, 0x22, 0x3C, 0x22, 0x22, 0x3C, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x1C, 0x22, 0x20, 0x20, 0x20, 0x22, 0x1C, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x3C, 0x12, 0x12, 0x12, 0x12, 0x12, 0x3C, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x3E, 0x20, 0x20, 0x3C, 0x20, 0x20, 0x3E, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x3E, 0x20, 0x20, 0x3C, 0x20, 0x20, 0x20, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x1C, 0x22, 0x20, 0x26, 0x22, 0x22, 0x1E, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x3E, 0x22, 0x22, 0x22, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x1C, 0x08, 0x08, 0x08, 0x08, 0x08, 0x1C, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x0E, 0x04, 0x04, 0x04, 0x04, 0x24, 0x18, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x22, 0x24, 0x28, 0x30, 0x28, 0x24, 0x22, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3E, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x22, 0x36, 0x2A, 0x2A, 0x22, 0x22, 0x22, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x22, 0x32, 0x2A, 0x26, 0x22, 0x22, 0x22, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x3E, 0x22, 0x22, 0x22, 0x22, 0x22, 0x3E, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x3C, 0x22, 0x22, 0x3C, 0x20, 0x20, 0x20, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x1C, 0x22, 0x22, 0x22, 0x2A, 0x26, 0x1E, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x3C, 0x22, 0x22, 0x3C, 0x28, 0x24, 0x22, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x1C, 0x22, 0x20, 0x1C, 0x02, 0x22, 0x1C, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x3E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x1C, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x14, 0x14, 0x08, 0x08, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x2A, 0x2A, 0x36, 0x22, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x22, 0x22, 0x14, 0x08, 0x14, 0x22, 0x22, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x22, 0x22, 0x14, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x3E, 0x02, 0x04, 0x08, 0x10, 0x20, 0x3E, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x1C, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1C, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x38, 0x08, 0x08, 0x08, 0x08, 0x08, 0x38, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x08, 0x14, 0x22, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x10, 0x20, 0x7F, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00,

 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //64
 0x00, 0x00, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x3C, 0x7E, 0x7E, 0x7E, 0x7E, 0x3C, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x2A, 0x1C, 0x08, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x08, 0x04, 0x7E, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
 0x10, 0x10, 0x10, 0x10, 0x10, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x10, 0x10, 0x10, 0x10, 0x10, 0xFF, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x10, 0x10, 0x10, 0x10, 0x10, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
 0x10, 0x10, 0x10, 0x10, 0x10, 0xF0, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
 0x00, 0x72, 0x8A, 0x8A, 0x72, 0x02, 0x3E, 0x02, 0x3E, 0x20, 0x3E, 0x00,
 0x00, 0x72, 0x8A, 0x72, 0xFA, 0x2E, 0x42, 0x3E, 0x3E, 0x20, 0x3E, 0x00,
 0x00, 0x22, 0x22, 0xFA, 0x02, 0x72, 0x8B, 0x8A, 0x72, 0x22, 0xFA, 0x00,
 0x00, 0x10, 0x10, 0x28, 0x44, 0x82, 0x00, 0xFE, 0x10, 0x10, 0x10, 0x10,
 0x00, 0x7C, 0x44, 0x7C, 0x10, 0xFE, 0x00, 0x7C, 0x04, 0x04, 0x04, 0x00,
 0x00, 0x7C, 0x04, 0x04, 0x00, 0xFE, 0x00, 0x7C, 0x44, 0x44, 0x7C, 0x00,
 0x00, 0x7C, 0x40, 0x78, 0x40, 0x40, 0x7C, 0x10, 0x10, 0x10, 0xFE, 0x00,
 0x00, 0x82, 0x8E, 0x82, 0x8E, 0x82, 0xFA, 0x02, 0x40, 0x40, 0x7E, 0x00,
 0x00, 0x02, 0x22, 0x22, 0x22, 0x52, 0x52, 0x8A, 0x8A, 0x02, 0x02, 0x00,
 0x00, 0x44, 0x7C, 0x44, 0x7C, 0x00, 0xFE, 0x10, 0x50, 0x40, 0x7C, 0x00,
 0x00, 0x10, 0x10, 0xFE, 0x28, 0x44, 0x82, 0x10, 0x10, 0x10, 0xFE, 0x00,
 0x00, 0x01, 0x05, 0xF5, 0x15, 0x15, 0x17, 0x25, 0x45, 0x85, 0x05, 0x00,
 0x00, 0x01, 0x05, 0xF5, 0x85, 0x85, 0x87, 0x85, 0xF5, 0x05, 0x05, 0x00,
 0x00, 0x02, 0x72, 0x8A, 0x8A, 0x8A, 0x72, 0x02, 0x42, 0x40, 0x7E, 0x00,
 0x00, 0x00, 0x7C, 0x40, 0x40, 0x40, 0x7C, 0x10, 0x10, 0x10, 0xFE, 0x00,
 0x00, 0x02, 0x72, 0x8A, 0x72, 0xFA, 0x2E, 0x42, 0x22, 0x20, 0x3E, 0x00,

 0x00, 0x00, 0x00, 0x3E, 0x22, 0x3E, 0x22, 0x3E, 0x00, 0x00, 0x00, 0x00, // 128
 0x00, 0x00, 0x3E, 0x22, 0x3E, 0x22, 0x3E, 0x22, 0x42, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x10, 0x54, 0x54, 0x10, 0x28, 0x44, 0x82, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x10, 0x12, 0xFC, 0x38, 0x34, 0x52, 0x91, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x10, 0x10, 0xFE, 0x10, 0x38, 0x54, 0x92, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x10, 0x28, 0x7C, 0x92, 0x7C, 0x54, 0xFE, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x10, 0x10, 0x10, 0x7C, 0x10, 0x10, 0xFE, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x20, 0x7E, 0x80, 0x7C, 0x50, 0xFE, 0x10, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x08, 0xFC, 0xA8, 0xFE, 0xA4, 0xFE, 0x14, 0x04, 0x00, 0x00,
 0x00, 0x00, 0x10, 0x28, 0x44, 0xFE, 0x14, 0x24, 0x48, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x74, 0x24, 0xF5, 0x65, 0xB2, 0xA4, 0x28, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x10, 0x10, 0x10, 0x54, 0x92, 0x30, 0x10, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x10, 0x10, 0xFE, 0x10, 0x28, 0x44, 0x82, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x10, 0x10, 0x10, 0x28, 0x44, 0x82, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x10, 0x28, 0x7C, 0x82, 0x7C, 0x44, 0x7C, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x3C, 0x44, 0xA8, 0x10, 0x3E, 0xE2, 0x3E, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x08, 0x7F, 0x08, 0x7F, 0x08, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x06, 0x18, 0x20, 0x18, 0x06, 0x00, 0x3E, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x30, 0x0C, 0x02, 0x0C, 0x30, 0x00, 0x3E, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x08, 0x08, 0x3E, 0x08, 0x08, 0x00, 0x3E, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x22, 0x14, 0x08, 0x14, 0x22, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x3E, 0x49, 0x7F, 0x49, 0x3E, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x08, 0x14, 0x22, 0x7F, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x7F, 0x22, 0x14, 0x08, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x08, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
 0x0C, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x60,
 0x00, 0x0F, 0x08, 0x08, 0x08, 0x48, 0xA8, 0x18, 0x08, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x7E, 0x20, 0x10, 0x20, 0x7E, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x3E, 0x14, 0x14, 0x14, 0x14, 0x00, 0x00, 0x00, 0x00,

 0x00, 0x60, 0x90, 0x90, 0x90, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 160
 0x00, 0x20, 0x60, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x60, 0x90, 0x20, 0x40, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x60, 0x90, 0x20, 0x90, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x60, 0xA0, 0xA0, 0xF0, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0xF0, 0x80, 0xF0, 0x10, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x60, 0x80, 0xF0, 0x90, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0xF0, 0x10, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x60, 0x90, 0x60, 0x90, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x60, 0x90, 0xF0, 0x10, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x02, 0x34, 0x48, 0x48, 0x36, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x18, 0x24, 0x38, 0x24, 0x24, 0x38, 0x20, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x4E, 0x30, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x18, 0x24, 0x24, 0x3C, 0x24, 0x24, 0x18, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x1C, 0x20, 0x20, 0x18, 0x24, 0x24, 0x18, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x08, 0x1C, 0x2A, 0x2A, 0x1C, 0x08, 0x00, 0x00, 0x00,
 0x80, 0x40, 0x40, 0x20, 0x10, 0x10, 0x08, 0x04, 0x04, 0x02, 0x01, 0x01,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0xFE, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x7C, 0x00, 0xFE, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0xFE, 0xAA, 0xAA, 0xAA, 0xFE, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x7C, 0x10, 0x7C, 0x14, 0x14, 0xFE, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x10, 0x10, 0xFE, 0x00, 0x28, 0x44, 0x82, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x20, 0x20, 0x20, 0xFE, 0x20, 0x20, 0x3E, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x44, 0x82, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x20, 0x20, 0xFC, 0x24, 0x24, 0x44, 0x86, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x36, 0x49, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x18, 0x20, 0x18, 0x24, 0x18, 0x04, 0x18, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x08, 0x22, 0x14, 0x49, 0x14, 0x22, 0x08, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x38, 0x00, 0x7C, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00,
 0x00, 0x60, 0x90, 0x6E, 0x11, 0x10, 0x10, 0x11, 0x0E, 0x00, 0x00, 0x00,
 0x01, 0x02, 0x02, 0x04, 0x08, 0x08, 0x10, 0x20, 0x20, 0x40, 0x80, 0x80,

 0x00, 0x00, 0x20, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 192
 0x00, 0x00, 0x00, 0x00, 0x3C, 0x02, 0x1E, 0x22, 0x1F, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x20, 0x20, 0x2C, 0x32, 0x22, 0x32, 0x2C, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x1C, 0x22, 0x20, 0x22, 0x1C, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x02, 0x02, 0x1A, 0x26, 0x22, 0x26, 0x1A, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x1C, 0x22, 0x3E, 0x20, 0x1E, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x0C, 0x10, 0x10, 0x7C, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x1A, 0x26, 0x22, 0x26, 0x1A, 0x02, 0x1C, 0x00,
 0x00, 0x00, 0x20, 0x20, 0x2C, 0x32, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x08, 0x00, 0x18, 0x08, 0x08, 0x08, 0x1C, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x04, 0x00, 0x0C, 0x04, 0x04, 0x04, 0x24, 0x18, 0x00, 0x00,
 0x00, 0x00, 0x20, 0x20, 0x22, 0x24, 0x28, 0x34, 0x22, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x18, 0x08, 0x08, 0x08, 0x08, 0x08, 0x1C, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x76, 0x49, 0x49, 0x49, 0x49, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x2C, 0x32, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x1C, 0x22, 0x22, 0x22, 0x1C, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x2C, 0x32, 0x22, 0x32, 0x2C, 0x20, 0x20, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x1A, 0x26, 0x22, 0x26, 0x1A, 0x02, 0x02, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x2E, 0x30, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x1E, 0x20, 0x1C, 0x02, 0x3C, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x10, 0x10, 0x38, 0x10, 0x10, 0x12, 0x0C, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x26, 0x1A, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x14, 0x08, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x49, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x22, 0x14, 0x08, 0x14, 0x22, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x26, 0x1A, 0x02, 0x1C, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x3E, 0x04, 0x08, 0x10, 0x3E, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x08, 0x10, 0x10, 0x20, 0x10, 0x10, 0x08, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x54, 0xFE, 0x54, 0xFE, 0x54, 0x28, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x10, 0x08, 0x08, 0x04, 0x08, 0x08, 0x10, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x32, 0x4C, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x01, 0x3E, 0x54, 0x14, 0x14, 0x00, 0x00, 0x00,

 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 224
 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
