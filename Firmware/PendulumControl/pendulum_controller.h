#ifndef __PENDULUM_CONTROLLER_H
#define __PENDULUM_CONTROLLER_H


#ifdef __cplusplus
#include <low_level.h>
extern "C" {
#endif

//default timeout waiting for phase measurement signals
#define CONTROL_LOOP_TIMEOUT 2 // [ms]

/* Exported types ------------------------------------------------------------*/
typedef enum {
    M_SIGNAL_CONTROL_LOOP_TICK = 1u << 0
} Control_thread_signals_t;

// TODO: State Space

extern Encoder_t pendulum_encoder;
extern osThreadId control_thread;
extern float x_pos;
void pendulum_controller_thread_entry();
void update_pendulum_position();

void control_timer_cb();

#ifdef __cplusplus
}
#endif

#endif /* __PENDULUM_CONTROLLER_H */
