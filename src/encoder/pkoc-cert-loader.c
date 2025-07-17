#include <stdio.h>
#include <string.h>


#include <PCSC/wintypes.h>
#include <PCSC/pcsclite.h>
#include <PCSC/winscard.h>


#include <loader.h>
unsigned char SELECT_PKOC [] = 
  {0x00, 0xa4, 0x04, 0x00, 0x08, 0xA0, 0x00, 0x00, 0x08, 0x98, 0x00, 0x00, 0x01, 0x00};


//call pcsc to init
//if status good continue




int main
  (int argc,
  char *arg [])

{ /* main for pkoc-cert-loader */

  unsigned char cert_buffer [8192];
  int cert_length;
  FILE *cfile; // for certificate
  OB_CONTEXT *ctx;
  OB_CONTEXT my_context;
  int payload_length;
  unsigned char receive_buffer [OB_STRING_MAX];
  DWORD receive_length;
  unsigned char smartcard_command [OB_STRING_MAX];
  int smartcard_command_length;
  int status;
  DWORD status_pcsc;


  status = ST_OK;
  ctx = &my_context;
  memset(ctx, 0, sizeof(*ctx));
ctx->verbosity = 9;
ctx->reader_index = 1;
strcpy(ctx->cert_filename, "cert.der");

  fprintf(stderr, "PKOC Certificate Loader %s\n", PKOC_CERT_LOADER_VERSION);
  fprintf(stderr, "Reader %d Cert file %s\n", ctx->reader_index, ctx->cert_filename);

  if (status EQUALS ST_OK)
  {
    status = ob_init_smartcard(ctx);
  };
  if (status EQUALS ST_OK)
  {
    memcpy(smartcard_command, SELECT_PKOC, sizeof(SELECT_PKOC));
    smartcard_command_length = sizeof(SELECT_PKOC);
  };
  if (status EQUALS ST_OK)
  {
    if (ctx->verbosity > 3)
      ob_dump_buffer(ctx, LOG_STDERR, "PKOC Select:", smartcard_command, smartcard_command_length);

    // send application select to card

    receive_length = sizeof(receive_buffer);
    status_pcsc = SCardTransmit(ctx->pcsc, &ctx->pioSendPci, smartcard_command, smartcard_command_length, NULL, receive_buffer, &receive_length);
    ctx->last_pcsc_status = status_pcsc;
    if (SCARD_S_SUCCESS != status_pcsc)
      status = STOB_PCSC_TRANSMIT_1;
  };

  if (status EQUALS ST_OK)
  {
    if (ctx->verbosity > 3)
      ob_dump_buffer(ctx, LOG_STDERR, "PKOC Select Response:", receive_buffer, receive_length);

    cfile = fopen("cert.der", "r");
    if (cfile EQUALS NULL)
      status = STOB_FILE_ERROR;
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
    memcpy(smartcard_command+11, cert_buffer, cert_length);
    smartcard_command_length = 11 + cert_length;
    if (ctx->verbosity > 3)
      ob_dump_buffer(ctx, LOG_STDERR, "PKOC put-certificate:", smartcard_command, smartcard_command_length);
  };

  if (status != ST_OK)
    fprintf(stderr, "pkoc-cert-loader exit status was %d.\n", status);
  return(status);

} /* main for pkoc-cert-loader */

