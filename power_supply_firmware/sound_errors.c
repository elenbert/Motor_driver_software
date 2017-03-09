#include <util/delay.h>
#include "sound.h"
#include "sound_errors.h"

void sound_boot_ok()
{
	play_sound(1, 280, 0);
}

void sound_ps_enabled()
{
	play_sound(1, 150, 0);
}

void sound_5v_failure()
{
	play_sound(1, 350, 0);
	_delay_ms(200);
	play_sound(1, 200, 0);
}

void sound_12v_failure()
{
	play_sound(1, 350, 0);
	_delay_ms(200);
	play_sound(1, 200, 0);
	_delay_ms(140);
	play_sound(1, 200, 0);
}

void sound_12v_5v_failure()
{
	play_sound(1, 350, 0);
	_delay_ms(200);
	play_sound(1, 200, 0);
	_delay_ms(110);
	play_sound(1, 200, 0);
	_delay_ms(110);
	play_sound(1, 200, 0);
}

void sound_external_block()
{
	play_sound(1, 80, 0);
	_delay_ms(10);
	play_sound(1, 80, 0);
	_delay_ms(10);
	play_sound(1, 80, 0);
}

void sound_overcurrent_ra()
{
	
}

void sound_overcurrent_dec()
{
	
}

void sound_nocurrent_ra()
{
	
}

void sound_nocurrent_dec()
{
	
}

void sound_communication_failure()
{
	
}

