
#include "pendulum_controller.h"
#include <stdlib.h>
#include "low_level.h"
#include "utils.h"
#include "kalman.h"
#include <tim.h>

#define PENDULUM_ENCODER_CPR (600 * 4)
static const float counts_per_rad = 2 * M_PI / PENDULUM_ENCODER_CPR;

Encoder_t pendulum_encoder;
void pendulum_controller_thread_entry() {

    // HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);

    // Zero Timer
    // pendulum_encoder.encoder_timer->Instance->CNT = 0;

float pos_change = pos_setpoint - motor->encoder.pll_pos;
if(fabs(pos_change) > 100)
{
    motor->vel_limit = 10000.0f;
}
else 
{
    motor->vel_limit = vel_feed_forward;
}
if (pos_setpoint < (motor->encoder.pll_pos - 100)) {
        motor->vel_limit = 10000.0f};
        else if (pos_setpoint > (motors[0].encoder.encoder_state + 100)) {
        motor->vel_limit = 10000.0f};
        else {
        motor->vel_limit = vel_feed_forward
        };
}
    //Setup pendulum encoder
    pendulum_encoder.use_index = false;
    pendulum_encoder.index_found = false;
    pendulum_encoder.encoder_timer = &htim4;
    pendulum_encoder.calibrated = true;
    pendulum_encoder.idx_search_speed = 10.0f;  // [rad/s electrical]
    pendulum_encoder.encoder_cpr = PENDULUM_ENCODER_CPR;
    pendulum_encoder.encoder_offset = 0;
    pendulum_encoder.encoder_state = 0;
    pendulum_encoder.motor_dir = 1;   // 1 or -1
    pendulum_encoder.phase = 0.0f;    // [rad]
    pendulum_encoder.pll_pos = 0.0f;  // [rad]
    pendulum_encoder.pll_vel = 0.0f;  // [rad/s]
    pendulum_encoder.pll_kp = 0.0f;   // [rad/s / rad]
    pendulum_encoder.pll_ki = 0.0f;   // [(rad/s^2) / rad]

    Motor_t* motor = &motors[0];
    //osDelay(15000);
    int dir = 1;
    // TODO: Really need to know when it is up up for good
    while (!motor->axis_legacy.control_loop_up) { // Wait For the Bring Up
        osDelay(500);
    }

    while (motor->axis_legacy.control_loop_up) {
        update_pendulum_position();

        motor->pos_setpoint = 10000 * dir;
        motor->vel_setpoint = 0;
        motor->current_setpoint = 0;
        motor->control_mode = CTRL_MODE_POSITION_CONTROL;

        dir *= -1;
        osDelay(500);
    }

vTaskDelete(osThreadGetId());
}

void update_pendulum_position() {
    // update internal encoder state
    int16_t delta_enc = (int16_t)pendulum_encoder.encoder_timer->Instance->CNT - (int16_t)pendulum_encoder.encoder_state;
    pendulum_encoder.encoder_state += (int32_t)delta_enc;
    pendulum_encoder.pll_pos = (float)pendulum_encoder.encoder_state * counts_per_rad;
    // run pll (for now pll is in units of encoder counts)
    // TODO pll_pos runs out of precision very quickly here! Perhaps decompose into integer and fractional part?
    // Predict current pos
    // encoder.pll_pos += current_meas_period * encoder.pll_vel;
    // // discrete phase detector
    // float delta_pos = (float)(encoder.encoder_state - (int32_t)floorf(encoder.pll_pos));
    // // pll feedback
    // encoder.pll_pos += current_meas_period * encoder.pll_kp * delta_pos;
    // encoder.pll_vel += current_meas_period * encoder.pll_ki * delta_pos;
}
