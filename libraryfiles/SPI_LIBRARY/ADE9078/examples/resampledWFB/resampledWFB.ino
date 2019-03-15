// Basic Test Demonstration for ADE9078 to read and report values (ADE9078_TEST)
//California Plug Load Research Center - 2019


#include <ADE9078.h>
#include <SPI.h>
#include <EEPROM.h>
#include <string.h>

//Architecture Control:
//Make sure you select in the ADE9078.h file the proper board architecture, either Arduino/AVR/ESP8266 or ESP32
//REMINDER: ONLY SELECT THE SINGLE OPTION FOR THE BOARD TYPE YOU ARE USING!


//Define ADE9078 object with hardware parameters specified
#define local_SPI_freq 115200  //Set SPI_Freq at 1MHz  - used for Arduino/AVR configuration
//#define local_SPI_freq 1000000  //Set SPI_Freq at 1MHz  - used for Arduino/AVR configuration
#define local_SS 10  //Set the SS pin for SPI communication as pin 10, typical on Arduino Uno and similar boards

#define WFB_ALL_SEGMENTS 512
#define BURST_MEMORY_BASE 0x800

struct FullResample
{
    int16_t Ia[WFB_ALL_SEGMENTS];
    int16_t Va[WFB_ALL_SEGMENTS];
    int16_t Ib[WFB_ALL_SEGMENTS];
    int16_t Vb[WFB_ALL_SEGMENTS];
    int16_t Ic[WFB_ALL_SEGMENTS];
    int16_t Vc[WFB_ALL_SEGMENTS];
    int16_t In[WFB_ALL_SEGMENTS];
};

const int readCount = WFB_ALL_SEGMENTS/WFB_RESAMPLE_SEGMENTS;

struct InitializationSettings* is = new InitializationSettings; //define structure for initialized values

ADE9078 myADE9078(local_SS, local_SPI_freq, is); // Call the ADE9078 Object with hardware parameters specified, local variables are copied to private variables inside the class when object is created.


void setup() {

	//Need to add:
	//1)Read in EEPROM values, check the "Configured bit", if 1, display, calibration values available, load these calibration values to the ADE9078's RAM
	//2)If 0, then display that no calibration values are present and load default values (Tell user to run calibration firmware)
	//3) Proceed with load and operation of this demo
  Serial.begin(115200);
  delay(200);
  is->vAGain=1;
  is->vBGain=1;
  is->vCGain=1;

  is->iAGain=1;
  is->iBGain=1;
  is->iCGain=1;
  is->iNGain=1;

  is->powerAGain=1;
  is->powerBGain=1;
  is->powerCGain=1;

  //Use these settings to configure wiring configuration at stertup
  //FYI: B010=2 in DEC
  //FYI: B100 = 4 in DEC

	//4 Wire Wye configuration - non-Blondel compliant:
	is->vConsel=0;
	is->iConsel=0;

	//Delta, Blondel compliant:
	//is->vConsel=4; //byte value of 100,
	//is->iConsel=0;


	 //Please continue for all cases

  SPI.begin();
  delay(200);
  myADE9078.initialize(); //Call initialization of the ADE9078 withe default configuration plus options specified
	//EEPROMInit()  //call only once on a virgin chip to "partition" EEPROM for the input type expected moving forward
	//load_data_allfields();  //load EEPROM values



}


void loop() {


    FullResample fullResample;// = (FullResample*)malloc(sizeof(FullResample));

    myADE9078.configureWFB(1); // begin

    delay(300);
    // int samplingDone = 0;
    // while (samplingDone == 0)
    // {
    //   if (myADE9078.isDoneSampling())
    //   {
    //       samplingDone = 1;
    //   }
    // }

    Serial.println("Finished sampling. Reading beginning.");


    for (int i=0; i < 8; ++i)
    {
        Serial.println("Outer loop starting");

        uint16_t burstMemoryOffset = i* 4 * 64; // each segment is 16 bytes, we read in sets of 64
        uint16_t startingAddress = BURST_MEMORY_BASE + burstMemoryOffset;
        myADE9078.spiBurstResampledWFB(startingAddress);

        Serial.print("Outer Loop: ");
        Serial.println(i);

        //memcpy
        for (int seg=0; seg < 64; ++seg)
        {
          // int s = sizeof(FullResample);
          // Serial.print("Size of FullResample: ");
          // Serial.println(s); // should print 7168, and it is.

          Serial.print("Loop position: ");
          Serial.print(i);
          Serial.print(", ");
          Serial.println(seg);
          Serial.print("Segment Offset: ");
          int segOffSet = seg + (i*64);
          Serial.println(segOffSet);

          myADE9078.readIrms();
          Serial.print("A, B, C rms (I): ");
          Serial.print(myADE9078.lastReads.irms.a);
          Serial.print(" ");
          Serial.print(myADE9078.lastReads.irms.b);
          Serial.print(" ");
          Serial.print(myADE9078.lastReads.irms.c);
          Serial.print(" ");


          Serial.print("Ia,Va, Ib,Vb, Ic,Vc, In: ");
          Serial.print(myADE9078.lastReads.resampledData.Ia[seg]); Serial.print(" ");
          Serial.print(myADE9078.lastReads.resampledData.Va[seg]); Serial.print(" ");
          Serial.print(myADE9078.lastReads.resampledData.Ib[seg]); Serial.print(" ");
          Serial.print(myADE9078.lastReads.resampledData.Vb[seg]); Serial.print(" ");
          Serial.print(myADE9078.lastReads.resampledData.Ic[seg]); Serial.print(" ");
          Serial.print(myADE9078.lastReads.resampledData.Ic[seg]); Serial.print(" ");
          Serial.println(myADE9078.lastReads.resampledData.In[seg]);

          // fullResample.Ia[segOffSet] = myADE9078.lastReads.resampledData.Ia[seg];
          // fullResample->Va[segOffSet] = myADE9078.lastReads.resampledData.Va[seg];
          // fullResample->Ib[segOffSet] = myADE9078.lastReads.resampledData.Ib[seg];
          // fullResample->Vb[segOffSet] = myADE9078.lastReads.resampledData.Vb[seg];
          // fullResample->Ic[segOffSet] = myADE9078.lastReads.resampledData.Ic[seg];
          // fullResample->Vc[segOffSet] = myADE9078.lastReads.resampledData.Vc[seg];
          // fullResample->In[segOffSet] = myADE9078.lastReads.resampledData.In[seg];
        }
    }

    myADE9078.configureWFB(0); // per datasheet, have to set a certain bit to 0 to restart with stop on full
    // might need to write 23rd bit in STATUS0 to 0?

    Serial.println("Finished reading from ADE chip.");

    // for (int i=0; i < WFB_ALL_SEGMENTS; ++i)
    // {
    //     Serial.print(i);
    //     Serial.print("Ia, Va, Ib, Vb, Ic, Vc, In: ");
    //     Serial.print(fullResample.Ia[i]);
    //     Serial.print(fullResample.Va[i]);
    //     Serial.print(fullResample.Ib[i]);
    //     Serial.print(fullResample.Vb[i]);
    //     Serial.print(fullResample.Ic[i]);
    //     Serial.print(fullResample.Vc[i]);
    //     Serial.print(fullResample.In[i]);
    //     Serial.println();
    // }
}
