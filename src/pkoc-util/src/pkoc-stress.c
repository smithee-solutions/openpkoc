// request a certificate from a PKOC card

// NOT: define STRESS_PROTOCOL 

#include <stdio.h>
#include <string.h>

#include <eac-encode.h>
#include <smartcard.h>


#include <test-pkoc.h>


int main
  (int argc,
  char *argv [])

{
  SMARTCARD_CONTEXT *ctx;
  SMARTCARD_CONTEXT my_context;
  unsigned char payload [8192];
int param_transaction_length;
  int payload_lth;
  unsigned char protocol_version [255];
  unsigned char reader_identifier [32];
  int ret_lth;
  unsigned char returned_apdu [8192];
  int status;
  unsigned char transaction_identifier [PKOC_TRANSACTION_IDENTIFIER_MAX];
  int ver_lth;

  fprintf(stderr, "7816 test is in startup.\n");
  ctx = &my_context;

  // temp hard-coded settings
  param_transaction_length = PKOC_TRANSACTION_IDENTIFIER_MIN;

  status = sc_init(ctx);
  if (status EQUALS ST_OK)
  {
    if (ctx->verbosity > 0)
      fprintf(LOG, "test-7816 %s\n", TEST_7816_VERSION);
    if (ctx->verbosity > 3)
    {
      fprintf(LOG, "verbosity is %d.\n", ctx->verbosity);
      fprintf(LOG, "transaction identifier is %d. octets.\n",
        param_transaction_length);
    };

    payload_lth = sizeof(payload);

    memset(transaction_identifier, 0x17, sizeof(transaction_identifier));

    // published protocol version
    protocol_version [0] = 1;
    protocol_version [1] = 0;
    ver_lth = 2;
#ifdef STRESS_PROTOCOL
    memset(protocol_version, 2, sizeof(protocol_version));
    protocol_version [2] = 1;
    protocol_version [3] = 0;
    ver_lth = 4;
fprintf(stderr, "DEBUG: Warning: off-nominal version list used.\n");
#endif

    memset(reader_identifier, 3, sizeof(reader_identifier));
    reader_identifier [0] = 0xCA;
    reader_identifier [1] = 0xFE;
    reader_identifier [2] = 0xDE;
    reader_identifier [3] = 0xED;
    status = setup_pkoc_authenticate(ctx, payload, &payload_lth,
      transaction_identifier, param_transaction_length, protocol_version, ver_lth, reader_identifier);
  };
  if (status EQUALS ST_OK)
  {
    // PKOC Auth CLA 80 INS 80 P1 0 P2 1 Lc lth data Le 0

    ret_lth = sizeof(returned_apdu);
    status = sc_sendrcv(ctx, 0x80, 0x80, 0x00, 0x01,
      payload_lth, 0x00, payload, returned_apdu, &ret_lth, ACTION_WAIT);
    if (ctx->verbosity > 3)
      fprintf(LOG, "sc_sendrcv returned %d.\n", status);
  };

  if (status != ST_OK)
    fprintf(stderr, "test-7816 returning status %d.\n", status);
  return(status);

}


