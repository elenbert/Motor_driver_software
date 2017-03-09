#ifndef SOUND_ERRORS_H
#define SOUND_ERRORS_H

void sound_boot_ok();
void sound_ps_enabled();

void sound_5v_failure();
void sound_12v_failure();
void sound_12v_5v_failure();

void sound_external_block();

void sound_overcurrent_ra();
void sound_overcurrent_dec();
void sound_nocurrent_ra();
void sound_nocurrent_dec();

void sound_communication_failure();

#endif

