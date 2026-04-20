/*
  ob-pkoc - PKOC-specific routines

  (C)Copyright 2023-2026 Smithee Solutions LLC

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/


#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>


#include <PCSC/wintypes.h>
#include <PCSC/pcsclite.h>
#include <PCSC/winscard.h>


#include <eac-encode.h>
#include <openbadger-common.h>
#include <ob-crypto.h>
#include <ob-7816.h>
#include <openpkoc.h>

// specifically for NIST P256R1

unsigned char ec_public_key_der_skeleton [] =
{
  0x30,0x59,0x30,0x13,0x06,0x07,0x2a,0x86,0x48,0xce,0x3d,0x02,0x01,0x06,0x08,0x2a,0x86,0x48,0xce,0x3d,0x03,0x01,0x07,0x03,0x42,0x00
  // actual 65 byte key goes after this
};
unsigned char ec_signature_der_skeleton_1 [] =
{
  0x30,0x44,0x02,0x20,0x00 
// then 32 of part 1
};
unsigned char ec_signature_der_skeleton_2 [] =
{
  0x02,0x20,0x00 
// then 32 of part 2
};
extern void eac_log(char *msg);


// the input is the x9.62 (04...) sequence in the raw buffer

int op_initialize_pubkey_DER
  (OB_CONTEXT *ctx,
  OB_CRYPTO_OBJECT *key)

{ /* op_intiialize_pubkey_DER */

  FILE *ec_der_key;
  unsigned char key_buffer_temp [32768];
  int key_buffer_temp_lth;
  unsigned char marshalled_DER [32768];
  int marshalled_length;
  int status;


  marshalled_length = 0;
  key_buffer_temp_lth = key->raw_lth;
  memcpy(key_buffer_temp, key->raw, key->raw_lth);
  memset(key, 0, sizeof(*key));  // we get to initialize it.

  // copy the encoded data to the buffer

    ec_der_key = fopen(OPENPKOC_PUBLIC_KEY, "w");
    fwrite(ec_public_key_der_skeleton, 1, sizeof(ec_public_key_der_skeleton), ec_der_key);
    memcpy(marshalled_DER, ec_public_key_der_skeleton, sizeof(ec_public_key_der_skeleton));
    marshalled_length = sizeof(ec_public_key_der_skeleton);

    fwrite(key_buffer_temp, 1, key_buffer_temp_lth, ec_der_key);
    memcpy(marshalled_DER+sizeof(ec_public_key_der_skeleton), key_buffer_temp, key_buffer_temp_lth);
    marshalled_length = marshalled_length + key_buffer_temp_lth;

    fclose(ec_der_key);

  memcpy(key->encoded, marshalled_DER, marshalled_length);
  key->enc_lth = marshalled_length;
  key->key_parameters [0] = 0; // this is DER
  status = ob_crypto_initialize_key(ctx, key, OB_CRYPTO_ALG_EC, EAC_KEY_EC_CURVE_SECP256R1);

  return(status);

} /* op_intiialize_pubkey_DER */


int op_initialize_signature_DER
  (OB_CONTEXT *ctx,
  PKOC_CONTEXT *pkoc,
  OB_CRYPTO_OBJECT *signature)

{ /* op_initialize_signature_DER */

#if DEPRECATED
  unsigned char *pwholesig;
  int whole_length;


  memset(signature, 0, sizeof(*signature));  // we get to initialize it.

  // if the pieces have the high order bit set insert a null byte

  // fiddle the outer length accordingly

  whole_length = 32 + 32 + 4;
  if (0x80 & *part_1)
  {
    whole_length++;
    ec_signature_der_skeleton_1 [3] = 0x21;
    if (ctx->verbosity > 9)
      fprintf(LOG, "part 1 first octet %02X\n", *part_1);
  };
  if (0x80 & *part_2)
  {
    whole_length++;
    ec_signature_der_skeleton_2 [1] = 0x21;
    if (ctx->verbosity > 9)
      fprintf(LOG, "part 2 first octet %02X\n", *part_2);
  };
  ec_signature_der_skeleton_1 [1] = whole_length;

  ec_der_sig = fopen("ec-sig.der", "w");
  lth = sizeof(ec_signature_der_skeleton_1);
  if (!(0x80 & *part_1))
    lth--;
  if (ctx->verbosity > 9)
    fprintf(LOG, "part 1 write length %d.\n", lth);

  fwrite(ec_signature_der_skeleton_1, 1, lth, ec_der_sig);
  pwholesig = marshalled_signature;
  memcpy(pwholesig, ec_signature_der_skeleton_1, lth);
  pwholesig = pwholesig + lth;
  *whole_sig_lth = lth;

  fwrite(part_1, 1, part1lth, ec_der_sig);
  memcpy(pwholesig, part_1, part1lth);
  pwholesig = pwholesig + part1lth;
  *whole_sig_lth = *whole_sig_lth + part1lth;

  lth = sizeof(ec_signature_der_skeleton_2);
  if (!(0x80 & *part_2))
    lth--;
  if (ctx->verbosity > 9)
    fprintf(LOG, "part 2 write length %d.\n", lth);

  fwrite(ec_signature_der_skeleton_2, 1, lth, ec_der_sig);
  memcpy(pwholesig, ec_signature_der_skeleton_2, lth);
  pwholesig = pwholesig + lth;
  *whole_sig_lth = *whole_sig_lth + lth;

  fwrite(part_2, 1, part2lth, ec_der_sig);
  memcpy(pwholesig, part_2, part2lth);
  *whole_sig_lth = *whole_sig_lth + part2lth;

  fclose(ec_der_sig);

  return(ST_OK);
#endif

  FILE *ec_der_sig;
  int lth;
  unsigned char *marshalled_DER;
  unsigned char *part_1;
  unsigned char *part_2;
  int whole_length;


  /*
    the raw signature is in the context structure.  
    this fabricates a proper DER-encoded version, writes it to disk, 
    and sets it up in the "encoded" section of the signature crypto object.
  */

  signature->enc_lth = 0;
  part_1 = pkoc->pkoc_signature;
  part_2 = 32+part_1;
  whole_length = 32 + 32 + 4;

  /*
    if the pieces have the high order bit set insert a null byte.
    this requires tweaking the outer DER TLV.
  */
  if (0x80 & *part_1)
  {
    whole_length++;
    ec_signature_der_skeleton_1 [3] = 0x21;
    if (ctx->verbosity > 9)
      fprintf(LOG, "part 1 first octet %02X\n", *part_1);
  };
  if (0x80 & *part_2)
  {
    whole_length++;
    ec_signature_der_skeleton_2 [1] = 0x21;
    if (ctx->verbosity > 9)
      fprintf(LOG, "part 2 first octet %02X\n", *part_2);
  };
  ec_signature_der_skeleton_1 [1] = whole_length;

  // build it out in the signature object and also write it out

  marshalled_DER = signature->encoded;

  // first part of skeleton, tweaked

  lth = sizeof(ec_signature_der_skeleton_1);
  if (!(0x80 & *part_1))
    lth--;
  memcpy(marshalled_DER, ec_signature_der_skeleton_1, lth);
  marshalled_DER = marshalled_DER + lth;
  signature->enc_lth = lth;

  // first part of signature

  memcpy(marshalled_DER, part_1, 32);
  marshalled_DER = marshalled_DER + 32;
  signature->enc_lth = signature->enc_lth + 32;

  // second part of skeleton, tweaked
  lth = sizeof(ec_signature_der_skeleton_2);
  if (!(0x80 & *part_2))
    lth--;
  memcpy(marshalled_DER, ec_signature_der_skeleton_2, lth);
  marshalled_DER = marshalled_DER + lth;
  signature->enc_lth = signature->enc_lth + lth;

  // second part of signature

  memcpy(marshalled_DER, part_2, 32);
  marshalled_DER = marshalled_DER + 32;
  signature->enc_lth = signature->enc_lth + 32;

  ec_der_sig = fopen("ec-sig.der", "w");
  fwrite(signature->encoded, sizeof(signature->encoded[0]), signature->enc_lth, ec_der_sig);
  fclose(ec_der_sig);

  return(ST_OK);

} /* op_initialize_signature_DER */


int ob_validate_select_response
  (OB_CONTEXT *ctx,
  PKOC_CONTEXT *pkoc_ctx,
  unsigned char *response,
  int response_length)

{ /* ob_validate_select_response */

  unsigned char *p;
  int prot_ver_lth;
  int remainder;
  int status;


  status = ST_OK;

  // todo: should both contexts be here?
  pkoc_ctx->verbosity = ctx->verbosity;

  if (status EQUALS ST_OK)
  {
    p = response;
    remainder = response_length;

    if (response_length < OB_PKOC_SELECT_RESPONSE_MIN)
      status = STOB_SELECT_RESPONSE;
  };
  if (status EQUALS ST_OK)
  {
    // first thing is a "Version" TLV.  Check tag

    if (*p != OB_PKOC_TAG_PROTOCOL_VERSION)
      status = STOB_SELRSP_TAG;
  };
  if (status EQUALS ST_OK)
  {
    p++;
    remainder--;

    // sanity check length

    prot_ver_lth = *p;
    p++;
    remainder--;
    if (ctx->verbosity > 3)
      fprintf(LOG, "Select response: protocol version (%d. octets) is %02X%02X\n",
        prot_ver_lth, *p, *(p+1));
    memcpy(pkoc_ctx->protocol_version, p, prot_ver_lth);
    pkoc_ctx->protocol_version_length = prot_ver_lth;
    p = p + prot_ver_lth;
    remainder = remainder - prot_ver_lth;
status = ST_OK;
  };
  if (status EQUALS ST_OK)
  {
    if (remainder > 2)
    {
      // card nonce

      memcpy(pkoc_ctx->card_arbval, p, 2);
      status = ST_OK;
    };
  };

  return(status);

} /* ob_validate_select_response */


// assumes pkoc_public_key is fully initialized.

int op_verify_signature
  (PKOC_CONTEXT *ctx,
  OB_CRYPTO_OBJECT *pkoc_public_key,
  unsigned char *message,
  int message_lth,
  OB_CRYPTO_OBJECT *signature_object)

{ /* op_verify_signature */

  char log_message [2048];
  OB_CONTEXT openbadger_context;
  int status;


  status = ST_OK;
  memset(&openbadger_context, 0, sizeof(openbadger_context));
  openbadger_context.verbosity = ctx->verbosity;
  openbadger_context.log = ctx->log;

  if (status EQUALS ST_OK)
  {
    if (ctx->verbosity > 3)
    {
      if (strlen(pkoc_public_key->description) > 0)
      {
        sprintf(log_message, "public key init status %d public key type is %s\n", status, pkoc_public_key->description);
        fprintf(openbadger_context.log, "%s", log_message);
      };
    };
  };
  if (status EQUALS ST_OK)
  {
    status = ob_crypto_verify_signature(&openbadger_context, 
      pkoc_public_key, message, message_lth, signature_object);
    if (status EQUALS ST_OK)
      fprintf(LOG, "***SIGNATURE VALID***\n");
  };

  return(status);

} /* op_verify_signature */

