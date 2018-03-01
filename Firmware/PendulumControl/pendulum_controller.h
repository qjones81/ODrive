#ifndef __PENDULUM_CONTROLLER_H
#define __PENDULUM_CONTROLLER_H


#ifdef __cplusplus
#include <low_level.h>
extern "C" {
#endif


extern Encoder_t pendulum_encoder;

void pendulum_controller_thread_entry();
void update_pendulum_position();

#ifdef __cplusplus
}
#endif

#endif /* __PENDULUM_CONTROLLER_H */
