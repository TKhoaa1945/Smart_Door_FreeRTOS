#include <stdio.h>
#include "driver/ledc.h"

#define SPEED_MODE      LEDC_LOW_SPEED_MODE
#define TIMER_NUM       LEDC_TIMER_0
#define DUTY_RESOLUTION LEDC_TIMER_13_BIT
#define FREQUENCY       50
#define CLK_CONFIG      LEDC_AUTO_CLK
#define CHANNEL         LEDC_CHANNEL_0
#define INTR_TYPE       LEDC_INTR_DISABLE

#define SERVO_MIN_PULSEWIDTH 500  // Minimum pulse width in microsecond
#define SERVO_MAX_PULSEWIDTH 2500  // Maximum pulse width in microsecond
#define SERVO_MAX_DEGREE     40    // Maximum angle in degree up to which servo can rotate

void setup_pwm(uint8_t SERVO_PIN);

void set_servo_angle(int angle);


// #ifndef SERVO_H
// #define SERVO_H

// #include <driver/ledc.h>
// #include <stdint.h>

// // Cấu hình LEDC (PWM)
// #define SPEED_MODE      LEDC_LOW_SPEED_MODE
// #define TIMER_NUM       LEDC_TIMER_0       // Dùng timer LEDC_0 (điều khiển tần số & độ phân giải PWM)
// #define DUTY_RESOLUTION LEDC_TIMER_13_BIT  // Độ phân giải 13-bit (0-8191)
// #define FREQUENCY       50                 // 50 Hz cho servo
// #define CLK_CONFIG      LEDC_AUTO_CLK
// #define CHANNEL         LEDC_CHANNEL_0
// #define INTR_TYPE       LEDC_INTR_DISABLE

// // Cấu hình pulse cho servo (đơn vị µs)
// // Ở đây giả định: góc 0° tương ứng với pulse = 1120µs, góc 130° với pulse = 1920µs.
// #define SERVO_MIN_PULSEWIDTH 1120  
// #define SERVO_MAX_PULSEWIDTH 1920  
// #define SERVO_MAX_DEGREE     60    

// // Khai báo hàm
// void setup_pwm(uint8_t SERVO_PIN);
// void set_servo_angle(int angle);

// #endif // SERVO_H