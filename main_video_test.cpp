/*****************************************************************//**
 * @file main_video_test.cpp
 *
 * @brief Basic test of 4 basic i/o cores
 *
 * @author p chu
 * @version v1.0: initial release
 *********************************************************************/

//#define _DEBUG
#include "chu_init.h"
#include "gpio_cores.h"
#include "vga_core.h"
#include "sseg_core.h"
#include "xadc_core.h"
#include "ps2_core.h"

double map(double x1, double x2, double y1, double y2, double x)
{
	return (x - x1) *(y2 - y1) / (x2 - x1) + y1;
}

bool collided(int tx0, int ty0, int th, int tv,
	int sx0, int sy0, int sh, int sv)
{
	int tx1 = tx0 + th;
	int ty1 = ty0 + tv;
	int sx1 = sx0 + sh - 2;
	sx0 = sx0 + 2;
	int sy1 = sy0 + sv - 2;
	sy0 = sy0 + 2;

	//	uart.disp("Log x0/y0: ");uart.disp(tx0);uart.disp("/");uart.disp(ty0);uart.disp("\n\r");
	//	uart.disp("Log x1/y1: ");uart.disp(tx1);uart.disp("/");uart.disp(ty1);uart.disp("\n\r");
	//	uart.disp("Frog x0/y0: ");uart.disp(sx0);uart.disp("/");uart.disp(sy0);uart.disp("\n\r");
	//	uart.disp("Frog x1/y1: ");uart.disp(sx1);uart.disp("/");uart.disp(sy1);uart.disp("\n\r");

	if (((sy0 >= ty0 and sy0 <= ty1) or(sy1 >= ty0 and sy1 <= ty1)) and	// if top/bottom of self-object is in target's verti boundary
		((sx0 >= tx0 and sx0 <= tx1) or(sx1 >= tx0 and sx1 <= tx1)))
	{
		// if left/right of self-object is in target's horiz boundary

		//		uart.disp("COLLIDED!\n\n\n\r");
		return true;
	}

	//	uart.disp("NO TOUCH!\n\n\n\r");
	return false;
}

void text_update(OsdCore *osd_p, int lvl, int lives, int lvlx, int lvly, int lifex, int lifey)
{
	// Level update
	osd_p->wr_char(lvlx + 7, lvly, (char)((lvl / 10) % 10) + '0');	// 10
	osd_p->wr_char(lvlx + 8, lvly, (char)(lvl % 10) + '0');	// 1

	// Lives update
	osd_p->wr_char(lifex + 7, lifey, (char)(lives) + '0');	// 1
}

void gamestate(OsdCore *osd_p, int stage)
{
	switch (stage)
	{
		case 0:
			{
				int gvx = 34, gvy = 15;
				osd_p->write(gvx, gvy, "Frosty Flakes");
				break;
			}

		case 1:
			{
				int lvlx = 0, lvly = 1;
				osd_p->write(lvlx, lvly, "Level: ");

				int lifex = 195, lifey = 1;
				osd_p->write(lifex, lifey, "Lives: ");
				break;
			}

		case 2:
			{
				int gvx = 34, gvy = 15;
				osd_p->write(gvx, gvy, "Game Over!");
				osd_p->write(gvx, gvy+1, "Press Space to continue!");
				break;
			}
	}
}

void crashed(DupliCore *frog_p, int frogx, int frogy)
{
	int g = 10, d = 15;
	// all sprites halt if frog is dead
	for (int i = 0; i < 7; i++)
	{
		frog_p->move_xy(frogx + g, frogy, 0);
		sleep_ms(d);
		frog_p->move_xy(frogx, frogy + g, 0);
		sleep_ms(d);
		frog_p->move_xy(frogx - g, frogy, 0);
		sleep_ms(d);
		frog_p->move_xy(frogx, frogy - g, 0);
		sleep_ms(d);
	}
}

void jump(char cmd, int frogx, int frogy, int jumpdist, DupliCore *frog_p) {
	static int sdir = 0;

	if (cmd == 'a') sdir = 2;
	if (cmd == 'd') sdir = 0;

	sleep_ms(50);
	for (int i = 0; i < 4; i++) { frog_p->move_xy(-1, -1, i);	}

	switch(cmd) {
	case 'w':	frog_p->move_xy(frogx, frogy + (jumpdist / 2), sdir + 1); break; // mid-jump up (opposite direction sign)
	case 'a':	frog_p->move_xy(frogx + (jumpdist / 2), frogy, sdir + 1); break; // mid-jump left
	case 's':	frog_p->move_xy(frogx, frogy - (jumpdist / 2), sdir + 1); break; // mid-jump down
	case 'd':	frog_p->move_xy(frogx - (jumpdist / 2), frogy, sdir + 1); break; // mid-jump right
	}

	sleep_ms(50);
	for (int i = 0; i < 4; i++) { frog_p->move_xy(-1, -1, i);	}

	frog_p->move_xy(frogx, frogy, sdir); // land-jump, I don't know why 'frogx' needs -5, it glitches without it
}

void frogger (DupliCore *frog_p, DupliCore *car_p, DupliCore *log_p, DupliCore *turtle_p, SpriteCore *dance_p,
		Ps2Core *ps2_p, XadcCore *adc_p, OsdCore *osd_p)
{
	frog_p->set_color_dir(0,0,0);	// apply default color & direction 0 on frog sprite 0
	frog_p->set_color_dir(0,1,1);
	frog_p->set_color_dir(0,2,2);
	frog_p->set_color_dir(0,3,3);


	unsigned int time = 0;
	unsigned int cooldown;
	static unsigned int lives = 5;

	// Ps2
	int id;
	char ch;

	// enable OSD text sprite
	osd_p->set_color(0xfff, 0x000);	// dark gray/green
	osd_p->bypass(0);
	osd_p->clr_screen();

	int lvlx = 0, lvly = 1;
	int lifex = 195, lifey = 1;

	while (true)
	{
		gamestate(osd_p, 0);
		if (ps2_p->get_kb_ch(&ch))
		{
			if (ch == ' ')
			{
				lives = 5;
				break;
			}
		}
	}

	osd_p->clr_screen();
	gamestate(osd_p, 1);

	// enable ghost sprites
	int frogx, frogy, x1, y1, x2, y2, x3, y3, r1, r2, r3, tbnd, lbnd, bbnd, rbnd;
	int log_spd = 20;
	frog_p->bypass(0);

	lbnd = 0;	rbnd = 639;	tbnd = 0;	bbnd = 479;
	frogx = 320;	frogy = 200;
	x1 = rbnd;	y1 = 200;	r1 = -log_spd;
	x2 = lbnd;	y2 = 200;	r2 = +log_spd + 3;
	x3 = rbnd;	y3 = 400;	r3 = -log_spd - 7;

	// XADC
	double xadc;
	static double prev_adc = -1.0;
	int speed = 100;

	// universal: collision detection
	int jumpdist = 20;
	int level;
	bool on_log = false;
	bool safe1, safe2, safe3;

	do {
		if (lives == 0) break;
		else text_update(osd_p, level, lives, lvlx, lvly, lifex, lifey);

		xadc = adc_p->read_adc_in(0);
		speed = map(0.0, 1.0, 10, 200, xadc);

		if ((prev_adc - xadc) > 0.1 || (prev_adc - xadc) < -0.1)
		{
			prev_adc = xadc;
			level = map(10, 200, 10, 1, speed);
		}

					speed = 100;	// debug

		if (ps2_p->get_kb_ch(&ch))
		{
			if (ch == 'w' || ch == 'W') frogy = frogy - jumpdist;
			if (ch == 'a' || ch == 'A') frogx = frogx - jumpdist;
			if (ch == 's' || ch == 'S') frogy = frogy + jumpdist;
			if (ch == 'd' || ch == 'D') frogx = frogx + jumpdist;

			jump(ch, frogx, frogy, jumpdist, frog_p);

			if (frogy < 350) on_log = true;
			else on_log = false;
		}
	} while (true);

	osd_p->clr_screen();
	while (true)
	{
		gamestate(osd_p, 2);
		if (ps2_p->get_kb_ch(&ch))
		{
			if (ch == ' ')
			{
				lives = 5;
				break;
			}
		}
	}
}

void sprites_demo (DupliCore *frog_p, DupliCore *car_p, DupliCore *log_p, DupliCore *turtle_p, SpriteCore *dance_p)
{
	int w = 640;

//	frame.bypass(0);
	frog_p->bypass(0);
	car_p->bypass(0);
	log_p->bypass(0);
	turtle_p->bypass(0);
	dance_p->bypass(0);

	// NOTE: MUST SET COLOR AND DIRECTION AT THE SAME TIME.


	// duplicate frog core
	// four colors, four directions
	frog_p->set_color_dir(0,0,0);	// apply default color & direction 0 on frog sprite 0
	frog_p->set_color_dir(1,1,1);
	frog_p->set_color_dir(2,2,2);
	frog_p->set_color_dir(3,3,3);

	frog_p->move_xy(w/5, 0, 0);
	frog_p->move_xy(2*w/5, 0, 1);
	frog_p->move_xy(3*w/5, 0, 2);
	frog_p->move_xy(4*w/5, 0, 3);


	// car core
	// four colors, two directions
	car_p->set_color_dir(0,0,0);	// apply default color & direction 0 on car sprite 0
	car_p->set_color_dir(1,1,1);
	car_p->set_color_dir(2,0,2);
	car_p->set_color_dir(3,1,3);
	car_p->set_color_dir(3,1,4);
	car_p->set_color_dir(3,1,5);
	car_p->set_color_dir(3,1,6);
	car_p->set_color_dir(3,1,4);
	car_p->set_color_dir(3,1,5);
	car_p->set_color_dir(3,1,6);

	car_p->move_xy(w/5, 100, 0);
	car_p->move_xy(2*w/5, 100, 1);
	car_p->move_xy(3*w/5, 100, 2);
	car_p->move_xy(4*w/5, 100, 3);
	car_p->move_xy(w/5, 200, 4);
	car_p->move_xy(2*w/5, 200, 5);
	car_p->move_xy(3*w/5, 200, 6);


	// log core
	// four colors, two sprites
	// 1st sprite = short log, 2nd sprite = extension log
	log_p->move_xy(w/5, 200, 0);		// short log

	// extended log
	log_p->move_xy(5/2*w/5+32, 200, 2);				// front-end of log (USE A BIGGER SPRITE ADDRESS)
	log_p->set_color_dir(0,1,1);
	log_p->move_xy((5/2*w/5+32) - 52, 200, 1);		// subtract x-coordinate by ~52 pixels to extend

	// double-extended log
	log_p->move_xy(4*w/5, 200, 5);				// front-end of log (USE BIGGEST SPRITE ADDRESS)
	log_p->set_color_dir(0,1,4);				// middle log
	log_p->move_xy((4*w/5) - 52, 200, 4);		// subtract x-coordinate by ~52 pixels to extend
	log_p->set_color_dir(0,1,3);				// back-end of log (USE THE SMALLEST SPRITE ADDRESS)
	log_p->move_xy((4*w/5) - 52*2, 200, 3);		// subtract x-coordinate by ~52*2 pixels to double-extend


	// turtle core
	// four colors, two sprites
	// 1st sprite = turtle, 2nd sprite = underwater turtle
	turtle_p->set_color_dir(0,0,0);		// apply default color & direction 0 on turtle sprite 0
	turtle_p->set_color_dir(1,1,1);
	turtle_p->set_color_dir(2,0,2);
	turtle_p->set_color_dir(3,1,3);

	turtle_p->move_xy(w/5, 300, 0);
	turtle_p->move_xy(2*w/5, 300, 1);
	turtle_p->move_xy(3*w/5, 300, 2);
	turtle_p->move_xy(4*w/5, 300, 3);


	// dancing frog core
	// four colors, four sprites
	dance_p->move_xy(w/2, 400);
	dance_p->set_color_anim_dir(0,1,0);		// turn on animation mode
}

// external core instantiation
GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpiCore sw(get_slot_addr(BRIDGE_BASE, S3_SW));
FrameCore frame(FRAME_BASE);
GpvCore bar(get_sprite_addr(BRIDGE_BASE, V7_BAR));
SpriteCore dance(get_sprite_addr(BRIDGE_BASE, V6_DANCE), 1024);
DupliCore turtle(get_sprite_addr(BRIDGE_BASE, V5_TURTLE), 1024);
DupliCore log(get_sprite_addr(BRIDGE_BASE, V4_LOG), 1024);
DupliCore car(get_sprite_addr(BRIDGE_BASE, V3_CAR), 1024);
DupliCore frog(get_sprite_addr(BRIDGE_BASE, V1_FROG), 1024);
OsdCore osd(get_sprite_addr(BRIDGE_BASE, V2_OSD));
SsegCore sseg(get_slot_addr(BRIDGE_BASE, S8_SSEG));
Ps2Core ps2(get_slot_addr(BRIDGE_BASE, S11_PS2));
XadcCore adc(get_slot_addr(BRIDGE_BASE, S5_XDAC));


int main() {

	// bypass all cores
	  frame.bypass(0);
	  bar.bypass(1);
	  car.bypass(1);
	  osd.bypass(1);
	  frog.bypass(1);
	  log.bypass(1);
	  turtle.bypass(1);
	  dance.bypass(1);

	while (1) {

      //sleep_ms(3000);
      sprites_demo(&frog, &car, &log, &turtle, &dance);
      frogger(&frog, &car, &log, &turtle, &dance, &ps2, &adc, &osd);

   } // while
} //main
