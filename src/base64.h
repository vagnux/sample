#define HIGHBITS 0x30 /* 00110000 */
#define MIDBITS  0x0C /* 00001100 */
#define LOWBITS  0x03 /* 00000011 */
#define CHAR1(x,y) ((x << 2) | ((y & HIGHBITS) >> 4))
#define CHAR2(x,y) (((x & (MIDBITS | LOWBITS)) << 4) | ((y & (HIGHBITS | MIDBITS)) >> 2))
#define CHAR3(x,y) (((x & LOWBITS) << 6) | y)
