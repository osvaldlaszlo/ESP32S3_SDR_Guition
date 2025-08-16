
#include "src/dsp_lib/xtensa_const_structs.h"

xtensa_cfft_instance_f32 cfft;
xtensa_cfft_instance_f32 rfft;

float wind[NUM_SAMPLE_BUF];

void window_init(float *window, int len) 
{
    const float a0 = 0.42;
    const float a1 = 0.5;
    const float a2 = 0.08;

    float len_mult = 1/(float)(len-1);
    for (int i = 0; i < len; i++) {
        window[i] = a0 - a1 * cosf(i * 2 * M_PI * len_mult) + a2 * cosf(i * 4 * M_PI * len_mult);
    }

}


void fft_init(){
      cfft = xtensa_cfft_sR_f32_len1024;
      rfft = xtensa_cfft_sR_f32_len512;
      window_init(wind,NUM_SAMPLE_BUF);//Blackman-window
      for(int i=WP_LINE;i>0;i--){wp_num[i-1]=i-1;} //oštevilči niz številk vrstic "slapa"
}

/*int IRAM_ATTR sel_c(int val,int max,int mod){

  int md = max/mod;
  int i=0;
  if (val<=md)return i;
  for (i=0;i<mod;i++){
    if (i*md > val){return i;}
  }
  return i-1;
}*/


/*void fft_init(){
      for(int i=WP_LINE;i>0;i--){wp_num[i-1]=i-1;} //oštevilči niz številk vrstic "slapa"
}*/

uint16_t hsv_to_rgb(uint16_t h, uint8_t s, uint8_t v)
{
    h = (uint32_t)((uint32_t)h * 255) / 360; //color      [0..359]
    s = (uint16_t)((uint16_t)s * 255) / 100; //saturation [0..100]
    v = (uint16_t)((uint16_t)v * 255) / 100; //value      [0..100]
    uint8_t r, g, b;
    uint8_t region, remainder, p, q, t;
    if(s == 0) {
        return (uint16_t)((v >> 3) | ((v >> 2) << 5) | ((v >> 3) << 11));
    }
    region    = h / 43;
    remainder = (h - (region * 43)) * 6;
    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;
    switch(region) {
        case 0:
            r = v; g = t; b = p;
            break;
        case 1:
            r = q; g = v; b = p;
            break;
        case 2:
            r = p; g = v; b = t;
            break;
        case 3:
            r = p; g = q; b = v;
            break;
        case 4:
            r = t; g = p; b = v;
            break;
        default:
            r = v; g = p; b = q;
            break;
    }
    uint16_t result = (r >> 3) | ((g >> 2) << 5) | ((b >> 3) << 11);
    return result;
}

uint16_t select_color(int val,int max){
  uint16_t h = map(val,0,max,5,101);  //vsaj 5
  //uint16_t h = map(val,0,300,5,101);  //300 določa obcutljivost barv, doloceno v max
  uint8_t  s = 100;
  uint8_t  v = map(h,0,100,20,100);  //20 zato, da je vsaj temno plava
  return hsv_to_rgb(h,s,v);
  /*val = val >> 1;
  if(val>255) val = 255;
   // Convert amplitude (0-255) to RGB565 color
  uint8_t r = val > 127 ? map(val, 128, 255, 0, 255) : 0;
  uint8_t g = val > 63 ? map(val, 64, 127, 0, 255) : 0;
  uint8_t b = map(val, 0, 63, 0, 255);
  //return tft.color565(r, g, b);
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);*/

}

/*void fft_for_display(uint8_t* input){
    sum_fft = 0.0f;
    max_fft = 0.0f;
    min_fft = 1000000000.0f;
    for (int i = 0 ; i < NUM_SAMPLE_BUF; i++) {
      if(input[i]>=limited_fft)input[i]=limited_fft;
      if (max_fft < input[i])max_fft=input[i];
      if (min_fft > input[i])min_fft=input[i];
      //kopiranje magnitud v prikazan medpomnilnik, katerega elementi se nenehno zmanjšujejo
      if(input[i]<=fft_inter[i]) input[i]=fft_inter[i];
      if(input[i]>fft_inter[i])fft_inter[i] = input[i];
      //zapolnite zgornjo vrstico matrike, da prikažete "slap"
      wp_value[i]=select_color(input[i],limited_fft);
      wp[wp_num[0]][i]=wp_value[i];
      sum_fft = sum_fft+input[i];
    }
}*/

void IRAM_ATTR fft_for_display(float* input){

    for (int i = 0 ; i < NUM_SAMPLE_BUF; i++) {
      input[i*2] = input[i*2] * wind[i];
      input[i*2+1] = input[i*2+1] * wind[i];
    }
    xtensa_cfft_f32(&rfft,input,0,1);
    for (int i=NUM_SAMPLE_BUF;i<NUM_SAMPLE_BUF*2;i++){
      input[i]=0;
    }
    xtensa_cmplx_mag_f32(input,fft,NUM_SAMPLE_BUF);// pridobivanje realnih vrednosti spektralnih komponent
    float sum_fft = 0.0f;
    max_fft = 0.0f;
    float min_fft = 1000000.0f;
    int k = 0;
    for (int i = 0 ; i < NUM_SAMPLE_BUF/2; i++) {
        if(i>3){fft[i]*=0.01f;}else{fft[i]=0;}  //prve 3 ali td ne risem ker je veliko bruma 
        if (max_fft < fft[i])max_fft=fft[i];
        if (min_fft > fft[i])min_fft=fft[i];
        if(fft[i]>=700)fft[i]=700;
        //kopiranje magnitud v prikazan medpomnilnik, katerega elementi se nenehno zmanjšujejo
        if(fft[i]<=fft_inter[k]) fft[i]=fft_inter[k];
        if(fft[i]>fft_inter[k])fft_inter[k] = fft[i];
        //zapolnite zgornjo vrstico matrike, da prikažete "slap"
        wp_value[i]=select_color((int)fft[i],limited_fft); //prej input[i]
        //wp[wp_num[0]][i]=wp_value[i];

        if(fill_fft)wp[wp_num[0]][k]= wp_value[i]; //colors_w[sel_c((int)fft[i],FWW,CWW)];  //fill_fft je stalno true
        sum_fft = sum_fft+fft[i];
        k++;
    }
    avg_fft = (sum_fft-max_fft)/(NUM_SAMPLE_BUF/2);
    if(max_fft>600  && !dec_Ifgain) dec_Ifgain = true;
    if(avg_fft < 15 && !inc_Ifgain) inc_Ifgain = true;
    if(avg_fft > 20 && !dec_Ifgain) dec_Ifgain = true;   
}