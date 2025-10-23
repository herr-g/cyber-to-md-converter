// Cyberstick to Megadrive Converter (minimal version)
// for Arduino ATmega328p 5V 16MHz 

#define CPUHZ 16000000
#define PRS1  1
#define T1HZ  (CPUHZ/PRS1)

// Timer definitions
#define TIMER_1USEC    (unsigned int)(0x10000-(T1HZ/1000000))
#define TIMER_12USEC   (unsigned int)(0x10000-(T1HZ/83333))
#define TIMER_22USEC   (unsigned int)(0x10000-(T1HZ/45454))
#define TIMER_4USEC    (unsigned int)(0x10000-(T1HZ/250000))

// Cyberstick Input (read from Cyberstick)
#define CYBERDAT_PORT PORTC
#define CYBERDAT_DDR  DDRC
#define CYBERDAT_PIN  PINC
#define CYBERDAT_MASK 0xf

#define CYBER_PORT PORTB
#define CYBER_DDR  DDRB
#define CYBER_PIN  PINB 
#define CYBER_BITACK (1<<5)  // ACK on pin B5
#define CYBER_BITREQ (1<<3)  // REQ on pin B3
#define CYBER_REQ_H  CYBER_PORT|=CYBER_BITREQ
#define CYBER_REQ_L  CYBER_PORT&=~CYBER_BITREQ
#define UNTIL_CYBER_ACK_H  while((CYBER_PIN&CYBER_BITACK)==0)
#define UNTIL_CYBER_ACK_L  while(CYBER_PIN&CYBER_BITACK)

// Megadrive Output (output to MD)
#define MD_PORT      PORTD
#define MD_DDR       DDRD
#define MD_DAT_SHIFT 2
#define MD_BITUDLR   (0b1111<<2)  // Up/Down/Left/Right bits
#define MD_BITALL    (0b111111<<2)
#define MD_BITLH     (1<<6)
#define MD_BITACK    (1<<7)
#define MD_LH_L      MD_PORT&=~MD_BITLH
#define MD_LH_H      MD_PORT|=MD_BITLH
#define MD_ACK_L     MD_PORT&=~MD_BITACK
#define MD_ACK_H     MD_PORT|=MD_BITACK

#define REQ_PORT PORTB
#define REQ_PIN  PINB
#define REQ_DDR  DDRB
#define REQ_BIT  (1<<0)
#define UNTIL_REQ_H while((REQ_PIN&REQ_BIT)==0)
#define UNTIL_REQ_L while((REQ_PIN&REQ_BIT)!=0)

// Initialize timer
void timer_init(void) {
  TCCR1A = 0;
  TCCR1B = 1;  // Prescaler /1
}

// Wait microseconds
void timer_uswait(unsigned int limitcnt) {
  TCNT1 = limitcnt;
  TIFR1 |= (1 << TOV1);
  while(!(TIFR1 & (1 << TOV1)));
}

// Wait milliseconds
void timer_delay(int milisec) {
  while(milisec--) {
    delayMicroseconds(1000);
  }
}

// Initialize Megadrive port
void mdport_init(void) {
  REQ_DDR &= ~REQ_BIT;
  REQ_PORT |= REQ_BIT;
  MD_DDR |= MD_BITALL;
  MD_PORT |= MD_BITALL;
}

// Main function: Cyberstick to Megadrive
void cyber_to_megadrive(void) {
  int datanum;
  unsigned char temp;
  unsigned char cyberbuff[11];
  unsigned char sendbuf[11];

  // Configure Cyberstick input pins
  CYBERDAT_DDR  &= ~CYBERDAT_MASK;
  CYBERDAT_PORT |= CYBERDAT_MASK;
  CYBER_DDR |= CYBER_BITREQ;
  CYBER_DDR &= ~CYBER_BITACK;
  CYBER_PORT |= CYBER_BITACK;
  CYBER_REQ_H;
  
  // Configure Megadrive output pins
  mdport_init();
  timer_delay(10);
  
  while(1) {
    cli();  // Disable interrupts
    
    // Read data from Cyberstick (11 bytes)
    CYBER_REQ_L;
    timer_uswait(TIMER_1USEC);
    
    for(datanum=0; datanum<11; datanum++) {
      UNTIL_CYBER_ACK_L;
      CYBER_REQ_H;
      cyberbuff[datanum] = CYBERDAT_PIN & CYBERDAT_MASK;
      UNTIL_CYBER_ACK_H;
    }
    sei();  // Enable interrupts

    // Remap data for Megadrive
    sendbuf[0] = cyberbuff[1];   // E1/E2/START/SELECT
    sendbuf[1] = cyberbuff[0];   // A/B/C/D
    sendbuf[2] = cyberbuff[3];   // CH1 High
    sendbuf[3] = cyberbuff[2];   // CH0 High
    sendbuf[4] = cyberbuff[5];   // CH3 High
    sendbuf[5] = cyberbuff[4];   // CH2 High
    sendbuf[6] = cyberbuff[7];   // CH1 Low
    sendbuf[7] = cyberbuff[6];   // CH0 Low
    sendbuf[8] = cyberbuff[9];   // CH3 Low
    sendbuf[9] = cyberbuff[8];   // CH2 Low
    sendbuf[10] = 0xf;

    cli();  // Disable interrupts
    
    // Wait for Megadrive REQ and send data
    UNTIL_REQ_H;
    UNTIL_REQ_L;
    timer_uswait(TIMER_4USEC);

    for(datanum=0; datanum<11; datanum++) {
      // Toggle LH signal
      if((datanum & 1)==0) {
        MD_LH_L;
      } else {
        MD_LH_H;
      }
      
      // Output data to port
      temp = (MD_PORT & ~MD_BITUDLR);
      temp |= (sendbuf[datanum] << MD_DAT_SHIFT);
      MD_PORT = temp;
      
      // Send ACK pulse
      MD_ACK_L;
      timer_uswait(TIMER_12USEC);
      MD_ACK_H;
      
      // Wait time
      if((datanum & 1)==0) {
        timer_uswait(TIMER_4USEC);
      } else {
        timer_uswait(TIMER_22USEC);
      }
    }
    
    MD_PORT |= MD_BITALL;
    MD_LH_L;
    sei();  // Enable interrupts
  }
}

void setup() {
  timer_init();
  cyber_to_megadrive();
}

void loop() {
  // Never reached, as cyber_to_megadrive() is an endless loop
}