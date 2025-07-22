/*
  simulate-smartcard - simulates the card side of a 7816-4 dialog.

  for now either a stub or PKOC

  define the appropriate symbol to get one of the variants to compile:


    SIMULATE_STUB
    SIMULATE_PKOC
*/
#include <stdio.h>
#include <string.h>


#include <eac-encode.h>
#include <eac-smartcard.h>


#ifdef SIMULATE_STUB
int simulate_smartcard (SMARTCARD_CONTEXT *ctx, unsigned char cla, unsigned char ins,
  unsigned char p1, unsigned char p2, unsigned int Lc, unsigned char Le,
  unsigned char *payload, int payload_length, int action)
{
return(STSC_NOT_IMPLEMENTED);
}
#endif


#ifdef SIMULATE_PKOC

int simulate_smartcard
  (EAC_SMARTCARD_CONTEXT *ctx,
  unsigned char cla,
  unsigned char ins,
  unsigned char p1,
  unsigned char p2,
  unsigned int Lc,
  unsigned char Le,
  unsigned char *payload,
  unsigned char *ret_apdu,
  int *ret_lth,
  int action)

{ /* simulate_smartcard */

  int match_authenticate;
  int status;


  status = ST_OK;
  match_authenticate = 0;
  if ((cla EQUALS 0x80) &&
    (ins EQUALS 0x80) &&
    (p1 EQUALS 0x00) &&
    (p2 EQUALS 0x01) &&
    (Le EQUALS 0x00))
    match_authenticate = 1;
  if (match_authenticate)
  {
    if (ctx->verbosity > 3)
      fprintf(LOG, "simulator: authentication request identified.\n");

    eac_encode_dump_buffer("PKOC Authentication Command:\n", payload, Lc);

{
  unsigned char fixed_response [] = {
    0x5A, 0x04, 0x11, 0x22, 0x33, 0x44,
    0x9E, 0x04, 0xaa, 0xbb, 0xcc, 0xdd,

// and this is the cert response
    0xF8, 0x04, 0xcc, 0xee,  0x33, 0x77,

    0x90, 0x00
  };
  memcpy(ret_apdu, fixed_response, sizeof(fixed_response));
  *ret_lth = sizeof(fixed_response);
}
    eac_encode_dump_buffer("PKOC Authentication Response:\n", ret_apdu, *ret_lth);
  };

  return(status);

} /* simulate_smartcard */

#endif

