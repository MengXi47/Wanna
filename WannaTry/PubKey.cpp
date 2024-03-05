#include "Keys.h"

UCHAR _WannaPublicKey[] = {
0x52, 0x53, 0x41, 0x31, 0x00, 0x08, 0x00, 0x00,
0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x01, 0x00, 0x01, 0xCA, 0x92, 0x2A, 0xAD, 0x30,
0xF5, 0x95, 0xDA, 0x9A, 0xB2, 0x1B, 0x07, 0xAB,
0xAC, 0xDD, 0x75, 0xE4, 0xFF, 0x5F, 0x24, 0x22,
0x81, 0xDF, 0x43, 0x6C, 0xB3, 0x76, 0x9E, 0x13,
0x83, 0x5B, 0x90, 0x0C, 0xBD, 0xC2, 0x8C, 0x59,
0x1E, 0x37, 0x36, 0x9F, 0x7E, 0x5D, 0x38, 0x5B,
0xE8, 0x0B, 0x0C, 0x8C, 0x76, 0x6D, 0xA0, 0x24,
0x06, 0x81, 0x2A, 0x9A, 0x9F, 0x3A, 0x8C, 0xCF,
0xBC, 0xAE, 0xA2, 0x53, 0xCA, 0x49, 0x6F, 0xFA,
0x99, 0x81, 0x18, 0x50, 0x3D, 0x59, 0x11, 0xA2,
0x41, 0x0C, 0xFC, 0x93, 0x37, 0x32, 0x73, 0x1D,
0x8F, 0x8D, 0x87, 0x27, 0x34, 0x28, 0x8C, 0x67,
0x0F, 0x3C, 0x58, 0x3A, 0xA5, 0x3F, 0xA1, 0x34,
0x83, 0xBF, 0xB2, 0x43, 0x56, 0xE8, 0x0C, 0xE0,
0x1D, 0x97, 0x2C, 0x35, 0x49, 0x68, 0x18, 0x53,
0xA6, 0xE6, 0x89, 0x44, 0x19, 0xE4, 0xCD, 0xBF,
0xF8, 0xB2, 0xB0, 0xC3, 0xBC, 0xCB, 0xA4, 0xD9,
0x62, 0xEE, 0xFC, 0xD8, 0xBF, 0xA9, 0x5E, 0xC2,
0x50, 0xC9, 0xAB, 0x72, 0x13, 0x6A, 0xF9, 0x27,
0x3D, 0xCB, 0x6E, 0x74, 0x6C, 0xC6, 0xEE, 0x98,
0xD2, 0xBD, 0xDE, 0xE8, 0xB6, 0x8F, 0x87, 0xC3,
0x3D, 0x5B, 0xB0, 0x2E, 0xF3, 0xED, 0x58, 0xA5,
0xFB, 0x55, 0x8F, 0x08, 0x9F, 0x29, 0xAE, 0x68,
0x40, 0x24, 0xDC, 0x99, 0x53, 0x55, 0xA4, 0x22,
0xA4, 0x35, 0x94, 0x39, 0x9A, 0x0A, 0x0C, 0xE8,
0x64, 0xAA, 0x46, 0xCE, 0x8A, 0xC8, 0x0B, 0x3C,
0xB2, 0xCC, 0xFD, 0x9F, 0x82, 0x96, 0x1A, 0xA0,
0x27, 0x4E, 0x3B, 0x44, 0xB3, 0xA8, 0xD8, 0x93,
0x57, 0x13, 0x6A, 0x53, 0xDB, 0xE8, 0xEE, 0x88,
0xA6, 0xC6, 0x84, 0x5B, 0x1C, 0xE0, 0xC3, 0x0A,
0x6F, 0xA6, 0xF2, 0xD5, 0xDB, 0x26, 0xCF, 0xA1,
0x71, 0x33, 0xAE, 0xD5, 0xBD, 0x84, 0x0E, 0xA8,
0x65, 0xBE, 0xD9 };

PUCHAR WannaPublicKey()
{
	return _WannaPublicKey;
}

ULONG WannaPublicKeySize()
{
	return sizeof(_WannaPublicKey);
}