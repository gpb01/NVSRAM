/*

   Sample usage of NVSRAM library with different Arduino data types.
   Guglielmo Braguglia - Dec. 2020
   
   Please, verify that you have selected the correct memory chip
   you are using, inside the library.

*/

#include <NVSRAM.h>

/*
   Leave the following #define uncommented if you want to do a complete
   WRITE/READ BACK cycle. Comment if you want only to da a READ BACK cycle
   to verify battery data retention
*/

#define WR_RD

/*
   Lave the following #define uncommented if you want to use the automatic
   default SPI bus initialization. Comment if you want to use a manual SPI
   bus initialization with specific parameters
*/

#define SPI_INIT

/*
   ------------------------------------------------------------------------
*/

#ifdef SPI_INIT

NVSRAM myNVSRAM ( 10 );

#else

NVSRAM myNVSRAM ( 10, false );

#endif

void setup ( void ) {
   /*
      Declare many different type of variables/structure to save/restore
      Address is the NVSRAM memory address used to store data

      b (one byte)   start at address 0x0000
      i (two bytes)  start at address 0x0001
      f (four bytes) start at address 0x0003
      a (ten bytes)  start at address 0x0007
      s (17 bytes)   start at address 0x0011

   */
   byte  b = 0;
   int   i = 0;
   float f = 0.0;
   byte  a[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
   struct myStruct {
      byte  by = 0;
      int   in = 0;
      float fl = 0.0;
      byte  ar[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
   } s;

   unsigned long tStart = 0, tEnd = 0;
   unsigned int  block_crc = 0;
   byte          j;

   delay ( 500 );
   Serial.begin ( 115200 );
 
#ifndef SPI_INIT

   SPI.begin();
   SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
   myNVSRAM.begin();   

#endif

#ifdef WR_RD

   Serial.println ( "Demo program WRITE/READ begins ..." );

   /*
      Erase all the NVSRAM memory
   */

   tStart = micros();
   myNVSRAM.erase();
   tEnd = micros();
   Serial.print ( "Memory erased in " );
   Serial.print ( tEnd - tStart );
   Serial.print ( " µsec; " );
   Serial.print ( "Bytes erased: " );
   Serial.print ( myNVSRAM.length() );
   Serial.print ( ", µsec per byte≈ " );
   Serial.println ( ( tEnd - tStart ) / myNVSRAM.length() );
   Serial.println( );

#else

   Serial.println ( "Demo program READ ONLY begins ..." );

#endif

#ifdef WR_RD

   b = 127;
   myNVSRAM.write ( 0x0000, b );

#endif

   b = myNVSRAM.read ( 0x0000 );
   Serial.print ( "Byte b reading with read (expected 127): " );
   Serial.println ( b );

   b = myNVSRAM[ 0 ];
   Serial.print ( "Byte b reading with [] (expected 127): " );
   Serial.println ( b );

#ifdef WR_RD

   i = -127;
   myNVSRAM.put ( 0x0001, i );

#endif

   myNVSRAM.get ( 0x0001, i );
   Serial.print ( "Integer i reading with get (expected -127): " );
   Serial.println ( i );

#ifdef WR_RD

   f = 127.12;
   myNVSRAM.put ( 0x0003, f );

#endif

   myNVSRAM.get ( 0x0003, f );
   Serial.print ( "Float f reading with get (expected 127.12): " );
   Serial.println ( f );

#ifdef WR_RD

   for ( j = 0; j < 10; j++) a[j] = j;
   myNVSRAM.put ( 0x0007, a );

#endif

   myNVSRAM.get ( 0x0007, a );
   Serial.print ( "Array a reading with get (expected 0..9): " );
   for ( j = 0; j < 10; j++ ) {
      Serial.print ( a[j] );
      if ( j < 9 ) Serial.print ( ", " );
   }
   Serial.println ( );

#ifdef WR_RD

   s.by = 127;
   s.in = -127;
   s.fl = 127.12;
   for ( j = 0; j < 10; j++) s.ar[j] = j;
   myNVSRAM.put ( 0x0011, s );

#endif

   myNVSRAM.get ( 0x0011, s );
   Serial.println ( "Structure s reading with get ..." );
   Serial.print ( "  - byte by value (expected 127): " );
   Serial.println ( s.by );
   Serial.print ( "  - integer in value (expected -127): " );
   Serial.println ( s.in );
   Serial.print ( "  - float fl value (expected 127.12): " );
   Serial.println ( s.fl );
   Serial.print ( "  - byte array ar value (expected 0..9): " );
   for ( j = 0; j < 10; j++ ) {
      Serial.print ( s.ar[j] );
      if ( j < 9 ) Serial.print ( ", " );
   }
   Serial.println ( );
   Serial.println ( );

   block_crc = myNVSRAM.crc ( 0, 34 );
   Serial.print ( "CRC16 of first 34 bytes (expected 12711): " );
   Serial.println ( block_crc );
   Serial.println ( );
   Serial.println ( "Test completed." );

#ifndef SPI_INIT

   SPI.endTransaction();

#endif
}

void loop ( void ) {

}
