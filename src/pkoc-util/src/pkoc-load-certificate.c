#include <stdio.h>
#include <string.h>
#include <time.h>


#include <PCSC/wintypes.h>
#include <PCSC/pcsclite.h>
#include <PCSC/winscard.h>


#include <openbadger-common.h>
#include <eac-encode.h>
#include <ob-stub.h>
#include <pkoc-util.h>


unsigned char SELECT_PKOC [] = 
  {0x00, 0xa4, 0x04, 0x00, 0x08, 0xA0, 0x00, 0x00, 0x08, 0x98, 0x00, 0x00, 0x01, 0x00};


//call pcsc to init
//if status good continue


int pkoc_load_certificate
  (PKOC_UTIL_CONTEXT *ctx,
  int argc,
  char *argv [])

{ /* pkoc_load_certificate */

  unsigned char cert_buffer [8192];
  int cert_length;
  FILE *cfile; // for certificate
  int idx;
  OB_CONTEXT ob_context;
  int payload_length;
  unsigned char receive_buffer [EAC_STRING_MAX];
  DWORD receive_length;
  unsigned char smartcard_command [EAC_STRING_MAX];
  EAC_SMARTCARD_CONTEXT_EX smartcard;
  int smartcard_command_length;
  int status;
  DWORD status_pcsc;


  status = ST_OK;
  memset(&smartcard, 0, sizeof(smartcard));
  smartcard.log = ctx->log;
  smartcard.verbosity = ctx->verbosity;
  smartcard.reader_index = ctx->reader_index;
  memset(&ob_context, 0, sizeof(ob_context));
  ob_context.log = ctx->log;
  ob_context.verbosity = ctx->verbosity;

  fprintf(stderr, "Reader %d Cert file %s\n", ctx->reader_index, ctx->certificate_filename);

  if (status EQUALS ST_OK)
  {
    status = ob_init_smartcard_ex(&smartcard);
  };
  if (status EQUALS ST_OK)
  {
    memcpy(smartcard_command, SELECT_PKOC, sizeof(SELECT_PKOC));
    smartcard_command_length = sizeof(SELECT_PKOC);
  };
  if (status EQUALS ST_OK)
  {
    if (ctx->verbosity > 3)
    {
      fprintf(ctx->log, "PKOC Select");
      ob_dump_buffer(&ob_context, smartcard_command, smartcard_command_length, 0);
    };

    // send application select to card

    receive_length = sizeof(receive_buffer);
    status_pcsc = SCardTransmit(smartcard.pcsc, &smartcard.pioSendPci, smartcard_command, smartcard_command_length, NULL, receive_buffer, &receive_length);
    smartcard.last_pcsc_status = status_pcsc;
    if (SCARD_S_SUCCESS != status_pcsc)
      status = STEAC_PCSC_TRANSMIT_1;
  };

  if (status EQUALS ST_OK)
  {
    if (ctx->verbosity > 3)
    {
      fprintf(ctx->log, "PKOC Select Response:");
      ob_dump_buffer(&ob_context, receive_buffer, receive_length, 0);
    };

    if (ctx->verbosity > 3)
      fprintf(ctx->log, "Accessing DER-encoded certificate %s\n", ctx->certificate_filename);
    cfile = fopen(ctx->certificate_filename, "r");
    if (cfile EQUALS NULL)
    {
      status = STEAC_FILE_ERROR;
      if (ctx->verbosity > 3)
        fprintf(ctx->log, "error opening certificate file.\n");
    };
  };
  if (status EQUALS ST_OK)
  {
    cert_length = fread(cert_buffer, sizeof(cert_buffer [0]), sizeof(cert_buffer), cfile);

    smartcard_command [0] = 0x00; // CLA
    smartcard_command [1] = 0xE2; // INS
    smartcard_command [2] = 0x00; // P1
    smartcard_command [3] = 0x00; // p2
    smartcard_command [4] = 0x00; // first octet of Lc for extended APDU mode
    payload_length = 3 + cert_length;
    smartcard_command [5] = (0xFF00 & payload_length) >> 8;
    smartcard_command [6] = 0xFF & payload_length;
    smartcard_command [7] = 0xF8; // tag in PKOC tag space for certificate
    smartcard_command [8] = 0x82; // DER length will be 82 xx xx
    smartcard_command [ 9] = (0xFF00 & cert_length) >> 8;
    smartcard_command [10] = 0x00ff & cert_length;
    idx = 10;
    idx++;
    memcpy(smartcard_command+idx, cert_buffer, cert_length);
    idx = idx + cert_length;
    smartcard_command [idx] = 0; // Le
    smartcard_command_length = 11 + cert_length + 1;
    if (ctx->verbosity > 3)
    {
      fprintf(ctx->log, "PKOC put-certificate:");
      ob_dump_buffer(&ob_context, smartcard_command, smartcard_command_length, 0);
    };
  };

  if (status != ST_OK)
    fprintf(stderr, "pkoc-cert-loader exit status was %d.\n", status);
  return(status);

} /* pkoc_load_certificate */

