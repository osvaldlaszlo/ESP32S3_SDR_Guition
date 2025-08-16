//A local frequ osc for SDR using Si5351 board
//Created by S52UV Apr 2025
// The Si5351 instance.
Si5351 si5351;

int currentFrequency = -1;
int lastMult = -1;

void changeFrequency( int freq )
{
    int mult = 0;
    freq = freq * 4; // za QRP Lab Tayloe pomnozim s 4, sicer ne
    currentFrequency = freq;
    if ( freq < 680000 )
      mult = 1300;
    else if ( freq < 1000000 )
      mult = 900;
    else if ( freq < 1400000 )
      mult = 630;
    else if ( freq < 2000000 )
      mult = 450;    
    else if ( freq < 3000000 )
      mult = 300;
    else if ( freq < 4000000 )
      mult = 200;
    else if ( freq < 5000000 )
      mult = 150;
    else if ( freq < 6000000 )
      mult = 120;
    else if ( freq < 8000000 )
      mult = 100;
    else if ( freq < 11000000 )
      mult = 80;
    else if ( freq < 15000000 )
      mult = 50;
    else if ( freq < 22000000 )
      mult = 40;
    else if ( freq < 30000000 )
      mult = 30;
    else if ( freq < 40000000 )
      mult = 20;
    else if ( freq < 50000000 )
      mult = 16;
    else if ( freq < 70000000 )
      mult = 12;    
    else if ( freq < 90000000 )
      mult = 10;
    else if ( freq < 110000000 )
      mult = 8;
    else if ( freq < 150000000 )
      mult = 6;
    else if ( freq < 200000000 )
      mult = 4;
    else
      mult = 4;
      
    uint64_t f = freq * 100ULL;
    uint64_t pllFreq = freq * mult * 100ULL;


  //si5351->set_freq( f, SI5351_CLK0 );
  //si5351->set_freq( f, SI5351_CLK2 );
  
    si5351.set_freq_manual(f, pllFreq, SI5351_CLK0);
    //si5351.set_freq_manual(f, pllFreq, SI5351_CLK1);
    //si5351->set_freq_manual(f, pllFreq, SI5351_CLK2);


    if ( mult != lastMult )
    {
      si5351.set_phase(SI5351_CLK0, 0);
      //si5351.set_phase(SI5351_CLK1, mult);
      //si5351->set_phase(SI5351_CLK2, mult);
      si5351.pll_reset(SI5351_PLLA);
      //si5351->pll_reset(SI5351_PLLB);
      si5351.update_status();

      lastMult = mult;
    }
}

void setupSynth()
{
  //si5351 = new Si5351( &wire );
  //si5351 = new Si5351( &wire );
  //si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);  //ne dela kristal
  si5351.init(SI5351_CRYSTAL_LOAD_8PF, SI5351_REF, CORRECTION);
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_4MA); // Set for 4ma power default is 2MA
  //si5351.drive_strength(SI5351_CLK1, SI5351_DRIVE_4MA); // Set for 4ma power default is 2MA
}