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

 

uint32_t crc32_formula_normal_STM32( uint32_t crc_start,size_t len,void *data )
{
#define POLY 0x04C11DB7
   uint8_t *buffer = (uint8_t*)data;
   uint32_t crc = crc_start;
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

  uint8_t pBuffer_c[] = {0x00,0x00,0x02,0x20, 0xE5,0x66,0x00,0x08, 0x8E,0x09,0x6F,0x07}; //первые два слова crc32: 0x76f098e 0
  //uint8_t pBuffer_c[12] = {0};

  FILE* fp = fopen("cg.bin", "r"); //подставить название bin файла скаченного с барьера, размер файла долженбыть 0x7000 (0x1C00 * 4)
  int i = 0;
  uint32_t crc32_start = -1; 
  for(int t = 0; t < sizeof(pBuffer_c)/sizeof(uint32_t) - 1;t++){  // здесь поставить размер 0x1C00
      uint8_t pChun[4] = {0xff,0xff,0xff,0xff};
      for(int g = 0; g < sizeof(uint32_t);g++){
         int c = fgetc(fp);
         if(c == EOF)
            {break;}
         pChun[g] = (uint8_t)c;
      }
      crc32_start = crc32_formula_normal_STM32(crc32_start,4,&pChun[0]);   
  }
  fclose(fp);
  //записать полученное crc через stlink по адресам
  //fw[6FFC] = crc32_start;
  //fw[6FFD] = crc32_start >> 8;
  //fw[6FFE] = crc32_start >> 16;
  //fw[6FFF] = crc32_start >> 24;
  
  //рассчет прошивки CRC32
  uint32_t crc32 = crc32_start;//crc32_formula_normal_STM32(-1,8,&pBuffer_c[0]);
  //---помещаем рассчитанное crc32 в конец как little endian
  //pBuffer_c[8]  = crc32;
  //pBuffer_c[9]  = crc32 >> 8;
  //pBuffer_c[10] = crc32 >> 16;
  //pBuffer_c[11] = crc32 >> 24;
  //---
  uint32_t crc_cal3 = 0;
  //алгоритм программный crc прошивки + сам результат crc (8 байт данных + 4 байта crc),  получается в итоге crc == 0
  crc_cal3 = crc32_formula_normal_STM32(-1,12,&pBuffer_c[0]);      
  printf("crc32: %lx %lx\n",crc32,crc_cal3);
  return crc_cal3;
}