/*

   Sample usage of NVSRAM library with different data types.
   Guglielmo Braguglia - Nov. 2020

*/

#include <string.h>
#include <NVSRAM.h>

#define  WR_RD  // comment this line to READ ONLY from the NVSRAM, leave uncommented to WRITE/READ

// Istanziate the NVSRAM class using pin 10 as CS pin, but DON'T initialize the SPI bus
// because after, in the program, the bus will be initialized with custom paramenters.

NVSRAM myNVSRAM ( 10, false );

void setup() {
   float f = 127.12;
   int   i = -127;
   byte  b = 127;
   byte  a[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
   struct myStruct {
      float fl = 127.12;
      int   in = -127;
      byte  by = 127;
      byte  ar[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
   } s;
   //
   unsigned long tStart, tEnd;
   unsigned int block_crc = 0;
   //
   delay ( 500 );
   Serial.begin ( 115200 );
#ifdef WR_RD
   Serial.println ( "Test WR_RD begin ..." );
#else
   Serial.println ( "Test only RD begin ..." );
#endif

   // Before using the NVSRAM is necessary to initialize the SPI bus and to configure the NVSRAM chip.
   
   SPI.begin();
   SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
   myNVSRAM.begin();
   
   //
#ifdef WR_RD
   tStart = micros();
   myNVSRAM.erase();
   tEnd = micros();
   Serial.print ( "Memory erased in " );
   Serial.print ( tEnd - tStart );
   Serial.print ( ", microSeconds; num. of bytes erased: " );
   Serial.print ( myNVSRAM.length() );
   Serial.print ( ", microSeconds per byte≈ " );
   Serial.println ( ( tEnd - tStart ) / myNVSRAM.length() );
   Serial.println( );
   //
   myNVSRAM.write ( 10, b );
#endif
   b = 0;
   b = myNVSRAM.read ( 10 );
   Serial.print ( "Byte reading with read (expected 127): " );
   Serial.println ( b );
   b = 0;
   b = myNVSRAM[ 10 ];
   Serial.print ( "Byte reading with [] (expected 127): " );
   Serial.println ( b );
   //
#ifdef WR_RD
   myNVSRAM.put ( 20, f );
#endif
   f = 0.0;
   myNVSRAM.get ( 20, f );
   Serial.print ( "Float reading with get (expected 127.12): " );
   Serial.println ( f );
   //
#ifdef WR_RD
   myNVSRAM.put ( 30, i );
#endif
   i = 0;
   myNVSRAM.get ( 30, i );
   Serial.print ( "Integer reading with get (expected -127): " );
   Serial.println ( i );
   //
#ifdef WR_RD
   myNVSRAM.put ( 40, a );
#endif
   memset ( &a, 0x00, sizeof ( a ) );
   myNVSRAM.get ( 40, a );
   Serial.print ( "Array reading with get (expected 0..9): " );
   for ( i = 0; i < 10; i++ ) {
      Serial.print ( a[i] );
      Serial.print ( ", " );
   }
   Serial.println ();
   //
#ifdef WR_RD
   myNVSRAM.put ( 50, s );
#endif
   memset ( &s, 0x00, sizeof ( s ) );
   myNVSRAM.get ( 50, s );
   Serial.println ( "Structure reading with get ..." );
   Serial.print ( "  float value (expected 127.12): " );
   Serial.println ( s.fl );
   Serial.print ( "  integer value (expected -127): " );
   Serial.println ( s.in );
   Serial.print ( "  byte value (expected 127): " );
   Serial.println ( s.by );
   Serial.print ( "  byte array value (expected 0..9): " );
   for ( i = 0; i < 10; i++ ) {
      Serial.print ( s.ar[i] );
      Serial.print ( ", " );
   }
   Serial.println ( );
   Serial.println ( );
   //
   block_crc = myNVSRAM.crc ( 0, 100 );
   Serial.print ( "CRC16 of first 100 bytes (expected 17588): " );
   Serial.println ( block_crc );
   //
   Serial.println ( );
   Serial.println ( "Test completed." );
   //
   SPI.endTransaction();
}

void loop() {

}
