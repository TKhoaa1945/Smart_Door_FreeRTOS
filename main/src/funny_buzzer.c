#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523

void buzzer_play_note(int note, int duration_ms) {
    ledc_set_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0, note);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 512); // 50% duty
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
    vTaskDelay(pdMS_TO_TICKS(duration_ms));
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
    vTaskDelay(pdMS_TO_TICKS(50)); // nghỉ giữa các nốt
}

void buzzer_play_song() {
    buzzer_play_note(NOTE_C4, 300);
    buzzer_play_note(NOTE_C4, 300);
    buzzer_play_note(NOTE_D4, 600);
    buzzer_play_note(NOTE_C4, 600);
    buzzer_play_note(NOTE_F4, 600);
    buzzer_play_note(NOTE_E4, 1000);
    
    buzzer_play_note(NOTE_C4, 300);
    buzzer_play_note(NOTE_C4, 300);
    buzzer_play_note(NOTE_D4, 600);
    buzzer_play_note(NOTE_C4, 600);
    buzzer_play_note(NOTE_G4, 600);
    buzzer_play_note(NOTE_F4, 1000);
}
