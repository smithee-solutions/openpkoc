/*
  ob-pkoc.h - PKOC parameter definitions

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


#define OB_PKOC_PUBKEY_LENGTH                (65)
#define OB_PKOC_SIG_LENGTH                   (64)
#define OB_PKOC_TRANSACTID_LENGTH            (16)
#define OB_PKOC_SITE_KEY_IDENTIFIER_LENGTH   (16)
#define OB_PKOC_READER_KEY_IDENTIFIER_LENGTH (16)

// tag, 1 byte length, 1 byte value, 2 byte arbval

#define OB_PKOC_TAG_TRANSACTION_IDENTIFIER (0x4C)
#define OB_PKOC_TAG_READER_IDENTIFIER      (0x4D)
#define OB_PKOC_TAG_UNCOMP_PUBLIC_KEY      (0x5A)
#define OB_PKOC_TAG_PROTOCOL_VERSION       (0x5C)
#define OB_PKOC_TAG_DIGITAL_SIGNATURE      (0x9E)

#define OB_PKOC_SELECT_RESPONSE_MIN (3+2)

// PKOC OSDP spec parameters

#define PKOC_TAG_ERROR_7816    (0x7F)
#define PKOC_TAG_ERROR_GENERAL (0x7E)
#define OSDP_MFGREP_PKOC_CARD_PRESENT (0xE0)
#define OSDP_MFGREP_PKOC_READER_ERROR (0xFE)
#define OSDP_MFG_PKOC_NEXT_TRANSACTION (0xE3)

// OSDP spec parameters
#define OSDP_RAW_FORMAT_PRIVATE (0x80)

// program option settings

#define PKOC_CARD_PRESENT_MFG (0)
#define PKOC_CARD_PRESENT_RAW (1)

#define OBPKOCOPT_HELP         (  1)
#define OBPKOCOPT_VERBOSITY    (  2)
#define OBPKOCOPT_NOOP         (  3)
#define OBPKOCOPT_BITS         (  4)
#define OBPKOCOPT_CONTROL_PORT (  5)
#define OBPKOCOPT_CARD_VERSION (  6)
#define OBPKOCOPT_ERROR        (  7)
#define OBPKOCOPT_RESPONSE_RAW (  8)
#define OBPKOCOPT_OUI          (  9)
#define OBPKOCOPT_MFGREP       ( 10)
#define OBPKOCOPT_NEXT_TRANSACTION (11)



typedef struct pkoc_context
{
  int bits;
  int card_present_method;
  FILE *console;
  char control_port [1024];
  char error_tlv_hex [1024];
  FILE *log;
  char next_transaction_details [1024];
  int option;
  unsigned char oui [3];
  int request_response_voice; // 0=request
  int verbosity;

  unsigned char protocol_version [OB_STRING_MAX];
  int protocol_version_length;
  unsigned char transaction_identifier [OB_PKOC_TRANSACTID_LENGTH];
  unsigned char site_key_identifier [OB_PKOC_SITE_KEY_IDENTIFIER_LENGTH];
  unsigned char reader_key_identifier [OB_PKOC_READER_KEY_IDENTIFIER_LENGTH];
  unsigned char card_arbval [2]; // arbitrary value
  unsigned char ec_public_key [65];
  unsigned char pkoc_signature [64];

} PKOC_CONTEXT;

// select command: CLA=00 INS=A4 P1=04 P2=00 Lc=08 <AID> Le=00

#ifdef ALLOCATE_SELECT
unsigned char SELECT_PKOC [] = 
  {0x00, 0xa4, 0x04, 0x00, 0x08, 0xA0, 0x00, 0x00, 0x08, 0x98, 0x00, 0x00, 0x01, 0x00};
#endif
#ifndef ALLOCATE_SELECT
extern unsigned char SELECT_PKOC [];
#endif

int ob_pkoc_commandline(PKOC_CONTEXT *ctx, int argc, char *argv[]);
int ob_pkoc_help(PKOC_CONTEXT *ctx);
int ob_validate_select_response(OB_CONTEXT *ctx, PKOC_CONTEXT *pkoc_ctx, unsigned char *response, int response_length);

