/*
 * Power supply controller header file
 * Kutkov Oleg
 * December 2016
 */

#ifndef __PS_CONTROLLER_H__
#define __PS_CONTROLLER_H__

void init_ps_controller();

void init_ps();
void enable_ps();

void ps_poll();
void ps_ping();

int is_ps_activated();
int is_ps_enabled();
int is_ps_online();

#endif

