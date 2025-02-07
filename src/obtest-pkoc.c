//#define XTN_IDENT_SPEC
#define XTN_IDENT_TEST
unsigned char spec_identifier [] = {
  0x6f, 0xcf, 0x50, 0x12,  0xb2, 0x24, 0x04, 0x3b,
  0x09, 0x35, 0x0a, 0x4f,  0xc5, 0xe5, 0x6a, 0x8f
};

/*
  obtest-pkoc - - test PKOC challenge/response 

  reads from first card reader

  generates on stdout a command appropriate to cause
  libosdp-conformance to emit an osdp_RAW message via OSDP

  (C)Copyright 2023-2025 Smithee Solutions LLC

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
#include <stdlib.h>


#include <PCSC/wintypes.h>
#include <PCSC/pcsclite.h>
#include <PCSC/winscard.h>


#include <eac-encode.h>
#include <ob-crypto.h>
#include <openbadger-common.h>
#include <ob-7816.h>
#include <ob-pcsc.h>
#include <openpkoc-version.h>
#include <openpkoc.h>
int ob_read_settings(OB_CONTEXT *ctx);


int main
  (int argc,
  char *argv [])

{ /* main for obtest-pkoc */

  OB_CONTEXT *ctx;
  DWORD dwRecvLength;
  int index_lc;
  unsigned char msg_cla;
  unsigned char msg_ins;
  unsigned char msg_p1;
  unsigned char msg_p2;
  unsigned char msg_lc;
  unsigned char msg_le;
  BYTE pbRecvBuffer [2*OB_7816_APDU_PAYLOAD_MAX];
  OB_RDRCTX pcsc_reader_context;
  PKOC_CONTEXT pkoc_context;
  EAC_ENCODE_OBJECT pkoc_public_key;
  unsigned char pkoc_signature [EAC_CRYPTO_MAX_DER];
  unsigned char pubkey_der [8192];
  int pubkey_der_length;
  OB_RDRCTX *rdrctx;
  unsigned char saved_public_key [1024];
  unsigned char smartcard_command [OB_7816_BUFFER_MAX];
  int smartcard_command_length;
  int status;
  LONG status_pcsc;
  OB_CONTEXT test_pkoc_context;
  char verify_command [1024];
  unsigned char whole_sig [16384];
  int whole_sig_lth;


  status = ST_OK;
  ctx = &test_pkoc_context;
  memset(ctx, 0, sizeof(*ctx));
  ctx->log = LOG;
  ctx->rdrctx = &pcsc_reader_context;
  pkoc_context.log = LOG;
  pkoc_context.ob_ctx = ctx;
  ctx->bits_to_return = 128;

  // use crypto context for verify signature operation

  memset(&pkoc_public_key, 0, sizeof(pkoc_public_key));
  memset(pkoc_signature, 0, sizeof(pkoc_signature));
  if (status EQUALS ST_OK)
  {
    status = ob_read_settings(ctx);
  };
  rdrctx = ctx->rdrctx;
  smartcard_command_length = 0;
  fprintf(LOG, "obtest PKOC tester %s\n", OPENPKOC_VERSION);
  if (ctx->verbosity > 3)
  {
    fprintf(LOG, "Reader %d.\n", ctx->reader_index);
    fprintf(LOG, "PD Control %s Verbosity %d.\n", ctx->pd_control, ctx->verbosity);
    fprintf(LOG, "Bits to return: %d.\n", ctx->bits_to_return);
  };

  if (status EQUALS ST_OK)
  {
    status = ob_init_smartcard(ctx);

    memcpy(smartcard_command, SELECT_PKOC, sizeof(SELECT_PKOC));
    smartcard_command_length = sizeof(SELECT_PKOC);
  };

  if (ctx->verbosity > 3)
  {
    ob_dump_buffer(ctx, smartcard_command, smartcard_command_length, 1);
  };

  // send application select to card

  dwRecvLength = sizeof(pbRecvBuffer);
  status_pcsc = SCardTransmit(rdrctx->pcsc, &rdrctx->pioSendPci, smartcard_command, smartcard_command_length, NULL, pbRecvBuffer, &dwRecvLength);
  rdrctx->last_pcsc_status = status_pcsc;
  if (SCARD_S_SUCCESS != status_pcsc)
    status = STOB_PCSC_TRANSMIT_1;
  if (status EQUALS ST_OK)
  {
    if (ctx->verbosity > 3)
    {
      fprintf(LOG, "Select returns: ");
      ob_dump_buffer (ctx, pbRecvBuffer, dwRecvLength, 1);
    };

    status = ob_validate_select_response(ctx, &pkoc_context, pbRecvBuffer, dwRecvLength);
  };

  if (status EQUALS ST_OK)
  {
    // set up authentication command

    msg_cla = 0x80;
    msg_ins = 0x80;
    msg_p1 = 0x00;
    msg_p2 = 0x01;
    msg_lc = 0x00;
    msg_le = 0x00;
    smartcard_command_length = 0;
    memset(smartcard_command, 0, sizeof(smartcard_command));
    smartcard_command [smartcard_command_length] = msg_cla;
    smartcard_command_length++;
    smartcard_command [smartcard_command_length] = msg_ins;
    smartcard_command_length++;
    smartcard_command [smartcard_command_length] = msg_p1;
    smartcard_command_length++;
    smartcard_command [smartcard_command_length] = msg_p2;
    smartcard_command_length++;
    // fill in Lc later
    index_lc = smartcard_command_length;
    smartcard_command_length++;
    
    // tag,length,value - protocol version

    smartcard_command [smartcard_command_length] = OB_PKOC_TAG_PROTOCOL_VERSION;
    smartcard_command_length++;
    smartcard_command [smartcard_command_length] = 2;
    smartcard_command_length++;
    memcpy(smartcard_command+smartcard_command_length, pkoc_context.protocol_version, 2);
    smartcard_command_length = smartcard_command_length + 2;
    
    // tag,length,value - transaction id

    smartcard_command [smartcard_command_length] = OB_PKOC_TAG_TRANSACTION_IDENTIFIER;
    smartcard_command_length++;
    smartcard_command [smartcard_command_length] = 0x10;
    smartcard_command_length++;
#ifdef XTN_IDENT_TEST
    memset(pkoc_context.transaction_identifier, 0x17, OB_PKOC_TRANSACTID_LENGTH);
#endif
#ifdef XTN_IDENT_SPEC
    memcpy(pkoc_context.transaction_identifier, spec_identifier, OB_PKOC_TRANSACTID_LENGTH);
#endif
    memcpy(smartcard_command+smartcard_command_length, pkoc_context.transaction_identifier, OB_PKOC_TRANSACTID_LENGTH);
    if (ctx->verbosity > 3)
    {
      fprintf(LOG, "Transaction Identifer:\n");
      ob_dump_buffer (ctx, smartcard_command+smartcard_command_length, OB_PKOC_TRANSACTID_LENGTH, 1);
    };
    smartcard_command_length = smartcard_command_length + OB_PKOC_TRANSACTID_LENGTH;
    
    // tag,length,value - reader identifier

    smartcard_command [smartcard_command_length] = OB_PKOC_TAG_READER_IDENTIFIER;
    smartcard_command_length++;
    smartcard_command [smartcard_command_length] = 0x20;
    smartcard_command_length++;
    memset(pkoc_context.site_key_identifier, 'S', OB_PKOC_SITE_KEY_IDENTIFIER_LENGTH);
    memcpy(smartcard_command+smartcard_command_length, pkoc_context.site_key_identifier, OB_PKOC_SITE_KEY_IDENTIFIER_LENGTH);
    smartcard_command_length = smartcard_command_length + 0x10;
    memset(pkoc_context.reader_key_identifier, 'r', OB_PKOC_READER_KEY_IDENTIFIER_LENGTH);
    memcpy(smartcard_command+smartcard_command_length, pkoc_context.reader_key_identifier, OB_PKOC_READER_KEY_IDENTIFIER_LENGTH);
    smartcard_command_length = smartcard_command_length + 0x10;
    msg_lc = smartcard_command_length - index_lc - 1;
    smartcard_command [index_lc] = msg_lc;
    smartcard_command [smartcard_command_length] = msg_le;
    smartcard_command_length++;

    if (ctx->verbosity > 3)
    {
      fprintf(LOG, "Marshalled Authentication Command:\n");
      ob_dump_buffer (ctx, smartcard_command, smartcard_command_length, 1);
    };
  };

  if (status EQUALS ST_OK)
  {
    // send authentication command to card

    dwRecvLength = sizeof(pbRecvBuffer);
    status_pcsc = SCardTransmit(rdrctx->pcsc, &rdrctx->pioSendPci, smartcard_command, smartcard_command_length, NULL, pbRecvBuffer, &dwRecvLength);
    rdrctx->last_pcsc_status = status_pcsc;
    if (SCARD_S_SUCCESS != status_pcsc)
      status = STOB_PKOC_AUTH;
  };
  if (status EQUALS ST_OK)
  {
    if (ctx->verbosity > 3)
    {
      fprintf(LOG, "Authentication returns:\n");
      ob_dump_buffer (ctx, pbRecvBuffer, dwRecvLength, 1);
    };
  };
  if (status EQUALS ST_OK)
  {
    unsigned char *p;
    int payload_size;
    int remainder;

    // extract the signature and public key.  in tlv, arbitrary order.

    p = pbRecvBuffer;
    remainder = dwRecvLength;
    if (*p EQUALS OB_PKOC_TAG_UNCOMP_PUBLIC_KEY)
    {
      p++;
      remainder--;
      payload_size = *p;
      p++;
      remainder--;

      memcpy(pkoc_public_key.encoded, p, payload_size);
      pkoc_public_key.enc_lth = payload_size;

      memcpy(pkoc_context.ec_public_key, p, payload_size);
      p = p + payload_size;
      remainder = remainder - payload_size;
    };
    if (*p EQUALS OB_PKOC_TAG_DIGITAL_SIGNATURE)
    {
      p++;
      remainder--;
      payload_size = *p;
      p++;
      remainder--;

      memcpy(pkoc_signature, p, payload_size);

      memcpy(pkoc_context.pkoc_signature, p, payload_size);
      p = p + payload_size;
      remainder = remainder - payload_size;
    };
    if (*p EQUALS OB_PKOC_TAG_UNCOMP_PUBLIC_KEY)
    {
      p++;
      remainder--;
      payload_size = *p;
      p++;
      remainder--;

      memcpy(pkoc_public_key.encoded, p, payload_size);
      pkoc_public_key.enc_lth = payload_size;

      memcpy(pkoc_context.ec_public_key, p, payload_size);
      p = p + payload_size;
      remainder = remainder - payload_size;
    };
    if (ctx->verbosity > 3)
    {
      fprintf(LOG, "Public Key:\n");
      ob_dump_buffer (ctx, pkoc_public_key.encoded, pkoc_public_key.enc_lth, 1);
      fprintf(LOG, "Signature:\n");
      ob_dump_buffer (ctx, pkoc_context.pkoc_signature, 64, 1);
    };

    // output a DER-formatted copy of the public key.
    status = op_initialize_pubkey_DER(ctx, pkoc_public_key.encoded, pkoc_public_key.enc_lth, pubkey_der, &pubkey_der_length);
    op_pkoc_print(&pkoc_context, pkoc_public_key.encoded, pkoc_public_key.enc_lth, LOG);
    if (ctx->verbosity > 3)
    {
      fprintf(LOG, "DEBUG: public key (%d.)\n", pkoc_public_key.enc_lth);
      ob_dump_buffer(ctx, pkoc_public_key.encoded, pkoc_public_key.enc_lth, 1);
    };
memcpy(saved_public_key, pkoc_public_key.encoded, pkoc_public_key.enc_lth);
    if (status EQUALS ST_OK)
      fprintf(LOG, "file %s created\n", OPENPKOC_PUBLIC_KEY);
  };
  if (status EQUALS ST_OK)
  {
    // output a DER-formatted copy of the signature.
    status = op_initialize_signature_DER(ctx, pkoc_signature, 32, pkoc_signature+32, 32, whole_sig, &whole_sig_lth);
    if (status EQUALS ST_OK)
      fprintf(LOG, "file ec-sig.der created\n");
  };
  if (status EQUALS ST_OK)
  {
    FILE *tbs_blob;

    // write the to-be-signed blob to disk and verify the signature.  just the transaction identifier.

    tbs_blob = fopen("tbs-pkoc.bin", "w");
    fwrite(pkoc_context.transaction_identifier, 1, OB_PKOC_TRANSACTID_LENGTH, tbs_blob);
    fclose(tbs_blob);

    /*
      having constructed a proper DER-formatted signature and a proper
      DER-formatted copy of the public key extracted from the card,
      use openssl to perform an ECDSA signature verification operation.
    */
    sprintf(verify_command, "openssl version;openssl dgst -sha256 -verify %s -signature ec-sig.der tbs-pkoc.bin", OPENPKOC_PUBLIC_KEY);
    if (ctx->verbosity > 3)
      fprintf(LOG, "verify command: %s\n", verify_command);
    if (ctx->verbosity > 0)
      fprintf(LOG, "Checking signature with openssl\n");
    system(verify_command);
  };

  if (status EQUALS ST_OK)
  {
    fprintf(LOG, "Verifying signature.\n");
    status = op_verify_signature(&pkoc_context, pubkey_der, pubkey_der_length);
  };

  if (status EQUALS ST_OK)
  {
    char command [1024];
    int i;
    char octet_string [3];
    int offset;
    char osdp_command [2048];
    unsigned char raw_key [64];
    FILE *resp;
    int return_size;

    memset(raw_key, 0, sizeof(raw_key));

    if (ctx->bits_to_return EQUALS 64)
    {
      offset = 1+(128/8)+(64/8);
      memcpy(raw_key, saved_public_key+offset, ctx->bits_to_return/8);
      return_size = ctx->bits_to_return/8;
    }
    else
    {
fprintf(LOG, "assuming low order 128 bits.\n");
      offset = 1+(128/8);
      memcpy(raw_key, saved_public_key+offset, 128/8);
      return_size = 128/8;
    }

    sprintf(osdp_command, "{\"command\":\"present-card\",\"format\":\"raw\",\"bits\":\"%d\",\"raw\":\"", ctx->bits_to_return);

    for (i=0; i<return_size; i++)
    {
      sprintf(octet_string, "%02X", raw_key [i]);
      strcat(osdp_command, octet_string);
    };
    strcat(osdp_command, "\"}\n");
    if (ctx->verbosity > 2)
      fprintf(LOG, "OSDP Response will be:\n%s", osdp_command);
    resp = fopen("pkoc-read.json", "w");
    fprintf(resp, "%s", osdp_command);
    fclose(resp);
    fprintf(LOG, "file %s created, sending read command to PD.\n", "pkoc-read.json");
    sprintf(command, "/opt/osdp-conformance/bin/open-osdp-kick PD <pkoc-read.json");
    system(command);
  };

  if (status != ST_OK)
    fprintf(LOG, "return status %d. last PCSC status %lX %s\n", status, rdrctx->last_pcsc_status, ob_pcsc_error_string(rdrctx->last_pcsc_status));
  return(status);

} /* main for obtest-pkoc */

