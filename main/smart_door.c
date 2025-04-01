#include "header.h"

#define servo_pin 13
spi_device_handle_t spi;

volatile bool doorOpened = false;  // Cửa ban đầu đóng
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
 
void servo_open_door() {
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
// typedef struct {
//     spi_device_handle_t spi;

// }

void rfid_Task(void  * pvParameters ){
    if(PICC_IsNewCardPresent(spi)){
    PICC_ReadCardSerial(spi);	                   //READ CARD
    PICC_DumpToSerial(spi,&uid);                  //DETAILS OF UID ALONG WITH SECTORS
    if(PICC_Servo_Controll(&uid)){
        printf("GOOD!\n");
        if(!doorOpened) {
            servo_open_door();
            vTaskDelay(pdMS_TO_TICKS(1000));
            servo_close_door();
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }else{
        printf("\nInvalid card scanned.\n");
            
        //printf(uid->uid)
        //vTaskDelay(pdMS_TO_TICKS(1000));
        
        vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }else vTaskDelay(pdMS_TO_TICKS(1000));
}
void add_rfid(void * pvParameters){
    
}
void app_main()
{
    setup_pwm(servo_pin);
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

    	// if(PICC_IsNewCardPresent(spi))                   //Checking for new card
    	// {
    	// 	PICC_ReadCardSerial(spi);	                   //READ CARD
    	// 	PICC_DumpToSerial(spi,&uid);                  //DETAILS OF UID ALONG WITH SECTORS
        //     if(PICC_Servo_Controll(&uid)){
        //         printf("GOOD!\n");
        //         if(!doorOpened) servo_open_door();
        //     }else 
        //         printf("\nInvalid card scanned.\n");
            
        //     //printf(uid->uid)
        //     vTaskDelay(pdMS_TO_TICKS(1000));
        //     servo_close_door();
        //     vTaskDelay(pdMS_TO_TICKS(1000));
    	// 	//vTaskDelay(1000 / portTICK_PERIOD_MS);`

    	// }
    	// else
    	// {
        //     //doorOpened = false;
        //     //xTaskCreate(sensorTask, "Sensor Task", 4096, NULL, 5, NULL);
        //     //(130);
        //     //vTaskDelay(pdMS_TO_TICKS(500));
        //     //sensorTask(doorOpened);
    	// 	vTaskDelay(pdMS_TO_TICKS(500));
    	// }
        // // int currentState = gpio_get_level(GPIO_NUM_12);
        // // if(doorOpened && currentState==0){
        // //         set_servo_angle(80);
        // //         doorOpened=false;
        // //         printf("LOCKED!\n");
        // //     }
        // vTaskDelay(pdMS_TO_TICKS(100));
        vTaskDelay(pdMS_TO_TICKS(100));
     }
}



// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/queue.h"
// #include "esp_system.h"
// #include "driver/spi_master.h"
// #include "driver/gpio.h"
// #include "MFRC522.h"
// #include "servo.h"  // Giả sử có hàm setup_pwm() và có thể dùng set_servo_angle()

// void setup_pwm(uint8_t SERVO_PIN) {
//     // Configure the LEDC timer
//     ledc_timer_config_t ledc_timer = {
//         .speed_mode       = SPEED_MODE,
//         .timer_num        = TIMER_NUM,
//         .duty_resolution  = DUTY_RESOLUTION,
//         .freq_hz          = FREQUENCY,
//         .clk_cfg          = CLK_CONFIG
//     };
//     ledc_timer_config(&ledc_timer);

//     // Configure the LEDC channel
//     ledc_channel_config_t ledc_channel = {
//         .speed_mode     = SPEED_MODE,
//         .channel        = CHANNEL,
//         .timer_sel      = TIMER_NUM,
//         .intr_type      = INTR_TYPE,
//         .gpio_num       = SERVO_PIN,
//         .duty           = 0, // Initial duty cycle
//         .hpoint         = 0
//     };
//     ledc_channel_config(&ledc_channel);
// }

// void set_servo_angle(int angle) 
// {
//     // Convert angle to duty cycle
//     int pulsewidth = SERVO_MIN_PULSEWIDTH + ((SERVO_MAX_PULSEWIDTH - SERVO_MIN_PULSEWIDTH) * angle) / SERVO_MAX_DEGREE;
//     int duty = (pulsewidth * 8192) / 20000; // 8192 is 2^13 for 13-bit resolution
//     ledc_set_duty(SPEED_MODE, CHANNEL, duty);
//     ledc_update_duty(SPEED_MODE, CHANNEL);
// }


// // Khai báo biến toàn cục
// spi_device_handle_t g_spi = NULL;
// volatile bool doorOpened = false;  // Cửa ban đầu đóng

// // Hàm điều khiển servo: thiết lập góc servo (có thể thay thế bằng hàm có sẵn từ servo.h)
// void servo_set_angle(uint8_t angle) {
//     // Ví dụ: nếu sử dụng LEDC hay hàm có sẵn của thư viện servo, gọi hàm đó ở đây.
//     // Ở đây ta chỉ in ra góc để minh họa.
//     printf("Servo angle set to %d degrees.\n", angle);
//     // Nếu có hàm từ servo.h: set_servo_angle(angle);
// }

// // Hàm mở cửa: quay servo từ 80° (đóng) sang 0° (mở)
// void servo_open_door() {
//     printf("Opening door...\n");
//     for (int angle = 80; angle >= 0; angle--) {
//         servo_set_angle(angle);
//         vTaskDelay(pdMS_TO_TICKS(20)); // Delay 20ms giữa các bước (điều chỉnh cho mượt)
//     }
//     doorOpened = true;
//     printf("Door opened.\n");
// }

// // Hàm đóng cửa: quay servo từ 0° (mở) sang 80° (đóng)
// void servo_close_door() {
//     printf("Closing door...\n");
//     for (int angle = 0; angle <= 80; angle++) {
//         servo_set_angle(angle);
//         vTaskDelay(pdMS_TO_TICKS(20));
//     }
//     doorOpened = false;
//     printf("Door closed.\n");
// }


// // Task quét thẻ RFID
// void rfidTask(void *pvParameters) {
//     while (1) {
//         // Kiểm tra xem có thẻ mới không
//         if(PICC_IsNewCardPresent(g_spi)) {
//             printf("*************** CARD PRESENT *****************\r\n");
//             //unsigned long p = getID();
//             //printf(uid)
//             // Đọc UID của thẻ
//             if(PICC_ReadCardSerial(g_spi)) {
//                 printf("UID: ");
//                 for (uint8_t i = 0; i < uid.size; i++) {
//                     printf("%02X ", uid.uidByte[i]);
//                 }
//                 printf("\r\n");
//                 //Kiểm tra UID (định dạng 4 byte: F3 9F D1 0F)
//                 if(
//                    uid.uidByte[0] == 0xF3 &&
//                    uid.uidByte[1] == 0x9F &&
//                    uid.uidByte[2] == 0xD1 &&
//                    uid.uidByte[3] == 0x0F) {
//                     printf("Valid card detected.\n");
//                     // Nếu cửa chưa mở thì mới mở
//                     if(!doorOpened) {
//                         servo_open_door();
//                 } else {
//                     printf("\nInvalid card scanned.\n");
//                 }
//                // printf(p);
//             }
//             // Nghỉ một thời gian trước khi quét lại
//             PICC_HaltA();
//             vTaskDelay(pdMS_TO_TICKS(1000));
// } else {
//             vTaskDelay(pdMS_TO_TICKS(100));
// }
//     }}
// }

// // Task theo dõi cảm biến ở chân số 12
// void sensorTask(void *pvParameters) {
//     // Lấy trạng thái ban đầu của cảm biến
//     int lastState = gpio_get_level(GPIO_NUM_12);
//     while(1) {
//         int currentState = gpio_get_level(GPIO_NUM_12);
//         // Nếu cửa đang mở và có chuyển từ high sang low thì đóng cửa
//         if(doorOpened && (lastState == 1 && currentState == 0)) {
//             printf("Sensor triggered: closing door.\n");
//             servo_close_door();
//         }
//         lastState = currentState;
//         vTaskDelay(pdMS_TO_TICKS(50));
//     }
// }

// void app_main() {
//     esp_err_t ret;
//     spi_device_handle_t spi;
//     spi_bus_config_t buscfg = {
//         .miso_io_num = PIN_NUM_MISO,
//         .mosi_io_num = PIN_NUM_MOSI,
//         .sclk_io_num = PIN_NUM_CLK,
//         .quadwp_io_num = -1,
//         .quadhd_io_num = -1
//     };
//     spi_device_interface_config_t devcfg = {
//         .clock_speed_hz = 5000000,     // 5 MHz
//         .mode = 0,                     // SPI mode 0
//         .spics_io_num = PIN_NUM_CS,    // CS pin
//         .queue_size = 7,
//     };

//     // Khởi tạo bus SPI
//     ret = spi_bus_initialize(HSPI_HOST, &buscfg, 1);
//     assert(ret == ESP_OK);
//     // Gắn thiết bị RFID vào bus SPI
//     ret = spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
//     assert(ret == ESP_OK);
//     // Lưu spi handle toàn cục để dùng ở các task
//     g_spi = spi;

//     // Khởi tạo MFRC522
//     PCD_Init(spi);

//     // Cấu hình chân cảm biến (GPIO12) làm input với pull-up
//     gpio_config_t io_conf = {
//         .intr_type = GPIO_INTR_DISABLE,
//         .mode = GPIO_MODE_INPUT,
//         .pin_bit_mask = (1ULL << 12),
//         .pull_down_en = GPIO_PULLDOWN_DISABLE,
//         .pull_up_en = GPIO_PULLUP_ENABLE,
//     };
//     gpio_config(&io_conf);

//     // Khởi tạo servo: setup PWM cho servo (giả sử hàm setup_pwm được định nghĩa trong servo.h)
//     // Ví dụ: setup_pwm(SERVO_PIN);
//     setup_pwm(servo_pin);
//     // Đưa servo về góc 80° (trạng thái ban đầu: cửa đóng)
//     servo_set_angle(80);
//     doorOpened = false;

//     // Tạo các task sử dụng FreeRTOS
//     xTaskCreate(rfidTask, "RFID Task", 4096, NULL, 4, NULL);
//     xTaskCreate(sensorTask, "Sensor Task", 4096, NULL, 5, NULL);

//     // Vòng lặp chính có thể rỗng vì các task đã đảm nhiệm các chức năng
//     while (1) {
//         vTaskDelay(pdMS_TO_TICKS(1000));
//     }
// }
