/*****************************************************************//**
 * @file timer_core.cpp
 *
 * @brief implementation of various video core classes
 *
 * @author p chu
 * @version v1.0: initial release
 ********************************************************************/

#include "vga_core.h"

/**********************************************************************
 * General purpose video core methods
 *********************************************************************/
GpvCore::GpvCore(uint32_t core_base_addr) {
   base_addr = core_base_addr;
}
GpvCore::~GpvCore() {
}

void GpvCore::wr_mem(int addr, uint32_t data) {
   io_write(base_addr, addr, data);
}

void GpvCore::bypass(int by) {
   io_write(base_addr, BYPASS_REG, (uint32_t ) by);
}

/**********************************************************************
 * Sprite core methods
 *********************************************************************/
SpriteCore::SpriteCore(uint32_t core_base_addr, int sprite_size) {
   base_addr = core_base_addr;
   size = sprite_size;
}
SpriteCore::~SpriteCore() {
}

void SpriteCore::wr_mem(int addr, uint32_t color) {
   io_write(base_addr, addr, color);
}

void SpriteCore::bypass(int by) {
   io_write(base_addr, BYPASS_REG, (uint32_t ) by);
}

void SpriteCore::move_xy(int x, int y) {
   io_write(base_addr, X_REG, x);
   io_write(base_addr, Y_REG, y);
   return;
}

void SpriteCore::wr_ctrl(int32_t cmd) {
   io_write(base_addr, SPRITE_CTRL_REG, cmd);
}

void SpriteCore::set_color_anim_dir(int32_t color, int auto_sel, int direction) {
	int32_t cmd;

	cmd = (color << 3) | (auto_sel << 2) | direction;
	wr_ctrl(cmd);
}

void SpriteCore::clr_sprite() {
	move_xy(645, 485);
}

/**********************************************************************
 * Duplication core methods
 *********************************************************************/
DupliCore::DupliCore(uint32_t core_base_addr, int sprite_size) {
   base_addr = core_base_addr;
   size = sprite_size;
   clr_all();				// move all sprites off-screen
}
DupliCore::~DupliCore() {
}

void DupliCore::bypass(int by) {
   io_write(base_addr, BYPASS_REG, (uint32_t ) by);
}

void DupliCore::move_xy(uint32_t x, uint32_t y, int sprite) {
	// designed for 0:19 sprites
	io_write(base_addr, X_REG_START + sprite*3, x);
    io_write(base_addr, Y_REG_START + sprite*3, y);
}

void DupliCore::wr_ctrl(int32_t cmd, int sprite) {
	io_write(base_addr, CTRL_REG_START + sprite*3, cmd);
}

void DupliCore::set_color_dir(int32_t color, int direction, int sprite) {
	int32_t cmd;

	cmd = (color << 2) | direction;
	wr_ctrl(cmd, sprite);
}

void DupliCore::clr_sprite(int sprite) {
	move_xy(645, 485, sprite);
}

void DupliCore::clr_all() {
	for (int i = 0; i <= 19; i++) clr_sprite(i);
}

/**********************************************************************
 * OSD core methods
 *********************************************************************/
OsdCore::OsdCore(uint32_t core_base_addr) {
   base_addr = core_base_addr;
   set_color(0x0f0, CHROMA_KEY_COLOR);  // green on black
}
OsdCore::~OsdCore() {
}
// not used

void OsdCore::set_color(uint32_t fg_color, uint32_t bg_color) {
   io_write(base_addr, FG_CLR_REG, fg_color);
   io_write(base_addr, BG_CLR_REG, bg_color);
}

void OsdCore::wr_char(uint8_t x, uint8_t y, char ch, int reverse) {
   uint32_t ch_offset;
   uint32_t data;

   ch_offset = (y << 7) + (x & 0x07f);   // offset is concatenation of y and x
   if (reverse == 1)
      data = (uint32_t)(ch | 0x80);
   else
      data = (uint32_t) ch;
   io_write(base_addr, ch_offset, data);
   return;
}

void OsdCore::write(uint8_t x, uint8_t y, char ch[], int reverse) {
	int i = 0;
	while (ch[i] != '\0') {
		wr_char(x+i, y, ch[i]);
		i++;
	}

   return;
}

void OsdCore::clr_screen() {
   int x, y;

   for (x = 0; x < CHAR_X_MAX; x++)
      for (y = 0; y < CHAR_Y_MAX; y++) {
         wr_char(x, y, NULL_CHAR);
      }
   return;
}

void OsdCore::bypass(int by) {
   io_write(base_addr, BYPASS_REG, (uint32_t ) by);
}

/**********************************************************************
 * FrameCore core methods
 *********************************************************************/
FrameCore::FrameCore(uint32_t frame_base_addr) {
   base_addr = frame_base_addr;
}
FrameCore::~FrameCore() {
}
// not used

void FrameCore::wr_pix(int x, int y, int color) {
   uint32_t pix_offset;

   pix_offset = HMAX * y + x;
   io_write(base_addr, pix_offset, color);
   return;
}

void FrameCore::clr_screen(int color) {
   int x, y;

   for (x = 0; x < HMAX; x++)
      for (y = 0; y < VMAX; y++) {
         wr_pix(x, y, color);
      }
   return;
}

void FrameCore::bypass(int by) {
   io_write(base_addr, BYPASS_REG, (uint32_t ) by);
}

// from AdaFruit
void FrameCore::plot_line(int x0, int y0, int x1, int y1, int color) {
   int dx, dy;
   int err, ystep, steep;

   if (x0 > x1) {
      swap(x0, x1);
      swap(y0, y1);
   }
   // slope is high
   steep = (abs(y1 - y0) > abs(x1 - x0)) ? 1 : 0;
   if (steep) {
      swap(x0, y0);
      swap(x1, y1);
   }
   dx = x1 - x0;
   dy = abs(y1 - y0);
   err = dx / 2;
   if (y0 < y1) {
      ystep = 1;
   } else {
      ystep = -1;
   }
   for (; x0 <= x1; x0++) {
      if (steep) {
         wr_pix(y0, x0, color);
      } else {
         wr_pix(x0, y0, color);
      }
      err = err - dy;
      if (err < 0) {
         y0 = y0 + ystep;
         err = err + dx;
      }
   }
}

void FrameCore::swap(int &a, int &b) {
   int tmp;

   tmp = a;
   a = b;
   b = tmp;
}

