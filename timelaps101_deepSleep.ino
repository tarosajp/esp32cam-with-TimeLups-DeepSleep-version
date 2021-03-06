#include "esp_camera.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "SD.h"

#include "FS.h"

//SD Card Pins
#define sd_sck  14
#define sd_mosi 15
#define sd_ss   13
#define sd_miso  2

// read and write from flash memory
#include <EEPROM.h>

// define the number of bytes you want to access
#define EEPROM_SIZE 1

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

RTC_DATA_ATTR int myF_No = 0;
RTC_DATA_ATTR int myS_No = 0;

char myFileName_str1[4];
char myFileName_str2[4];

void setup()
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  Serial.begin(115200);
  delay(10);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  config.frame_size = FRAMESIZE_VGA;  // UXGA|SXGA|XGA|SVGA|VGA|CIF|QVGA|HQVGA|QQVGA
  config.jpeg_quality = 10;
  config.fb_count = 2;
  
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    delay(1000);
    ESP.restart();
      }

    
// LED Pin Mode
    pinMode(33, OUTPUT);
     
      digitalWrite(33,LOW);
      delay (500);
      
  camera_fb_t * fb = NULL;
  
  // Take Picture with Camera
    Serial.println(" ");
    Serial.println("Take Picture ");
    fb = esp_camera_fb_get();  
  
  // Path where new picture will be saved in SD Card
    Serial.println("Save SD Card");

  // Get File Name
       
  if (myS_No == 0 || myS_No == 999){

     // initialize EEPROM with predefined size

      EEPROM.begin(EEPROM_SIZE);
      myF_No = EEPROM.read(0) + 1;
      EEPROM.write(0, myF_No);
      EEPROM.commit();

      myS_No = 1;
      
    }else{
      
     myS_No = myS_No + 1;
    
    }

    sprintf(myFileName_str1, "%03d",myF_No);
    sprintf(myFileName_str2, "%03d",myS_No);

    String path = "/p" + String(myFileName_str1)+"_"+ String(myFileName_str2) +".jpg";
    Serial.printf("Picture file name: %s\n", path.c_str());

    fs::FS &fs = SD;

    SPI.begin(sd_sck, sd_miso, sd_mosi, sd_ss);
    SD.begin(sd_ss);
  
    File file = fs.open(path.c_str(), FILE_WRITE);
    file.write(fb->buf, fb->len); 

    file.close();
   
    esp_camera_fb_return(fb); 

    Serial.println("complete");

      digitalWrite(33,HIGH);

 // 1000000u????????????1???
  esp_sleep_enable_timer_wakeup(30 *1000000);

      Serial.println("Deep Sleep Start.");
      Serial.println(" ");

  esp_deep_sleep_start();


}

void loop() {
}
