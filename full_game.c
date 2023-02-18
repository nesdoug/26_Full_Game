/*	example code for cc65, for NES
 *  Scrolling Right with metatile system
 *	, basic platformer
 *	, with coins and enemies
 *	using neslib
 *	Doug Fraker 2018 (edit 2023)
 */	
 
#include "LIB/neslib.h"
#include "LIB/nesdoug.h"
#include "Sprites.h" // holds our metasprite data
#include "full_game.h"
#include "level_data.c"


unsigned char bounce[] = {
    0,0,0,1,2,2,2,1
};
	
	
void main (void) {
	
	ppu_off(); // screen off
	
	// use the second set of tiles for sprites
	// both bg and sprites are set to 0 by default
	bank_spr(1);
	
	set_vram_buffer(); // do at least once
	
	load_title();
	
	ppu_on_all(); // turn on screen
	
	scroll_x = 0;
	set_scroll_x(scroll_x);
	//level = 2; // debugging, start on level...
	
	while (1){
		while(game_mode == MODE_TITLE){
			ppu_wait_nmi();
			
			temp1 = get_frame_count();
			temp1 = (temp1 >> 3) & 3;
			pal_col(3,title_color_rotate[temp1]);
			
			
			pad1 = pad_poll(0); // read the first controller
			pad1_new = get_pad_new(0);
			if(pad1_new & PAD_START){
				pal_fade_to(4,0); // fade to black
				ppu_off();
				load_room();
				game_mode = MODE_GAME;
				pal_bg(palette_bg);
				song = SONG_GAME;
				music_play(song);
				scroll_x = 0;
				set_scroll_x(scroll_x);
				ppu_on_all();		
				pal_bright(4); // back to normal brightness	
			}
			
			
			
		}
		
		
		
		while(game_mode == MODE_GAME){
			ppu_wait_nmi();
			
			set_music_speed(8);
		
			pad1 = pad_poll(0); // read the first controller
			pad1_new = get_pad_new(0);
			
			movement();
			
			check_spr_objects(); // see which objects are on screen
			
			sprite_collisions();
			
			// set scroll
			set_scroll_x(scroll_x);
			
			draw_screen_R();
			
			draw_sprites();
			
			
			if(pad1_new & PAD_START){
				game_mode = MODE_PAUSE;
				song = SONG_PAUSE;
				music_play(song);
				color_emphasis(COL_EMP_DARK);
				break; // out of the game loop
			}
			
			//gray_line(); //debugging
			
			if(level_up) {
				game_mode = MODE_SWITCH;
				level_up = 0;
				bright = 4;
				bright_count = 0;
				++level;
			}
			else if(death) {
				death = 0;
				bright = 0;
				scroll_x = 0;
				set_scroll_x(scroll_x);
				ppu_off();
                clear_vram_buffer();
				delay(5);
				
                oam_clear();
                game_mode = MODE_GAME_OVER;
                vram_adr(NAMETABLE_A);
                vram_fill(0,1024); // blank the screen
                ppu_on_all();
				
			}
		}
		
		
		
		// switch rooms, due to level++
		// also, death, restart level (removed feature)
		while(game_mode == MODE_SWITCH){ 
			ppu_wait_nmi();
			++bright_count;
			if(bright_count >= 0x10) { // fade out
				bright_count = 0;
				--bright;
				if(bright != 0xff) pal_bright(bright); // fade out
			}
			set_scroll_x(scroll_x);
			
			if(bright == 0xff) { // now switch rooms
				ppu_off();
				oam_clear();
				scroll_x = 0;
				set_scroll_x(scroll_x);
				if(level < 3){
					load_room();
					game_mode = MODE_GAME;
					ppu_on_all();
					pal_bright(4); // back to normal brighness
				}
				else { // set end of game. Did we win?
					game_mode = MODE_END;
					vram_adr(NAMETABLE_A);
					vram_fill(0,1024);
					ppu_on_all();
					pal_bright(4); // back to normal brighness
				}
			}
		}
		
		
		
		while(game_mode == MODE_PAUSE){
			ppu_wait_nmi();

			pad1 = pad_poll(0); // read the first controller
			pad1_new = get_pad_new(0);
			
			draw_sprites();
			
			if(pad1_new & PAD_START){
				game_mode = MODE_GAME;
				song = SONG_GAME;
				music_play(song);
				color_emphasis(COL_EMP_NORMAL);
			}
		}
		
		
		
		while(game_mode == MODE_END){
			ppu_wait_nmi();
			oam_clear();
			
			multi_vram_buffer_horz(END_TEXT, sizeof(END_TEXT), NTADR_A(6,13));
			multi_vram_buffer_horz(END_TEXT2, sizeof(END_TEXT2), NTADR_A(8,15));
			multi_vram_buffer_horz(END_TEXT3, sizeof(END_TEXT3), NTADR_A(11,17));
			temp1 = (coins / 10) + 0x30;
			temp2 = (coins % 10) + 0x30;
			one_vram_buffer(temp1, NTADR_A(18,17));
			one_vram_buffer(temp2, NTADR_A(19,17));
			
			set_scroll_x(0);
			
			music_stop();
            game_mode = MODE_RESET;
		}
		
		
		
		while(game_mode == MODE_GAME_OVER){ // you died, death
			ppu_wait_nmi();
			oam_clear();
			
			multi_vram_buffer_horz(DEAD_TEXT, sizeof(DEAD_TEXT), NTADR_A(12,14));
			
			set_scroll_x(0);
			
			music_stop();
            
            game_mode = MODE_RESET;
		}
		
        
        while(game_mode == MODE_RESET){
            // just sit and wait for a while
            // then reset if start pressed
            delay(240); // 4 seconds
            delay(60); // 1 second
            
            while(1){
                ppu_wait_nmi();
                pad1 = pad_poll(0); // read the first controller
                pad1_new = get_pad_new(0);
                if(pad1_new & PAD_START){
                    ppu_off();
                    load_title();
                    ppu_on_all();
                    game_mode = MODE_TITLE;
                    coins = 0;
                    level = 0;
                    death = 0;
                    break;
                }
            }
            
        }
	}
}



void load_title(void){
	pal_bg(palette_title);
	pal_spr(palette_sp);
	vram_adr(NAMETABLE_A);
	vram_unrle(title);
	game_mode = MODE_TITLE;
}




void load_room(void){
	clear_vram_buffer();
	offset = Level_offsets[level];
	
	set_data_pointer(Levels_list[offset]);
	set_mt_pointer(metatiles1);
	for(y=0; ;y+=0x20){ 
		for(x=0; ;x+=0x20){
			address = get_ppu_addr(0, x, y);
			index = (y & 0xf0) + (x >> 4);
			buffer_4_mt(address, index); // ppu_address, index to the data
			flush_vram_update2();
			if (x == 0xe0) break;
		}
		if (y == 0xe0) break;
	}
	
	
	++offset;
	
	// a little bit in the next room
	set_data_pointer(Levels_list[offset]);
	for(y=0; ;y+=0x20){ 
		x = 0;
		address = get_ppu_addr(1, x, y);
		index = (y & 0xf0);
		buffer_4_mt(address, index); // ppu_address, index to the data
		flush_vram_update2();
		if (y == 0xe0) break;
	}
	
	--offset;
	// copy the room to the collision map
	// the second one should auto-load with the scrolling code
	memcpy (c_map, Levels_list[offset], 240);
	
	
	sprite_obj_init();
	BoxGuy1.x = 0x4000;
	BoxGuy1.y = 0xc400;
	BoxGuy1.vel_x = 0;
	BoxGuy1.vel_y = 0;
	
	map_loaded = 0;
}




void draw_sprites(void){
	// clear all sprites from sprite buffer
	oam_clear();
	
	temp_x = high_byte(BoxGuy1.x);
	if(temp_x > 0xfc) temp_x = 1;
	if(temp_x == 0) temp_x = 1;
	// draw 1 hero
	if(direction == LEFT) {
		oam_meta_spr(temp_x, high_byte(BoxGuy1.y), RoundSprL);
	}
	else{
		oam_meta_spr(temp_x, high_byte(BoxGuy1.y), RoundSprR);
	}
	
	// draw coin sprites
	for(index = 0; index < MAX_COINS; ++index){
		temp_y = coin_y[index];
		if(temp_y == TURN_OFF) continue;
        if(!coin_active[index]) continue;
        temp_x = coin_x[index];
		if(temp_x > 0xf0) continue;
        
		// // bounce the coin
        temp1 = get_frame_count();
        temp1 = (temp1 >> 2) & 7;
        temp1 = bounce[temp1];
        temp_y += temp1;
        
		if(temp_y < 0xf0) {
			if(coin_type[index] == COIN_REG){
				oam_meta_spr(temp_x, temp_y, CoinSpr);
			}
			else {
				oam_meta_spr(temp_x, temp_y, BigCoinSpr);
			}
			
		}
	}

	// draw enemy sprites
	offset = get_frame_count() & 3;
	offset = offset << 4; // * 16, the size of the shuffle array
	for(index = 0; index < MAX_ENEMY; ++index){
		index2 = shuffle_array[offset];
		++offset;
		temp_y = enemy_y[index2];
		if(temp_y == TURN_OFF) continue;
		if(!enemy_active[index2]) continue;
		temp_x = enemy_x[index2];
		if(temp_x == 0) temp_x = 1; // problems with x = 0
		if(temp_x > 0xf0) continue;
		if(temp_y < 0xf0) {
			oam_meta_spr(temp_x, temp_y, enemy_anim[index2]);
		}
	}
	
	
	//last, draw coin in upper left
	oam_meta_spr(0x10, 0x0f, CoinHud);
	temp1 = (coins / 10) + 0xf0;
	temp2 = (coins % 10) + 0xf0;
	oam_spr(0x20, 0x17, temp1, 1);
	oam_spr(0x28, 0x17, temp2, 1);
}
	

	
	
void movement(void){
	
// handle x

	old_x = BoxGuy1.x;
	
	if(pad1 & PAD_LEFT){
		direction = LEFT;
		
        if(BoxGuy1.vel_x >= DECEL){
            BoxGuy1.vel_x -= DECEL;
        }
        else if(BoxGuy1.vel_x > 0){
            BoxGuy1.vel_x = 0;
        }
		else {
			BoxGuy1.vel_x -= ACCEL;
			if(BoxGuy1.vel_x < -MAX_SPEED) BoxGuy1.vel_x = -MAX_SPEED;
		}
	}
	else if (pad1 & PAD_RIGHT){
		
		direction = RIGHT;

		if(BoxGuy1.vel_x <= DECEL){
            BoxGuy1.vel_x += DECEL;
        }
        else if(BoxGuy1.vel_x < 0){
            BoxGuy1.vel_x = 0;
        }
		else {
			BoxGuy1.vel_x += ACCEL;
			if(BoxGuy1.vel_x >= MAX_SPEED) BoxGuy1.vel_x = MAX_SPEED;
		}
	}
	else { // nothing pressed
		if(BoxGuy1.vel_x >= ACCEL) BoxGuy1.vel_x -= ACCEL;
		else if(BoxGuy1.vel_x < -ACCEL) BoxGuy1.vel_x += ACCEL;
		else BoxGuy1.vel_x = 0;
	}
	
	BoxGuy1.x += BoxGuy1.vel_x;
	
	if(BoxGuy1.x > 0xf000) { // too far, don't wrap around
        
        if(old_x >= 0x8000){
            BoxGuy1.x = 0xf000; // max right
        }
        else{
            BoxGuy1.x = 0x0000; // max left
        }
        
		BoxGuy1.vel_x = 0;
	} 
	
	Generic.x = high_byte(BoxGuy1.x); // this is much faster than passing a pointer to BoxGuy1
	Generic.y = high_byte(BoxGuy1.y);
	Generic.width = HERO_WIDTH;
	Generic.height = HERO_HEIGHT;
	
    if(BoxGuy1.vel_x < 0){
        if(bg_coll_L() ){ // check collision left
            high_byte(BoxGuy1.x) = high_byte(BoxGuy1.x) - eject_L;
            BoxGuy1.vel_x = 0;
            if(BoxGuy1.x > 0xf000) {
                // no wrap around
                BoxGuy1.x = 0xf000;
            }
        }
    }
    else if(BoxGuy1.vel_x > 0){
        if(bg_coll_R() ){ // check collision right
            high_byte(BoxGuy1.x) = high_byte(BoxGuy1.x) - eject_R;
            BoxGuy1.vel_x = 0;
            if(BoxGuy1.x > 0xf000) {
                // no wrap around
                BoxGuy1.x = 0x0000;
            }
        }
    }
    // skip collision if vel = 0

	
// handle y

// gravity

	// BoxGuy1.vel_y is signed
	if(BoxGuy1.vel_y < 0x300){
		BoxGuy1.vel_y += GRAVITY;
	}
	else{
		BoxGuy1.vel_y = 0x300; // consistent
	}
	BoxGuy1.y += BoxGuy1.vel_y;
	
	Generic.x = high_byte(BoxGuy1.x);
	Generic.y = high_byte(BoxGuy1.y);
	
    if(BoxGuy1.vel_y > 0){
        if(bg_coll_D() ){ // check collision below
            high_byte(BoxGuy1.y) = high_byte(BoxGuy1.y) - eject_D;
            BoxGuy1.y &= 0xff00;
            if(BoxGuy1.vel_y > 0) {
                BoxGuy1.vel_y = 0;
            }
        }
    }
    else if(BoxGuy1.vel_y < 0){
        if(bg_coll_U() ){ // check collision above
            high_byte(BoxGuy1.y) = high_byte(BoxGuy1.y) - eject_U;
            BoxGuy1.vel_y = 0;
        }
    }
    
	// check collision down a little lower than hero
	Generic.y = high_byte(BoxGuy1.y); // the rest should be the same
	
	if(pad1_new & PAD_A) {
        if(bg_coll_D2() ) {
			BoxGuy1.vel_y = JUMP_VEL; // JUMP
			sfx_play(SFX_JUMP, 0);
			short_jump_count = 1;
		}
		
	}
	
	// allow shorter jumps
	if(short_jump_count){
		++short_jump_count;
		if(short_jump_count > 30) short_jump_count = 0;
	}
	if((short_jump_count) && ((pad1 & PAD_A) == 0) && (BoxGuy1.vel_y < -0x200)){
		BoxGuy1.vel_y = -0x200;
		short_jump_count = 0;
	}
	
	// do we need to load a new collision map? (scrolled into a new room)
	if((scroll_x & 0xff) < 4){
		if(!map_loaded){
			new_cmap();
			map_loaded = 1; // only do once
		}
		
	}
	else{
		map_loaded = 0;
	}
	
// scroll
	temp5 = BoxGuy1.x;
	if (BoxGuy1.x > MAX_RIGHT){
		temp1 = (BoxGuy1.x - MAX_RIGHT) >> 8;
        if (temp1 > 3) temp1 = 3; // max scroll change
		scroll_x += temp1;
		high_byte(BoxGuy1.x) = high_byte(BoxGuy1.x) - temp1;
	}

	if(scroll_x >= MAX_SCROLL) {
		scroll_x = MAX_SCROLL; // stop scrolling right, end of level
		BoxGuy1.x = temp5; // but allow the x position to go all the way right
		if(high_byte(BoxGuy1.x) >= 0xf1) {
			BoxGuy1.x = 0xf100;
		}
	}
}	




void check_spr_objects(void){
	++enemy_frames;
	Generic2.x = high_byte(BoxGuy1.x);
	// mark each object "active" if they are, and get the screen x
	
	for(index = 0; index < MAX_COINS; ++index){
		coin_active[index] = 0; //default to zero
		if(coin_y[index] != TURN_OFF){
			high_byte(temp5) = coin_room[index];
			low_byte(temp5) = coin_actual_x[index];
			coin_active[index] = get_position();
			coin_x[index] = temp_x; // screen x coords
		}

	}
	

	for(index = 0; index < MAX_ENEMY; ++index){
		enemy_active[index] = 0; //default to zero
		if(enemy_y[index] != TURN_OFF){
			high_byte(temp5) = enemy_room[index];
			low_byte(temp5) = enemy_actual_x[index];
			temp1 = enemy_active[index] = get_position();
			if(temp1 == 0) continue;
			enemy_x[index] = temp_x; // screen x coords
			
			enemy_moves(); // if active, do it's moves now
		}

	}

	
}



char get_position(void){
	// is it in range ? return 1 if yes
	
	temp5 -= scroll_x;
	temp_x = temp5 & 0xff;
	if(high_byte(temp5)) return 0;
	return 1;
}




void enemy_moves(void){
	if(enemy_type[index] == ENEMY_CHASE){
		//for bg collisions
		Generic.x = enemy_x[index];
		Generic.y = enemy_y[index] + 6; // mid point
		Generic.width = 13;
        Generic.height = 15;
        
        // note, Generic2 is the hero's x position
		
		enemy_anim[index] = EnemyChaseSpr;
		if(enemy_frames & 1) return; // half speed
		if(enemy_x[index] > Generic2.x){
			Generic.x -= 1; // test going left
            bg_collision_fast();
			if(collision_L) return;
            // else, no collision, do the move.
			if(enemy_actual_x[index] == 0) --enemy_room[index];
			--enemy_actual_x[index];
		}
		else if(enemy_x[index] < Generic2.x){
			Generic.x += 1; // test going right
            bg_collision_fast();
			if(collision_R) return;
			++enemy_actual_x[index];
			if(enemy_actual_x[index] == 0) ++enemy_room[index];
		}
	}
	else if(enemy_type[index] == ENEMY_BOUNCE){
		temp1 = enemy_frames + (index << 3);
        temp1 &= 0x3f;
		if(temp1 < 16){ // stand still
			enemy_anim[index] = EnemyBounceSpr;
		}
        else if(temp1 < 22){
			--enemy_y[index]; // jump
            --enemy_y[index]; // jump faster
			enemy_anim[index] = EnemyBounceSpr2;
		}
		else if(temp1 < 42){
			--enemy_y[index]; // jump
			enemy_anim[index] = EnemyBounceSpr2;
		}
        else if(temp1 < 44){ // use short anim. 2 frames
            --enemy_y[index]; // jump
            enemy_anim[index] = EnemyBounceSpr;
        }
		else {
            ++enemy_y[index]; // fall
            if(temp1 < 62){
                ++enemy_y[index]; // fall faster
            }
			enemy_anim[index] = EnemyBounceSpr2;
			temp1 = enemy_y[index];
			//check ground collision
			Generic.x = enemy_x[index];
			Generic.y = enemy_y[index];
			Generic.width = 15;
			Generic.height = 14;
			
			if(bg_coll_D()){
				enemy_y[index] -= eject_D;
			}
            
		}
	}

}




void bg_collision_fast(void){
	// rewrote this for enemies, bg_collision was too slow
    // test 1 point on each side
    
	collision_L = 0;
	collision_R = 0;
    
	if(Generic.y >= 0xf0) return;
	
	temp5 = Generic.x + scroll_x;
	temp_x = temp5 & 0xff; // low byte x
	temp_room = temp5 >> 8; // high byte x
	
	temp_y = Generic.y + 6; // y middle
	
	bg_collision_sub();
	
	if(bg_collision_sub() & COL_ALL){
		++collision_L;
	}
	
	// right side
	temp5 += Generic.width;
	temp_x = temp5 & 0xff; // low byte x
	temp_room = temp5 >> 8; // high byte x
	
	// temp_y is unchanged
	bg_collision_sub();
	
	if(bg_collision_sub() & COL_ALL){ // find a corner in the collision map
		++collision_R;
	}
}



char bg_coll_L(void){
    // check 2 points on the left side
    temp5 = Generic.x + scroll_x;
    temp_x = (char)temp5; // low byte
    temp_room = temp5 >> 8; // high byte
    
    eject_L = temp_x | 0xf0;
    temp_y = Generic.y + 2;
    if(bg_collision_sub() & COL_ALL) return 1;
    
    temp_y = Generic.y + Generic.height;
    temp_y -= 2;
    if(bg_collision_sub() & COL_ALL) return 1;
    
    return 0;
}

char bg_coll_R(void){
    // check 2 points on the right side
    temp5 = Generic.x + scroll_x + Generic.width;
    temp_x = (char)temp5; // low byte
    temp_room = temp5 >> 8; // high byte
    
    eject_R = (temp_x + 1) & 0x0f;
    temp_y = Generic.y + 2;
    if(bg_collision_sub() & COL_ALL) return 1;
    
    temp_y = Generic.y + Generic.height;
    temp_y -= 2;
    if(bg_collision_sub() & COL_ALL) return 1;
    
    return 0;
}

char bg_coll_U(void){
    // check 2 points on the top side
    temp5 = Generic.x + scroll_x;
    temp5 += 2;
    temp_x = (char)temp5; // low byte
    temp_room = temp5 >> 8; // high byte
    
    temp_y = Generic.y;
    eject_U = temp_y | 0xf0;
    if(bg_collision_sub() & COL_ALL) return 1;
    
    temp5 = Generic.x + scroll_x + Generic.width;
    temp5 -= 2;
    temp_x = (char)temp5; // low byte
    temp_room = temp5 >> 8; // high byte
    
    if(bg_collision_sub() & COL_ALL) return 1;
    
    return 0;
}

char bg_coll_D(void){
    // check 2 points on the bottom side
    temp5 = Generic.x + scroll_x;
    temp5 += 2;
    temp_x = (char)temp5; // low byte
    temp_room = temp5 >> 8; // high byte
    
    temp_y = Generic.y + Generic.height;
    
    if((temp_y & 0x0f) > 3) return 0; // bug fix
    // so we don't snap to those platforms
    // don't fall too fast, or might miss it.
    
    eject_D = (temp_y + 1) & 0x0f;
    
    if(bg_collision_sub() ) return 1;
    
    temp5 = Generic.x + scroll_x + Generic.width;
    temp5 -= 2;
    temp_x = (char)temp5; // low byte
    temp_room = temp5 >> 8; // high byte
    
    if(bg_collision_sub() ) return 1;
    
    return 0;
}

char bg_coll_D2(void){
    // check 2 points on the bottom side
    // a little lower, for jumping
    temp5 = Generic.x + scroll_x;
    temp5 += 2;
    temp_x = (char)temp5; // low byte
    temp_room = temp5 >> 8; // high byte
    
    temp_y = Generic.y + Generic.height;
    temp_y += 2;
    if(bg_collision_sub() ) return 1;
    
    temp5 = Generic.x + scroll_x + Generic.width;
    temp5 -= 2;
    temp_x = (char)temp5; // low byte
    temp_room = temp5 >> 8; // high byte
    
    if(bg_collision_sub() ) return 1;
    
    return 0;
}




char bg_collision_sub(void){
    if(temp_y >= 0xf0) return 0;
    
	coordinates = (temp_x >> 4) + (temp_y & 0xf0);
    // we just need 4 bits each from x and y
	
	map = temp_room&1; // high byte
	if(!map){
		collision = c_map[coordinates];
	}
	else{
		collision = c_map2[coordinates];
	}
	
    return is_solid[collision];
}



void draw_screen_R(void){
	// scrolling to the right, draw metatiles as we go
	pseudo_scroll_x = scroll_x + 0x120;
	
	temp1 = pseudo_scroll_x >> 8;
	
	offset = Level_offsets[level];
	offset += temp1;
	
	set_data_pointer(Levels_list[offset]);
	nt = temp1 & 1;
	x = pseudo_scroll_x & 0xff;
	
	switch(scroll_count){
		case 0:
			address = get_ppu_addr(nt, x, 0);
			index = 0 + (x >> 4);
			buffer_4_mt(address, index); // ppu_address, index to the data
			
			address = get_ppu_addr(nt, x, 0x20);
			index = 0x20 + (x >> 4);
			buffer_4_mt(address, index); // ppu_address, index to the data
			break;
			
		case 1:
			address = get_ppu_addr(nt, x, 0x40);
			index = 0x40 + (x >> 4);
			buffer_4_mt(address, index); // ppu_address, index to the data
			
			address = get_ppu_addr(nt, x, 0x60);
			index = 0x60 + (x >> 4);
			buffer_4_mt(address, index); // ppu_address, index to the data
			break;
			
		case 2:
			address = get_ppu_addr(nt, x, 0x80);
			index = 0x80 + (x >> 4);
			buffer_4_mt(address, index); // ppu_address, index to the data
			
			address = get_ppu_addr(nt, x, 0xa0);
			index = 0xa0 + (x >> 4);
			buffer_4_mt(address, index); // ppu_address, index to the data
			break;
			
		default:
			address = get_ppu_addr(nt, x, 0xc0);
			index = 0xc0 + (x >> 4);
			buffer_4_mt(address, index); // ppu_address, index to the data
			
			address = get_ppu_addr(nt, x, 0xe0);
			index = 0xe0 + (x >> 4);
			buffer_4_mt(address, index); // ppu_address, index to the data
	}


	++scroll_count;
	scroll_count &= 3; //mask off top bits, keep it 0-3
}




void new_cmap(void){
	// copy a new collision map to one of the 2 c_map arrays
	room = ((scroll_x >> 8) +1); //high byte = room, one to the right
	offset = Level_offsets[level];
	offset += room;
	
	map = room & 1; //even or odd?
	if(!map){
		memcpy (c_map, Levels_list[offset], 240);
	}
	else{
		memcpy (c_map2, Levels_list[offset], 240);
	}
}





void sprite_collisions(void){

	Generic.x = high_byte(BoxGuy1.x);
	Generic.y = high_byte(BoxGuy1.y);
	Generic.width = HERO_WIDTH;
	Generic.height = HERO_HEIGHT;
	
	for(index = 0; index < MAX_COINS; ++index){
		if(coin_active[index]){
			if(coin_type[index] == COIN_REG){
				Generic2.width = COIN_WIDTH;
				Generic2.height = COIN_HEIGHT;
			}
			else{
				Generic2.width = BIG_COIN;
				Generic2.height = BIG_COIN;
			}
			Generic2.x = coin_x[index];
			Generic2.y = coin_y[index];
			if(check_collision(&Generic, &Generic2)){
				coin_y[index] = TURN_OFF;
				sfx_play(SFX_DING, 0);
				++coins;
				
				if(coin_type[index] == COIN_END) ++level_up;
			}
		}
	}

	Generic2.width = ENEMY_WIDTH;
	Generic2.height = ENEMY_HEIGHT;
	
	for(index = 0; index < MAX_ENEMY; ++index){
		if(enemy_active[index]){
			Generic2.x = enemy_x[index];
			Generic2.y = enemy_y[index];
			if(check_collision(&Generic, &Generic2)){
				enemy_y[index] = TURN_OFF;
				enemy_active[index] = 0;
				sfx_play(SFX_NOISE, 0);
				if(coins) {
					coins -= 5;
					if(coins > 0x80) coins = 0;
				}
				else { // die
					++death;
				} 
			}
		}
	}
}








// cc65 is very slow if 2 arrays/pointers are on the same line, so I
// broke them into 2 separate lines with temp1 as a passing variable
void sprite_obj_init(void){

	pointer = Coins_list[level];
	for(index = 0,index2 = 0;index < MAX_COINS; ++index){
		
		coin_x[index] = 0;

		temp1 = pointer[index2]; // get a byte of data
		coin_y[index] = temp1;
		
		if(temp1 == TURN_OFF) break;

		++index2;
		
		coin_active[index] = 0;

		
		temp1 = pointer[index2]; // get a byte of data
		coin_room[index] = temp1;
		
		++index2;
		
		temp1 = pointer[index2]; // get a byte of data
		coin_actual_x[index] = temp1;
		
		++index2;
		
		temp1 = pointer[index2]; // get a byte of data
		coin_type[index] = temp1;
		
		++index2;
	}
	
	for(++index;index < MAX_COINS; ++index){
		coin_y[index] = TURN_OFF;
	}
	
	
	

	pointer = Enemy_list[level];
	for(index = 0,index2 = 0;index < MAX_ENEMY; ++index){
		
		enemy_x[index] = 0;

		temp1 = pointer[index2]; // get a byte of data
		enemy_y[index] = temp1;
		
		if(temp1 == TURN_OFF) break;

		++index2;
		
		enemy_active[index] = 0;
		
		temp1 = pointer[index2]; // get a byte of data
		enemy_room[index] = temp1;
		
		++index2;
		
		temp1 = pointer[index2]; // get a byte of data
		enemy_actual_x[index] = temp1;
		
		++index2;
		
		temp1 = pointer[index2]; // get a byte of data
		enemy_type[index] = temp1;
		
		++index2;
	}
	
	for(++index;index < MAX_ENEMY; ++index){
		enemy_y[index] = TURN_OFF;
	}
}







