#ifndef __PENDULUM_CONTROLLER_H
#define __PENDULUM_CONTROLLER_H

#include <low_level.h>

void pendulum_controller_thread_entry();
void update_pendulum_position();

Encoder_t pendulum_encoder;

#endif /* __PENDULUM_CONTROLLER_H */
