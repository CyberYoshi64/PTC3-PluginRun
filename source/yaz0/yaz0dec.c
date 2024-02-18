#include <3ds.h>
#include <string.h>
#include <malloc.h>
#include <machine/endian.h>

void _yaz0Decode(Handle inf, Handle outf, u32 uncompressedSize) {
  u32 bytWrit;
  u32 srcPos = 16, dstPos = 0; //current read/write positions
  
  u32 validBitCount = 0; //number of valid bits left in "code" byte
  u8 currCodeByte;
  u8* buf = malloc(0x400);
  while(dstPos < uncompressedSize) {
    //read new "code" byte if the current one is used up
    if(validBitCount == 0) {
      FSFILE_Read(inf, &bytWrit, srcPos++, &currCodeByte, 1);
      validBitCount = 8;
    }
    
    if((currCodeByte & 0x80) != 0) {
      //straight copy
      FSFILE_Read(inf, &bytWrit, srcPos++, buf, 1);
      FSFILE_Write(outf, &bytWrit, dstPos++, buf, 1, 0);
    } else {
      //RLE part
      FSFILE_Read(inf, &bytWrit, srcPos, buf, 3);
      u8 byte1 = buf[0];
      u8 byte2 = buf[1];
      srcPos += 2;
      
      u32 dist = ((byte1 & 0xF) << 8) | byte2;
      u32 copySource = dstPos - (dist + 1);

      u32 numBytes = byte1 >> 4;
      if(numBytes == 0) {
        numBytes = buf[2] + 0x12;
        srcPos++;
      } else
        numBytes += 2;

      //copy run
      FSFILE_Read(outf, &bytWrit, copySource, buf, numBytes);
      FSFILE_Write(outf, &bytWrit, dstPos, buf, numBytes, FS_WRITE_FLUSH);
      dstPos += numBytes;
    }
    
    //use next bit from "code" byte
    currCodeByte <<= 1;
    validBitCount-=1;    
  }
  FSFILE_Flush(outf);
  free(buf);
}


int yaz0Decode(Handle inputFile, Handle outputFile) {
  u32 buf, bytRead;
  FSFILE_Read(inputFile, &bytRead, 0, &buf, 4);
  if (strncmp((char*)&buf, "Yaz0", 4) != 0) return 1;
  FSFILE_Read(inputFile, &bytRead, 4, &buf, 4);
  u32 size = __bswap32(buf);
  _yaz0Decode(inputFile, outputFile, size);
  return 0;
}
