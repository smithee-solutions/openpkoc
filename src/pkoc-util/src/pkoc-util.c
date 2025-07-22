#include <stdio.h>
#include <string.h>


#include <PCSC/wintypes.h>
#include <PCSC/pcsclite.h>
#include <PCSC/winscard.h>


#define PKOC_UTIL_REQUEST_CERTIFICATE (1)
#define PKOC_UTIL_LOAD_CERTIFICATE    (2)


#include <loader.h>
#include <eac-smartcard.h>
#include <pkoc-util.h>
#include <pkoc-util-version.h>


int main
  (int argc,
  char *argv [])

{ /* main for pkoc-cert-util */

  PKOC_UTIL_CONTEXT *ctx;
  PKOC_UTIL_CONTEXT my_context;
  EAC_SMARTCARD_CONTEXT my_smartcard_context;
  int status;


  ctx = &my_context;
  memset(ctx, 0, sizeof(*ctx));
  ctx->verbosity = 9;
  ctx->log = stderr;
  ctx->sc_ctx = (void *)&my_smartcard_context;


ctx->command = PKOC_UTIL_REQUEST_CERTIFICATE;
ctx->certificate_index = 0;

  fprintf(stderr, "pkoc-cert-util %s\n", PKOC_CERT_UTIL_VERSION);
  if (ctx->verbosity > 3)
  {
    fprintf(ctx->log, "verbosity is %d.\n", ctx->verbosity);
  };
  switch(ctx->command)
  {
  case PKOC_UTIL_REQUEST_CERTIFICATE:
    status = pkoc_request_certificate(ctx, ctx->certificate_index);
    break;
  case PKOC_UTIL_LOAD_CERTIFICATE:
    status = pkoc_load_certificate(argc, argv);
    break;
  default:
    status = -1;
    break;
  };

  if (status != ST_OK)
    fprintf(stderr, "pkoc-cert-util exit status was %d.\n", status);
  return(status);

} /* main for pkoc-cert-util */

