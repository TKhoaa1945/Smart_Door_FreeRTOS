#include "../header/servo.h"

void setup_pwm(uint8_t SERVO_PIN) {
    // Configure the LEDC timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = SPEED_MODE,
        .timer_num        = TIMER_NUM,
        .duty_resolution  = DUTY_RESOLUTION,
        .freq_hz          = FREQUENCY,
        .clk_cfg          = CLK_CONFIG
    };
    ledc_timer_config(&ledc_timer);

    // Configure the LEDC channel
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = SPEED_MODE,
        .channel        = CHANNEL,
        .timer_sel      = TIMER_NUM,
        .intr_type      = INTR_TYPE,
        .gpio_num       = SERVO_PIN,
        .duty           = 0, // Initial duty cycle
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel);
}

void set_servo_angle(int angle) 
{
    // Convert angle to duty cycle
    int pulsewidth = SERVO_MIN_PULSEWIDTH + ((SERVO_MAX_PULSEWIDTH - SERVO_MIN_PULSEWIDTH) * angle) / SERVO_MAX_DEGREE;
    int duty = (pulsewidth * 8192) / 20000; // 8192 is 2^13 for 13-bit resolution
    ledc_set_duty(SPEED_MODE, CHANNEL, duty);
    ledc_update_duty(SPEED_MODE, CHANNEL);
}


// #include "servo.h"
// #include <stdio.h>

// void setup_pwm(uint8_t SERVO_PIN) {
//     // Cấu hình timer LEDC
//     ledc_timer_config_t ledc_timer = {
//         .speed_mode       = SPEED_MODE,
//         .timer_num        = TIMER_NUM,
//         .duty_resolution  = DUTY_RESOLUTION,
//         .freq_hz          = FREQUENCY,
//         .clk_cfg          = CLK_CONFIG
//     };
//     ledc_timer_config(&ledc_timer);

//     // Cấu hình kênh LEDC
//     ledc_channel_config_t ledc_channel = {
//         .speed_mode     = SPEED_MODE,
//         .channel        = CHANNEL,
//         .timer_sel      = TIMER_NUM,
//         .intr_type      = INTR_TYPE,
//         .gpio_num       = SERVO_PIN,
//         .duty           = 0,    // Duty cycle ban đầu = 0
//         .hpoint         = 0
//     };
//     ledc_channel_config(&ledc_channel);
// }

// void set_servo_angle(int angle) {
//     // Tính pulse width dựa trên góc (µs)
//     int pulsewidth = SERVO_MIN_PULSEWIDTH +
//         ((SERVO_MAX_PULSEWIDTH - SERVO_MIN_PULSEWIDTH) * angle) / SERVO_MAX_DEGREE;
//     // Với độ phân giải 13-bit, giá trị duty có giá trị từ 0 đến 8191, chu kỳ 20,000µs (50Hz)
//     int duty = (pulsewidth * 8192) / 20000;
//     ledc_set_duty(SPEED_MODE, CHANNEL, duty);
//     ledc_update_duty(SPEED_MODE, CHANNEL);
// }