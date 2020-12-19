/*

   Sample usage of NVSRAM library with different Arduino data types.
   Guglielmo Braguglia - Dec. 2020

*/

#include <string.h>
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

      b (one byte)   start at address 0x0000
      i (two bytes)  start at address 0x0001
      f (four bytes) start at address 0x0003
      a (ten bytes)  start at address 0x0007
      s (17 bytes)   start at address 0x0011

   */
   byte  b = 127;
   int   i = -127;
   float f = 127.12;
   byte  a[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
   struct myStruct {
      byte  by = 127;
      int   in = -127;
      float fl = 127.12;
      byte  ar[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
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

   myNVSRAM.write ( 0x0000, b );

#endif

   b = 0;
   b = myNVSRAM.read ( 0x0000 );
   Serial.print ( "Byte b reading with read (expected 127): " );
   Serial.println ( b );

   b = myNVSRAM[ 0 ];
   Serial.print ( "Byte b reading with [] (expected 127): " );
   Serial.println ( b );

#ifdef WR_RD

   myNVSRAM.put ( 0x0001, i );

#endif

   i = 0;
   myNVSRAM.get ( 0x0001, i );
   Serial.print ( "Integer i reading with get (expected -127): " );
   Serial.println ( i );

#ifdef WR_RD

   myNVSRAM.put ( 0x0003, f );

#endif

   f = 0.0;
   myNVSRAM.get ( 0x0003, f );
   Serial.print ( "Float f reading with get (expected 127.12): " );
   Serial.println ( f );

#ifdef WR_RD

   myNVSRAM.put ( 0x0007, a );

#endif

   memset ( a, 0x00, sizeof ( a ) );
   myNVSRAM.get ( 0x0007, a );
   Serial.print ( "Array a reading with get (expected 0..9): " );
   for ( j = 0; j < 10; j++ ) {
      Serial.print ( a[j] );
      if ( j < 9 ) Serial.print ( ", " );
   }
   Serial.println ( );

#ifdef WR_RD

   myNVSRAM.put ( 0x0011, s );

#endif

   memset ( &s, 0x00, sizeof ( s ) );
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
