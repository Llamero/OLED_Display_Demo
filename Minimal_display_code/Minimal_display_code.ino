//This code was written by Benjamin Smith
//It is publically available at: https://github.com/Llamero/OLED_Display_Demo
//This is open-source and licensed under Apache 2.0 https://www.apache.org/licenses/LICENSE-2.0

//WIRING:
//Pin 9 - Display CS
//Pin 11 - SPI:MOSI (data from Arduino to display)
//Pin 12 - SPI:MISO (data from display to Arduino)
//Pin 13 - SPI:SCK (clock signal)

//Functions:
//printString(char [] or "Text") - Print text at current cursor position
//command(byte) - Send a numerical command to display, such as move cursor, toggle display on and off, etc
//bitbang(byte) - Sends one byte of data to the display via SPI
//RsRW(boolean RS, boolean RW) - Sends the two header bits via SPI
//initializeDisplay() - Send initialization sequence from datasheet: http://www.newhavendisplay.com/specs/NHD-0420DZW-AB5.pdf

//Sample commands: (see datasheet for full list)
//1 - Clear display
//2 - Move cursor to home position R1, C1
//8 - Turn off display
//12 - Turn on display
//128 + address - Move cursor to point on screen - see p. 10 of datasheet for table of positions

const uint8_t d = 2; //ms delay for display SPI between commands
const char arrow[] = {B11110110, '\0'}; //Draw special arrow character.  '\0' is needed to keep as valid str->char array.
uint8_t a = 0; //Counter for indexing arrows in loop


void setup() {
  //Setup PortB (pins 8-13) for SPI
  DDRB |= B00101010; //Set CS pin 9 and SPI pins 11, 13 as output
  DDRB &= B11101011; //Set CS pin 10 and SPI pin 12  as input
  PORTB |= B00000010; //Set CS pin 9 high to block SPI to display
  initializeDisplay(); //Send initialization sequence from datasheet: http://www.newhavendisplay.com/specs/NHD-0420DZW-AB5.pdf

  //Show splash screen demo text
  command(2); //Move cursor to home position
  printStr("This is row 1."); //Print text on line 1
  command(128 + 0x40); //Move cursor to R2, C1
  printStr("This is row 2.");
  command(128 + 0x14); //Move cursor to R3, C1
  printStr("This is row 3.");
  command(128 + 0x54); //Move cursor to R4, C1
  printStr("This is row 4.");
}

//Raster arrows across display
void loop() {
    //Draw arrows
    command(128 + 0x0E + a); //Move cursor
    printStr(arrow); //print arrow character
    command(128 + 0x4e + a); //Move cursor
    printStr(arrow); //print arrow character
    command(128 + 0x22 + a); //Move cursor
    printStr(arrow); //print arrow character
    command(128 + 0x62 + a); //Move cursor
    printStr(arrow); //print arrow character 

    delay(200);

    //Clear arrows
    command(128 + 0x0e + a); //Move cursor
    printStr(" "); //print space character
    command(128 + 0x4e + a); //Move cursor
    printStr(" "); //print space character
    command(128 + 0x22 + a); //Move cursor
    printStr(" "); //print space character
    command(128 + 0x62 + a); //Move cursor
    printStr(" "); //print space character 

    //Increment counter
    a++;
    if(a==6) a=0;
}

//Function for sending bytes via SPI
void bitbang (byte ptr) {
    for(int b = 0; b<8; b++){
      PORTB &= B11011111; //Set CLK low
      if (ptr & 0x80) PORTB |= B00001000; //Set MOSI pin to data state
      else PORTB &= B11110111;
      PORTB |= B00100000; //Set CLK high to latch data
      ptr <<= 1; //Sift to next bit
    }
}

//Function for parsing char arrays, and sending each character to the diplay
void printStr(char str[]){
  //Get the length of the char array
  uint8_t strLen = strlen(str);
  
  //Start communication by pulling CS pin (9) low
  PORTB &= B11111101;
      
  //Initialize communication settings with header
  RsRW(1,0);

  //Since Arduino and display both use ASCII, send ASCII char bytes one at a time
  for(int a = 0; a<strLen; a++) bitbang(str[a]);
  
  //end communication by pulling CS pin (9) high
  PORTB |= B0000010; 
} 

//Function for sending commands to the display
void command (uint8_t command){
  //Start communication by pulling CS pin (9) low
  PORTB &= B11111101;
  
  //Initialize communication settings with header
  RsRW(0,0);
  bitbang(command);  //send command

  //end communication by pulling CS pin (9) high
  PORTB |= B0000010;  
  delay(d);
}

//Function to bit bang the RS and R/W leading bits to the display
void RsRW(boolean RS, boolean RW){
  PORTB &= B11011111; //Set CLK low
  if(RS) PORTB |= B00001000; //Set MOSI pin to data state
  else   PORTB &= B11110111;
  PORTB |= B00100000; //Set CLK high to latch data

  PORTB &= B11011111; //Set CLK low
  if(RW) PORTB |= B00001000; //Set MOSI pin to data state
  else   PORTB &= B11110111;
  PORTB |= B00100000; //Set CLK high to latch data
}

void initializeDisplay(){
  int a = 0;

  //Clear display and reset settings
  //Start communication by pulling CS pin (9) low
  PORTB &= B11111101;
  
  //Initialize communication settings with header
  RsRW(0,0);
  bitbang(B00111011);  //Set function - choose western european table 2
  delay(d);
  RsRW(0,0);
  bitbang(B00001000); //Turn off display
  delay(d);
  RsRW(0,0);
  bitbang(B00000001); //Clear display    
  delay(d);
  RsRW(0,0);
  bitbang(B00000110); //Entry mode set to increment  
  delay(d);
  RsRW(0,0);
  bitbang(B00000010); //Home Command
  delay(d);
  RsRW(0,0);
  bitbang(B00001100); //Display On - no cursor

  //end communication by pulling CS pin (9) high
  PORTB |= B0000010;  
  delay(d);
}
