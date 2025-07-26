// request a certificate from a PKOC card

// NOT: define STRESS_PROTOCOL 

#include <stdio.h>
#include <string.h>


#include <PCSC/wintypes.h>
#include <PCSC/pcsclite.h>
#include <PCSC/winscard.h>

#include <eac-encode.h>
#include <eac-smartcard.h>
#include <ob-stub.h>
#include <pkoc-util.h>
int sc_simulator_init(EAC_SMARTCARD_CONTEXT *ctx, SC_SIMULATOR_CONTEXT *sim_ctx);


int pkoc_request_certificate
  (PKOC_UTIL_CONTEXT *ctx,
  int cert_index)

{ /* pkoc_request_certificate */

  extern SC_SIMULATOR_CONTEXT my_simulator_context;
  unsigned char payload [8192];
  int payload_lth;
  unsigned char protocol_version [255];
  unsigned char reader_identifier [32];
  int ret_lth;
  unsigned char returned_apdu [8192];
  EAC_SMARTCARD_CONTEXT *sc_ctx;
  int status;
  unsigned char transaction_identifier [PKOC_TRANSACTION_IDENTIFIER_MAX];
  int ver_lth;


  // temp hard-coded settings
  ctx->transaction_length = PKOC_TRANSACTION_IDENTIFIER_MIN;

  sc_ctx = ctx->sc_ctx;
  memset(sc_ctx, 0, sizeof(*sc_ctx));
  sc_ctx->verbosity = ctx->verbosity;
  sc_ctx->subsystem = ctx->smartcard_subsystem;
  strcpy(my_simulator_context.certificate_filename, ctx->certificate_filename);

  status = sc_simulator_init(sc_ctx, &my_simulator_context);
  if (status EQUALS ST_OK)
  {
    status = sc_init(sc_ctx);
  };
  if (status EQUALS ST_OK)
  {
    if (ctx->verbosity > 3)
    {
      fprintf(LOG, "transaction identifier is %d. octets.\n",
        ctx->transaction_length);
    };

    payload_lth = sizeof(payload);

    memset(transaction_identifier, 0x17, sizeof(transaction_identifier));

    // published protocol version
    protocol_version [0] = 1;
    protocol_version [1] = 0;
    ver_lth = 2;

    memset(reader_identifier, 3, sizeof(reader_identifier));
    reader_identifier [0] = 0xCA;
    reader_identifier [1] = 0xFE;
    reader_identifier [2] = 0xDE;
    reader_identifier [3] = 0xED;
    status = setup_pkoc_authenticate(ctx, payload, &payload_lth,
      transaction_identifier, ctx->transaction_length, protocol_version, ver_lth, reader_identifier,
      cert_index);
  };
  if (status EQUALS ST_OK)
  {
    if (ctx->verbosity > 3)
    {
      eac_encode_dump_buffer("PKOC Authenticate Command:\n", payload, payload_lth);
    };

    // PKOC Auth CLA 80 INS 80 P1 0 P2 1 Lc lth data Le 0

    ret_lth = sizeof(returned_apdu);
    if (ctx->verbosity > 3)
    {
      fprintf(ctx->log, "smartcard context is %lX\n", (unsigned long int)(ctx->sc_ctx));
    };
    status = sc_sendrcv(ctx->sc_ctx, 0x80, 0x80, 0x00, 0x01,
      payload_lth, 0x00, payload, returned_apdu, &ret_lth, ACTION_WAIT);
    if (ctx->verbosity > 3)
      fprintf(LOG, "sc_sendrcv returned %d.\n", status);
  };

  if (status != ST_OK)
    fprintf(stderr, "test-7816 returning status %d.\n", status);
  return(status);

}


