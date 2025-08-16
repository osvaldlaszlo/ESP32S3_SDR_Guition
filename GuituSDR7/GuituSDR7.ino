//Ver01  umaknjen kapacitivni touch pad dela samo grafika
//ver02  dodam R touch
//ver03  dodam s5351
//ver04  umaknem encoder,ppt check in UART rutine
//ver04a zamenjam select color v slap rutini in pohitrim slap, potem nazaj na orig select color first na false
//ver05  dodam AGC
//ver06  premaknem grid, 6a LO povecam na 4x in samo CLK0
//ver06b  poiskusim z manj fft elementi za ozjo AM demodulacijo
//ver06c popravim max frekvenco bila je le do cca 13Mhz
//ver06d dodam tuning with varicap 5kHz on pin 15
//ver07  optimized varicap table and BAT measurement
//#include <WiFi.h>
#include "time.h"
//#include "driver/pulse_cnt.h"
#include "driver/pcnt.h"
#include <driver/i2s.h>
#include <EEPROM.h>
#include <si5351.h>
#include <Wire.h>
//#include "src/tamc_gt911/TAMC_GT911.h" //https://github.com/TAMCTec/gt911-arduino
#include <Arduino_GFX_Library.h> //https://github.com/moononournation/Arduino_GFX
#include "include/FreeMonoBold8.h"
#include "src/dsp_lib/xtensa_math.h" //taken from https://github.com/whyengineer/esp32-lin/tree/master/components/dsp_lib
//#include "include/s7.h"          //7-segments font
#include "include/Picopixel.h"
#include "include/CourierCyr6.h"
#include "include/FreeMonoBold14.h"
#include "_tmp.h"
#include "global.h"
#include "g90.h"
#include "ci_v.h"
#include "fft.h"
#include "filters.h"
#include "init.h"
#include "include/s7.h"          //7-segments font
#include "include/images.h"
#include "screens.h"
#include "txrx.h"
#include "rx.h"
#include "tools.h"
#include "losi5351.h"
#include "InterpolationLib.h"   //for varicup


void setup()
{ // Sets up a channel (0-15), a PWM duty cycle frequency, and a PWM resolution (1 - 16 bits) 
  // ledcSetup(uint8_t channel, double freq, uint8_t resolution_bits);
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  // ledcAttachPin(uint8_t pin, uint8_t channel);
  ledcAttachPin(PWM_OUTPUT_PIN, PWM_CHANNEL);
  //ledcWrite(PWM_CHANNEL, 432);  //value for 7Mhz
  x = (int) (7123000/1000);
      x = constrain(x, 3000, 13800);    // omejim za delovanje formule
      pwmduty = (int) (Interpolation::Linear(xValues, yValues, numValues, x, true));             
      ledcWrite(PWM_CHANNEL, pwmduty);  //value for 7Mhz 
  get_conf();//восстанавливаем параметры
  //init_uart(); //2 канала uart для приема и передачи в/из g90
  Wire.setPins(46,9);   //določim nove pine za DAT,CLK za si5351
  lcd_init(1);
  //encoder_init();  //zmanjsana vsebina!
  tp_init();
  //touch.begin();
  fft_init();
  setupSynth();  //init for si5351
  start_ok(); //заставка, вкл wifi, получение времени,выкл wifi
  changeFrequency(rx_freq - LO_OFFSET);
  txrx_mode=RX_MODE;
  //vSemaphoreCreateBinary(xIN);
  //vSemaphoreCreateBinary(xUART);
  vSemaphoreCreateBinary(xRXDSP);
  vSemaphoreCreateBinary(xRXIN);
  vSemaphoreCreateBinary(xRXOUT);
  //vSemaphoreCreateBinary(xTXDSP);
  //vSemaphoreCreateBinary(xTXIN);
  //vSemaphoreCreateBinary(xTXOUT);

  i2sinit();//I2S0
  //xTaskCreatePinnedToCore(x_in,"rxin",STACK_SIZE*2,NULL,14,NULL,0);
  xTaskCreatePinnedToCore(rx_in,  "rxin",  2048, NULL, 100, NULL, 1);//
  delay(100);
  xTaskCreatePinnedToCore(rx_out, "rxout", 2048, NULL, 110, NULL, 1);//
  delay(100);
  xTaskCreatePinnedToCore(rx_dsp, "rxdsp", 2048, NULL, 220, NULL, 0);// 
  delay(100);
  /*xTaskCreatePinnedToCore(x_uart_rx,"uart_rx",STACK_SIZE*2,NULL,12,NULL,1);
  delay(100);
  xTaskCreatePinnedToCore(x_uart_tx,"uart_tx",STACK_SIZE*2,NULL,11,NULL,1);
  delay(100);
  xTaskCreatePinnedToCore(x_encoder,"xencoder",STACK_SIZE/2, NULL, 13, NULL, 1);
  delay(100);
  xTaskCreatePinnedToCore(x_cat,"cat",STACK_SIZE, NULL, 14, NULL, 1);
  delay(100);*/
  //xSemaphoreGive(xUART);
}

void loop(void){
    cur_ms=millis();
    screens(txrx_mode);
    gfx->flush();
    time_001();
    time_050();
    time_200();
    time1();
    control();
    fps=millis()-cur_ms;
}
