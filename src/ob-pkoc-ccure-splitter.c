/*
  ob-pkoc-ccure-splitter h hexval 
  b binary filename
*/


#include <stdio.h>
#include <string.h>


#define FORMAT_HEX (1)
#define FORMAT_BINARY (2)
#define FORMAT_DECIMAL (3)


void split_identifier(FILE *report, unsigned char *whole_identifier, int status_io);

int main
  (int argc,
  char *argv [])

{

  int i;
  int in_format;
  FILE *infile;
  int j;
  char octet [3];
  char *p;
  unsigned long long pkoc_decimal;
  int status;
  int status_io;
  unsigned char whole_identifier [256/8];


  status = 0;
  octet [2] = 0;
  in_format = FORMAT_BINARY;
  if (*(argv [1]) == 'h')
    in_format = FORMAT_HEX;
  if (*(argv [1]) == 'd')
    in_format = FORMAT_DECIMAL;
  switch(in_format)
  {
  default:
    fprintf(stderr, "Unknown format requested\n");
    break;
  case FORMAT_HEX:
#if 0
    p = argv [2];
    octet [2] = 0;
    i=0;
fprintf(stderr, "length %ld\n", strlen(argv[2]));
    while (i < strlen(argv[2]))
    {
//fprintf(stderr, "index %2d c1 %c c2 %c\n", i, *(p+2*i), *(1+p+2*i));
      memcpy(octet, p+i, 2);
fprintf(stderr, "octet string is %s\n", octet);
      sscanf(octet, "%x", &j);
fprintf(stderr, "write to %d.\n", i/2);
      whole_identifier [i/2] = j;
      i++;i++;
fprintf(stderr, "bottom index %d\n", i);
fflush(stderr);
    };
    split_identifier(stdout, whole_identifier, strlen(argv[2])/2);
#endif

{
  int length;
  int bits;

  bits = 0;
fprintf(stderr, "size whole %ld\n", sizeof(whole_identifier));
    p = argv [2];
    length = strlen(p);
    fprintf(stderr, "length %d\n", length);
    for (i=0; i<(256/8); i++)
    {
      memcpy(octet, p, 2);
      sscanf(octet, "%x", &j);
      bits = bits + 8;
      whole_identifier [i] = j;
fprintf(stderr, "Storing %02X into offset %02d. %d. bits\n", j, i, bits);
      p = p+2;
fprintf(stderr, "string now %s\n", p);
fflush(stderr);
    };
};
    split_identifier(stdout, whole_identifier, strlen(argv[2])/2);
    break;
  case FORMAT_BINARY:
    infile = fopen(argv [2], "r");
    if (infile != NULL)
    {
      status_io = fread(whole_identifier, sizeof(whole_identifier[0]), sizeof(whole_identifier), infile);
      if (status_io != (256/8))
        status = -2;
      else
        split_identifier(stdout, whole_identifier, status_io);
    }
    else
    {
      status = -1;
    };
    break;
  case FORMAT_DECIMAL:
    sscanf(argv [2], "%lld", &pkoc_decimal);
    fprintf(stderr, "decimal %s %lld\n", argv [2], pkoc_decimal);
{
  int i;
  unsigned char value;
  unsigned long long temp1;
  temp1 = pkoc_decimal;
  for (i=0; i<sizeof(pkoc_decimal); i++)
  {
    value = 0xff & temp1;
    fprintf(stderr, "i %ld. octet %02X\n",
      sizeof(pkoc_decimal)-i, value);
    temp1 = temp1 >> 8;
  };
}

    break;
  }
  return(status);

}


void split_identifier
  (FILE *report,
  unsigned char *whole_identifier,
  int status_io)

{
  unsigned char agency_code [32/8];
  unsigned char card_number [64/8];
  unsigned char cardint1 [32/8];
  unsigned char cardint2 [32/8];
  unsigned char cardint3 [32/8];
  unsigned char cardint4 [32/8];
  int i;
  int offset;
  unsigned char personnel_identifier [32/8];
  unsigned long int value;


  for (i=0; i<(256/8); i++)
    fprintf(stderr, " %02X", whole_identifier [i]);
  fprintf(stderr, "\n");
  offset = 0;
  memcpy(agency_code, whole_identifier+offset, sizeof(agency_code));
  offset = offset + sizeof(agency_code);
  memcpy(cardint1, whole_identifier+offset, sizeof(cardint1));
  offset = offset + sizeof(cardint1);
  memcpy(cardint2, whole_identifier+offset, sizeof(cardint2));
  offset = offset + sizeof(cardint1);
  memcpy(cardint3, whole_identifier+offset, sizeof(cardint3));
  offset = offset + sizeof(cardint1);
  memcpy(cardint4, whole_identifier+offset, sizeof(cardint4));
  offset = offset + sizeof(cardint1);
  memcpy(personnel_identifier, whole_identifier+offset, sizeof(personnel_identifier));
  offset = offset + sizeof(cardint1);
  memcpy(card_number, whole_identifier+offset, sizeof(card_number));
  offset = offset + sizeof(cardint1);

  fprintf(report,
"         Agency Code HEX:");
  value = 0;
  for (i=0; i<sizeof(agency_code); i++)
  {
    value = (value << 8) + agency_code [i];
    fprintf(report, " %02X", agency_code [i]);
  };
  fprintf(report, " Decimal: %lu\n", value);

  fprintf(report,
"            CardInt1 HEX:");
  value = 0;
  for (i=0; i<sizeof(cardint1); i++)
  {
    value = (value << 8) + cardint1 [i];
    fprintf(report, " %02X", cardint1 [i]);
  };
  fprintf(report, " Decimal: %lu\n", value);

  fprintf(report,
"            CardInt2 HEX:");
  value = 0;
  for (i=0; i<sizeof(cardint2); i++)
  {
    value = (value << 8) + cardint2 [i];
    fprintf(report, " %02X", cardint2 [i]);
  };
  fprintf(report, " Decimal: %lu\n", value);

  fprintf(report,
"            CardInt3 HEX:");
  value = 0;
  for (i=0; i<sizeof(cardint3); i++)
  {
    value = (value << 8) + cardint3 [i];
    fprintf(report, " %02X", cardint3 [i]);
  };
  fprintf(report, " Decimal: %lu\n", value);

  fprintf(report,
"            CardInt4 HEX:");
  value = 0;
  for (i=0; i<sizeof(cardint4); i++)
  {
    value = (value << 8) + cardint4 [i];
    fprintf(report, " %02X", cardint4 [i]);
  };
  fprintf(report, " Decimal: %lu\n", value);

  fprintf(report,
"Personnel Identifier HEX:");
  value = 0;
  for (i=0; i<sizeof(personnel_identifier); i++)
  {
    value = (value << 8) + personnel_identifier [i];
    fprintf(report, " %02X", personnel_identifier [i]);
  };
  fprintf(report, " Decimal: %lu\n", value);

  value = 0;
  fprintf(report,
"         Card Number HEX:");
  for (i=0; i<sizeof(card_number); i++)
  {
    value = (value << 8) + agency_code [i];
    fprintf(report, " %02X", card_number [i]);
  };
  fprintf(report, " Decimal: %lu\n", value);

}

