// set up PKOC 7816 APDU(s)


#include <stdio.h>
#include <string.h>


#include <PCSC/wintypes.h>
#include <PCSC/pcsclite.h>
#include <PCSC/winscard.h>


#include <pkoc-parameters.h>
#include <eac-encode.h>
#include <eac-smartcard.h>
#include <ob-stub.h>
#include <pkoc-util.h>


/*
  setup_pkoc_authenticate - given a transaction id, protocol version, and reader identifier
  create the Authentication Request APDU.

  Arguments:
    context pointer
    output buffer pointer
    output buffer length (pass in max, returns actual)
    variable length transaction id
    variable length protocol version
    reader identifier
*/

int setup_pkoc_authenticate
  (PKOC_UTIL_CONTEXT *ctx,
  unsigned char *buffer,
  int *buffer_length,
  unsigned char *transaction_id,
  int xtn_id_lth,
  unsigned char *protocol_version,
  int ver_lth,
  unsigned char *reader_identifier,
  int cert_index)

{ /* setup_pkoc_authenticate */

  unsigned char *p;
  int plth;
  int status;


  status = ST_OK;
  plth = 0;
  p = buffer;

  // tag,length,value - protocol version
  *p = PKOC_TAG_PROTOCOL_VERSION;
  p++; plth++;
  *p = ver_lth;
// sanity check version length
  p++; plth++;
  memcpy(p, protocol_version, ver_lth);
  p = p + ver_lth;
  plth = plth + ver_lth;

  // tag,length,value - transaction id
  *p = PKOC_TAG_TRANSACTION_IDENTIFIER;
  p++; plth++;
  *p = xtn_id_lth;
  p++; plth++;
  memcpy(p, transaction_id, xtn_id_lth);
  p = p + xtn_id_lth;
  plth = plth + xtn_id_lth;

  // tag,length,value - reader identifier
  *p = PKOC_TAG_READER_IDENTIFIER;
  p++; plth++;
  *p = sizeof(ctx->site_key_identifier) + sizeof(ctx->reader_location_identifier);
  p++; plth++;
  memcpy(p, ctx->site_key_identifier, sizeof(ctx->site_key_identifier));
  p = p + sizeof(ctx->site_key_identifier);
  plth = plth + sizeof(ctx->site_key_identifier);
  memcpy(p, ctx->reader_location_identifier, sizeof(ctx->reader_location_identifier));
  p = p + sizeof(ctx->reader_location_identifier);
  plth = plth + sizeof(ctx->reader_location_identifier);

#ifdef SIMULATE_PKOC

  // tag F9 Return Certificate
  // for now just ask for the first certificate.
  {
    int param_cert_index;

    param_cert_index = 0;
    fprintf(LOG, "Requesting certificate at index %d.\n", param_cert_index);
    *p = 0xF9; // Return Certificate
    p++; plth++;
    *p = 1; // fixed length certificate index
    p++; plth++;
    *p = param_cert_index; // request certificate index zero
    p++; plth++;
  };
#endif

  *buffer_length = plth;
  return(status);

} /* setup_pkoc_authenticate */


