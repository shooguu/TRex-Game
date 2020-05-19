/*
 * Project5.c
 *
 * Created: 3/13/2020 7:06:22 PM
 * Author : shogonakamura
 */ 

#include <avr/io.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include "avr.h"
#include "lcd.h"

#define LCD_SIZE 16
#define LEVEL_SIZE 90

bool GAMEOVER = false;

// flag to notify whether it's in jump frame or not
// this is so pressing the jump button during the jump
// doesn't affect the code
bool IN_JUMP = false;

// the level starts at 1
// if the end of level is reached,
// this should increase
int level = 1;

// current frame is set at 0 and is increase by the alpha
// increasing the alpha should speed the frame, therefore
// increase the speed of the game
//double alpha = 0.00001;
double alpha = 0.05;
double current = 0;
double timer = 0;


// Jump frame is how long the jump frame lasts
int jump_frame = 2;


char top_platform[LCD_SIZE] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
							   ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '
							  };
char bottom_platform[LCD_SIZE] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
								  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '
								 };

char level_1[LEVEL_SIZE] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '
};

char level_2[LEVEL_SIZE] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '
};

char level_3[LEVEL_SIZE] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '
};


char keypad[17] = {
	'1', '2', '3', 'A',
	'4', '5', '6', 'B',
	'7', '8', '9', 'C',
	'*', '0', '#', 'D'
};


int is_pressed(int r, int c)
{
	DDRC = 0x00;
	PORTC = 0x00;
	
	SET_BIT(DDRC, r);
	SET_BIT(PORTC, c + 4);
	avr_wait(1);
	
	if (GET_BIT(PINC, c + 4)) {
		return 0;
	}
	
	return 1;
}

int get_key(void)
{
	int r, c;
	for (r = 0; r < 4; ++r)
	{
		for(c = 0; c < 4; ++c)
		{
			if(is_pressed(r,c))
			return (r *4) + c + 1;
		}
	}
	return 0;
}


void make_level()
{
	int counter = 5;

	// MAKE LEVEL 1
	while (counter < LEVEL_SIZE)
	{
		if (rand() % 2 == 0)
		{
			level_1[counter] = '|';
			counter += 4;
		}
		counter += 1;
	}

	// MAKE LEVEL 2
	counter = 5;
	while (counter < LEVEL_SIZE)
	{
		if (rand() % 2 == 0)
		{
			level_2[counter] = '|';
			counter += 4;
		}
		counter += 1;
	}

	// MAKE LEVEL 3
	counter = 5;
	while (counter < LEVEL_SIZE)
	{
		if (rand() % 2 == 0)
		{
			level_3[counter] = '|';
			counter += 5;
		}
		counter += 1;
	}
}

void make_platform(int level, int current)
{
	if (level == 1)
	{
		for (int i = 0; i < LCD_SIZE; ++i)
		{
			if (bottom_platform[0] == 'x' && level_1[current] == '|')
			{
				GAMEOVER = true;
				break; // GAME OVER
			}
			if (IN_JUMP == false)
			{
				bottom_platform[0] = 'x';
			}
			bottom_platform[i] = level_1[current + i];
		}
	}
	else if (level == 2)
	{
		for (int i = 0; i < LCD_SIZE; ++i)
		{
			if (bottom_platform[0] == 'x' && level_2[current] == '|')
			{
				GAMEOVER = true;
				break; // GAME OVER
			}
			if (IN_JUMP == false)
			{
				bottom_platform[0] = 'x';
			}
			bottom_platform[i] = level_2[current + i];
		}
	}
	else if (level == 3)
	{
		for (int i = 0; i < LCD_SIZE; ++i)
		{
			if (bottom_platform[0] == 'x' && level_3[current] == '|')
			{
				GAMEOVER = true;
				break; // GAME OVER
			}
			if (IN_JUMP == false)
			{
				bottom_platform[0] = 'x';
			}
			bottom_platform[i] = level_3[current + i];
		}
	}
}

// 'x' denotes the character
void jump()
{
	if (bottom_platform[0] == 'x')
	{
		bottom_platform[0] = ' ';
	}
	top_platform[0] = 'x';
}

void jump_down()
{
	bottom_platform[0] = 'x';
	top_platform[0] = ' ';
}

void next_level()
{
	lcd_clr();
	lcd_pos(0, 0);
	lcd_puts2("NEXT LEVEL");
	avr_wait(2000);
	level += 1;
	if (level == 2)
	{
		alpha = 0.1;
	}
	if (level == 3)
	{
		alpha = 0.2;
	}
	current = 0;
	timer = 0;
	lcd_clr();
	
}

int main(void)
{
	srand(time(NULL));  // initialize srand
	make_level();
	lcd_init();


	while (1)
	{
			
		
		if (GAMEOVER == true)
		{
			lcd_clr();
			lcd_pos(0, 0);
			lcd_puts2("GAME OVER");
			while (1)
			{
				if (keypad[get_key() - 1] == '2')
				{
					memset(level_1, ' ', LEVEL_SIZE);
					memset(level_2, ' ', LEVEL_SIZE);
					memset(level_3, ' ', LEVEL_SIZE);
					make_level();
					GAMEOVER = false;
					level = 1;
					timer = 0;
					current = 0;
					IN_JUMP = false;
					break;
				}
			}
		}
	
		lcd_clr();
		lcd_pos(0, 0);
		lcd_puts2(top_platform);
		lcd_pos(1, 0);
		lcd_puts2(bottom_platform);
		
		// if the current frame reaches the end of the level
		// increase the level
		
		if ((int)(current + LCD_SIZE) == (int)LEVEL_SIZE)
		{
			next_level();
		}
	
		
		// This is increasing the alpha
		// We can adjust this to increase or
		// decrease game speed
		current = current + alpha;
		

		// I'm matching to see if the alpha equals an integer
		// If it does I increase the frame of the platform
		if (abs(current - timer) < 0.00000001)
		{
			// jump frame is how many frames the jump lasts for
			// It's currently set to 2 frames
			if (jump_frame == 0)
			{
				jump_down();
				IN_JUMP = false;
				jump_frame = 2;
			}
			if (IN_JUMP)
			{
				jump_frame--;
			}
			
			make_platform(level, (int)current);
			
			timer += 1;
		}
		
		// This executes the jump
		// The jump-frame counts how many frames it should jump for
		if (keypad[get_key() - 1] == '1')
		{
			if (IN_JUMP == false)
			{
				jump();
				IN_JUMP = true;
				lcd_clr();
				lcd_pos(0, 0);
				lcd_puts2(top_platform);
				lcd_pos(1, 0);
				lcd_puts2(bottom_platform);
			}
		}
	
	}
	lcd_clr();
	lcd_pos(0, 0);
	lcd_puts2("GAME OVER");
}


