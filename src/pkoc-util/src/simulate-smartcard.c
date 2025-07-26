#define LOG stderr
/*
  simulate-smartcard - simulates the card side of a 7816-4 dialog.

  for now either a stub or PKOC

  define the appropriate symbol to get one of the variants to compile:


    SIMULATE_STUB
    SIMULATE_PKOC
*/
#include <stdio.h>
#include <string.h>


#include <PCSC/wintypes.h>
#include <PCSC/pcsclite.h>
#include <PCSC/winscard.h>


#include <eac-encode.h>
#include <eac-smartcard.h>
#include <ob-stub.h>
#include <pkoc-util.h>
extern char certificate_filename [1024];;


#ifdef SIMULATE_STUB

int simulate_smartcard (SMARTCARD_CONTEXT *ctx, unsigned char cla, unsigned char ins,
  unsigned char p1, unsigned char p2, unsigned int Lc, unsigned char Le,
  unsigned char *payload, int payload_length, int action)
{
return(STSC_NOT_IMPLEMENTED);
}
int sc_simulator_init
  (EAC_SMARTCARD_CONTEXT *ctx,
  SC_SIMULATOR_CONTEXT *sim_ctx)
{
  return(ST_OK);
}
#endif


#ifdef SIMULATE_PKOC

int load_cert_file
  (EAC_SMARTCARD_CONTEXT *ctx,
  SC_SIMULATOR_CONTEXT *sim_ctx,
  unsigned char *cert_buffer,
  int *cert_buffer_length)
{
  FILE *cfile;
  int status;


  status = ST_OK;
//  strcpy(tmps, certificate_filename); printf("Accessing DER-encoded certificate "); printf("%s", tmps);
  if (ctx->verbosity > 3)
    fprintf(stderr, "Accessing DER-encoded certificate %s\n", sim_ctx->certificate_filename);
  cfile = fopen(sim_ctx->certificate_filename, "r");
  if (cfile EQUALS NULL)
  {
    status = STEAC_FILE_ERROR;
    if (ctx->verbosity > 3)
      fprintf(LOG, "error opening certificate file.\n");
  };
  if (status EQUALS ST_OK)
  {
    *cert_buffer_length = fread(cert_buffer, sizeof(*cert_buffer), *cert_buffer_length, cfile);
    if (*cert_buffer_length < 1)
      status= STEAC_FILE_ERROR;
  };
  return(status);
}


int sc_simulator_init
  (EAC_SMARTCARD_CONTEXT *ctx,
  SC_SIMULATOR_CONTEXT *sim_ctx)
{
  sim_ctx->verbosity = ctx->verbosity;
  ctx->subsystem_context = sim_ctx;
  return(ST_OK);
}


int simulate_smartcard
  (EAC_SMARTCARD_CONTEXT *ctx,
  SC_SIMULATOR_CONTEXT *sim_ctx,
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

  unsigned char cert_buffer [8192];
  int cert_buffer_length;
  int match_authenticate;
  int ret_idx;
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
  // fixed response and tag for cert
  unsigned char fixed_response [] = {
    0x5A, 0x04, 0x11, 0x22, 0x33, 0x44,
    0x9E, 0x04, 0xaa, 0xbb, 0xcc, 0xdd,
    0xF8};

  cert_buffer_length = sizeof(cert_buffer);
  status = load_cert_file(ctx, sim_ctx, cert_buffer, &cert_buffer_length);

  if (status EQUALS ST_OK)
  {
    ret_idx = 0;
    memcpy(ret_apdu, fixed_response, sizeof(fixed_response));
    ret_idx = ret_idx + sizeof(fixed_response);
    ret_apdu [ret_idx] = 0x82; // assume der encoding of 2-octet length
    ret_idx++;
    ret_apdu [ret_idx] = (cert_buffer_length & 0xff00) >> 8;
    ret_idx++;
    ret_apdu [ret_idx] = 0xff & cert_buffer_length;
    ret_idx++;
    memcpy(ret_apdu + ret_idx, cert_buffer, cert_buffer_length);
    ret_idx = ret_idx + cert_buffer_length;
    ret_apdu [ret_idx] = 0x90; // 7816-4 good status return
    ret_idx++;
    ret_apdu [ret_idx] = 0x00;
    ret_idx++;
    *ret_lth = ret_idx;
  };
}
  if (status EQUALS ST_OK)
    eac_encode_dump_buffer("PKOC Authentication Response:\n", ret_apdu, *ret_lth);
  };

  return(status);

} /* simulate_smartcard */

#endif

