#include "header.h"

#define servo_pin 13
#define BUZZER_GPIO 27
spi_device_handle_t spi;

//config wifi
char ssid[] = "P204 / 209";
char pass[] = "30041975";

volatile bool doorOpened = false;  // Cửa ban đầu đóng
void setup_pwm(uint8_t SERVO_PIN);
void buzzer_init();
void set_servo_angle(int angle); 
void servo_open_door();
void servo_close_door();
void buzzer_play(int frequency, int duration_ms);
void rfid_Task(void  * pvParameters);
void add_rfid(void * pvParameters);

void app_main()
{
    setup_pwm(servo_pin);
    buzzer_init();
    lcd_init();                             // Initialize the LCD
    esp_err_t ret;
    //spi_device_handle_t spi;
    //g_spi = spi;
    spi_bus_config_t buscfg={
        .miso_io_num=PIN_NUM_MISO,
        .mosi_io_num=PIN_NUM_MOSI,
        .sclk_io_num=PIN_NUM_CLK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1
    };
    spi_device_interface_config_t devcfg={
        .clock_speed_hz=5000000,               //Clock out at 5 MHz
        .mode=0,                                //SPI mode 0
        .spics_io_num=PIN_NUM_CS,               //CS pin
        .queue_size=7,                          //We want to be able to queue 7 transactions at a time
        //.pre_cb=ili_spi_pre_transfer_callback,  //Specify pre-transfer callback to handle D/C line
    };
    //Initialize the SPI bus
    ret=spi_bus_initialize(HSPI_HOST, &buscfg, 1);
    assert(ret==ESP_OK);
    //Attach the RFID to the SPI bus
    ret=spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
    assert(ret==ESP_OK);
    PCD_Init(spi);
        gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << 12),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&io_conf);
    set_servo_angle(40);
    doorOpened = false;
    xTaskCreate(rfid_Task, "RFID Task", 4096, NULL, 2, NULL);
    //xTaskCreate(sensorTask, "sensor Task", 4096, NULL, 3, NULL);
    while(1)
    {
        vTaskDelay(10/portTICK_PERIOD_MS);
     }
}

void rfid_Task(void  * pvParameters ){
    if(PICC_IsNewCardPresent(spi)){
    PICC_ReadCardSerial(spi);	                   //READ CARD
    PICC_DumpToSerial(spi,&uid);                  //DETAILS OF UID ALONG WITH SECTORS
    if(PICC_Servo_Controll(&uid)){
        lcd_clear();            
        lcd_put_cursor(0, 0);                   // Set the cursor position to the first row, first column
        lcd_send_string("GOOD!"); 
        vTaskDelay(pdMS_TO_TICKS(1000));
        buzzer_play(1000, 1000);
        printf("GOOD!\n");
        if(!doorOpened) {
            // Clear the LCD screen
            servo_open_door();
            vTaskDelay(pdMS_TO_TICKS(1000));
            servo_close_door();
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }else{
        lcd_clear();            
        lcd_put_cursor(0, 0);                   // Set the cursor position to the first row, first column
        lcd_send_string("Stupid Door!");                // Clear the LCD screen
        printf("\nStupid Door\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
        buzzer_play(1000, 3000);    
        //printf(uid->uid)
        //vTaskDelay(pdMS_TO_TICKS(1000));
        
        vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }else vTaskDelay(pdMS_TO_TICKS(1000));
}

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

void buzzer_init()
{
    ledc_timer_config_t timer_conf = {
        .duty_resolution = LEDC_TIMER_10_BIT, // Set resolution to 10 bits
        .freq_hz = 2000,                      // Default frequency 2kHz
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0
    };
    ledc_timer_config(&timer_conf);

    ledc_channel_config_t channel_conf = {
        .gpio_num = BUZZER_GPIO,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,  // Start with 0 duty cycle (off)
        .hpoint = 0
    };
    ledc_channel_config(&channel_conf);
}
void set_servo_angle(int angle) 
{
    // Convert angle to duty cycle
    int pulsewidth = SERVO_MIN_PULSEWIDTH + ((SERVO_MAX_PULSEWIDTH - SERVO_MIN_PULSEWIDTH) * angle) / SERVO_MAX_DEGREE;
    int duty = (pulsewidth * 8192) / 20000; // 8192 is 2^13 for 13-bit resolution
    ledc_set_duty(SPEED_MODE, CHANNEL, duty);
    ledc_update_duty(SPEED_MODE, CHANNEL);
}

void servo_open_door(){
    printf("Opening door...\n");
    for (int angle =40 ; angle >= 0; angle--) {
        set_servo_angle(angle);
        vTaskDelay(pdMS_TO_TICKS(20)); 
    }
    doorOpened = true;
    printf("Door opened.\n");
}

void servo_close_door() {
    printf("Closing door...\n");
    for (int angle = 0; angle <= 40; angle++) {
        set_servo_angle(angle);
        vTaskDelay(pdMS_TO_TICKS(20));
    }
    doorOpened = false;
    printf("Door closed.\n");
}

void buzzer_play(int frequency, int duration_ms)
{
    ledc_set_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0, frequency);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 512); // 50% duty cycle
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);

    vTaskDelay(pdMS_TO_TICKS(duration_ms));  // Play sound for duration

    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0); // Turn off buzzer
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
}