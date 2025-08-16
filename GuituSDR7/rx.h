
xSemaphoreHandle xRXDSP;
xSemaphoreHandle xRXIN;
xSemaphoreHandle xRXOUT;

int * input_buffer_ptr = &input_buffer[NUM_SAMPLE_BUF/2].re;

void IRAM_ATTR rx_in(void * pvParameters){
  size_t readsize = 0;
  
  while (true) {
          #ifdef DEBUG_RUN 
            int rx_time = micros();
          #endif
          xSemaphoreTake(xRXIN, portMAX_DELAY);//ждем сигнала от dsp-обработчика о готвности премного буфера для приема след.партии отсчетов
          #ifdef DEBUG_RUN 
            if (access_in_wait){rx_in_wait_result = micros()-rx_time;access_in_wait=false;}
            rx_time = micros();
          #endif
          //копирование ранее принятых отсчетов из старшей части рабочего буфера в младшую (50% overlap&save)
          for (int i=0;i<NUM_SAMPLE_BUF;i++){
            workbuf_in[i].re = workbuf_tmp[i].re;
            workbuf_in[i].im = workbuf_tmp[i].im;
          }
          //прием след.порции отсчетов и перенос в старшую часть рабочего буфера
          if(current_mode==RX_MODE)i2s_read(I2S_NUM_0, &input_buffer, sizeof(input_buffer), &readsize, portMAX_DELAY);
          for (int i=0; i<NUM_SAMPLE_BUF; i++) { 
            workbuf_in[i+NUM_SAMPLE_BUF].re = workbuf_tmp[i].re = ((float)(input_buffer[i].re>>12)) * ifingain;
            workbuf_in[i+NUM_SAMPLE_BUF].im = workbuf_tmp[i].im = ((float)(input_buffer[i].im>>12)) * ifingain;
            fft_in[i].re = workbuf_tmp[i].re;//заполняем fft-буфер для панорамы и спектра
            fft_in[i].im = workbuf_tmp[i].im;
          }
          fft_for_display((float*)&fft_in);//izračun magnitude za spekter in za eno novo vrstico v slapu za kasnejši prikaz
          #ifdef DEBUG_RUN 
            if (access_in_run){rx_in_run_result = micros()-rx_time;access_in_run=false;}
          #endif
          if(current_mode==RX_MODE){xSemaphoreGive(xRXDSP);}//разрешаем демодуляцию и фильтрацию рабочего буфера
  }
}

void IRAM_ATTR rx_out(void * pvParameters){
  size_t readsize = 0;
  while(true){
          #ifdef DEBUG_RUN 
            int rx_time = micros();
          #endif
          xSemaphoreTake(xRXOUT, portMAX_DELAY);//ждем окончания dsp-обработки
          #ifdef DEBUG_RUN 
             if (access_out_wait){rx_out_wait_result = micros()-rx_time;access_out_wait=false;}
             rx_time = micros();
          #endif
          if(!agc)agc_koeff=5.0f;//
          for (int i=0; i<NUM_SAMPLE_BUF; i++) { //переносим обработанный массив в выходной буфер с нормализацией в I2S-формат
            output_buffer[i].re = speak_out ? ((int)(workbuf_out[i].re*agc_koeff))<<12: 0;
            output_buffer[i].im = speak_out ? ((int)(workbuf_out[i].im*agc_koeff))<<12: 0;
          }
          xSemaphoreGive(xRXIN);
          if(current_mode==RX_MODE)i2s_write(I2S_NUM_0, &output_buffer, sizeof(output_buffer), &readsize, portMAX_DELAY );//вывод звука
          #ifdef DEBUG_RUN 
            if (access_out_run){rx_out_run_result = micros()-rx_time;access_out_run=false;}
          #endif
  }
}

uint32_t IRAM_ATTR S_metr_fm(float* input)
  {  
  int bins = 2*bandwidth/(I2S_SAMPLE_RATE/NUM_SAMPLE_BUF);  // 3600/(48000/512) = 38 *2
  //int bin_start = indent/(I2S_SAMPLE_RATE/NUM_SAMPLE_BUF);  // 0
  int bin_start = bins;  // 0
  float tmp=0;
  float S_metr_buf=0;
  float beta=0.99f;
  float alpha=(1-beta);
  for (int i=bin_start*2;i<(bins+bin_start)*2;i+=2)
  {
    if (input[i]<0){tmp=-input[i];}else{tmp=input[i];}
    S_metr_buf=(S_metr_buf*beta)+(alpha*tmp);
  }
 
  int32_t level=(int32_t)(2.0f*sqrtf((S_metr_buf/bins)));
    if (level<=0)level=0;
    if (level>150)level=150;
    return level;
  }

uint32_t IRAM_ATTR S_metr_am()
{  
  int32_t level = (int32_t)(0.8f*sqrtf(zDC));
  if (level<=0)level=0;
  if (level>150)level=150;
  return level;
}

uint32_t IRAM_ATTR S_metr_ssb(float* input)
{ 
int bins = bandwidth/(I2S_SAMPLE_RATE/NUM_SAMPLE_BUF);
int bin_start = indent/(I2S_SAMPLE_RATE/NUM_SAMPLE_BUF);
float tmp=0;
float S_metr_buf=0;
float beta=0.99f;
float alpha=(1-beta);
  for (int i=bin_start*2;i<(bins+bin_start)*2;i+=2)
  {
    if (input[i]<0){tmp=-input[i];}else{tmp=input[i];}
    S_metr_buf=(S_metr_buf*beta)+(alpha*tmp);
  }
 
  int32_t level=(int32_t)(2.0f*sqrtf((S_metr_buf/bins)));
  if (level<=0)level=0;
  if (level>150)level=150;
  return level;
}

void IRAM_ATTR get_ssb(int pos,struct COMPLEX* input){//демодуляция SSB в частотной области

          switch (rf_mode){
          case LSB: //
            for (int i=0;i<NUM_FFT_BUF/2;i++) {
                input[i].re = input[pos*2-i].re;
                input[i].im = -input[pos*2-i].im;
            }
            break;
          case USB: //
            for (int i=0;i<NUM_FFT_BUF/2;i++) {
                input[i].re = input[pos*2+i].re;
                input[i].im = input[pos*2+i].im;
            }
            break;
          }
          for (int i=NUM_FFT_BUF/2;i<NUM_FFT_BUF;i++) {//ponastavimo negativne frekvence
            input[i].re = input[i].im = 0;
          }
}

void IRAM_ATTR get_am(int pos,struct COMPLEX* input){
    /*for (int i=0;i<NUM_FFT_BUF/4;i++) {
                input[i].re = input[pos+i].re;  //za 2x 6kHz sirino
                input[i].im = input[pos+i].im;
            }*/

    for (int i=0;i<204;i++) {
                input[i].re = input[26+pos+i].re; //za 2x cca5kHz sirino
                input[i].im = input[26+pos+i].im;
            }
     
     for (int i=204;i<NUM_FFT_BUF;i++) {//ponastavimo 0 in negativne frekvence tudi na 0
            input[i].re = input[i].im = 0;
          }
     /*for (int i=NUM_FFT_BUF/4;i<NUM_FFT_BUF;i++) {//ponastavimo negativne frekvence za 6kHz
            input[i].re = input[i].im = 0;
          }*/  
}

void IRAM_ATTR rx_dsp(void *pvParameters){
    
    while(true){
          #ifdef DEBUG_RUN 
            int rx_time = micros();
          #endif
          xSemaphoreTake(xRXDSP, portMAX_DELAY);//ждем сигнала о готвности приемного буфера
          #ifdef DEBUG_RUN 
            if (access_dsp_wait){rx_dsp_wait_result = micros()-rx_time;access_dsp_wait=false;}
            rx_time = micros();
          #endif
          init_filters (num_filter);
          if(rf_mode == LSB || rf_mode == USB)
          {
            xtensa_cfft_f32(&cfft,(float*)&workbuf_in,0,1);  //ifftFlag,bitReverseFlag nardei fft
            get_ssb(pos_fft,(struct COMPLEX*)&workbuf_in);//demodulacija odseka spektra ssb na položaju pos_fft
            smeter = S_metr_ssb((float*) &workbuf_in);
            xtensa_cfft_f32(&cfft,(float*)&workbuf_in,1,1);  //ifftFlag,bitReverseFlag naredi inverse fft
          }
          else
          {
            //get_am((struct COMPLEX*)&workbuf_in);//še ne deluje pravilno
            xtensa_cfft_f32(&cfft,(float*)&workbuf_in,0,1);  //ifftFlag,bitReverseFlag)
            get_am(pos_fft,(struct COMPLEX*)&workbuf_in);//poiskus poberem USB pas ali oboje polovico
            //smeter = S_metr_am();
            if(rf_mode == FM){smeter = S_metr_fm((float*) &workbuf_in);}   //also for AM and FM
            xtensa_cfft_f32(&cfft,(float*)&workbuf_in,1,1);  //ifftFlag,bitReverseFlag naredi inverse fft
            switch (rf_mode){
          case AM: //
            for(int i=0;i<NUM_FFT_BUF/2;i++){
           //float Demod_AM = sqrtf((input[i].re*input[i].re) + (input[i].im*input[i].im));
           float Demod_AM = sqrtf((workbuf_in[i].re*workbuf_in[i].re) + (workbuf_in[i].im*workbuf_in[i].im));
           //float Demod_AM = sqrtf((input[pos*2+i].re*input[pos*2+i].re) + (input[pos*2+i].im*input[pos*2+i].im)); //poberem okrog pos*2 lege
           zDC = Demod_AM*0.005 + zDC*0.995; //LPF for getting DC component
           workbuf_in[i].re = workbuf_in[i].im = Demod_AM - zDC;
           workbuf_in[i+NUM_FFT_BUF/2].re = workbuf_in[i+NUM_FFT_BUF/2].im = 0;  // is this needed?
           smeter = S_metr_am(); //for AM 
            }
            break;
          case FM: //
          static float prevPhase; // = 0.0;
            for (int i=0;i<NUM_FFT_BUF/2;i++) {
            float currentPhase = atan2(workbuf_in[i].re, workbuf_in[i].im);
            float phaseDiff = currentPhase - prevPhase;
			  
	         // Adjust the phase difference to ensure it stays within a reasonable range
           if (phaseDiff > M_PI) {
           phaseDiff -= 2 * M_PI;
            } else if (phaseDiff < -M_PI) {
           phaseDiff += 2 * M_PI;
            }
		      float Demod_FM = phaseDiff * 8000.0f; // mogoče daj na 1000
          zDC = Demod_FM*0.005 + zDC*0.995; // LPF for getting DC component
		      float signal = 1.0f*(Demod_FM- zDC); // Reject DC
          workbuf_in[i].re = workbuf_in[i].im = signal;
          prevPhase = currentPhase;
          //smeter is above calculated from fft
            }
            break;
           }

          }
          fir_f32(&fir_rx, (float*)&workbuf_in, (float*)&workbuf_out, NUM_FFT_BUF);//osnovni filter NUM_FFT_BUF = NUM_SAMPLE_BUF*2 = 1024
          //Adding simple low pass filter
          for (int i=0; i<NUM_SAMPLE_BUF; i++) { //
            zRX=workbuf_out[i].re*0.18f + zRX*0.82f; //LPF for AF output
            zRU=(int)(zRX*glasnost*modegain);
            workbuf_out[i].re = zRU;
            workbuf_out[i].im = zRU;
          }
          
          //разрешаем выводить звук и  прием след.партии отсчетов
          #ifdef DEBUG_RUN 
            if (access_dsp_run){rx_dsp_run_result = micros()-rx_time;access_dsp_run=false;}
          #endif
          if(current_mode == RX_MODE){xSemaphoreGive(xRXOUT);} 
          if (smeter > old_smeter){old_smeter=smeter;}
          if(old_smeter>70)old_smeter=70;
    }
}
