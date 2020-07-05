#ifndef __base64__
#define __base64__

void _base64_encode_triple(unsigned char triple[3], char result[4]);

int gsf_base64_encode(unsigned char *source, unsigned int sourcelen, char *target, unsigned int targetlen);

#endif //__file__

