/*
  pkoc-util - pkoc NFC credential manipulation utility
*/

#include <stdio.h>
#include <string.h>


#include <PCSC/wintypes.h>
#include <PCSC/pcsclite.h>
#include <PCSC/winscard.h>


#define PKOC_UTIL_REQUEST_CERTIFICATE (1)
#define PKOC_UTIL_LOAD_CERTIFICATE    (2)


#include <eac-encode.h>
#include <eac-smartcard.h>
#include <ob-stub.h>
#include <pkoc-util.h>
#include <pkoc-util-version.h>
int initialize_pkoc_util(PKOC_UTIL_CONTEXT *ctx);
EAC_SMARTCARD_CONTEXT my_smartcard_context;


int main
  (int argc,
  char *argv [])

{ /* main for pkoc-cert-util */

  PKOC_UTIL_CONTEXT *ctx;
  PKOC_UTIL_CONTEXT my_context;
  int status;


  ctx = &my_context;
  status = initialize_pkoc_util(ctx);

  fprintf(stderr, "pkoc-cert-util %s\n", PKOC_CERT_UTIL_VERSION);
  if (ctx->verbosity > 3)
  {
    fprintf(ctx->log, "verbosity is %d.\n", ctx->verbosity);
  };
  switch(ctx->command)
  {
  case PKOC_UTIL_REQUEST_CERTIFICATE:
    if (ctx->verbosity > 3)
    {
      fprintf(ctx->log, "Performing Request Certificate\n");
    };
    status = pkoc_request_certificate(ctx, ctx->certificate_index);
    break;
  case PKOC_UTIL_LOAD_CERTIFICATE:
fprintf(stderr, "DEBUG: reader index hard-coded to 1\n");
ctx->reader_index = 1;
    status = pkoc_load_certificate(ctx, argc, argv);
    break;
  default:
    status = STPKOC_UNKNOWN_COMMAND;
    break;
  };

  if (status != ST_OK)
    fprintf(stderr, "pkoc-cert-util exit status was %d.\n", status);
  return(status);

} /* main for pkoc-cert-util */


int initialize_pkoc_util
  (PKOC_UTIL_CONTEXT *ctx)

{ /* initialize_pkoc_util */

  int status;


  status = ST_OK;
  memset(ctx, 0, sizeof(*ctx));

  // defaults
  ctx->verbosity = 3;
  ctx->log = stderr;
  ctx->smartcard_subsystem = SC_SUBSYSTEM_SIMULATOR;
// read log-filename from settings
  ctx->command = PKOC_UTIL_REQUEST_CERTIFICATE;
// read the command from a command line switch
  ctx->certificate_index = 0;
// read the certificate index from a command line switch

  // initialize the sub-context(s)

  ctx->sc_ctx = (void *)&my_smartcard_context;

  // read the settings file
  if (status EQUALS ST_OK)
  {
    status = pkoc_util_read_settings(ctx);
  };
  return(status);

} /* initialize_pkoc_util */

