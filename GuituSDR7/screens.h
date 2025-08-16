//draw_Grid(0+xwin,72+ywin,255,103); //480,103);//nariše mrezo
void draw_Grid(int px, int py, int w, int h )
{
    // рисование сетки на спектре
    //int div =  (int)(h / 5);
    int div = 21;
    //for (int x = 0; x < w + div; x += div)if((px+x)<xwin+479){
    for (int x = 19; x < w; x += div){
      gfx->drawFastVLine(px+x, py, h, GRAY);  //vertikale
    }
    //gfx->drawFastVLine(xwin+479, py, h, GRAY);
    for (int y = 0; y < h + div; y += div){
        if(y>h)break;
        //gfx->drawFastHLine(px, px+w, py+y,GRAY);
        gfx->drawLine(px,py+y, px+w-4, py+y,GRAY);  //horizontale
      }
}

void draw_spectr(){//narise spekter
      int x = 0;
      int y = 0;
      int static p_y=0;
      int static p_x=0;
      int16_t col_s;
      //int width_b = bandwidth/(i2s_sample_rate_rx/NUM_SAMPLE_BUF);
      //int indent_b = indent/(i2s_sample_rate_rx/NUM_SAMPLE_BUF);
      int width_b = bandwidth/(I2S_SAMPLE_RATE/NUM_SAMPLE_BUF);
      int indent_b = indent/(I2S_SAMPLE_RATE/NUM_SAMPLE_BUF);
      gfx->fillRect(0+xwin,70+ywin,253,108,BLACK); //bilo je 256
      if(!swr_scan){
      for (int i = 4; i<NUM_SAMPLE_BUF/2;i++){
       //y=wp_value[i]/20+5;if(y>102)y=102; //tukaj je info o točki v spektru
       y = fft[i]/8;if(y>102)y=102; //tukaj je info o točki v spektru ///7 poveca amplitudo;
       col_s = 0b0000000101001100;
       //spremenite barvo spektra v pasovnem pasu in narišite "zaveso"
       if(rf_mode==LSB){ //|| rf_mode==L_D || rf_mode==CW){
          if((i<=pos_fft-indent_b)&&(i>=pos_fft-width_b-indent_b)){
            gfx->drawFastVLine(x+xwin,74+ywin,102,0b0001100110000111); //"zavesa"
            col_s = col_s|0b1000010000010000; //povečanje svetlosti spektra v prepustnem pasu
            }
       }
       if(rf_mode==USB){ // || rf_mode==U_D || rf_mode==CWR){
          if((i>=pos_fft+indent_b)&&(i<=pos_fft+width_b+indent_b)){
            gfx->drawFastVLine(x+xwin,74+ywin,102,0b0001100110000111);//"zavesa"
            col_s = col_s|0b1000010000010000; //povečanje svetlosti spektra v prepustnem pasu
            }
       }
       if(rf_mode==AM || rf_mode==FM){
          if(((i<=pos_fft)&&(i>=pos_fft-width_b))||((i>=pos_fft)&&(i<=pos_fft+width_b))){
            gfx->drawFastVLine(x+xwin,74+ywin,102,0b0001100110000111);//"zavesa"
            col_s = col_s|0b1000010000010000; //povečanje svetlosti spektra v prepustnem pasu
            }
       }

      gfx->drawFastVLine(x+xwin,175-y+ywin,y,col_s); //risanje pripravljenega spektra
      if(x>0)gfx->drawLine(p_x,p_y,x+xwin,175-y+ywin,GRAY|0b0100001000001000);//črta na vrhu spektra
      p_x=x+xwin;p_y=175-y+ywin;
      x++;if(x>252)break; //bilo je 255 zdaj zmanjsana sirina spektra za 4
      }
      }else{
      for(int i=0;i<160;i++){
        y=swr_value[i]*10;if(y>80)y=80;
        if(x>0)gfx->drawLine(p_x,p_y,x+xwin,175-y+ywin,0b01000011111101000);//črto na vrhu spektra
        p_x=x+xwin;p_y=175-y+ywin;
        x+=3;
      }
      gfx->setFont(&FreeMonoBold8pt8b);gfx->setTextColor(0b1000010000010000);
      gfx->setCursor(3+xwin,170+ywin);gfx->print("1.0");
      gfx->setCursor(3+xwin,153+ywin);gfx->print("2.0");
      gfx->setCursor(3+xwin,136+ywin);gfx->print("3.0");
      gfx->setCursor(3+xwin,119+ywin);gfx->print("4.0");
      gfx->setCursor(3+xwin,102+ywin);gfx->print("5.0");
     }
     /*if(txrx_mode==TX_MODE || txpower_g90>0 || tuning || swr_scan){
        gfx->setFont(&FreeMonoBold8pt8b);
        gfx->setTextColor(WHITE);
        gfx->setCursor(40+xwin,84+ywin);
        gfx->print("ALC: ");gfx->print(alc_g90);gfx->print("%");
        gfx->setCursor(150+xwin,84+ywin);
        gfx->print("SWR: ");gfx->print(swr_s90+1);
    }*/
}

void draw_waterfall(){ //prikaže niz medpomnilnikov fft za slap

  uint8_t y = 190;
  int x = -4; //zamaknem v levo
  for (int i=WP_LINE;i>0;i--){
    //izhod pripravljenih vrstic slapa v video medpomnilnik
    gfx->draw16bitRGBBitmap(x+xwin,i+y+ywin,(uint16_t*)&wp[wp_num[i-1]][0],NUM_SAMPLE_BUF/2,1);
  }
  gfx->drawFastVLine(x+xwin+pos_fft,y+ywin+1,WP_LINE,YELLOW); //vertikalni marker rumeni
  //if(!menu)gfx->setCursor(280+xwin,85+ywin);else gfx->setCursor(280+xwin,85+ywin); // dBm meter
  /*gfx->fillRect(270+xwin,75+ywin,30,15,BLACK);
  gfx->setCursor(270+xwin,85+ywin); // dBm meter
  gfx->setTextColor(GREEN);
  gfx->setFont(&CourierCyr6pt8b);
  if(txrx_mode==RX_MODE){gfx->print("-"); gfx->printf("%03d",150-old_smeter); gfx->print(" dBm");}*/
  //if(txrx_mode==TX_MODE && !swr_scan){gfx->print((float)smeter_g90/10.0,1);gfx->print("W");}
}
void draw_service(){
    const char* mod;const char* filtr;const char* tun;const char* type_fir;const char* step;
    gfx->drawRect(0+xwin,0+ywin,250,70,YELLOW);gfx->drawRect(249+xwin,0+ywin,256,70,YELLOW);
    gfx->fillRect(250+xwin,1+ywin,254,68,BLACK);
    gfx->drawFastVLine(400+xwin,0+ywin,70,YELLOW);
    gfx->drawFastVLine(479+xwin,0+ywin,70,YELLOW);
    if (rf_mode == LSB) mod = "LSB";
    //if (rf_mode == L_D) mod = "L_D";
    if (rf_mode == USB) mod = "USB";
    //if (rf_mode == U_D) mod = "U_D";
    if (rf_mode == CW)  mod = " CW";
    //if (rf_mode == CWR) mod = "CWR";
    if (rf_mode == FM)  mod = " FM";
    if (rf_mode == AM)  mod = " AM";
    
    
    if (numstep==1) step = "  100";        
    else if (numstep==2) step = " 1000";    
    else if (numstep==3) step = " 5000";
    else if (numstep==4) step = "10000";
    else if (numstep==5) step = "25000";
    else if (numstep==6) step = " 100k";
    else if (numstep==7) step = " 1MHz";
    if(!swr_scan){
        gfx->setFont(&CourierCyr6pt8b);gfx->setTextColor(WHITE,BLACK);
        gfx->setCursor(403+xwin,15+ywin);gfx->print("Mode:");
        gfx->setCursor(403+xwin,31+ywin);gfx->print("St:");
        gfx->setCursor(403+xwin,47+ywin);gfx->print("Vol:");
        gfx->setCursor(403+xwin,63+ywin);gfx->print("Ing:");

        gfx->setFont(&FreeMonoBold8pt8b);
        gfx->setTextColor(colors[5],BLACK);
        gfx->setCursor(444+xwin,15+ywin);gfx->print(mod);
        gfx->setCursor(420+xwin,31+ywin);gfx->print(step);
        gfx->setCursor(435+xwin,47+ywin);gfx->print(glasnost,1);gfx->print(" "); //gfx->print((tuner)?"On":"Off");
        gfx->setCursor(435+xwin,63+ywin);gfx->print(ifingain,2); //gfx->print(agc[agc_speed]);
    }else{
        gfx->setFont(&FreeMonoBold8pt8b);
        gfx->setTextColor(WHITE,BLACK);
        gfx->setCursor(403+xwin,31+ywin);gfx->print("  STOP");
        gfx->setCursor(403+xwin,47+ywin);gfx->print("  scan");
    }
        if(tun_mode == RIT)gfx->setTextColor(WHITE,BLACK);else gfx->setTextColor(GRAY|0b0100001000001000,BLACK);
        gfx->setCursor(257+xwin,58+ywin);
        //gfx->print("RIT ");gfx->printf("%2d %03d,%02d",  rx_freq/1000000, (rx_freq/1000)%1000, (rx_freq/10)%100 );
        gfx->print("Filter  ");gfx->print((int)(high_fc));
      int c_freq = freq; //
      gfx->setFont(&Seven_Segment18pt8b);gfx->setTextSize(1);
      if(txrx_mode==RX_MODE) gfx->setTextColor((colors[8]),BLACK);else gfx->setTextColor(RED,BLACK);
      gfx->setCursor(255+xwin,30+ywin);
      gfx->printf("%2d %03d,%02d",  c_freq/1000000, (c_freq/1000)%1000, (c_freq/10)%100 );
      gfx->drawFastHLine(249+xwin,38+ywin,152,YELLOW);
        
      gfx->setFont(&Picopixel);
      gfx->setTextColor(GRAY|0b0100001000001000,BLACK);
      gfx->fillRect(xwin,176+ywin,300,15,BLACK);
      if(!swr_scan){
       for(int i=0;i<NUM_SAMPLE_BUF/2;i+=54){ //markerji skale frekvence
         gfx->setCursor(i+xwin,185+ywin);
         c_freq=(freq-I2S_SAMPLE_RATE/4)+hz*i;
         gfx->printf("%2d %03d,%01d",  c_freq/1000000, (c_freq/1000)%1000, (c_freq/10)%100 );
         //gfx->drawFastVLine(i+xwin,175+ywin,3,GRAY|0b0100001000001000);//crtice
       }
      }else{
        int k = 0;
        for(int i=0;i<160;i++){
           if(k==0){
              gfx->setCursor(i*3+xwin,185+ywin);
              c_freq=swr_freq[i];
              gfx->printf("%2d %03d,%02d",  c_freq/1000000, (c_freq/1000)%1000, (c_freq/10)%100 );
              gfx->drawFastVLine(i*3+xwin,175+ywin,3,GRAY|0b0100001000001000);//маркеры на частотах
              k+=3;
           }else {k+=3;if(k>52)k=0;}
        }
      }
}

void draw_smeter(){//s-meter , wifi, power, out_power

  int x = xwin+125;
  int y = ywin+120;
  int h = 110;
  static int d_x=x;
  static int d_y=y;
  gfx->draw16bitRGBBitmap(35+xwin,1+ywin,(uint16_t*)&s_smeter,184,60);
  float _smeter = -PI*(150-old_smeter)/205;
  
  d_x=x+cos(_smeter)*h;
  d_y= y+sin(_smeter)*h;
  gfx->drawLine(x, y, d_x, d_y,GREEN);
  gfx->fillRect(3+xwin,55+ywin,245,14,BLACK);
  //if(wifi)gfx->draw16bitRGBBitmap(3,5,(uint16_t*)&wifi_on,15,15);
  //else gfx->draw16bitRGBBitmap(3+xwin,5+ywin,(uint16_t*)&wifi_off,15,15);
  if(power_g90>10.0)gfx->setTextColor(GREEN); else gfx->setTextColor(RED);
  //gfx->setFont(&CourierCyr6pt8b);
  gfx->setFont(&FreeMonoBold8pt8b);
  gfx->setCursor(3+xwin,64+ywin);
  gfx->print(power_g90,1);gfx->print("V");
  //if(txrx_mode == RX_MODE){
    gfx->setTextColor(GREEN);
    gfx->setCursor(85+xwin,64+ywin);
    /*switch (bands[numband].att_mode){
      case 0:gfx->setCursor(70+xwin,64+ywin);gfx->print("Pre/ATT: off");break;
      case 1:gfx->setCursor(80+xwin,64+ywin);gfx->print("Preamp: on");break;
      case 2:gfx->setCursor(100+xwin,64+ywin);gfx->print(" S : ");break;
    }*/
    gfx->print(" S:");
  //}
  gfx->setCursor(115+xwin,64+ywin);
  //if(txrx_mode == RX_MODE)gfx->setTextColor(GREEN);else gfx->setTextColor(RED);
  //gfx->setTextColor(GREEN);
  if(txrx_mode == RX_MODE){
  String slevel = "0   ";
  if((old_smeter>0)&&(old_smeter<10))slevel = "1   ";
  else if((old_smeter>9)&&(old_smeter<15))slevel = "2   ";
  else if((old_smeter>14)&&(old_smeter<20))slevel = "3   ";
  else if((old_smeter>19)&&(old_smeter<25))slevel = "4   ";
  else if((old_smeter>24)&&(old_smeter<30))slevel = "5   ";
  else if((old_smeter>29)&&(old_smeter<35))slevel = "6   ";
  else if((old_smeter>34)&&(old_smeter<40))slevel = "7   ";
  else if((old_smeter>39)&&(old_smeter<45))slevel = "8   ";
  else if((old_smeter>44)&&(old_smeter<50))slevel = "9   ";
  else if((old_smeter>49)&&(old_smeter<55))slevel = "9+20";
  else if((old_smeter>54)&&(old_smeter<60))slevel = "9+30";
  else if((old_smeter>59)&&(old_smeter<65))slevel = "9+40";
  else if((old_smeter>64)&&(old_smeter<70))slevel = "9+60";
  else if(old_smeter>69){slevel = "9+70";} //old_smeter = 75;}
  gfx->print(slevel);
  } //else{gfx->setCursor(200+xwin,64+ywin); gfx->print(txpower_g90,1);}
  gfx->setCursor(195+xwin,64+ywin);
  gfx->print("AG:");gfx->print((int)agc_koeff);gfx->print(" ");
  //gfx->print("W");
}

void draw_temp_value(){
  uint16_t s_col = (0b11001010);
  if(show_par=="BAND"){
    gfx->setTextColor(0b1010001010000110);
    gfx->setFont(&Seven_Segment18pt8b);
    gfx->setTextSize(2);
    if(!menu)gfx->setCursor(100+xwin,150+ywin);else gfx->setCursor(180+xwin,150+ywin);
    gfx->print(bands[numband].name);
    gfx->setTextSize(1);
    return;
  }
  if(show_par=="VOL_"){
    gfx->setTextColor(0b1000010000010000);
    gfx->setFont(&FreeMonoBold14pt8b);
    gfx->setTextSize(2);
    gfx->setCursor(65+xwin,130+ywin);
    gfx->print("Vol: ");gfx->print(volume);
    gfx->setTextSize(1);
    return;
  }
    if(show_par=="POWR"){
    gfx->setTextColor(0b1000010000010000);
    gfx->setFont(&FreeMonoBold14pt8b);
    gfx->setTextSize(2);
    gfx->setCursor(10+xwin,130+ywin);
    gfx->print("Power:");gfx->print(rf_power);gfx->print("W");
    gfx->setTextSize(1);
    return;
  }

  if(show_par=="F__L"){
    gfx->setTextColor(0b1100011000011000);
    gfx->setFont(&FreeMonoBold8pt8b);
    gfx->setTextSize(2);
    gfx->setCursor(20+xwin,120+ywin);
    gfx->print("Low freq:");gfx->print(indent);gfx->print(" Hz");
    gfx->setTextSize(1);
    return;
  }
  if(show_par=="F__H"){
    gfx->setTextColor(0b1100011000011000);
    gfx->setFont(&FreeMonoBold8pt8b);
    gfx->setTextSize(2);
    gfx->setCursor(1+xwin,120+ywin);
    gfx->print("High freq:");gfx->print(bandwidth);gfx->print("Hz");
    gfx->setTextSize(1);
    return;
  }


  if(show_par=="PAR_"){
    gfx->setTextColor(0b1100011000011000);
    gfx->setFont(&FreeMonoBold8pt8b);
    gfx->setTextSize(2);
    gfx->setCursor(65+xwin,120+ywin);
    gfx->print("Parameters");
    gfx->setCursor(110+xwin,150+ywin);
    gfx->print("saved");
    gfx->setTextSize(1);
    return;
  }
    if(show_par=="AUXI"){
      gfx->setTextColor(0b1100011000011000);
      gfx->setFont(&FreeMonoBold8pt8b);
      gfx->setTextSize(2);
      gfx->setCursor(20+xwin,120+ywin);
      gfx->print("AUX input:");gfx->print(auxi);
      gfx->setTextSize(1);
      return;
  }
    if(show_par=="AUXO"){
      gfx->setTextColor(0b1100011000011000);
      gfx->setFont(&FreeMonoBold8pt8b);
      gfx->setTextSize(2);
      gfx->setCursor(20+xwin,120+ywin);
      gfx->print("AUX output:");gfx->print(auxo);
      gfx->setTextSize(1);
      return;
    }

      if(show_par=="MICG"){
      gfx->setTextColor(0b1100011000011000);
      gfx->setFont(&FreeMonoBold8pt8b);
      gfx->setTextSize(2);
      gfx->setCursor(50+xwin,120+ywin);
      gfx->print("Mic gain:");gfx->print(mic_gain);
      gfx->setTextSize(1);
      return;
    }

      if(show_par=="L__M"){
      gfx->setTextColor(0b1100011000011000);
      gfx->setFont(&FreeMonoBold8pt8b);
      gfx->setTextSize(2);
      gfx->setCursor(50+xwin,120+ywin);
      gfx->print("Input: ");gfx->print((mic_line)?"mic":"line");
      gfx->setTextSize(1);
      return;
    }

      if(show_par=="TUNE"){
      gfx->setTextColor(0b1111000000000000);
      gfx->setFont(&FreeMonoBold8pt8b);
      gfx->setTextSize(2);
      gfx->setCursor(120+xwin,120+ywin);
      gfx->print("Tuning... ");
      gfx->setTextSize(1);
      return;
    }

      if(show_par=="RF_G"){
      gfx->setTextColor(0b1100011000011000);
      gfx->setFont(&FreeMonoBold8pt8b);
      gfx->setTextSize(2);
      gfx->setCursor(50+xwin,120+ywin);
      gfx->print("RF Gain: ");gfx->print(rf_gain);
      gfx->setTextSize(1);
      return;
    }


}

void drawinfo(){
      //gfx->setCursor(270+xwin,220+ywin);
      gfx->fillRect(270+xwin,225+ywin,90,45,BLACK);
      gfx->setFont(&FreeMonoBold8pt8b);gfx->setTextColor(WHITE);
      
      gfx->setCursor(270,250);
      //gfx->print(tp_x);gfx->print(":");gfx->print(tp_y);gfx->print(":");gfx->print(tp_size);
      gfx->print(x);gfx->print(":");gfx->print(y); //gfx->print(":");gfx->print(z);
        //if(t_fps<=0)t_fps=25;
        //gfx->setTextColor(WHITE);
      gfx->setCursor(270,265);
      gfx->print(pwmduty);
        //gfx->print("Fps");

      gfx->setTextColor(WHITE);
      int  my_s = timeinfo.tm_sec;
      int  my_m = timeinfo.tm_min;
      int  my_h = timeinfo.tm_hour;
      int  my_d = timeinfo.tm_mday;
      int  my_mm = timeinfo.tm_mon+1;
      int  my_y = timeinfo.tm_year+1900;
      gfx->setCursor(5,271);
      //gfx->printf("%02d.%02d.%4d GMT:%02d:%02d:%02d",my_d,my_mm,my_y,my_h,my_m,my_s);
      gfx->printf("T:%02d:%02d:%02d",my_h,my_m,my_s);
      //gfx->print(" ");gfx->print(mount);
      //gfx->setCursor(5,250);
      /*gfx->print(" ");
      gfx->print(i_tx);gfx->print("/");
      //gfx->setCursor(5,270);
      gfx->print(i_rx);*/
}

void scroll_wp(){ //premik niza nizov "slap".
      uint8_t tmp = wp_num[WP_LINE-1];
      for (int i=WP_LINE-1;i>0;i--) {wp_num[i] = wp_num[i-1];}
      wp_num[0]=tmp;
}

void peak_down(){//gladko zmanjša raven spektra in panorame
   for (int i=0;i<NUM_SAMPLE_BUF;i++){
        fft_inter[i] = fft_inter[i]>0?fft_inter[i]-24:0;  //it was -1 too slow for me
   }
}

void screen_control(){  // se ne izvaja ce ni izbrano
if(!menu && !swr_scan){
 if(!more_menu){
  gfx->setFont(&FreeMonoBold8pt8b);
  gfx->setTextColor(WHITE);
  gfx->fillRoundRect(f_low.x_min,f_low.y_min,f_low.w,f_low.h,3,(show_par=="F__L")?0b0110000000001100:0b0000010000000000);
  gfx->setCursor(f_low.x_min+5,f_low.y_min+18);
  gfx->print(f_low.value);

  gfx->setFont(&FreeMonoBold8pt8b);
  gfx->setTextColor(WHITE);
  gfx->fillRoundRect(f_high.x_min,f_high.y_min,f_high.w,f_high.h,3,(show_par=="F__H")?0b0110000000001100:0b0000010000000000);
  gfx->setCursor(f_high.x_min+5,f_high.y_min+18);
  gfx->print(f_high.value);

  gfx->setFont(&FreeMonoBold8pt8b);
  gfx->setTextColor(WHITE);
  gfx->fillRoundRect(rfmode.x_min,rfmode.y_min,rfmode.w,rfmode.h,3,(rfmode.b_name==tap_name)?0b0110000000001100:0b0000010000000000);
  gfx->setCursor(rfmode.x_min+5,rfmode.y_min+18);
  gfx->print(rfmode.value);

  gfx->setFont(&FreeMonoBold8pt8b);
  gfx->setTextColor(WHITE);
  gfx->fillRoundRect(fstep.x_min,fstep.y_min,fstep.w,fstep.h,3,(fstep.b_name==tap_name)?0b0110000000001100:0b0000010000000000);
  gfx->setCursor(fstep.x_min+5,fstep.y_min+18);
  gfx->print(fstep.value);

  gfx->setFont(&FreeMonoBold8pt8b);
  gfx->setTextColor(WHITE);
  gfx->fillRoundRect(fagc.x_min,fagc.y_min,fagc.w,fagc.h,3,(fagc.b_name==tap_name)?0b0110000000001100:0b0000010000000000);
  gfx->setCursor(fagc.x_min+5,fagc.y_min+18);
  gfx->print(fagc.value);

  gfx->setFont(&FreeMonoBold8pt8b);
  gfx->setTextColor(WHITE);
  gfx->fillRoundRect(fpower.x_min,fpower.y_min,fpower.w,fpower.h,3,(show_par=="POWR")?0b0110000000001100:0b0000010000000000);
  gfx->setCursor(fpower.x_min+5,fpower.y_min+18);
  gfx->print(fpower.value);

  gfx->setFont(&FreeMonoBold8pt8b);
  gfx->setTextColor(WHITE);
  gfx->fillRoundRect(fout.x_min,fout.y_min,fout.w,fout.h,3,(fout.b_name==tap_name)?0b0110000000001100:0b0000010000000000);
  gfx->setCursor(fout.x_min+5,fout.y_min+18);
  gfx->print(fout.value);

  gfx->setFont(&FreeMonoBold8pt8b);
  gfx->setTextColor(WHITE);
  gfx->fillRoundRect(ftun.x_min,ftun.y_min,ftun.w,ftun.h,3,(ftun.b_name==tap_name)?0b0110000000001100:0b0000010000000000);
  gfx->setCursor(ftun.x_min+5,ftun.y_min+18);
  gfx->print(ftun.value);

  gfx->setFont(&FreeMonoBold8pt8b);
  gfx->setTextColor(WHITE);
  gfx->fillRoundRect(fmic.x_min,fmic.y_min,fmic.w,fmic.h,3,(fmic.b_name==tap_name)?0b0110000000001100:0b0000010000000000);
  gfx->setCursor(fmic.x_min+5,fmic.y_min+18);
  gfx->print(fmic.value);

  gfx->setFont(&FreeMonoBold8pt8b);
  gfx->setTextColor(WHITE);
  gfx->fillRoundRect(fatt.x_min,fatt.y_min,fatt.w,fatt.h,3,(fatt.b_name==tap_name)?0b0110000000001100:0b0000010000000000);
  gfx->setCursor(fatt.x_min+5,fatt.y_min+18);
  gfx->print(fatt.value);

  gfx->setFont(&FreeMonoBold8pt8b);
  gfx->setTextColor(WHITE);
  gfx->fillRoundRect(fmore.x_min,fmore.y_min,fmore.w,fmore.h,3,(fmore.b_name==tap_name)?0b0110000000001100:0b0100001000000000);
  gfx->setCursor(fmore.x_min+5,fmore.y_min+18);
  gfx->print(fmore.value);

  gfx->setFont(&FreeMonoBold8pt8b);
  gfx->setTextColor(WHITE);
  gfx->fillRoundRect(ftune.x_min,ftune.y_min,ftune.w,ftune.h,3,(ftune.b_name==tap_name)?0b0111100000000000:0b0100000000000000);
  gfx->setCursor(ftune.x_min+5,ftune.y_min+18);
  gfx->print(ftune.value);

 }
 else {

  gfx->setFont(&FreeMonoBold8pt8b);
  gfx->setTextColor(WHITE);
  gfx->fillRoundRect(fauxi.x_min,fauxi.y_min,fauxi.w,fauxi.h,3,(show_par=="AUXI")?0b0110000000001100:0b0000010000000000);
  gfx->setCursor(fauxi.x_min+5,fauxi.y_min+18);
  gfx->print(fauxi.value);

  gfx->setFont(&FreeMonoBold8pt8b);
  gfx->setTextColor(WHITE);
  gfx->fillRoundRect(fauxo.x_min,fauxo.y_min,fauxo.w,fauxo.h,3,(show_par=="AUXO")?0b0110000000001100:0b0000010000000000);
  gfx->setCursor(fauxo.x_min+5,fauxo.y_min+18);
  gfx->print(fauxo.value);

  gfx->setFont(&FreeMonoBold8pt8b);
  gfx->setTextColor(WHITE);
  gfx->fillRoundRect(fmicg.x_min,fmicg.y_min,fmicg.w,fmicg.h,3,(show_par=="MICG")?0b0110000000001100:0b0000010000000000);
  gfx->setCursor(fmicg.x_min+5,fmicg.y_min+18);
  gfx->print(fmicg.value);

  gfx->setFont(&FreeMonoBold8pt8b);
  gfx->setTextColor(WHITE);
  gfx->fillRoundRect(flm.x_min,flm.y_min,flm.w,flm.h,3,(show_par=="L__M")?0b0110000000001100:0b0000010000000000);
  gfx->setCursor(flm.x_min+5,flm.y_min+18);
  gfx->print(flm.value);

  gfx->setFont(&FreeMonoBold8pt8b);
  gfx->setTextColor(WHITE);
  gfx->fillRoundRect(frf.x_min,frf.y_min,frf.w,frf.h,3,(show_par=="RF_G")?0b0110000000001100:0b0000010000000000);
  gfx->setCursor(frf.x_min+5,frf.y_min+18);
  gfx->print(frf.value);

  gfx->setFont(&FreeMonoBold8pt8b);
  gfx->setTextColor(WHITE);
  gfx->fillRoundRect(fscan.x_min,fscan.y_min,fscan.w,fscan.h,3,(fscan.b_name==tap_name)?0b0110000000001100:0b0000010000000000);
  gfx->setCursor(fscan.x_min+5,fscan.y_min+18);
  gfx->print(fscan.value);


  gfx->setFont(&FreeMonoBold8pt8b);
  gfx->setTextColor(WHITE);
  gfx->fillRoundRect(fptt.x_min,fptt.y_min,fptt.w,fptt.h,3,(send_carrier)?0b0111100000000000:0b0011000000000000);
  gfx->setCursor(fptt.x_min+5,fptt.y_min+18);
  gfx->print(fptt.value);


  gfx->setFont(&FreeMonoBold8pt8b);
  gfx->setTextColor(WHITE);
  gfx->fillRoundRect(fmore.x_min,fmore.y_min,fmore.w,fmore.h,3,(fmore.b_name==tap_name)?0b0110000000001100:0b0100001000000000);
  gfx->setCursor(fmore.x_min+5,fmore.y_min+18);
  gfx->print(fmore.value);
 } 
}
  /*if(tp_band){
    int x=20;
    int y=80;
    int c = 0b0000010000000000;
    gfx->setFont(&FreeMonoBold8pt8b);
    gfx->setTextColor(WHITE);
    for(int i=0;i<N_BANDS;i++){
      if(i==numband)c=0b0110000000001100;else c = 0b0000010000000000;
      gfx->fillRoundRect(x,y,50,20,3,c);
      gfx->setCursor(x+5,y+13);gfx->print(bands[i].name);
      x+=60;if(x>300){x=20;y+=30;}
    }
  }*/
}


void x_display(){
   draw_smeter();    //отобразить s-метр
   draw_spectr();    //отобразить спектр или swr_scan
   draw_service();   //отобразить остальное
   draw_Grid(0+xwin,72+ywin,253,103); //480,103);//nariše mrezo 4 manj kot 255
   draw_temp_value(); //samo če se izbere show parameter
   draw_waterfall(); //отобразить панораму
   screen_control(); //кнопки тачскрина  ne kaze ce ni izbrano kaj
   drawinfo(); //показать fps и др.
}

void screens(uint8_t s){
  switch (s){
    case TX_MODE:x_display();break;
    case RX_MODE:x_display();break;
 }
}
