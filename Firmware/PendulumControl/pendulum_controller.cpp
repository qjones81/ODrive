
#include "pendulum_controller.h"
#include <stdlib.h>
#include <tim.h>
#include "DynamicalSystemModel.h"
#include "low_level.h"
#include "Scope.h"
#include "utils.h"

#define PENDULUM_ENCODER_CPR (600 * 4)

static const float counts_per_rad = 2 * M_PI / PENDULUM_ENCODER_CPR;

Encoder_t pendulum_encoder;
osThreadId control_thread = 0;

int32_t encoder_dir = 1;

uint16_t pulley_teeth;
float pulley_pitch;
float pulley_circumference;
float pulley_radius;
float Kv;
float Kt;
float I_max;
float m_per_count;
float counts_per_m;

float x_pos;
Motor_t* motor;
void pendulum_controller_thread_entry() {

    // Setup Scope Channel 1
    Scope::ChannelConfig_t scope_config;
    scope_config.sample_rate = 100; // 1Khz
    scope_config.trigger_level = 10000; // Trigger when crosses over 1000
    scope_config.trigger_type = Scope::TRIGGER_EDGE;
    scope_config.slope_type = Scope::EDGE_RISING;
    scope_config.signal_source = &pendulum_encoder.pll_pos;

    // Adjust Scope Params
    scope.set_sample_time_base(4000); // 2 seconds
    scope.set_update_rate(100); // 1 khz

    // Do this after setting up scope sampling and update rates.  Need to add a way to reconfigure buffers if these params change
    scope.AddChannel(scope_config);
    scope.Start(); // Start sampling!

    // Setup States
    int n = 3;  // Number of states
    int m = 2;  // Number of measurements
    int q = 1;  // Number of control inputes

    Eigen::MatrixXd A(n, n);  // System dynamics matrix
    Eigen::MatrixXd B(n, q);  // System dynamics matrix
    Eigen::MatrixXd C(m, n);  // Output matrix
    Eigen::MatrixXd D(m, q);  // Output matrix

    /*
  Eigen::MatrixXd Q(n, n); // Process noise covariance 1e-6
  Eigen::MatrixXd R(m, m); // Measurement noise covariance 1e-7
  Eigen::MatrixXd P(n, n); // Estimate error covariance*/

    motor = &motors[0];

    pulley_teeth = 20;
    pulley_pitch = 0.002;
    pulley_circumference = pulley_teeth * pulley_pitch;
    pulley_radius = pulley_circumference / M_PI / 2;
    Kv = 500.0f;
    Kt = 8.27f / Kv;
    I_max = 30;
    m_per_count = pulley_circumference / motor->encoder.encoder_cpr;
    counts_per_m = 1.0f / m_per_count;

    // HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);

    // Zero Timer
    // pendulum_encoder.encoder_timer->Instance->CNT = 0;
    x_pos = 0;

    control_thread = osThreadGetId();

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

    // TODO: Really need to know when it is up up for good
    while (!motor->axis_legacy.control_loop_up) {  // Wait For the Bring Up
        osDelay(500);
    }

    // Set Control Mode to Current
    // Start Timer
    HAL_TIM_Base_Start_IT(&htim10);

    //motor->pos_setpoint = counts_per_m * 0.381f;
    // motor->vel_setpoint = 0;
    // motor->current_setpoint = 0;
    //motor->control_mode = CTRL_MODE_POSITION_CONTROL;

    while (motor->axis_legacy.control_loop_up) {
        if (osSignalWait(M_SIGNAL_CONTROL_LOOP_TICK, CONTROL_LOOP_TIMEOUT).status != osEventSignal) {
            global_fault(ERROR_NO_ERROR);
            break;
        }

        //if(!scope.get_triggered())
        update_pendulum_position();
        
        // TODO: Move to new thrread or "ControlThreadDoneSignal" to new thread
        scope.SampleChannels(); // Sample Callback

        //motor->pos_setpoint = 10000;
        // motor->vel_setpoint = 0;
        // motor->current_setpoint = 0;
        // motor->control_mode = CTRL_MODE_POSITION_CONTROL;

        // dir *= -1;
        //  osDelay(500);
    }

    // TODO: Kill Motors Here Properly.
    motor->vel_setpoint = 0;
    motor->current_setpoint = 0;
    motor->control_mode = CTRL_MODE_CURRENT_CONTROL;

    vTaskDelete(osThreadGetId());
}

void update_pendulum_position() {
    // update internal encoder state
    int16_t delta_enc = (int16_t)pendulum_encoder.encoder_timer->Instance->CNT - (int16_t)pendulum_encoder.encoder_state;
    pendulum_encoder.encoder_state += (int32_t)delta_enc;
   // pendulum_encoder.pll_pos = wrap_pm_2pi((float)pendulum_encoder.encoder_state * counts_per_rad * encoder_dir);
    pendulum_encoder.pll_pos = (float)pendulum_encoder.encoder_state * encoder_dir;

    x_pos = motor->encoder.pll_pos * m_per_count;
}

void control_timer_cb() {
    osSignalSet(control_thread, M_SIGNAL_CONTROL_LOOP_TICK);

    // static int count = 0;
    //if(count++ == 1000) {
    //    pendulum_encoder.pll_pos = HAL_GetTick();
    //    count = 0;
    //}
}