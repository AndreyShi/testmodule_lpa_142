#include <stdint.h>
#include <stdio.h>

uint8_t *reorder4 (uint8_t *src, uint32_t len)
{
   static uint8_t dst[4];
   uint8_t appendlen, idx;

 

   len = (len % 4)+4;
   appendlen = (len % 4) ? 4-(len % 4) : 0;
   idx = 0;
   while(appendlen--)
   {
      dst[idx] = 0xFF;
      idx++;
   }
   while(len--)
   {
      dst[idx] = src[3-idx];
      idx++;
   }
   return dst;
}

 

uint32_t crc32_formula_normal_STM32( size_t len,
                                     void *data )
{

#define POLY 0x04C11DB7
   uint8_t *buffer = (uint8_t*)data;
   uint32_t crc = -1;
   uint32_t portion;
   uint8_t *reordered;
 
   while( len )
   {
      portion = len < 4 ? len : 4;
      reordered = reorder4(buffer, portion);
      for (uint8_t i=0; i < 4; i++)
      {
         crc = crc ^ ((uint32_t)reordered[i] << 24);
         for( int bit = 0; bit < 8; bit++ )
         {
            if( crc & (1L << 31)) crc = (crc << 1) ^ POLY;
            else                  crc = (crc << 1);
         }
      }
      buffer += portion;
      len -= portion;
   }
   return crc;
} 

int main(){

  uint8_t pBuffer_c[] = {0x00,0x00,0x02,0x20, 0xE5,0x66,0x00,0x08, 0x00,0x00,0x00,0x00};
  //рассчет прошивки CRC32
  uint32_t crc32 = crc32_formula_normal_STM32(8,&pBuffer_c[0]);
  //---помещаем рассчитанное crc32 в конец как little endian
  pBuffer_c[8]  = crc32;
  pBuffer_c[9]  = crc32 >> 8;
  pBuffer_c[10] = crc32 >> 16;
  pBuffer_c[11] = crc32 >> 24;
  //---
  uint32_t crc_cal3 = 0;
  //алгоритм программный crc прошивки + сам результат crc (8 байт данных + 4 байта crc),  получается в итоге crc == 0
  crc_cal3 = crc32_formula_normal_STM32(12,&pBuffer_c[0]);      
  printf("crc32: %lx %lx\n",crc32,crc_cal3);
  return crc_cal3;
}