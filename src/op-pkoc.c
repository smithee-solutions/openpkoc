/*
  ob-pkoc - PKOC-specific routines

  (C)Copyright 2023 Smithee Solutions LLC

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


#include <PCSC/wintypes.h>
#include <PCSC/pcsclite.h>
#include <PCSC/winscard.h>


#include <eac-encode.h>
#include <ob-crypto.h>
#include <openbadger-common.h>
#include <ob-7816.h>
#include <ob-pkoc.h>
#include <openpkoc.h>

// kludge.  init sig function should take pkoc context not openbadger context, it
// builds this.
unsigned char whole_sig_shared [16384];
int whole_sig_lth_shared;


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


int op_initialize_pubkey_DER
  (OB_CONTEXT *ctx,
  unsigned char *key_buffer,
  int kblth,
  unsigned char *marshalled_DER,
  int *marshalled_length)

{ /* op_intiialize_pubkey_DER */

  FILE *ec_der_key;


  ec_der_key = fopen(OPENPKOC_PUBLIC_KEY, "w");
  fwrite(ec_public_key_der_skeleton, 1, sizeof(ec_public_key_der_skeleton), ec_der_key);
  memcpy(marshalled_DER, ec_public_key_der_skeleton, sizeof(ec_public_key_der_skeleton));
  fwrite(key_buffer, 1, kblth, ec_der_key);
  memcpy(marshalled_DER+sizeof(ec_public_key_der_skeleton), key_buffer, kblth);
  fclose(ec_der_key);
  *marshalled_length = sizeof(ec_public_key_der_skeleton) + kblth;

  return(ST_OK);

} /* op_intiialize_pubkey_DER */


int op_initialize_signature_DER
  (OB_CONTEXT *ctx,
  unsigned char *part_1,
  int part1lth,
  unsigned char *part_2,
  int part2lth,
  unsigned char *marshalled_signature,
  int *whole_sig_lth)

{ /* op_initialize_signature_DER */

  FILE *ec_der_sig;
  int lth;
  unsigned char *pwholesig;
  int whole_length;

//todo note the marshalled signature is returned, eliminate the shared buffer kludge...


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

  whole_sig_lth_shared = *whole_sig_lth;
  memcpy(whole_sig_shared, marshalled_signature, whole_sig_lth_shared);

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


int op_verify_signature
  (PKOC_CONTEXT *ctx,
  unsigned char *pubkey_der,
  int pubkey_der_length)

{ /* op_verify_signature */

  EAC_ENCODE_CONTEXT crypto_context;
  unsigned char digest [EAC_CRYPTO_SHA256_DIGEST_SIZE];
  int digest_lth;
  EAC_ENCODE_OBJECT digest_object;
  char log_message [2048];
  OB_CONTEXT openbadger_context;
  EAC_ENCODE_OBJECT pkoc_public_key;
  EAC_ENCODE_OBJECT signature_info;
  EAC_ENCODE_OBJECT signature_object;
  int status;


  status = ST_OK;

  if (status EQUALS ST_OK)
  {
    memset(&openbadger_context, 0, sizeof(openbadger_context));
    openbadger_context.verbosity = ctx->verbosity;
    openbadger_context.log = ctx->log;
    memset(&crypto_context, 0, sizeof(crypto_context));
    crypto_context.verbosity = ctx->verbosity;
    crypto_context.eac_log = eac_log;
    status = eac_encode_allocate_object(&crypto_context, &pkoc_public_key);
    if (status EQUALS ST_OK)
      status = eac_crypto_internal_allocate(&crypto_context, &pkoc_public_key);
    if (status EQUALS ST_OK)
      status = eac_crypto_digest_init(&crypto_context, &digest_object);
  };
  if (status EQUALS ST_OK)
  {
    status = eac_crypto_digest_update(&crypto_context, &digest_object, ctx->transaction_identifier, sizeof(ctx->transaction_identifier));
  };
  if (status EQUALS ST_OK)
     status = eac_crypto_digest_finish(&crypto_context, &digest_object, digest, &digest_lth);
  if (status EQUALS ST_OK)
  {
    if (crypto_context.verbosity > 3)
    {
      fprintf(LOG, "digest...\n");
      ob_dump_buffer(&openbadger_context, digest, digest_lth, 1);
    };
  };
  if (digest_object.internal)
      free(digest_object.internal);

  if (status EQUALS ST_OK)
  {
    pkoc_public_key.key_parameters [0] = EAC_CRYPTO_EC;
    pkoc_public_key.key_parameters [1] = EAC_KEY_EC_CURVE_SECP256R1;

    status = eac_crypto_pubkey_init(&crypto_context, &pkoc_public_key, pubkey_der, pubkey_der_length);
  };
  if (status EQUALS ST_OK)
  {
    if (ctx->verbosity > 3)
    {
      if (strlen(pkoc_public_key.description) > 0)
      {
        sprintf(log_message, "public key init status %d public key type is %s\n", status, pkoc_public_key.description);
        (crypto_context.eac_log)(log_message);
      };
    };
  };
  if (status EQUALS ST_OK)
  {
    memcpy(signature_object.encoded, whole_sig_shared, whole_sig_lth_shared);
    signature_object.enc_lth = whole_sig_lth_shared;
    status = eac_crypto_verify_signature_ex(&crypto_context,
      &pkoc_public_key, digest, digest_lth,
      &signature_object, &signature_info);
    if (status EQUALS ST_OK)
      fprintf(LOG, "***SIGNATURE VALID***\n");
  };

  return(status);

} /* op_verify_signature */

