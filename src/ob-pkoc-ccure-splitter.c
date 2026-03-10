/*
  ob-pkoc-ccure-splitter h hexval 
  b binary filename
*/


#include <stdio.h>
#include <string.h>


#define FORMAT_HEX (1)
#define FORMAT_BINARY (2)


void split_identifier(FILE *report, unsigned char *whole_identifier, int status_io);

int main
  (int argc,
  char *argv [])

{

  int in_format;
  FILE *infile;
  int status;
  int status_io;
  unsigned char whole_identifier [256/8];


  status = 0;
  in_format = FORMAT_BINARY;
  switch(in_format)
  {
  default:
    fprintf(stderr, "Unknown format requested\n");
    break;
  case FORMAT_HEX:
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

