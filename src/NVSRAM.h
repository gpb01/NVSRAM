/*

  NVSRAM.h - Library for Microchip 23LCV512 (64 KB) and 23LCV1024 (128 KB)
             battery-backed SPI RAM memories.
  
  Copyright (c) 2020 Guglielmo Braguglia.  All right reserved.
  
  ---------------------------------------------------------------------
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
*/

#ifndef NVSRAM_h
#define NVSRAM_h

#include <Arduino.h>
#include <inttypes.h>
#include <util/crc16.h>
#include <SPI.h>

/*
  =====================================================================
  
          Uncomment ONLY the NVSRAM model you are compiling for
  
  =====================================================================
*/

#define NVSRAM_512
// #define NVSRAM_1024

/*
  =====================================================================
*/

#if defined( NVSRAM_512 ) && defined( NVSRAM_1024 )
#error "You can't define both NVSRAM_512 and NVSRAM_1024"
#endif
#if !defined( NVSRAM_512 ) && !defined( NVSRAM_1024 )
#error "You must define NVSRAM_512 or NVSRAM_1024"
#endif
#if defined( NVSRAM_512 )
#define NVSRAM_MAX   0xFFFF
#pragma message "INFO: You are compiling for 23LCV512 memory"
#elif defined( NVSRAM_1024 )
#define NVSRAM_MAX   0x0001FFFF
#pragma message "INFO: You are compiling for 23LCV1024 memory"
#endif

#define NVSRAM_READ  0x03
#define NVSRAM_WRITE 0x02
#define NVSRAM_RDMR  0x05
#define NVSRAM_WRMR  0x01


class NVSRAM {

   public:

      //Constructor

      NVSRAM ( uint8_t ss_Pin, bool initSPI = true ) {
         ssPin = ss_Pin;
         // initialize SS pin
         pinMode ( ssPin, OUTPUT );
         digitalWrite ( ssPin, HIGH );
         // if required, initialize SPI bus ...
         if ( initSPI ) {
            SPI.begin();
            // ... and after initialize 23LCV512 memory in "Sequential mode"
            begin();
         }
      };
      
      // NOTE: if SPI bus is initalized outside the constructor, a call to begin() method is mandatory
      
      void begin(void) {
         // initialize 23LCV512 memory in "Sequential mode"
         digitalWrite ( ssPin, LOW );
         SPI.transfer ( NVSRAM_WRMR );
         SPI.transfer ( 0x40 );
         digitalWrite ( ssPin, HIGH );
      }
            
      // function to retrive the memory size
      
      uint32_t length( void ) {
         return NVSRAM_MAX;
      }

#if defined( NVSRAM_512 )

      // *** For 23LCV512 64K x 8bit memory ***

      // erase function

      void erase ( void ) {
         uint16_t cnt;
         //
         digitalWrite ( ssPin, LOW );
         SPI.transfer ( NVSRAM_WRITE );
         SPI.transfer16 ( 0x0000 );
         for ( cnt = 0; cnt < NVSRAM_MAX; cnt++ )
            SPI.transfer ( 0x00 );
         digitalWrite ( ssPin, HIGH );
      }

      // read/write/update functions

      uint8_t operator[] ( uint16_t addr ) {
         return read ( addr );
      }

      uint8_t read ( uint16_t addr ) {
         uint8_t value;
         //
         digitalWrite ( ssPin, LOW );
         SPI.transfer ( NVSRAM_READ );
         SPI.transfer16 ( addr );
         value = SPI.transfer ( 0x00 );
         digitalWrite ( ssPin, HIGH );
         return value;
      };

      void write ( uint16_t addr, uint8_t value ) {
         digitalWrite ( ssPin, LOW );
         SPI.transfer ( NVSRAM_WRITE );
         SPI.transfer16 ( addr );
         SPI.transfer ( value );
         digitalWrite ( ssPin, HIGH );
      };

      void update ( uint16_t addr, uint8_t value ) {
         write ( addr, value );
      };

      // get/put functions

      template< typename T > T &get ( uint16_t addr, T &t ) {
         uint16_t t_size = sizeof ( T );
         //
         digitalWrite ( ssPin, LOW );
         SPI.transfer ( NVSRAM_READ );
         SPI.transfer16 ( addr );
         SPI.transfer ( ( void* ) &t, t_size );
         digitalWrite ( ssPin, HIGH );
         return t;
      };

      template< typename T > const T &put ( uint16_t addr, const T &t ) {
         uint16_t t_size = sizeof ( T );
         //
         digitalWrite ( ssPin, LOW );
         SPI.transfer ( NVSRAM_WRITE );
         SPI.transfer16 ( addr );
         SPI.transfer ( ( void* ) &t, t_size );
         digitalWrite ( ssPin, HIGH );
         return t;
      }

      // crc function

      uint16_t crc ( uint16_t addr, uint16_t lng ) {
         return calc_crc16 ( addr, lng );
      }

#elif defined( NVSRAM_1024 )

      // *** For 23LCV1024 128K x 8bit memory ***

      // erase function

      void erase ( void ) {
         uint32_t cnt;
         //
         digitalWrite ( ssPin, LOW );
         SPI.transfer ( NVSRAM_WRITE );
         SPI.transfer ( 0x00 );
         SPI.transfer ( 0x00 );
         SPI.transfer ( 0x00 );
         for ( cnt = 0; cnt < NVSRAM_MAX; cnt++ )
            SPI.transfer ( 0x00 );
         digitalWrite ( ssPin, HIGH );
      }

      // read/write/update functions

      uint8_t operator[] ( uint32_t addr ) {
         return read ( addr );
      }

      uint8_t read ( uint32_t addr ) {
         uint8_t value;
         //
         mem_addr.addr_32 = ( addr & NVSRAM_MAX );
         //
         digitalWrite ( ssPin, LOW );
         SPI.transfer ( NVSRAM_READ );
         SPI.transfer ( mem_addr.addr_byte[2] );
         SPI.transfer ( mem_addr.addr_byte[1] );
         SPI.transfer ( mem_addr.addr_byte[0] );
         value = SPI.transfer ( 0x00 );
         digitalWrite ( ssPin, HIGH );
         return value;
      };

      void write ( uint32_t addr, uint8_t value ) {
         mem_addr.addr_32 = ( addr & NVSRAM_MAX );
         //
         digitalWrite ( ssPin, LOW );
         SPI.transfer ( NVSRAM_WRITE );
         SPI.transfer ( mem_addr.addr_byte[2] );
         SPI.transfer ( mem_addr.addr_byte[1] );
         SPI.transfer ( mem_addr.addr_byte[0] );
         SPI.transfer ( value );
         digitalWrite ( ssPin, HIGH );
      };

      void update ( uint32_t addr, uint8_t value ) {
         write ( addr, value );
      };

      // get/put functions

      template< typename T > T &get ( uint32_t addr, T &t ) {
         uint32_t t_size = sizeof ( T );
         //
         mem_addr.addr_32 = ( addr & NVSRAM_MAX );
         //
         digitalWrite ( ssPin, LOW );
         SPI.transfer ( NVSRAM_READ );
         SPI.transfer ( mem_addr.addr_byte[2] );
         SPI.transfer ( mem_addr.addr_byte[1] );
         SPI.transfer ( mem_addr.addr_byte[0] );
         SPI.transfer ( ( void* ) &t, t_size );
         digitalWrite ( ssPin, HIGH );
         return t;
      };

      template< typename T > const T &put ( uint32_t addr, const T &t ) {
         uint32_t t_size = sizeof ( T );
         //
         mem_addr.addr_32 = ( addr & NVSRAM_MAX );
         //
         digitalWrite ( ssPin, LOW );
         SPI.transfer ( NVSRAM_WRITE );
         SPI.transfer ( mem_addr.addr_byte[2] );
         SPI.transfer ( mem_addr.addr_byte[1] );
         SPI.transfer ( mem_addr.addr_byte[0] );
         SPI.transfer ( ( void* ) &t, t_size );
         digitalWrite ( ssPin, HIGH );
         return t;
      }

      // crc function

      uint16_t crc ( uint32_t addr, uint32_t lng ) {
         return calc_crc16 ( ( addr & NVSRAM_MAX ), ( lng & NVSRAM_MAX ) );
      }

#endif

   private:
   
      uint8_t ssPin;
      
      union u_32_bit {
         uint8_t  addr_byte[4];
         uint32_t addr_32;
      } mem_addr;

#if defined( NVSRAM_512 )

      // *** For 23LCV512 64K x 8bit memory ***

      uint16_t calc_crc16 ( uint16_t startAddr, uint16_t len ) {
         uint8_t  ch;
         uint16_t crc;
         uint16_t i;
         //
         crc = 0;
         for ( i = 0; i < len; i++ ) {
            ch = read ( startAddr + i );
            crc = _crc16_update(crc, ch);
         }
         return ( crc );
      }

#elif defined( NVSRAM_1024 )

      // *** For 23LCV1024 128K x 8bit memory ***

      uint16_t calc_crc16 ( uint32_t startAddr, uint32_t len ) {
         uint8_t  ch;
         uint16_t crc;
         uint32_t i;
         //
         crc = 0;
         for ( i = 0; i < len; i++ ) {
            ch = read ( startAddr + i );
            crc = _crc16_update(crc, ch);
         }
         return ( crc );
      }

#endif

};
#endif
