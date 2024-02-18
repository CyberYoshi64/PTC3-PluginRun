#include <3ds.h>
#include <string.h>
#include <malloc.h>
#include <machine/endian.h>

// simple and straight encoding scheme for Yaz0
u32 yaz0simpleEnc(Handle inf, int size, int pos, u32 *pMatchPos)
{
  u8* src = malloc(0x4000); u32 bytRead;
  int startPos = pos - 0x1000;
  u32 numBytes = 1;
  u32 matchPos = 0;

  FSFILE_Read(inf, &bytRead, startPos, src, 0x4000);
  if (startPos < 0)
    startPos = 0;
  for (int i = startPos; i < pos; i++) {
    int j = 0;
    for (j = 0; j < size-pos; j++) {
      if (src[i + j - startPos] != src[j + pos - startPos])
        break;
    }
    if (j > numBytes) {
      numBytes = j;
      matchPos = i;
    }
  }
  *pMatchPos = matchPos;
  if (numBytes == 2)
    numBytes = 1;
  free(src);
  return numBytes;
}

// a lookahead encoding scheme for ngc Yaz0
u32 yaz0nintendoEnc(Handle inf, int size, int pos, u32 *pMatchPos) {
  u32 numBytes = 1;
  static u32 numBytes1;
  static u32 matchPos;
  static int prevFlag = 0;

  // if prevFlag is set, it means that the previous position was determined by look-ahead try.
  // so just use it. this is not the best optimization, but nintendo's choice for speed.
  if (prevFlag == 1) {
    *pMatchPos = matchPos;
    prevFlag = 0;
    return numBytes1;
  }
  prevFlag = 0;
  numBytes = yaz0simpleEnc(inf, size, pos, &matchPos);
  *pMatchPos = matchPos;

  // if this position is RLE encoded, then compare to copying 1 byte and next position(pos+1) encoding
  if (numBytes >= 3) {
    numBytes1 = yaz0simpleEnc(inf, size, pos+1, &matchPos);
    // if the next position encoding is +2 longer than current position, choose it.
    // this does not guarantee the best optimization, but fairly good optimization with speed.
    if (numBytes1 >= numBytes+2) {
      numBytes = 1;
      prevFlag = 1;
    }
  }
  return numBytes;
}

int _yaz0Encode(Handle inf, Handle outf, u32 srcSize) {
  u32 srcPos = 0, dstPos = 0, bytWrit;
  u8 dst[24];    // 8 codes * 3 bytes maximum
  int dstSize = 0;
  u8 src;
  
  u32 validBitCount = 0; //number of valid bits left in "code" byte
  u8 currCodeByte = 0;
  while(srcPos < srcSize) {
    u32 numBytes;
    u32 matchPos;

    numBytes = yaz0nintendoEnc(inf, srcSize, srcPos, &matchPos);
    if (numBytes < 3) {
      //straight copy
      FSFILE_Read(inf, &bytWrit, srcPos++, &src, 1);
      dst[dstPos++] = src;
      //set flag for straight copy
      currCodeByte |= (0x80 >> validBitCount);
    } else {
      //RLE part
      u32 dist = srcPos - matchPos - 1; 
      u8 byte1, byte2, byte3;

      if (numBytes >= 0x12) { // 3 byte encoding
        byte1 = 0 | (dist >> 8);
        byte2 = dist & 0xff;
        dst[dstPos++] = byte1;
        dst[dstPos++] = byte2;
        // maximum runlength for 3 byte encoding
        if (numBytes > 0xff+0x12)
          numBytes = 0xff+0x12;
        byte3 = numBytes - 0x12;
        dst[dstPos++] = byte3;
      } else { // 2 byte encoding
        byte1 = ((numBytes - 2) << 4) | (dist >> 8);
        byte2 = dist & 0xff;
        dst[dstPos++] = byte1;
        dst[dstPos++] = byte2;
      }
      srcPos += numBytes;
    }
    validBitCount++;
    //write eight codes
    if(validBitCount == 8) {
      FSFILE_Write(outf, &bytWrit, dstSize++, &currCodeByte, 1, 0);
      FSFILE_Write(outf, &bytWrit, dstSize, dst, dstPos, FS_WRITE_FLUSH);
      dstSize += dstPos;

      currCodeByte = 0;
      validBitCount = 0;
      dstPos = 0;
    }
  }
  if(validBitCount > 0) {
    FSFILE_Write(outf, &bytWrit, dstSize++, &currCodeByte, 1, 0);
    FSFILE_Write(outf, &bytWrit, dstSize, dst, dstPos, FS_WRITE_FLUSH);
    dstSize += dstPos;

    currCodeByte = 0;
    validBitCount = 0;
    dstPos = 0;
  }
  return dstSize;
}

void yaz0Encode(Handle inf, Handle outf) {
  u32 bytWrit, buf;
  u64 size; FSFILE_GetSize(inf, &size);

  FSFILE_Write(outf, &bytWrit, 0, (void*)"Yaz0", 4, 0);

  buf = __bswap32(size);
  FSFILE_Write(outf, &bytWrit, 4, &buf, 4, 0);

  buf = 0;
  FSFILE_Write(outf, &bytWrit, 8, &buf, 4, 0);
  FSFILE_Write(outf, &bytWrit, 12, &buf, 4, 0);

  _yaz0Encode(inf, outf, size);
}
