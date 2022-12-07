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

void gamestate(OsdCore *osd_p, Ps2Core *ps2_p, int stage)
{
	osd_p->clr_screen();
	switch (stage)
	{
		case 0:
			{
				int gvx = 34, gvy = 13;
				osd_p->write(gvx, gvy, "   Frogger Flakes");
				osd_p->write(gvx, gvy+1, "Choose up to 4 players");
				osd_p->write(gvx, gvy+5, "Press Space to begin!");
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
				osd_p->write(gvx, gvy+1, "Press Space to retry!");
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

void jump(char cmd, int i_frog, int color, int frogx, int frogy, int jumpdist, DupliCore *frog_p) {
		static int sdir;

		if (cmd == 'a') sdir = 2; if (cmd == 'd') sdir = 0;

		sleep_ms(50);

		for (int i = 0; i < i_frog; i++) { frog_p->move_xy(-1, -1, i_frog);	}

		frog_p->set_color_dir(color, sdir+1, i_frog);
		switch(cmd) {
		case 'w':	frog_p->move_xy(frogx, frogy + (jumpdist / 2), i_frog); break;// mid-jump up (opposite direction sign)
		case 'a':	frog_p->move_xy(frogx + (jumpdist / 2), frogy, i_frog); break;
		case 's':	frog_p->move_xy(frogx, frogy - (jumpdist / 2), i_frog); break; // mid-jump down
		case 'd':	frog_p->move_xy(frogx - (jumpdist / 2), frogy, i_frog); break; // mid-jump right
		}


		sleep_ms(50);
		for (int i = 0; i < i_frog; i++) { frog_p->move_xy(-1, -1, i_frog);	}
		frog_p->set_color_dir(color, sdir, i_frog);
		frog_p->move_xy(frogx, frogy, i_frog); // land-jump




//	sleep_ms(50);
//	if (cmd == 'a') sdir = 2;	if (cmd == 'd') sdir = 0;
//
//	for (int i = 0; i < i_frog; i++) { frog_p->move_xy(-1, -1, i);	}
//
//	switch(cmd) {
//	case 'w': case 'y': case 'p': case '5':	// mid-jump up (opposite direction sign)
//		frog_p->move_xy(frogx, frogy + (jumpdist / 2), sdir + 1); break;
//
//		frog_p->set_color_dir(color_frog[i], ani, i);
//		frog_p->move_xy(frogx[i], bbnd / 2 + 20, i);
//		break;
//	case 'a': case 'g': case 'l': case '1':	// mid-jump left
//		frog_p->move_xy(frogx + (jumpdist / 2), frogy, sdir + 1); break;
//	case 's': case 'h': case ';': case '2':
//		frog_p->move_xy(frogx, frogy - (jumpdist / 2), sdir + 1); break; // mid-jump down
//	case 'd': case 'j': case '\'': case '3':
//		frog_p->move_xy(frogx - (jumpdist / 2), frogy, sdir + 1); break; // mid-jump right
//	}
//
//	sleep_ms(50);
//	for (int i = 0; i < i_frog; i++) { frog_p->move_xy(-1, -1, i);	}
//
//	frog_p->move_xy(frogx, frogy, sdir); // land-jump
}

void frogger (DupliCore *frog_p, DupliCore *car_p, DupliCore *log_p, DupliCore *turtle_p, SpriteCore *dance_p,
		Ps2Core *ps2_p, XadcCore *adc_p, OsdCore *osd_p)
{
	// Universal init variables
	unsigned int time = 0;
	static unsigned int lives = 5;
	int jumpdist = 20;
	int level;
	bool water_arena = false;
	bool safe1, safe2, safe3;
	int lvlx = 0, lvly = 1, lifex = 195, lifey = 1;
	int ani = 0;

	static char n_frog = '4'; // default num of frogs and its int, DO NOT CHANGE
	int i_frog = 4;
	int color_frog[i_frog] = {0,0,0,0};
	
	int sc = 18; // sc: spawn count
	int tbnd = 0, lbnd = 0, bbnd = 479, rbnd = 639; 	// top/left/bot/right boundaries
	int frogx[i_frog], frogy[i_frog];		 	// player 1 & player 2
	int car[2][sc], log[2][sc], turtle[2][sc];	// 2D array[x][y]: x = coordinates, y = # of sprites,
	int incr = 5;
//	int t_row[2][6] = {{0,0,0,0,0,0}, {0,0,0,0,0,0}};
	int row_rates[6] = {65, 60, 70, 50, 60, 50};

	// Ps2
	char ch;

	// XADC
	static double prev_adc = -1.0;
	double xadc;
	int speed = 100;

	// Initialize OSD text sprite
	osd_p->clr_screen();
	osd_p->set_color(0xfff, 0x000);	// dark gray/green
	osd_p->bypass(0);

	// Initialize frog sprites
	frog_p->bypass(0);
	car_p->bypass(0);
	log_p->bypass(0);
	turtle_p->bypass(0);
	dance_p->bypass(0);

	dance_p->move_xy(-1,-1);


	// TITLE SCREEN
	gamestate(osd_p, ps2_p, 0);
	while (true)
	{
		if (ps2_p->get_kb_ch(&ch))
		{
			// Spawn up to 4 frogs, allow them to change color. '6', '7' frogs, 'Space' to start
			// Despawn all frogs, update n_frog based on input key, then set xy-coor & colors for new frogs
			if (ch == '6' ||ch == '7' || ch == '8' || ch == '9') {
				for (int i = 0; i < i_frog; i++) { frog_p->move_xy(-1, -1, i); }
				n_frog = ch; i_frog = n_frog - '0' - 5;
				for (int i = 0; i < i_frog; i++) { frogx[i] = (int) (rbnd * (i + 1) / (i_frog + 1)); }

			}

			if (ch == 'w') color_frog[0] = 0; if (ch == 'a') color_frog[0] = 1;
			if (ch == 's') color_frog[0] = 2; if (ch == 'd') color_frog[0] = 3;

			if (ch == 'y') color_frog[1] = 0; if (ch == 'g') color_frog[1] = 1;
			if (ch == 'h') color_frog[1] = 2; if (ch == 'j') color_frog[1] = 3;

			if (ch == 'p') color_frog[2] = 0; if (ch == 'l') color_frog[2] = 1;
			if (ch == ';') color_frog[2] = 2; if (ch == '\'') color_frog[2] = 3;

			if (ch == '5') color_frog[3] = 0; if (ch == '1') color_frog[3] = 1;
			if (ch == '2') color_frog[3] = 2; if (ch == '3') color_frog[3] = 3;

			// Set frogs colors and positions
			for (int i = 0; i < i_frog; i++) {
				frog_p->set_color_dir(color_frog[i], ani, i);
				frog_p->move_xy(frogx[i], bbnd / 2 + 10, i);
			}

			if (ch == ' ') {
				for (int i = 0; i < i_frog; i++) { frogy[i] = 450; frog_p->move_xy(frogx[i], frogy[i], i); }
				break;
			}
		}

		// Change frog[i]'s color, choose L/R direction & jump every 200ms, choose frog[i]
		if (now_ms() - time >= 200) {
			time = now_ms();
			for (int i = 0; i < i_frog; i++) { frog_p->set_color_dir(color_frog[i], ani, i); }
			ani = !ani;
		}

	}



	int t_row[6] = {0,0,0,0,0,0};

	// GAME STAGE
	gamestate(osd_p, ps2_p, 1);

	int spoint[6] = {0, 639, 0, 639, 0, 639};
	int distance_row[6] = {100, -120, 90, 140, 95, 120};

	int n_cars = 6;
	for (int m = 0 * n_cars; m < 1 * n_cars; m++) { car[1][m] = 370; }
	for (int m = 0 * n_cars; m < 1 * n_cars; m++) { car[0][m] = spoint[0]; spoint[0] += distance_row[0]; }

	for (int m = 1 * n_cars; m < 2 * n_cars; m++) { car[1][m] = 320; }
	for (int m = 1 * n_cars; m < 2 * n_cars; m++) { car[0][m] = spoint[1]; spoint[1] += distance_row[1]; }

	for (int m = 2 * n_cars; m < 3 * n_cars; m++) { car[1][m] = 270; }
	for (int m = 2 * n_cars; m < 3 * n_cars; m++) { car[0][m] = spoint[2]; spoint[2] += distance_row[2]; }

	int n_logs = 6;
	for (int m = 1 * n_logs; m < 2 * n_logs; m++) { log[1][m] = 170; }
	log[0][7] = 500; log[0][9] = 300; log[0][11] = 100;

	for (int m = 0 * n_logs; m < 1 * n_logs; m++) { log[1][m] = 120; }
	log[0][2] = 52; log[0][5] = 300;

	for (int m = 2 * n_logs; m < 3 * n_logs; m++) { log[1][m] = 70; }
	log[0][12] = 500; log[0][13] = 300; log[0][14] = 100;

	do {
		if (lives == 0) break;
		else text_update(osd_p, level, lives, lvlx, lvly, lifex, lifey);

		xadc = adc_p->read_adc_in(0);
		speed = map(0.0, 1.0, 10, -35, xadc);

		if ((prev_adc - xadc) > 0.1 || (prev_adc - xadc) < -0.1) {
			prev_adc = xadc;
			level = map(10, -35, 1, 10, speed);
		}

		if (frogy[0] <= 200) water_arena = true;
		else water_arena = false;

//		speed = 100;	// debug


		if (ps2_p->get_kb_ch(&ch))
		{
			if (ch == 'w') frogy[0] = frogy[0] - jumpdist; if (ch == 'a') frogx[0] = frogx[0] - jumpdist;
			if (ch == 's') frogy[0] = frogy[0] + jumpdist; if (ch == 'd') frogx[0] = frogx[0] + jumpdist;

//			if (ch == 'y') frogy[1] = frogy[1] - jumpdist; if (ch == 'g') frogx[1] = frogx[1] - jumpdist;
//			if (ch == 'h') frogy[1] = frogy[1] + jumpdist; if (ch == 'j') frogx[1] = frogx[1] + jumpdist;
//
//			if (ch == 'p') frogy[2] = frogy[2] - jumpdist; if (ch == 'l') frogx[2] = frogx[2] - jumpdist;
//			if (ch == ';') frogy[2] = frogy[2] + jumpdist; if (ch == '\'') frogx[2] = frogx[2] + jumpdist;
//
//			if (ch == '5') frogy[3] = frogy[3] - jumpdist; if (ch == '1') frogx[2] = frogx[3] - jumpdist;
//			if (ch == '2') frogy[3] = frogy[3] + jumpdist; if (ch == '3') frogx[2] = frogx[3] + jumpdist;
//
			int n = 0;
			jump(ch, n, color_frog[0], frogx[0], frogy[0], jumpdist, frog_p); // can only do one character at once anyway
//
		}
//
//
//		// Tried looping the rows, DIDN'T WORK
//		for (int row = 0; row < 3; row++) {
//			incr = (row % 2 == 0) ? incr : -1 * incr;
//			if (now_ms() - t_row[row] >= row_rates[row]) {
//				t_row[row] = now_ms();
//				for (int m = row * n_cars; m < (row + 1) * n_cars; m++) {
//					car[0][m] += incr;
//					car_p->move_xy(car[0][m], car[1][m], m);
//
//					if (row == 0) car_p->set_color_dir(1,0,m);
//					if (row == 1) car_p->set_color_dir(2,1,m);
//					if (row == 2) car_p->set_color_dir(3,0,m);
//					if (car[0][m] >= 639) car[0][m] = 0;
//					if (car[0][m] <= 0) car[0][m] = 639;
//				}
//			}
//		}

		// Car Sprites
		if (now_ms() - t_row[0] >= row_rates[0] + speed) {
			t_row[0] = now_ms();
			for (int m = 0 * n_cars; m < 1 * n_cars; m++) {
				car[0][m] += incr;
				car_p->set_color_dir(1,0,m);
				car_p->move_xy(car[0][m], car[1][m], m);
				if (car[0][m] >= 639) car[0][m] = 0;
			}
		}
		if (now_ms() - t_row[1] >= row_rates[1] + speed) {
			t_row[1] = now_ms();
			for (int m = 1 * n_cars; m < 2 * n_cars; m++) {
				car[0][m] -= incr;
				car_p->set_color_dir(2,1,m);
				car_p->move_xy(car[0][m], car[1][m], m);
				if (car[0][m] <= 0) car[0][m] = 639;
			}
		}
		if (now_ms() - t_row[2] >= row_rates[2] + speed) {
			t_row[2] = now_ms();
			for (int m = 2 * n_cars; m < 3 * n_cars; m++) {
				car[0][m] += incr;
				car_p->set_color_dir(3,0,m);
				car_p->move_xy(car[0][m], car[1][m], m);
				if (car[0][m] >= 639) car[0][m] = 0;
			}
		}

//		// Log Sprites
		if (now_ms() - t_row[3] >= row_rates[3] + speed) {
			t_row[3] = now_ms();
			// Row 4 logs
			for (int i = 7; i <= 11; i += 2) {
				log[0][i] -= incr;
				log_p->move_xy(log[0][i], log[1][i], i);				// front-end of log (USE A BIGGER SPRITE ADDRESS)
				log_p->set_color_dir(0,1,i-1);
				log_p->move_xy(log[0][i] - 52, log[1][i], i-1);		// subtract x-coordinate by ~52 pixels to extend
				if (log[0][i] <= 0) log[0][i] = rbnd;
			}

			if (frogy[0] >= 138 and frogy[0] <= 170 and water_arena) {
				for (int i = 0; i <= 18; i++) {
					bool touched = collided(log[0][i], log[1][i], 128, 32, frogx[0], frogy[0], 32, 32);
					if (touched) {
						crashed(frog_p, frogx[0], frogy[0]);
						frogx[0] = 320; frogy[0] = 430;
					} else {
						frogx[0] -= incr;
						frog_p->move_xy(frogx[0], frogy[0],0);
						break;
					}
				}
			}
		}

		if (now_ms() - t_row[4] >= row_rates[4] + speed) {
			t_row[4] = now_ms();

			// Row 5 logs
			// extended log
			log[0][2] += incr;
			log_p->move_xy(log[0][2], log[1][2], 2);				// front-end of log (USE A BIGGER SPRITE ADDRESS)
			log_p->set_color_dir(0,1,1);
			log_p->move_xy(log[0][2] - 52, log[1][2], 1);		// subtract x-coordinate by ~52 pixels to extend
			if (log[0][2] >= 750) log[0][2] = 0;

			log[0][5] += incr;
			for (int i = 5; i >= 3; i--) {
				// double-extended log
				if (i < 5) log_p->set_color_dir(0,1,i);
				log_p->move_xy(log[0][5] - 52 * (-1) * (i - 5), log[1][5], i);			// front-end of log (USE BIGGEST SPRITE ADDRESS)
			}
			if (log[0][5] >= 750) log[0][5] = 0;


			if (frogy[0] <= 138 and frogy[0] >= 100 and water_arena) {
				for (int i = 0; i <= 18; i++) {
					bool touched = collided(log[0][i], log[1][i], 128, 32, frogx[0], frogy[0], 32, 32);
					if (touched) {
						crashed(frog_p, frogx[0], frogy[0]);
						frogx[0] = 320; frogy[0] = 430;
					} else {
						frogx[0] += incr;
						frog_p->move_xy(frogx[0], frogy[0],0);
						break;
					}
				}
			}
		}

		if (now_ms() - t_row[5] >= row_rates[5] + speed) {
			t_row[5] = now_ms();
			log[0][12] -= incr;
			log[0][13] -= incr;
			log[0][14] -= incr;

			// Row 6 logs
			for (int i = 12; i <= 14; i++) {
				log_p->set_color_dir(0,1,i);
				log_p->move_xy(log[0][i], log[1][i], i);		// short log
			}
			if (log[0][12] <= 0) log[0][12] = rbnd;
			if (log[0][13] <= 0) log[0][13] = rbnd;
			if (log[0][14] <= 0) log[0][14] = rbnd;

			if (frogy[0] <= 100 and frogy[0] >= 68 and water_arena) {
				for (int i = 0; i <= 18; i++) {
					bool touched = collided(log[0][i], log[1][i], 64, 32, frogx[0], frogy[0], 32, 32);
					if (touched) {
						crashed(frog_p, frogx[0], frogy[0]);
						frogx[0] = 320; frogy[0] = 430;
					} else {
						frogx[0] -= incr;
						frog_p->move_xy(frogx[0], frogy[0],0);
						break;
					}
				}
			}
		}


		if (!water_arena) {
			if (frogy[0] >= 370) {
				for (int i = 0 * n_cars; i < 1 * n_cars; i++) {
					bool touched = collided(car[0][i], car[1][i], 32, 32, frogx[0], frogy[0], 32, 32);
					if (touched) {
						crashed(frog_p, frogx[0], frogy[0]);
						frogx[0] = 320; frogy[0] = 430; lives -= 1;
					}
				}
			} else if (frogy[0] >= 320) {
				for (int i = 1 * n_cars; i < 2 * n_cars; i++) {
					bool touched = collided(car[0][i], car[1][i], 32, 32, frogx[0], frogy[0], 32, 32);
					if (touched) {
						crashed(frog_p, frogx[0], frogy[0]);
						frogx[0] = 320; frogy[0] = 430; lives -= 1;
					}
				}
			} else if (frogy[0] >= 270) {
				for (int i = 2 * n_cars; i < 3 * n_cars; i++) {
					bool touched = collided(car[0][i], car[1][i], 32, 32, frogx[0], frogy[0], 32, 32);
					if (touched) {
						crashed(frog_p, frogx[0], frogy[0]);
						frogx[0] = 320; frogy[0] = 430; lives -= 1;
					}
				}
			}
		}

		// dancing frog core
		// four colors, four sprites
		if (frogy[0] <= 60) {
//			frog_p->move_xy(-1,-1,0);
			frog_p->bypass(1);
			dance_p->move_xy(frogx[0], frogy[0]);
			dance_p->set_color_anim_dir(0,1,0);		// turn on animation mode
		}


//		if (now_ms() - time >= 100)
//		{
//			time = now_ms();

//				if (safe1) frogx = frogx + r1;
//				if (safe2) frogx = frogx + r2;
//				if (safe3) frogx = frogx + r3;
//
//				if (frogx < lbnd) frogx = lbnd;
//				if (frogx > rbnd) frogx = rbnd;
//			}
//
//			if (x1 < lbnd) x1 = rbnd;
//			else x1 = x1 + r1;
//			if (x2 > rbnd) x2 = lbnd;
//			else x2 = x2 + r2;
//			if (x3 < lbnd) x3 = rbnd;
//			else x3 = x3 + r3;
//		}

		frog_p->move_xy(frogx[0], frogy[0], 0);

	} while (true);
	
	
	// GAME OVER SCREEN
	gamestate(osd_p, ps2_p, 2);
	while (true) {
		if (ps2_p->get_kb_ch(&ch)) {
			if (ch == ' ') {
				lives = 5;
				break;
			}
		}
	}
}
//
//void sprites_demo (DupliCore *frog_p, DupliCore *car_p, DupliCore *log_p, DupliCore *turtle_p, SpriteCore *dance_p)
//{
//	int w = 640;
//
////	frame.bypass(0);
//	frog_p->bypass(0);
//	car_p->bypass(0);
//	log_p->bypass(0);
//	turtle_p->bypass(0);
//	dance_p->bypass(0);
//
//	// NOTE: MUST SET COLOR AND DIRECTION AT THE SAME TIME.
//
//
//	// duplicate frog core
//	// four colors, four directions
//	frog_p->set_color_dir(0,0,0);	// apply default color & direction 0 on frog sprite 0
//	frog_p->set_color_dir(1,1,1);
//	frog_p->set_color_dir(2,2,2);
//	frog_p->set_color_dir(3,3,3);
//
//	frog_p->move_xy(w/5, 0, 0);
//	frog_p->move_xy(2*w/5, 0, 1);
//	frog_p->move_xy(3*w/5, 0, 2);
//	frog_p->move_xy(4*w/5, 0, 3);
//
//
//	// car core
//	// four colors, two directions
//	car_p->set_color_dir(0,0,0);	// apply default color & direction 0 on car sprite 0
//	car_p->set_color_dir(1,1,1);
//	car_p->set_color_dir(2,0,2);
//	car_p->set_color_dir(3,1,3);
//	car_p->set_color_dir(3,1,4);
//	car_p->set_color_dir(3,1,5);
//	car_p->set_color_dir(3,1,6);
//	car_p->set_color_dir(3,1,4);
//	car_p->set_color_dir(3,1,5);
//	car_p->set_color_dir(3,1,6);
//
//	car_p->move_xy(w/5, 100, 0);
//	car_p->move_xy(2*w/5, 100, 1);
//	car_p->move_xy(3*w/5, 100, 2);
//	car_p->move_xy(4*w/5, 100, 3);
//	car_p->move_xy(w/5, 200, 4);
//	car_p->move_xy(2*w/5, 200, 5);
//	car_p->move_xy(3*w/5, 200, 6);
//
//
//	// log core
//	// four colors, two sprites
//	// 1st sprite = short log, 2nd sprite = extension log
//	log_p->move_xy(w/5, 200, 0);		// short log
//
//	// extended log
//	log_p->move_xy(5/2*w/5+32, 200, 2);				// front-end of log (USE A BIGGER SPRITE ADDRESS)
//	log_p->set_color_dir(0,1,1);
//	log_p->move_xy((5/2*w/5+32) - 52, 200, 1);		// subtract x-coordinate by ~52 pixels to extend
//
//	// double-extended log
//	log_p->move_xy(4*w/5, 200, 5);				// front-end of log (USE BIGGEST SPRITE ADDRESS)
//	log_p->set_color_dir(0,1,4);				// middle log
//	log_p->move_xy((4*w/5) - 52, 200, 4);		// subtract x-coordinate by ~52 pixels to extend
//	log_p->set_color_dir(0,1,3);				// back-end of log (USE THE SMALLEST SPRITE ADDRESS)
//	log_p->move_xy((4*w/5) - 52*2, 200, 3);		// subtract x-coordinate by ~52*2 pixels to double-extend
//
//
//	// turtle core
//	// four colors, two sprites
//	// 1st sprite = turtle, 2nd sprite = underwater turtle
//	turtle_p->set_color_dir(0,0,0);		// apply default color & direction 0 on turtle sprite 0
//	turtle_p->set_color_dir(1,1,1);
//	turtle_p->set_color_dir(2,0,2);
//	turtle_p->set_color_dir(3,1,3);
//
//	turtle_p->move_xy(w/5, 300, 0);
//	turtle_p->move_xy(2*w/5, 300, 1);
//	turtle_p->move_xy(3*w/5, 300, 2);
//	turtle_p->move_xy(4*w/5, 300, 3);
//
//
//	// dancing frog core
//	// four colors, four sprites
//	dance_p->move_xy(w/2, 400);
//	dance_p->set_color_anim_dir(0,1,0);		// turn on animation mode
//}

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
	  frame.bypass(1);
	  bar.bypass(1);
	  car.bypass(1);
	  osd.bypass(1);
	  frog.bypass(1);
	  log.bypass(1);
	  turtle.bypass(1);
	  dance.bypass(1);

	while (1) {

      //sleep_ms(3000);
      //sprites_demo(&frog, &car, &log, &turtle, &dance);
      frogger(&frog, &car, &log, &turtle, &dance, &ps2, &adc, &osd);

   } // while
} //main
