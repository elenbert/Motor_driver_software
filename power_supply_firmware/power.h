#ifndef POWER_H
#define POWER_H

void init_power_supply();

void enable_power_supply();
void enable_high_voltage();
void disable_power_supply();
void disable_high_voltage();

int is_power_supply_enabled();

#endif

