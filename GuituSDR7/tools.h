#include "touch_config.h"
#include "InterpolationLib.h"   //for varicup

TouchConfig touch;

// Funkcija za izračun FIR pasovnega filtra (band-pass) druga izboljsan Hamming
void calculateFIRBP(int taps, float f1, float f2) {
    if (Ntaps > MAX_TAPS) Ntaps = MAX_TAPS; // Prepreči preseganje polja
    if (Ntaps % 2 == 0) Ntaps++;  // Zagotovimo liho število tapov
    
    int M = taps - 1;
    float fc1 = f1 / I2S_SAMPLE_RATE;  // Normalizirana spodnja frekvenca
    float fc2 = (f2 / I2S_SAMPLE_RATE) * 0.5f;  // Normalizirana zgornja frekvenca
    float sum = 0.0;   // * 0.5f filter potrebuje mejno frekvenco nizjo, zakaj?
    
    for (int n = 0; n < taps; n++) {
        int k = n - M / 2;

        // Sinc funkcija za pasovni filter
        float h;
        if (k == 0) {
            h = 2.0 * (fc2 - fc1);
        } else {
            h = (sin(2.0 * M_PI * fc2 * k) - sin(2.0 * M_PI * fc1 * k)) / (M_PI * k);
        }

        // Hammingovo okno
        float w = 0.54 - 0.46 * cos(2.0 * M_PI * n / M);

        lpf3000[n] = h * w;
        sum += lpf3000[n];
    }

    // Normalizacija koeficientov
    for (int n = 0; n < taps; n++) {
        lpf3000[n] /= sum;
    }
}

void value_plus(){

  /*if(show_par=="F__L"){indent+=50;if(indent>750)indent=750;f_low.value=indent;show_time=SHOW_VALUE;}
  if(show_par=="F__H"){bandwidth+=50;if(bandwidth>6000)bandwidth=6000;f_high.value=bandwidth;show_time=SHOW_VALUE;}
  if(show_par=="POWR"){rf_power+=1;if(rf_power>20)rf_power=20;tmp_rf_power=rf_power;show_time=SHOW_VALUE;}
  if(show_par=="AUXI"){auxi+=1;if(auxi>15)auxi=15;tmp_auxi=auxi;show_time=SHOW_VALUE;}
  if(show_par=="AUXO"){auxo+=1;if(auxo>15)auxo=15;tmp_auxo=auxo;show_time=SHOW_VALUE;}
  if(show_par=="MICG"){mic_gain+=1;if(mic_gain>20)mic_gain=20;tmp_mic_gain=mic_gain;show_time=SHOW_VALUE;}
  if(show_par=="RF_G"){rf_gain+=1;if(rf_gain>100)rf_gain=100;show_time=SHOW_VALUE;}*/
}

void value_minus(){

  /*if(show_par=="F__L"){indent-=50;if(indent<100)indent=100;f_low.value=indent;show_time=SHOW_VALUE;}
  if(show_par=="F__H"){bandwidth-=50;if(bandwidth<indent+200)bandwidth=indent+200;f_high.value=bandwidth;show_time=SHOW_VALUE;}
  if(show_par=="POWR"){rf_power-=1;if(rf_power<1)rf_power=1;tmp_rf_power=rf_power;show_time=SHOW_VALUE;}
  if(show_par=="AUXI"){auxi-=1;if(auxi<0)auxi=0;tmp_auxi=auxi;show_time=SHOW_VALUE;}
  if(show_par=="AUXO"){auxo-=1;if(auxo<0)auxo=0;tmp_auxo=auxo;show_time=SHOW_VALUE;}
  if(show_par=="MICG"){mic_gain-=1;if(mic_gain<0)mic_gain=0;tmp_mic_gain=mic_gain;show_time=SHOW_VALUE;}
  if(show_par=="RF_G"){rf_gain-=1;if(rf_gain<1)rf_gain=1;show_time=SHOW_VALUE;}*/
}
void tunevaric(){
      x = (int) (rx_freq/1000);
        x = constrain(x, 3000, 13800);    // omejim za delovanje formule meje iz nabora tock
        pwmduty = (int) (Interpolation::Linear(xValues, yValues, numValues, x, true));             
        ledcWrite(PWM_CHANNEL, pwmduty);  //value for new frequency
}

void swr_fill(){
  int swr_step = (bands[numband].f_max - bands[numband].f_min)/160;
  for(int i=0;i<160;i++){
    swr_freq[i]=bands[numband].f_min + swr_step*i;
  }
}

void action() { //screen x do 270 screen hight do 480
  //if (txrx_mode == RX_MODE) { //на экране приема
    /*if (lkey == 1) {tp_band=false;if (numband > N_BANDS - 1)numband = 0;tmp_rf_mode=rf_mode=bands[numband].mode;show_time = SHOW_VALUE;send_cat=true;swr_fill();}
    if (lkey == 2) {if(show_par!="____")value_plus();else{freq += i2s_sample_rate_rx/4;if (freq > 39999999)freq = 39999999;rx_freq=freq;}send_cat=true;}
    if (lkey == 3) {if(show_par!="____")value_minus();else{freq -= i2s_sample_rate_rx/4;if (freq < 1000000)freq = 1000000;rx_freq=freq;}send_cat=true;}
    if (lkey == 4) {}*/
    if (x > 72 && x < 105 && y > 375 && y < 435) {
          freq = freq - step_freq; //RX frekvenca prikazana
          if(freq < 400000){freq = 400000;}
          rx_freq = freq;
          changeFrequency(rx_freq - LO_OFFSET);
          // tukaj dodaj tuning na varicap
          tunevaric();
          } // stepup
    else if (x > 72 && x < 105 && y > 442) {
          freq = freq + step_freq; //RX frekvenca prikazana
          if(freq > 50000000){freq = 50000000;}
          rx_freq = freq;
          changeFrequency(rx_freq - LO_OFFSET);
          // tukaj dodaj tuning na varicap
          tunevaric();
          } // stepup      
    //if (lkey == 6) {rf_mode++;tmp_rf_mode=rf_mode;if(rf_mode>AM)tmp_rf_mode=rf_mode=LSB;send_cat=true;}
    else if (x > 170 && x < 202 && y > 440) {
          rf_mode = LSB;
          agc = true;
          modegain = 2.0f;
          high_fc = 3000.0f;
          if(high_fc>5000.0f)high_fc = 5000.0f;
          calculateFIRBP(Ntaps, low_fc, high_fc);
          indent=0;
          bandwidth=(int)high_fc;
          for(int i=0;i<5;i++){acc_hpf[i]=acc_lpf[i]=0.0f;}
          fir_init(&fir_rx,lpf3000, delay_state_rx, NTAPS_RX);
          } // lsb
    else if (x > 130 && x < 170 && y > 440) {
          rf_mode = USB;
          agc = true;
          modegain = 2.0f;
          high_fc = 3000.0f;
          if(high_fc>5000.0f)high_fc = 5000.0f;
          calculateFIRBP(Ntaps, low_fc, high_fc);
          indent=0;
          bandwidth=(int)high_fc;
          for(int i=0;i<5;i++){acc_hpf[i]=acc_lpf[i]=0.0f;}
          fir_init(&fir_rx,lpf3000, delay_state_rx, NTAPS_RX);
          } // usb
    else if (x > 170 && x < 202 && y > 370 && y < 430) {
          rf_mode = AM;
          agc = true;
          modegain = 1.0f;
          high_fc = 4400.0f;
          if(high_fc>5000.0f)high_fc = 5000.0f;
          calculateFIRBP(Ntaps, low_fc, high_fc);
          indent=0;
          bandwidth=(int)high_fc;
          for(int i=0;i<5;i++){acc_hpf[i]=acc_lpf[i]=0.0f;}
          fir_init(&fir_rx,lpf3000, delay_state_rx, NTAPS_RX);
          } // am
    else if (x > 140 && x < 170 && y > 375 && y < 434) {
          rf_mode = FM;
          agc = false;
          modegain = 0.5f;
          high_fc = 3600.0f;
          if(high_fc>5000.0f)high_fc = 5000.0f;
          calculateFIRBP(Ntaps, low_fc, high_fc);
          indent=0;
          bandwidth=(int)high_fc;
          for(int i=0;i<5;i++){acc_hpf[i]=acc_lpf[i]=0.0f;}
          fir_init(&fir_rx,lpf3000, delay_state_rx, NTAPS_RX);
          } // FM

    /*if (lkey == 7) {if (tun_mode == RIT){tun_mode = TUN;rx_freq=freq;}else tun_mode = RIT;}
    if (lkey == 8) {tmpfreq=freq;swr_scan=true;send_carrier=true;menu=false;rf_power=7;}*/
    else if (x > 205 && x < 235 && y > 438 ) {glasnost += 0.5f; if(glasnost>24.0f){glasnost=24.0f;}}
    else if (x > 240 && y > 438) {glasnost -= 0.5f; if(glasnost<0.0f) {glasnost=0.0f;}}

    else if (x > 205 && x < 235 && y < 430 && y > 372) {ifingain +=0.01f;if(ifingain>1.0f)ifingain=1.0f;}
    else if (x > 240 && y < 430 && y > 372) {ifingain -=0.01f;if(ifingain<0.01f)ifingain=0.01f;}

    else if (x > 42 && x < 66 && y < 412 && y > 335) {
          high_fc = high_fc + 200.0f;
          if(high_fc>5000.0f)high_fc = 5000.0f;
          calculateFIRBP(Ntaps, low_fc, high_fc);
          indent=0;
          bandwidth=(int)high_fc;
          for(int i=0;i<5;i++){acc_hpf[i]=acc_lpf[i]=0.0f;}
          fir_init(&fir_rx,lpf3000, delay_state_rx, NTAPS_RX);
          }
    else if (x > 42 && x < 66 && y < 335 && y > 255) {
          high_fc = high_fc - 200.0f;
          if(high_fc<600.0f)high_fc = 600.0f;
          calculateFIRBP(Ntaps, low_fc, high_fc);
          indent=0;
          bandwidth=(int)high_fc;
          for(int i=0;i<5;i++){acc_hpf[i]=acc_lpf[i]=0.0f;}
          fir_init(&fir_rx,lpf3000, delay_state_rx, NTAPS_RX);
          }
    else if (x > 105 && x < 138 && y < 432 && y > 376) {
          numstep -=1;
          if(numstep < 1)numstep = 1;
          //if(numstep > 7)numstep = 7;
          switch(numstep)
          {case 1:
          step_freq = 100;break;
          case 2:
          step_freq = 1000;break;
          case 3:
          step_freq = 5000;break;
          case 4:
          step_freq = 10000;break;
          case 5:
          step_freq = 25000;break;
          case 6:
          step_freq = 100000;break;
          case 7:
          step_freq = 1000000;break;
          break;}
          }
    else if (x > 105 && x < 138 && y > 438) {
          numstep +=1;
          //if(numstep < 1)numstep = 1;
          if(numstep > 7)numstep = 7;
          switch(numstep)
          {case 1:
          step_freq = 100;break;
          case 2:
          step_freq = 1000;break;
          case 3:
          step_freq = 5000;break;
          case 4:
          step_freq = 10000;break;
          case 5:
          step_freq = 25000;break;
          case 6:
          step_freq = 100000;break;
          case 7:
          step_freq = 1000000;break;
          break;}
          }
    else if (x > 77 && x < 274 && y < 280 && y > 256) {
          pwmduty = map(x,78,273,0,1023);
          ledcWrite(PWM_CHANNEL, pwmduty);  //value to varicap
          }      
    /*if (lkey ==11) {flag_write_parameters = true; show_time = SHOW_VALUE;}//(сохранение текущих парметров)
    if (lkey ==12) {agc_speed++;if(agc_speed>3)agc_speed=0;}
    if (lkey ==13) {show_par = "POWR";show_time=SHOW_VALUE+5;}
    if (lkey ==14) {speaker=!speaker;if(speaker)fout.value=(char*)"Spkr";else fout.value=(char*)"Head";}
    if (lkey ==15) {tuner=!tuner;}
    if (lkey ==16) {mic_c=!mic_c;if(mic_c)fmic.value=(char*)"MicC";else fmic.value=(char*)"Mic-";}
    if (lkey ==17) {bands[numband].att_mode++;if(bands[numband].att_mode>2)bands[numband].att_mode=0;}
    if (lkey ==18) {more_menu=!more_menu;}
    if (lkey ==19) {mic_line=!mic_line;}
    if (lkey ==20) {if(swr_scan)freq=tmpfreq;menu = !menu; swr_scan=false; send_carrier=false;}
    if (lkey ==21) {tuning=true;}
    if (lkey ==22) {tp_band=true;}
    if (lkey ==23) {send_carrier=!send_carrier;}*/
    
    lkey = 0;
  //}

}

void time1() {
  int static ms1;
  if ((cur_ms < ms1) || ((cur_ms - ms1) > 1000 )) { //1sec
    ms1 = cur_ms;
    if (show_time == 1)show_time = 0;
    show_time--;
    if (show_time < 0){show_time = -1;show_par="____";tuning=false;}
    if(show_tap>0){tap_name="_______";show_tap=0;}
    if(tap_name!="_______")show_tap++;
    t_fps = fps;
    //getLocalTime(&timeinfo,0);
    if(!power_button){shutdown_req++;}
    else shutdown_req=0;
    // za po eni sekundi
    timeinfo.tm_sec ++;
    if (timeinfo.tm_sec > 60) {
          timeinfo.tm_sec = 0;
          timeinfo.tm_min ++;
          if (timeinfo.tm_min > 60){timeinfo.tm_min = 0; timeinfo.tm_hour ++;}          
          }
    //Supply voltage measurement
    float rawnap = analogRead(5); // * 0.003994f;
    rawnap = rawnap * 0.007533f - 8.87198f; //skaliranje
    power_g90 = power_g90 * 0.9f + rawnap * 0.1f;      

    gfx->setFont(&FreeMonoBold8pt8b);
  gfx->setTextColor(WHITE);
  gfx->fillRoundRect(f_low.x_min,f_low.y_min,f_low.w,f_low.h,3,(show_par=="F__L")?0b0110000000001100:0b0000010000000000);
  gfx->setCursor(f_low.x_min+5,f_low.y_min+18);
  gfx->print("St-");

  //gfx->setFont(&FreeMonoBold8pt8b);
  //gfx->setTextColor(WHITE);
  gfx->fillRoundRect(f_high.x_min,f_high.y_min,f_high.w,f_high.h,3,(show_par=="F__H")?0b0110000000001100:0b0000010000000000);
  gfx->setCursor(f_high.x_min+5,f_high.y_min+18);
  gfx->print("St+");

  //gfx->setFont(&FreeMonoBold8pt8b);
  //gfx->setTextColor(WHITE);
  gfx->fillRoundRect(rfmode.x_min,rfmode.y_min,rfmode.w,rfmode.h,3,(rfmode.b_name==tap_name)?0b0110000000001100:0b0000010000000000);
  gfx->setCursor(rfmode.x_min+5,rfmode.y_min+18);
  gfx->print(rfmode.value);

  //gfx->setFont(&FreeMonoBold8pt8b);
  //gfx->setTextColor(WHITE);
  gfx->fillRoundRect(fstep.x_min,fstep.y_min,fstep.w,fstep.h,3,(fstep.b_name==tap_name)?0b0110000000001100:0b0000010000000000);
  gfx->setCursor(fstep.x_min+5,fstep.y_min+18);
  gfx->print(fstep.value);

  //gfx->setFont(&FreeMonoBold8pt8b);
  //gfx->setTextColor(WHITE);
  gfx->fillRoundRect(fagc.x_min,fagc.y_min,fagc.w,fagc.h,3,(fagc.b_name==tap_name)?0b0110000000001100:0b0000010000000000);
  gfx->setCursor(fagc.x_min+5,fagc.y_min+18);
  gfx->print(fagc.value);

  //gfx->setFont(&FreeMonoBold8pt8b);
  //gfx->setTextColor(WHITE);
  gfx->fillRoundRect(fpower.x_min,fpower.y_min,fpower.w,fpower.h,3,(show_par=="POWR")?0b0110000000001100:0b0000010000000000);
  gfx->setCursor(fpower.x_min+5,fpower.y_min+18);
  gfx->print(fpower.value);

  //gfx->setFont(&FreeMonoBold8pt8b);
  //gfx->setTextColor(WHITE);
  gfx->fillRoundRect(fout.x_min,fout.y_min,fout.w,fout.h,3,(fout.b_name==tap_name)?0b0110000000001100:0b0000010000000000);
  gfx->setCursor(fout.x_min+5,fout.y_min+18);
  gfx->print(fout.value);

  //gfx->setFont(&FreeMonoBold8pt8b);
  //gfx->setTextColor(WHITE);
  gfx->fillRoundRect(ftun.x_min,ftun.y_min,ftun.w,ftun.h,3,(ftun.b_name==tap_name)?0b0110000000001100:0b0000010000000000);
  gfx->setCursor(ftun.x_min+5,ftun.y_min+18);
  gfx->print(ftun.value);

  //gfx->setFont(&FreeMonoBold8pt8b);
  //gfx->setTextColor(WHITE);
  gfx->fillRoundRect(fmic.x_min,fmic.y_min,fmic.w,fmic.h,3,(fmic.b_name==tap_name)?0b0110000000001100:0b0100001000000000);
  gfx->setCursor(fmic.x_min+5,fmic.y_min+18);
  gfx->print(fmic.value);

  //gfx->setFont(&FreeMonoBold8pt8b);
  //gfx->setTextColor(WHITE);
  gfx->fillRoundRect(fatt.x_min,fatt.y_min,fatt.w,fatt.h,3,(fatt.b_name==tap_name)?0b0111100000000000:0b0100000000000000);
  gfx->setCursor(fatt.x_min+5,fatt.y_min+18);
  gfx->print(fatt.value);

  //gfx->setFont(&FreeMonoBold8pt8b);
  //gfx->setTextColor(WHITE);
  gfx->fillRoundRect(fmore.x_min,fmore.y_min,fmore.w,fmore.h,3,(fmore.b_name==tap_name)?0b0110000000001100:0b0100001000000000);
  gfx->setCursor(fmore.x_min+5,fmore.y_min+18);
  gfx->print(fmore.value);

  //gfx->setFont(&FreeMonoBold8pt8b);
  //gfx->setTextColor(WHITE);
  gfx->fillRoundRect(ftune.x_min,ftune.y_min,ftune.w,ftune.h,3,(ftune.b_name==tap_name)?0b0111100000000000:0b0100000000000000);
  gfx->setCursor(ftune.x_min+5,ftune.y_min+18);
  gfx->print(ftune.value);

  }
}

void time_001() {
  int static ms001 = 0;
  //int static swr_count = 0;
  //static bool swr_switch=false;
  if ((cur_ms < ms001) || ((cur_ms - ms001) > 40 )) {
    ms001 = cur_ms;
    /*if(txrx_mode==RX_MODE){smeter = map(256-smeter_g90,127,13,0,98);}
    else{float tx_smeter = ((float)smeter_g90)/10.0;smeter = map(tx_smeter,0.0,(float)rf_power,0,100);}
    if (smeter <= old_smeter && old_smeter - smeter >= 1) {old_smeter--;}
    if (smeter > old_smeter && smeter - old_smeter > 10)  {old_smeter += 10;}
    if (smeter > old_smeter && smeter - old_smeter > 5)   {old_smeter += 5;}
    if (smeter > old_smeter && smeter - old_smeter > 2)   {old_smeter += 2;}
    if (smeter > old_smeter && smeter - old_smeter >= 1)  {old_smeter++;}
    if (old_smeter < 0)old_smeter = 0;
    if (old_smeter > 150)old_smeter = 150;*/

    old_smeter--;//dekrement s-metra
       if(old_smeter<=1)old_smeter=1;
       if(rf_mode == FM) {agc_koeff=5.0f;}
       else{agc_koeff = (160/old_smeter);if(agc_koeff>30)agc_koeff=30;} // orig 120 in do 15, AGC kontrola se izvaja tukaj

    peak_down();//pomanjsanje indikatorjev spektra in panorame
    /*if(!power_button){show_par="____";}
    ////сканируем диапазон SWR
    if(!swr_switch && swr_scan)freq=swr_freq[swr_count];
    if(swr_switch && swr_scan){
      swr_value[swr_count]=from_body[2]+1;
      swr_count++;if(swr_count>160)swr_count=0;
    }*/
    //swr_switch=!swr_switch;
    }
  }

void time_050() {
  int static ms050 = 0;
  if ((cur_ms < ms050) || ((cur_ms - ms050) > 50 )) {
    ms050 = cur_ms;   
    scroll_wp();//сдвинуть панораму на 1 строку вниз    
      }
    }

void time_200() {
  int static ms200 = 0;
  if ((cur_ms < ms200) || ((cur_ms - ms200) > 150 )) {
    ms200 = cur_ms;
    //touched here
    if (touch.isTouched()) {
    //int x, y, z;
    touch.getScaledPoint(x, y, z);
    action();
      }
    }
  }

void change_band() {
  static int old_band = 100;
  static bool first = false; //true;  //postavi true da vpises prvic
  if (old_band == numband)return;
  //if (!first)freq = bands[numband].freq;
  freq = bands[numband].freq;
  old_band = numband; first = false;
}

#define BTN_SCAN (tp_x>fscan.x_min&&tp_x<fscan.x_max&&tp_y>fscan.y_min&&tp_y<fscan.y_max)
#define BTN_RFG (tp_x>frf.x_min&&tp_x<frf.x_max&&tp_y>frf.y_min&&tp_y<frf.y_max)
#define BTN_PTT (tp_x>fptt.x_min&&tp_x<fptt.x_max&&tp_y>fptt.y_min&&tp_y<fptt.y_max)
#define BTN_TUNE (tp_x>ftune.x_min&&tp_x<ftune.x_max&&tp_y>ftune.y_min&&tp_y<ftune.y_max)
#define BTN_FLM (tp_x>flm.x_min&&tp_x<flm.x_max&&tp_y>flm.y_min&&tp_y<flm.y_max)
#define BTN_MICG (tp_x>fmicg.x_min&&tp_x<fmicg.x_max&&tp_y>fmicg.y_min&&tp_y<fmicg.y_max)
#define BTN_AUXI (tp_x>fauxi.x_min&&tp_x<fauxi.x_max&&tp_y>fauxi.y_min&&tp_y<fauxi.y_max)
#define BTN_AUXO (tp_x>fauxo.x_min&&tp_x<fauxo.x_max&&tp_y>fauxo.y_min&&tp_y<fauxo.y_max)
#define BTN_MIC (tp_x>fmic.x_min&&tp_x<fmic.x_max&&tp_y>fmic.y_min&&tp_y<fmic.y_max)
#define BTN_TUN (tp_x>ftun.x_min&&tp_x<ftun.x_max&&tp_y>ftun.y_min&&tp_y<ftun.y_max)
#define BTN_OUT (tp_x>fout.x_min&&tp_x<fout.x_max&&tp_y>fout.y_min&&tp_y<fout.y_max)
#define BTN_RFPOWER (tp_x>fpower.x_min&&tp_x<fpower.x_max&&tp_y>fpower.y_min&&tp_y<fpower.y_max)
#define BTN_AGC (tp_x>fagc.x_min&&tp_x<fagc.x_max&&tp_y>fagc.y_min&&tp_y<fagc.y_max)
#define BTN_ATT (tp_x>fatt.x_min&&tp_x<fatt.x_max&&tp_y>fatt.y_min&&tp_y<fatt.y_max)
#define BTN_MORE (tp_x>fmore.x_min&&tp_x<fmore.x_max&&tp_y>fmore.y_min&&tp_y<fmore.y_max)
#define BTN_STEP (tp_x>fstep.x_min&&tp_x<fstep.x_max&&tp_y>fstep.y_min&&tp_y<fstep.y_max)
#define BTN_RMODE (tp_x>rfmode.x_min&&tp_x<rfmode.x_max&&tp_y>rfmode.y_min&&tp_y<rfmode.y_max)
#define BTN_F_LOW (tp_x>f_low.x_min&&tp_x<f_low.x_max&&tp_y>f_low.y_min&&tp_y<f_low.y_max)
#define BTN_F_HIGH (tp_x>f_high.x_min&&tp_x<f_high.x_max&&tp_y>f_high.y_min&&tp_y<f_high.y_max)
#define BTN_CONTROL (tp_x>400&&tp_x<472&&tp_y>10&&tp_y<70&&tp_size>30)
#define BTN_RIT   (tp_x>tp_rit.x_min&&tp_x<tp_rit.x_max&&tp_y>tp_rit.y_min&&tp_y<tp_rit.y_max)
#define BTN_BAND   (tp_x>tp_nband.x_min&&tp_x<tp_nband.x_max&&tp_y>tp_nband.y_min&&tp_y<tp_nband.y_max)
#define BTN_SBAND  (tp_x>10&&tp_x<470&&tp_y>70&&tp_y<150&&tp_band)
#define BTN_SAVE_CONF  (tp_x>30&&tp_x<220&&tp_y>20&&tp_y<65)
#define BTN_FREQ_PLUS  (tp_x>160&&tp_x<320&&tp_y>80&&tp_y<150&&!tp_band)
#define BTN_FREQ_PLUS_FULL  (tp_x>280&&tp_x<470&&tp_y>80&&tp_y<150&&!tp_band)
#define BTN_FREQ_MINUS (tp_x>25&&tp_x<160&&tp_y>70&&tp_y<150&&!tp_band)
#define BTN_FREQ_MINUS_FULL (tp_x>25&&tp_x<220&&tp_y>70&&tp_y<150&&!tp_band)
#define BTN_VOL_PLUS   (tp_x>160&&tp_x<320&&tp_y>200&&tp_y<270&&!tp_band)
#define BTN_VOL_PLUS_FULL   (tp_x>270&&tp_x<470&&tp_y>200&&tp_y<270&&!tp_band)
#define BTN_VOL_MINUS  (tp_x>25&&tp_x<160&&tp_y>200&&tp_y<270&&!tp_band)
#define BTN_VOL_MINUS_FULL  (tp_x>25&&tp_x<220&&tp_y>200&&tp_y<270&&!tp_band)

void tp_init(){
  //tp.begin();
  touch.begin();
  //tp.setRotation(ROTATION_INVERTED); //ze v beginu
}

void t_touched(){ //опрос тачскрина
    static bool start = true;
    static int l_key=0;
    static int b_key=0;
    /*if(start){t_press=true;l_key=1;start=false;}
    tp.read();
    if(tp.isTouched){
      tp_x=tp.points[0].x;tp_y=tp.points[0].y;tp_size=tp.points[0].size;
      if(!more_menu && !menu && !swr_scan){
        if(BTN_F_LOW)       {t_press=true;show_par = "F__L";show_time=SHOW_VALUE+5;}
        if(BTN_F_HIGH)      {t_press=true;show_par = "F__H";show_time=SHOW_VALUE+5;}      
        if(BTN_RFPOWER)     {t_press=true;show_par = "POWR";show_time=SHOW_VALUE+5;}
        if(BTN_RMODE)       {l_key=6;t_press=true;tap_name="RF_MODE";}      
        if(BTN_STEP)        {l_key=5;t_press=true;tap_name="STEP";}
        if(BTN_AGC)         {l_key=12;t_press=true;tap_name="AGC";}
        if(BTN_OUT)         {l_key=14;t_press=true;tap_name="OUT";}
        if(BTN_TUN)         {l_key=15;t_press=true;tap_name="TUN";}
        if(BTN_MIC)         {l_key=16;t_press=true;tap_name="MIC";}
        if(BTN_MORE)        {l_key=18;t_press=true;tap_name="MORE";}
        if(BTN_ATT)         {l_key=17;t_press=true;tap_name="ATT";}
        if(BTN_TUNE)        {l_key=21;t_press=true;tap_name="TUNE";show_par = "TUNE";show_time=SHOW_VALUE;}
        
      }
      if(more_menu && !menu && !swr_scan){
        if(BTN_AUXI)       {t_press=true;show_par = "AUXI";show_time=SHOW_VALUE+5;}
        if(BTN_AUXO)       {t_press=true;show_par = "AUXO";show_time=SHOW_VALUE+5;}
        if(BTN_MICG)       {t_press=true;show_par = "MICG";show_time=SHOW_VALUE+5;}
        if(BTN_FLM)        {l_key=19;t_press=true;show_par = "L__M";show_time=SHOW_VALUE;}
        if(BTN_MORE)       {l_key=18;t_press=true;tap_name="MORE";}
        if(BTN_PTT)        {l_key=23;t_press=true;tap_name="CPTT";}
        if(BTN_RFG)        {t_press=true;show_par = "RF_G";show_time=SHOW_VALUE+5;}
        if(BTN_SCAN)       {l_key=8;t_press=true;tap_name="Scan";}
      }
      if(BTN_RIT && !swr_scan)         {l_key=7;t_press=true;}//режим отстройки
      if(BTN_FREQ_PLUS && !menu && !swr_scan)   {l_key=2;t_press=true;}//+ SR/4 kHz
      if(BTN_FREQ_PLUS_FULL && menu && !swr_scan)   {l_key=2;t_press=true;}//+ SR/4 kHz
      if(BTN_FREQ_MINUS && !menu && !swr_scan)  {l_key=3;t_press=true;}//- SR/4 kHz
      if(BTN_FREQ_MINUS_FULL && menu && !swr_scan)  {l_key=3;t_press=true;}//- SR/4 kHz
      if(BTN_VOL_PLUS && !menu && !swr_scan)    {l_key=9;t_press=true;show_par="VOL_";}//+
      if(BTN_VOL_PLUS_FULL && menu && !swr_scan)    {l_key=9;t_press=true;show_par="VOL_";}//+
      if(BTN_VOL_MINUS && !menu && !swr_scan)   {l_key=10;t_press=true;show_par="VOL_";}//-
      if(BTN_VOL_MINUS_FULL && menu && !swr_scan)   {l_key=10;t_press=true;show_par="VOL_";}//-
      if(BTN_SAVE_CONF && !swr_scan)   {l_key=11;t_press=true;show_par = "PAR_";}//сохранение текущих параметров

      if(BTN_SBAND){ //выбор диапазона
        int x=20;int y=80;
        for(int i=0;i<N_BANDS;i++){
          if(tp_x>x&&tp_x<x+50&&tp_y>y&&tp_y<y+20){numband=i; l_key=1;t_press=true;show_par = "BAND"; break;}
          x+=60;if(x>300){x=20;y+=30;}
        }
      }
      if(BTN_CONTROL){l_key=20;t_press=true;}
      if(BTN_BAND && !swr_scan){l_key=22;t_press=true;}
    }
    else{
      if(t_press&&l_key!=0){lkey=l_key;l_key=0;t_press=false;t_release=true;}
    }*/
}

void check_ptt(){
    txrx_mode = RX_MODE;
    /*if(send_carrier)ptt=1;else ptt=!digitalRead(PTT)|cat_ptt;
    if (ptt){
      txrx_mode = TX_MODE;
    }
    if (!ptt){
      txrx_mode = RX_MODE;
    }*/
}

void select_bandwidth(){
  static int old_mode;
  if(old_mode==rf_mode)return;
  if(rf_mode==LSB||rf_mode==USB){f_low.value= 100;f_high.value=bandwidth=3000;}
  //if(rf_mode==L_D||rf_mode==U_D){f_low.value=indent = 100;f_high.value=bandwidth=3000;}
  //if(rf_mode==CW||rf_mode==CWR){f_low.value=indent = 500;f_high.value=bandwidth=1000;}
  if(rf_mode==AM){f_low.value = 100;f_high.value=bandwidth=4400;}
  if(rf_mode==FM){f_low.value = 100;f_high.value=bandwidth=3600;}
  old_mode=rf_mode;
}

void control() {
  //check_ptt();
  change_band();
  t_touched();//опрос тачскина
  //if (t_release) {t_release= false; action();}//если отпущена кнопка на экране вызов обработчика
  write_parameters(false);
  if(tun_mode == TUN) rx_freq=freq;
  select_bandwidth();
  //power_button=digitalRead(POWER_PIN);
}

void readConfig() {
  /*številka parametra
 0 najprej preizkusite
 1 vrednost ADC za gumb št. 1
 2 Vrednost ADC za gumb št. 2
 3 Vrednost ADC za gumb št. 3
 4 Vrednost ADC za gumb št. 4
 5 Vrednost ADC za gumb št. 5
 6 Vrednost ADC za gumb št. 6
 7 Vrednost ADC za gumb št. 7
 8 številka pasu
 9 nastavitev frekvence
 10. korak prestrukturiranja
 11 rf_moč
 12 agc (false=0,slow=1,fast=2,auto=3)
 13 mic_gain 0..20
 14 nizek filter
 15 višina filtra
 16 rf_ojačanje
     
  */
  int i;
  for (i = 0; i < 8; i++) {
    value_button[i] = EEPROM.readUInt(i * sizeof(uint32_t));
    if (value_button[i] > 4095)value_button[i] = 9999;
  }
  //numband = EEPROM.readUInt(i * sizeof(uint32_t));    if (numband > N_BANDS - 1 || numband < 0)numband = 0; i++;
  //freq = EEPROM.readUInt(i * sizeof(uint32_t));       if (freq > 30000000 || freq < 100000)freq = bands[numband].freq; i++;
  //numstep = EEPROM.readUInt(i * sizeof(uint32_t));    if (numstep > 5 || numstep < 0)numstep = 3; i++;
  rf_power= (uint8_t)(EEPROM.readUInt(i * sizeof(uint32_t)));    if (rf_power > 20 || rf_power < 1)rf_power = 1; i++;
  agc_speed = (uint8_t)(EEPROM.readUInt(i * sizeof(uint32_t)));    if (agc_speed > 3  || agc_speed <= 0)agc_speed = 1; i++;
  mic_gain= (uint8_t)(EEPROM.readUInt(i * sizeof(uint32_t)));    if (mic_gain > 20  || mic_gain <= 0)mic_gain = 10; i++;
  //indent = (EEPROM.readUInt(i * sizeof(uint32_t)));    if (indent > 750  || indent < 100)indent = 200; i++;
  bandwidth = (EEPROM.readUInt(i * sizeof(uint32_t)));    if (bandwidth > 6000  || bandwidth <= indent+100)bandwidth = 3000; i++;
  rf_gain = (uint8_t)(EEPROM.readUInt(i * sizeof(uint32_t))); if (rf_gain > 100  || rf_gain < 1) rf_gain=50; i++;
  f_low.value=100; //indent;
  f_high.value=bandwidth;
  tmp_rf_power=rf_power;
  tmp_mic_gain = mic_gain;
  //tmp_rf_mode=rf_mode=bands[numband].mode;
  tmp_rf_mode=bands[numband].mode;
} 


void writeConfig(uint32_t numpar, uint32_t value) {
  if(numpar==0)return;
  value_button[numpar] = value;
  EEPROM.writeUInt(numpar * sizeof(uint32_t), value);
  EEPROM.commit();
}

void write_parameters(bool first) {
  /*številka parametra
 * Najprej 0 testov
 1..7 vrednosti ADC za gumbe
 8 številka pasu
 9 nastavitev frekvence
 10. korak prestrukturiranja
 11 rf_moč 0..20
 12 agc 0..3
 13 mic_gain 0..20
 14 nizek filter
 15 višina filtra
 16 rf_ojačanje
  */
  if(first){
    flag_write_parameters = true;
    numband = 8;
    freq=14200000;
    numstep=2;
    rf_power=1;
    agc_speed=1;
    mic_gain=1;
    indent= 0; //200;
    bandwidth=3000;
    f_low.value=100; //indent;
    f_high.value=bandwidth;
    EEPROM.writeUInt(0 * sizeof(uint32_t), (uint32_t)0x5aa5);
    EEPROM.commit();
  }
  if (flag_write_parameters) {
    EEPROM.writeUInt(8  * sizeof(uint32_t), (uint32_t)numband);
    EEPROM.writeUInt(9  * sizeof(uint32_t), (uint32_t)freq);
    EEPROM.writeUInt(10 * sizeof(uint32_t), (uint32_t)numstep);
    EEPROM.writeUInt(11 * sizeof(uint32_t), (uint32_t)rf_power);
    EEPROM.writeUInt(12 * sizeof(uint32_t), (uint32_t)agc_speed);
    EEPROM.writeUInt(13 * sizeof(uint32_t), (uint32_t)mic_gain);
    //EEPROM.writeUInt(14 * sizeof(uint32_t), (uint32_t)indent);
    EEPROM.writeUInt(15 * sizeof(uint32_t), (uint32_t)bandwidth);
    EEPROM.writeUInt(16 * sizeof(uint32_t), (uint32_t)rf_gain);
    EEPROM.commit();
    flag_write_parameters = false;
  }
}

/*void x_encoder(void *args)
{
  while(1){
    int encoderDelta;
    //pcnt_unit_get_count(pcnt_unit, &encoderDelta);
    //pcnt_unit_clear_count(pcnt_unit);
    if (txrx_mode == RX_MODE) { //реакция на поворот энкодера на 0 экране
     if(show_par=="VOL_"){
      if (encoderDelta > 0){volume+=1;if(volume>28)volume=28;show_time=SHOW_VALUE;}
      if (encoderDelta < 0){volume-=1;if(volume>28 )volume=0;show_time=SHOW_VALUE;}
      }
     //if(show_par=="F__L"){
      //if (encoderDelta > 0){indent+=50;if(indent>750)indent=750;f_low.value=indent;show_time=SHOW_VALUE;}
      //if (encoderDelta < 0){indent-=50;if(indent <100 )indent=100;f_low.value=indent;show_time=SHOW_VALUE;}
     //}
     if(show_par=="F__H"){
      if (encoderDelta > 0){bandwidth+=50;if(bandwidth>6000)bandwidth=6000;f_high.value=bandwidth;show_time=SHOW_VALUE;}
      if (encoderDelta < 0){bandwidth-=50;if(bandwidth<indent+200)bandwidth=indent+200;f_high.value=bandwidth;show_time=SHOW_VALUE;}
     }
     if(show_par=="POWR"){
      if (encoderDelta > 0){rf_power+=1;if(rf_power>20)rf_power=20;tmp_rf_power=rf_power;show_time=SHOW_VALUE;}
      if (encoderDelta < 0){rf_power-=1;if(rf_power<1)rf_power=1;tmp_rf_power=rf_power;show_time=SHOW_VALUE;}
     }
     if(show_par=="AUXI"){
      if (encoderDelta > 0){auxi+=1;if(auxi>15)auxi=15;tmp_auxi=auxi;show_time=SHOW_VALUE;}
      if (encoderDelta < 0){auxi-=1;if(auxi<0)auxi=0;tmp_auxi=auxi;show_time=SHOW_VALUE;}
     }
     if(show_par=="AUXO"){
      if (encoderDelta > 0){auxo+=1;if(auxo>15)auxo=15;tmp_auxo=tmp_auxo;show_time=SHOW_VALUE;}
      if (encoderDelta < 0){auxo-=1;if(auxo<0)auxo=0;tmp_auxo=tmp_auxo;show_time=SHOW_VALUE;}
     }
     if(show_par=="MICG"){
      if (encoderDelta > 0){mic_gain+=1;if(mic_gain>20)mic_gain=20;tmp_mic_gain=mic_gain;show_time=SHOW_VALUE;}
      if (encoderDelta < 0){mic_gain-=1;if(mic_gain<0)mic_gain=0;tmp_mic_gain=mic_gain;show_time=SHOW_VALUE;}
     }
     if(show_par=="RF_G"){
      if (encoderDelta > 0){rf_gain+=1;if(rf_gain>100)rf_gain=100;show_time=SHOW_VALUE;}
      if (encoderDelta < 0){rf_gain-=1;if(rf_gain<1)rf_gain=1;show_time=SHOW_VALUE;}
     }
     //if(show_par=="____"){
      //if ((encoderDelta > 0) && (tun_mode == TUN)){freq = freq + step_freq[numstep];if (freq > 39999999)freq = 39999999;bands[numband].freq=freq;rx_freq = freq;}
      //if ((encoderDelta < 0) && (tun_mode == TUN)){freq = freq - step_freq[numstep];if (freq < 100000)freq = 100000;bands[numband].freq=freq;rx_freq = freq;}
      //if ((encoderDelta > 0) && (tun_mode == RIT)){rx_freq = rx_freq + step_freq[numstep];if (rx_freq > 39999999)rx_freq = 39999999;bands[numband].freq=rx_freq;}
      //if ((encoderDelta < 0) && (tun_mode == RIT)){rx_freq = rx_freq - step_freq[numstep];if (rx_freq < 100000)freq = 100000;bands[numband].freq=rx_freq;}
      //}
    }
    if (txrx_mode == SETUP_MODE) { //реакция на поворот энкодера на 2 экране
        if (encoderDelta > 0) {n_button++;if (n_button > 8)n_button = 8;redraw = true;}
        if (encoderDelta < 0) {n_button--;if (n_button < 1)n_button = 1;redraw = true;}
     }
     vTaskDelay(1);
    }
}*/
