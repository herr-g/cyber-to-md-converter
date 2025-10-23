# Sharp Cyber Stick / Micomsoft XE-1AJ to Mega Drive Controller Converter

## Introduction
In search for a solution to connect my Sharp Cyber Stick to a Mega Drive I came across this Pad-Converter Project: nicotakuya/multiway-pad-converter.
This solution uses a standard Arduino Microcontroller (ATmega328P), so the code can easily be read, understood and changed.
Since I just wanted a dedicated MD converter for the Cyber Stick I stripped down the original code to that single protocol converter function for the Cyber Stick.

## Compatibility
In its original form it is perfectly compatible for playing Afterburner II. However I couldn't get the controller to be detected by Galaxy Force II, so there's obviously still room for future improvement.
Reference to analogue compatible Mega Drive games: https://segaretro.org/XE-1_AP#Compatible_Games


## Cyber Stick to Mega Drive Converter - Pin Assignments 

### Cyber Stick Input (from Cyber Stick to Arduino)

| Arduino Pin | Function      | Cyber Stick Connection |

|-------------|---------------|-----------------------|

| A0 (PC0)    | Data Bit 0    | Pin 1                 |

| A1 (PC1)    | Data Bit 1    | Pin 2                 |

| A2 (PC2)    | Data Bit 2    | Pin 3                 |

| A3 (PC3)    | Data Bit 3    | Pin 4                 |

| D11 (PB3)   | REQ (Output)  | Pin 8                 |

| D13 (PB5)   | ACK (Input)   | Pin 7                 |

| GND         | Ground        | Pin 9                 |

| 5V          | +5V           | Pin 5                 |


/- The Cyber Stick sends 4-bit data to A0-A3 <br />
/- The Arduino communicates bidirectionally with the Cyber Stick via REQ (D11) and ACK (D13) <br />

### Mega Drive Output (from Arduino to Mega Drive)

| Arduino Pin | Function      | MD Controller Port Pin |

|-------------|---------------|------------------------|

| D2 (PD2)    | Data Bit 0    | Pin 1                  |

| D3 (PD3)    | Data Bit 1    | Pin 2                  |

| D4 (PD4)    | Data Bit 2    | Pin 3                  |

| D5 (PD5)    | Data Bit 3    | Pin 4                  |

| D6 (PD6)    | LH Signal     | Pin 6                  |

| D7 (PD7)    | ACK Signal    | Pin 9                  |

| D8 (PB0)    | REQ (Input)   | Pin 7 (Request from MD)|

| GND         | Ground        | Pin 8                  |

| 5V          | Power         | Pin 5                  |

/- The Arduino sends the converted data to the MD via D2-D7 <br />
/- D8 receives the request signal from the MD <br />
