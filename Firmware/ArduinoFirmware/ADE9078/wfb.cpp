//copy this stuff indo ADE9078.cpp

/******************************************************************************/
/********************************WFB FUNCTIONS*********************************/
/******************************************************************************/

//Setting up the WFB
//0. readOutWFBSPI() - set WF_IN_EN to 1 to read out through SPI
//1. stopFillingBuffer - this writes WFB_CAP_EN to 0
//2. choose what type of data you want to output
//3. select the mode you want the WFB in
//4. choose what channels you want to read from

//startFillingBuffer - this writes WFB_CAP_EN to 1


void ADE9078::readOutWFBSPI(){
  uint16_t addressContent = spiRead16(WFB_CFG_16);
  addressContent = (addressContent | (0b1 << 12));
  spiWrite16(WFB_CFG_16, addressContent);
  Serial.println("reading out of WFB using SPI");
}

//Start the WFB
void ADE9078::startFillingBuffer(){
  uint16_t addressContent = spiRead16(WFB_CFG_16);
  addressContent = (addressContent | (0b1 << 4));  //set WF_CAP_EN bit to 1 in the WFB_CFG register to start filling the buffer from Address 0x800.
  spiWrite16(WFB_CFG_16, addressContent);
  Serial.println("filling WFB from address 0x800 (first register of page 1)");
}
// Stop the WFB
void ADE9078::stopFillingBuffer(){
  uint16_t addressContent = spiRead16(WFB_CFG_16);
  addressContent = (addressContent & ~(0b1 << 4));  //set WF_CAP_EN bit to 0 in the WFB_CFG register
  spiWrite16(WFB_CFG_16, addressContent);
  Serial.println("waveform buffer is stopped");
  Serial.print("last page to be filled was: ")
  whichPageIsFull();
}
//see p.67 of datasheet
//fill WFB with data from Sinc4
void ADE9078::sinc4Ouput(){
  uint16_t addressContent = spiRead16(WFB_CFG_16);
  addressContent = (addressContent & ~(0b1 << 8));//WF_SRC bit to 00
  addressContent = (addressContent & ~(0b1 << 9));
  spiWrite16(WFB_CFG_16, addressContent);
}

//fill WFB with data from Sinc4+IIR LPF
void ADE9078::sinc4_IIR_LPFOutput(){
  uint16_t addressContent = spiRead16(WFB_CFG_16);
  addressContent = (addressContent & ~(0b1 << 8));//WF_SRC bit to 10
  addressContent = (addressContent | (0b1 << 9));
  spiWrite16(WFB_CFG_16, addressContent);
}

//fill WFB with data from Waveforms processed by DSP
void ADE9078::DSPwaveformOutput(){
  uint16_t addressContent = spiRead16(WFB_CFG_16);
  addressContent = (addressContent | (0b1 << 8));//WF_SRC bit to 11
  addressContent = (addressContent | (0b1 << 9));
  spiWrite16(address, addressContent);
}

//STOP WHEN BUFFER IS FULL MODE
//mode 0
void ADE9078::stopWhenBufferIsFull(){
  uint16_t addressContent = spiRead16(WFB_CFG_16);
  addressContent |= (0b1<<5); //WF_CAP_SEL = 1
  addressContent &= ~(0b1<<6); //WF_MODE bits = 00
  addressContent &= ~(0b1<<7);
  spiWrite16(WFB_CFG_16, addressContent);
  Serial.println("Mode 0: stop when buffer is full, notification given when buffer is full");
  isPageFull(15);
}


//Waveform Buffer Filling Indication—Fixed Data Rate Samples p.68
//set bits in WFB_PG_IRQEN to get a notification when a page is full
void ADE9078::isPageFull(int page){
  uint16_t addressContent = spiRead16(WFB_PG_IRQEN_16);
  addressContent |= (0b1<<page);
  spiWrite16(WFB_PG_IRQEN_16, addressContent);
  Serial.print("You will be notified when this page is full: ");
  Serial.println(page);
}

void ADE9078::whichPageIsFull(){
  uint16_t whichPage = spiRead16(WFB_TRG_STAT_16);
  whichPage = whichPage & 0xF000; //make all other bits 0 except Bits[15:12]
  whichPage >> 12;
  Serial.print("this page is full: ");
  Serial.println(whichPage);
}

//checks PAGE_FULL (bit 17 of STATUS0) and does an inturrupt event

//CONTINUOUS FILL MODE
//mode 1: stop filling on trigger
//It is recommended to use this mode to analyze the ADC samples leading up to an event of interest.
void ADE9078::stopFillingOnTrigger(){
  uint16_t nuckyTheBucky = spiRead16(WFB_CFG_16);
  nuckyTheBucky |= (0b1<<5); //WF_CAP_SEL = 1
  nuckyTheBucky |= (0b1<<6); //WF_MODE bits = 01
  nuckyTheBucky &= ~(0b1<<7);
  spiWrite16(WFB_CFG_16,nuckyTheBucky);
  Serial.println("Mode 1: stop filling on trigger");
}

//still have to implement rest of mode 1 (all the triggers) and implement mode 2 & 3 but i don't think we need it for now.

/*****************Set up SPI BURST READ FOR WFB********************************/

//read fixed Samples
void ADE9078::readFixedRateData(){
  uint16_t addressContent = spiRead16(WFB_CFG_16);
  addressContent |= (0b1<<5);
  spiWrite16(WFB_CFG_16, addressContent);
  Serial.println("reading fixed rate data");
}

//read resampled data
void ADE9078::readResampledData(){
  uint16_t addressContent = spiRead16(WFB_CFG_16);
  addressContent &= ~(0b1<<5);
  spiWrite16(WFB_CFG_16, addressContent);
  Serial.println("reading resampled data");
}

//see p.71
void ADE9078::burstAllChannels(){
  uint16_t writeValue = spiRead16(WFB_CFG_16);
  for (i = 0, i == 3, i++){
    writeValue = writeValue & ~(0b1<<i);
  }
  spiWrite16(WFB_CFG_16, writeValue);
  Serial.println("burst read all channels");
}
//implement the rest of the burst read options; we don't need them right now though.
