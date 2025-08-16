# ESP32S3_SDR_Guition
ESP32S3 SDR radio with a resistive touch screen from Guition and SSB, AM and FM demodulation
Got inspiration from Cvarc-Xtal project published on https://github.com/Cvarc-Xtal/ESP32-SDR-TRX
He made a very good basis project for the development of SDR transceiver using ESP32 I2S capabilities and added a comrehencive graphics which is popular in SDR radios.
I have modified code and added some new functionality and to imroove the AM demodulation while also added FM demodulation.
My goal was to make fully functional receiver with minimal hardware involvement.

Breaf functionality description:
- Audio sampling frequency: 48kHz
- Receiving frequency range : 400kHz....50Mhz (RX only for now)
- Demodulation: LSB, USB, AM, NFM
- Filter bandwith: Adjustable "on the fly" from 600Hz to 5kHz by 200Hz steps
- Adjustable freqency step: 100, 1k, 5k, 10k, 25k, 100k, 1M
- Tunable RF input : Resonance and Gain
- Touch control for everithing; no hardware switches needed except fo complete radio ON/OFF
- Graphics: Signal meter, Amplitude specter, Waterfall specter, Current frequency with larger fonts, Others like status and control pads.

Hardware needed:
- Guition JC4827W543 R ; consider it is a resistive touch version of display
- PCM1808 I2s ADC board 
- Si5351 board
- Tayloe mixer; I used https://qrp-labs.com/receiver.html
- Some DIY circuits for Voltage stabilizers and Tunable RF input 
- Housing; I used housing from an abandoned PC power supply and added my own 3D print for cover around display.

Sofware:
- Arduino IDE 2.3.6
- Please find Library dependancy out from the source code

Link to Demo video: https://youtu.be/Y9zZ3sQKupU

To do:
- Add TX functionality
