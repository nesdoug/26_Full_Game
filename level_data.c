




enum{COIN_REG, COIN_END};

//NOTE MAX_COINS = 12

//y, room, x, type
//y = TURN_OFF end of list
const unsigned char level_1_coins[]={
	0x90, 0, 0x70, COIN_REG, 
	0x90, 0, 0x90, COIN_REG, 
	0x50, 1, 0x40, COIN_REG,  
	0x70, 2, 0x00, COIN_REG, 
	0x50, 2, 0x70, COIN_REG, 
	0x50, 3, 0xa0, COIN_REG, 
	0x50, 3, 0xd0, COIN_REG, 
	0x60, 4, 0xf0, COIN_REG, 
	0x60, 5, 0x20, COIN_REG,  
	0x30, 6, 0xc0, COIN_REG, 
	0x30, 6, 0xe0, COIN_REG, 
	0x30, 7, 0x80, COIN_REG, 
	0xb0, 7, 0xc0, COIN_END, 	
	TURN_OFF
};

const unsigned char level_2_coins[]={
	0xa0, 1, 0x20, COIN_REG, 
	0xa0, 1, 0x40, COIN_REG,
	0x60, 2, 0x70, COIN_REG, 
	0x30, 3, 0x20, COIN_REG, 
	0x30, 3, 0x70, COIN_REG, 
	0xc0, 5, 0x00, COIN_REG, 
	0xc0, 5, 0x50, COIN_REG,
	0x40, 6, 0x90, COIN_REG, 
	0x40, 6, 0xd0, COIN_REG, 
	0x40, 7, 0x40, COIN_REG, 
	0xa0, 7, 0xc0, COIN_END, 
	TURN_OFF
};

const unsigned char level_3_coins[]={
	0x80, 0, 0x80, COIN_REG, 
	0x70, 1, 0x50, COIN_REG,
	0x80, 1, 0xd0, COIN_REG,
	0x80, 2, 0x40, COIN_REG, 
	0x80, 2, 0x80, COIN_REG,
	0x80, 2, 0xc0, COIN_REG, 
	0x80, 3, 0x30, COIN_REG, 
	0x50, 7, 0x25, COIN_REG, 
	0x50, 7, 0x80, COIN_END, 
	0xb0, 7, 0xd0, COIN_REG, 
	TURN_OFF
};

const unsigned char * const Coins_list[]={
	level_1_coins, level_2_coins, level_3_coins
};




enum{ENEMY_CHASE, ENEMY_BOUNCE};

//NOTE MAX_ENEMY = 10

//NOTE, after testing, we can only handle 4 enemies on the same screen

//y, room, x
//y = TURN_OFF end of list
const unsigned char level_1_enemies[]={

/* stress test	
	0x11,0,0xe2,ENEMY_CHASE,
	0x21,0,0xe4,ENEMY_CHASE,
	0x31,0,0xe6,ENEMY_CHASE,
	0x41,0,0xe8,ENEMY_CHASE,
	0x51,0,0xe8,ENEMY_CHASE,
	0x61,0,0xe8,ENEMY_CHASE,
	0x71,0,0xe8,ENEMY_CHASE,
*/
	
	0xc0, 0, 0xc0, ENEMY_CHASE, 
	0xc0, 1, 0xe0, ENEMY_BOUNCE, 
	0xc0, 2, 0x30, ENEMY_BOUNCE,  
	0xc0, 2, 0x90, ENEMY_CHASE, 
	0xb0, 3, 0x20, ENEMY_BOUNCE,  
	0xc0, 3, 0xb0, ENEMY_CHASE, 
	0x80, 5, 0x00, ENEMY_BOUNCE, 
	0xc0, 5, 0x90, ENEMY_CHASE, 
	0xc0, 6, 0x70, ENEMY_CHASE, 
	TURN_OFF
};


const unsigned char level_2_enemies[]={
	0xc0, 0, 0x90, ENEMY_CHASE, 
	0xc0, 1, 0xd0, ENEMY_CHASE, 
	0x40, 3, 0x40, ENEMY_BOUNCE, 
	0xc0, 4, 0x30, ENEMY_BOUNCE, 
	0xc0, 4, 0x80, ENEMY_BOUNCE, 
	0xc0, 6, 0x20, ENEMY_CHASE, 
	0xc0, 7, 0x20, ENEMY_BOUNCE, 
	0xc0, 7, 0x60, ENEMY_BOUNCE, 
	TURN_OFF
};

	
const unsigned char level_3_enemies[]={
	0xc0, 0, 0xc0, ENEMY_BOUNCE, 
	0xc0, 0, 0xf0, ENEMY_BOUNCE, 
	0xc0, 1, 0x80, ENEMY_CHASE, 
	0xc0, 1, 0xd0, ENEMY_CHASE, 
	0xc0, 2, 0x40, ENEMY_BOUNCE, 
	0xc0, 2, 0x80, ENEMY_BOUNCE, 
	0xc0, 2, 0xc0, ENEMY_BOUNCE, 
	0xb0, 3, 0x10, ENEMY_BOUNCE, 
	0xb0, 4, 0x60, ENEMY_CHASE, 
	0x90, 5, 0x40, ENEMY_BOUNCE, 
	0x90, 6, 0x50, ENEMY_BOUNCE, 
	0xa0, 6, 0xc0, ENEMY_BOUNCE,
	0xc0, 7, 0xe0, ENEMY_CHASE, 
	TURN_OFF
};

const unsigned char * const Enemy_list[]={
	level_1_enemies, level_2_enemies, level_3_enemies
};














// 5 bytes per metatile definition, tile TL, TR, BL, BR, palette 0-3
// T means top, B means bottom, L left,R right
// 51 maximum # of metatiles = 255 bytes
// 5th byte, 	1000 0000 = floor collision
// 				0100 0000 = all collision, solid
//				0000 0011 = palette

const unsigned char metatiles1[]={
	0, 0, 0, 0,  0,
	2, 2, 2, 2,  3,
	20, 20, 20, 20,  0,
	5, 6, 21, 22,  1,
	6, 6, 22, 22,  1,
	6, 7, 22, 23,  1,
	21, 22, 21, 22,  1,
	22, 22, 22, 22,  1,
	22, 23, 22, 23,  1,
	8, 9, 24, 25,  1,
	9, 9, 25, 25,  1,
	9, 10, 25, 26,  1
};

#define COL_DOWN 0x80
#define COL_ALL 0x40

const unsigned char is_solid[]={
	0,
	COL_DOWN,
	COL_ALL+COL_DOWN,
	COL_DOWN,
	COL_DOWN,
	COL_DOWN,
	0,
	0,
	0,
	0,
	0,
	0
};



#include "BG/Level1.c"
#include "BG/Level2.c"
#include "BG/Level3.c"

const unsigned char * const Levels_list[]={
Level1_0,Level1_1,Level1_2,Level1_3,Level1_4,Level1_5,Level1_6,Level1_7,
Level2_0,Level2_1,Level2_2,Level2_3,Level2_4,Level2_5,Level2_6,Level2_7,
Level3_0,Level3_1,Level3_2,Level3_3,Level3_4,Level3_5,Level3_6,Level3_7
};

const unsigned char Level_offsets[]={
	0,8,16
};

#define MAX_ROOMS (8-1)
#define MAX_SCROLL (MAX_ROOMS*0x100)-1
// data is exactly 240 bytes, 16 * 15
// doubles as the collision map data

