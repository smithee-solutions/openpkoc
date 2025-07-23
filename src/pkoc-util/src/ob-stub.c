#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include <PCSC/wintypes.h>
#include <PCSC/pcsclite.h>
#include <PCSC/winscard.h>


#include <openbadger-common.h>

// not quite stablized using openbadger and libeac-encode
#include <ob-stub.h>


/*
  ob_init_smartcard - initialize smartcard operations.

  assumes ctx->reader_index is the reader we want.  default is zero.
  uses ctx->verbosity to control logging
*/

int ob_init_smartcard_ex
  (EAC_SMARTCARD_CONTEXT_EX *ctx)

{ /* ob_init_smartcard */

  int done;
  DWORD dwActiveProtocol;
  LPTSTR mszReaders;
  DWORD reader_names_list_size;
  int offset;
  char *protocol_name;
  char *rdr_list;
  int reader_index;
  int status;
  LONG status_pcsc;


  status = ST_OK;

  // initialize PCSC

  status_pcsc = SCardEstablishContext (SCARD_SCOPE_SYSTEM, NULL, NULL, &(ctx->hContext));
  if (ctx->verbosity > 9)
  {
    fprintf(stderr, "DEBUG: SCardEstablishContext return value was %lX\n", status_pcsc);
  };
  if (status_pcsc != SCARD_S_SUCCESS)
    status = STEAC_SCARD_ESTABLISH;

  if (status EQUALS ST_OK)
  {
  // find out how many readers are attached

  status_pcsc = SCardListReaders (ctx->hContext, NULL, NULL, &reader_names_list_size);
  if (status_pcsc != SCARD_S_SUCCESS)
    status = STEAC_SCARD_ERROR;

  // enumerate to the log all reader names, until we find the one we want

  mszReaders = calloc(reader_names_list_size, sizeof(char));
  status_pcsc = SCardListReaders (ctx->hContext, NULL, mszReaders, &reader_names_list_size);
  if (status_pcsc != SCARD_S_SUCCESS)
    status = STEAC_SCARD_ERROR;
  };
  if (status EQUALS ST_OK)
  {
    done = 0;
    offset = 0;
    reader_index = 0;
    rdr_list = mszReaders;
    ctx->reader_name [0] = 0;
    while (!done)
    {
      if (ctx->verbosity > 3)
        fprintf(stderr, "  Reader %d: %s\n", reader_index, rdr_list);
      if (reader_index EQUALS ctx->reader_index)
      {
        strcpy (ctx->reader_name, rdr_list);
        done = 1;
      };
      if ((1+offset) >= reader_names_list_size)
        done = 1;
      reader_index ++;
      offset = offset + strlen(rdr_list) + 1;
      rdr_list = strlen(rdr_list) + 1 + mszReaders;
    };

    if (ctx->verbosity > 0)
      if (strlen(ctx->reader_name) > 0)
        fprintf (stderr, "Selected reader (%d) is %s\n", ctx->reader_index, ctx->reader_name);
  };

  // initialize PCSC connection to the desired reader.

  if (status EQUALS ST_OK)
  {
    status_pcsc = SCardConnect (ctx->hContext, ctx->reader_name, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &(ctx->pcsc), &dwActiveProtocol);
    if (SCARD_S_SUCCESS != status_pcsc)
      status = STEAC_SCARD_CONNECT;
  };
  if (status EQUALS ST_OK)
  {
    // record if it's T0 or T1

    switch (dwActiveProtocol)
    {
    case SCARD_PROTOCOL_T0:
      ctx->pioSendPci = *SCARD_PCI_T0;
      protocol_name = "T0";
      break;

    case SCARD_PROTOCOL_T1:
      ctx->pioSendPci = *SCARD_PCI_T1;
      protocol_name = "T1";
      break;
    }
    if (ctx->verbosity > 9)
      fprintf (stderr, "Protocol is %s\n", protocol_name);
  };
  ctx->last_pcsc_status = status_pcsc;
  return(status);

} /* ob_init_smartcard */

