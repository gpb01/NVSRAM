/*

   Sample usage of NVSRAM library with different data types.
   Guglielmo Braguglia - Nov. 2020

*/

#include <string.h>
#include <NVSRAM.h>

// istanziate the NVSRAM class using pin 10 as CS pin

NVSRAM myNVSRAM ( 10 );

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
   Serial.println ( "Test begin ..." );
   //
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
   b = 0;
   b = myNVSRAM.read ( 10 );
   Serial.print ( "Byte reading with read (expected 127): " );
   Serial.println ( b );
   b = 0;
   b = myNVSRAM[ 10 ];
   Serial.print ( "Byte reading with [] (expected 127): " );
   Serial.println ( b );
   //
   myNVSRAM.put ( 20, f );
   f = 0.0;
   myNVSRAM.get ( 20, f );
   Serial.print ( "Float reading with get (expected 127.12): " );
   Serial.println ( f );
   //
   myNVSRAM.put ( 30, i );
   i = 0;
   myNVSRAM.get ( 30, i );
   Serial.print ( "Integer reading with get (expected -127): " );
   Serial.println ( i );
   //
   myNVSRAM.put ( 40, a );
   memset ( &a, 0x00, sizeof ( a ) );
   myNVSRAM.get ( 40, a );
   Serial.print ( "Array reading with get (expected 0..9): " );
   for ( i = 0; i < 10; i++ ) {
      Serial.print ( a[i] );
      Serial.print ( ", " );
   }
   Serial.println ();
   //
   myNVSRAM.put ( 50, s );
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
   Serial.print ( "CRC16 of first 100 bytes: " );
   Serial.println ( block_crc );
   //
   Serial.println ( );
   Serial.println ( "Test completed." );
}

void loop() {

}
