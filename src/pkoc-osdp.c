#if 0
no-reader-id
no-transaction-id
settings
#endif
#if 0
pd side stuff, anything that can be done on a command line.

also has a settings file, openbadger filename style

openbadger-settings.json


osdp_MFGREP called 00 {"1":"00","2":"0A0017","3":"E0","4":"fd03019000"}
second arg is json string
tag 2 is OUI
tag 3 is command
tag 4 is payload
#endif

/*
  Usage:

  --error <tlv string> - sends error response
  --mfgrep <value arg from ACU action call-out
  --next-transaction <tlv>
  --response-raw
  --verbosity <level)
  --help

Examples:

  pkoc-pd --error FD03019000
    sends TLV string as an MFG error response

  pkoc-pd --card-present FC065C0201004C00
    sends card-present as an MFG card present response

*/


#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>

#include <jansson.h>

#include <ob-stub-include.h>


typedef struct __attribute__((packed)) pkoc_raw_card_present_payload
{
  unsigned char format_code;
  unsigned char bits_lsb;
  unsigned char bits_msb;
  unsigned char data [1024];
} PKOC_RAW_CARD_PRESENT_PAYLOAD;


void bytes_to_hex(unsigned char *raw, int length, char *byte_string);
int hex_to_binary(PKOC_CONTEXT *ctx, char *hex, unsigned char *binary, int *length);
int osdp_send_response_MFG(PKOC_CONTEXT *ctx, unsigned char mfg_response_code, unsigned char * mfg_response_payload, int lth);
int osdp_send_response_RAW(PKOC_CONTEXT *ctx, PKOC_RAW_CARD_PRESENT_PAYLOAD *raw, int lth);
void osdp_submit_command(PKOC_CONTEXT *ctx, char *command);
int pkoc_read_settings(PKOC_CONTEXT *ctx);

PKOC_CONTEXT pkoc_context;

void bytes_to_hex
  (unsigned char *raw,
  int length,
  char *byte_string)

{ /* bytes_to_hex */

  int i;
  char *p;

  p = byte_string;
  for (i=0; i<length; i++)
  {
    sprintf(p, "%02X", *(raw+i));
    p = p + 2;
fprintf(stderr, "DEBUG: bytes as hex (%d.) %s\n", length, byte_string);
  };

} /* bytes_to_hex */


int hex_to_binary
  (PKOC_CONTEXT *ctx,
  char *hex,
  unsigned char *binary,
  int *length)

{ /* hex_to_binary */

  int count;
  int hexit;
  char octet_string [3];
  char *p;
  unsigned char *pbinary;


  p = hex;
  pbinary = binary;
  count = strlen(hex);
  if ((count % 2) != 0)
  {
    count = count - 1;
    fprintf(ctx->log, "trimming hex string to even number of hexits.\n");
  };
  while (count > 0)
  {
    memcpy(octet_string, p, 2);
    octet_string [2] = 0;
    sscanf(octet_string, "%x", &hexit);
    *pbinary = hexit;
    pbinary++;
    p = p + 2;
    count = count - 2;
    (*length)++;
  };

  return(ST_OK);

} /* hex_to_binary */


int main
  (int argc,
  char *argv[])

{ /* main for pkoc-pd */

  unsigned char card_present_payload [1024];
  int card_present_payload_length;
  char command [1024];
  PKOC_CONTEXT *ctx;
  json_t *details_root;
  int i;
  char mfgrep_details [1024];
  PKOC_RAW_CARD_PRESENT_PAYLOAD response_raw;
  int status;
  json_error_t status_json;


  status = ST_OK;
  memset(&pkoc_context, 0, sizeof(pkoc_context));
  ctx = &pkoc_context;
ctx->verbosity = 9;
  ctx->log = stderr;
  ctx->console = stdout;
  ctx->card_present_method = PKOC_CARD_PRESENT_MFG;
  status = pkoc_read_settings(ctx);
  if (status EQUALS ST_OK)
    status = ob_pkoc_commandline(ctx, argc, argv);

  // process options

  if (status EQUALS ST_OK)
  {

    // if --error then send error response

    if (strlen(ctx->error_tlv_hex) > 0)
    {
      memset(&response_raw, 0, sizeof(response_raw));
      status = hex_to_binary(ctx, ctx->error_tlv_hex, card_present_payload, &card_present_payload_length);
      response_raw.format_code = OSDP_RAW_FORMAT_PRIVATE;
      response_raw.bits_lsb = (card_present_payload_length*8) & 0xff;
      response_raw.bits_msb = ((card_present_payload_length*8) & 0xff00) >> 8;
      memcpy(response_raw.data, card_present_payload, card_present_payload_length);
      if (status EQUALS 0)
      {
        if (ctx->card_present_method EQUALS PKOC_CARD_PRESENT_RAW)
          status = osdp_send_response_RAW(ctx, &response_raw, card_present_payload_length);
        else
          status = osdp_send_response_MFG(ctx, OSDP_MFGREP_PKOC_READER_ERROR, card_present_payload, card_present_payload_length);
      };
    };

    // if mfgrep details then we are being called as the MFGREP action routine

    if (strlen(mfgrep_details) > 0)
    {
      int mfgrep_command;
      char mfgrep_command_string [1024];
      char octet [3];
      char pkoc_reader_error_payload [1024];
      int process; 
      unsigned char requested_oui [3];
      char requested_oui_string [1024];
      json_t *value;

      process = 0;
      if (mfgrep_details [0] EQUALS '@')
        details_root = json_load_file(mfgrep_details+1, 0, &status_json);
      else
        details_root = json_loads(mfgrep_details, 0, &status_json);
      if (details_root EQUALS NULL)
        fprintf(stderr, "payload parse error\n");
      if (details_root != NULL)
      {
        memset(requested_oui_string, 0, sizeof(requested_oui_string));
        value = json_object_get(details_root, "2");
        if (json_string_value(value))
          strcpy(requested_oui_string, json_string_value(value));

        octet[2] = 0;
        memcpy(octet, requested_oui_string+0, 2);
        sscanf(octet, "%x", &i);
        requested_oui [0] = i;
        memcpy(octet, requested_oui_string+2, 2);
        sscanf(octet, "%x", &i);
        requested_oui [1] = i;
        memcpy(octet, requested_oui_string+4, 2);
        sscanf(octet, "%x", &i);
        requested_oui [2] = i;
        if (0 EQUALS memcmp(requested_oui, ctx->oui, 3))
          process = 1;
        if (process)
        {
          mfgrep_command_string [0] = 0;
          value = json_object_get(details_root, "3");
          if (json_string_value(value))
            strcpy(mfgrep_command_string, json_string_value(value));
          sscanf(mfgrep_command_string, "%x", &mfgrep_command); 

          switch(mfgrep_command)
          {
          default:
            fprintf(stderr, "Unknown MFGREP command (%02X)\n", mfgrep_command);
            status = -1;
            break;
          case OSDP_MFGREP_PKOC_READER_ERROR:
            pkoc_reader_error_payload [0] = 0;
            value = json_object_get(details_root, "4");
            if (json_string_value(value))
              strcpy(pkoc_reader_error_payload, json_string_value(value));
            fprintf(stderr, "PKOC: Reader Error - %s\n", pkoc_reader_error_payload);
            break;
          };
        };
      };
    };

    if (strlen(ctx->next_transaction_details) > 0)
    {
      sprintf(command, 
"{\\\"command\\\":\\\"mfg\\\",\\\"request-id\\\":\\\"%X\\\",\\\"request-specific-data\\\":\\\"%s\\\"}",
        OSDP_MFG_PKOC_NEXT_TRANSACTION, ctx->next_transaction_details);
      osdp_submit_command(ctx, command);
    };
  };

  if (status != ST_OK)
  {
    fprintf(ctx->log, "pkoc-pd exit status %d.\n", status);
    fprintf(stderr, "pkoc-pd exit status %d.\n", status);
  };
  return(status);

} /* main for pkoc-pd */


int osdp_send_response_MFG
  (PKOC_CONTEXT *ctx,
  unsigned char mfg_response_code,
  unsigned char *payload,
  int lth)

{ /* osdp_send_response_MFG */

  char byte_string [1024];
  char command [1024];
  int status;

  status = ST_OK;
  strcpy(byte_string, "00");
  bytes_to_hex((unsigned char *)payload, lth, byte_string);
  sprintf(command, 
"{\\\"command\\\":\\\"mfg-response\\\",\\\"response-id\\\":\\\"%X\\\",\\\"response-specific-data\\\":\\\"%s\\\"}",
    mfg_response_code, byte_string);
  if (strlen(ctx->control_port) EQUALS 0)
  {
    strcpy(ctx->control_port, "/opt/osdp-conformance/run/PD/open-osdp-control");
    if (ctx->verbosity > 3)
      fprintf(stderr, "MFGREP so assuming standard PD\n");
  };
  osdp_submit_command(ctx, command);
  return(status);

} /* osdp_send_response_MFG */


int osdp_send_response_RAW
  (PKOC_CONTEXT *ctx,
  PKOC_RAW_CARD_PRESENT_PAYLOAD *raw,
  int lth)

{ /* osdp_send_response_RAW */

  int bits;
  char byte_string [1024];
  char command [1024];
  int status;


  status = ST_OK;
fprintf(stderr, "DEBUG: use lth to calc size of response %d\n", lth);
  bytes_to_hex((unsigned char *)raw, sizeof(raw), byte_string);
  bits = sizeof((unsigned char *)raw) * 8;
  sprintf(command, 
"{\"command\":\"present-card\",\"bits\":\"%d\",\"format\":\"80\",\"data\":\"%s\"}",
    bits, byte_string);
  osdp_submit_command(ctx, command);

  return(status);

} /* osdp_send_response_RAW */


void osdp_submit_command
  (PKOC_CONTEXT *ctx,
  char *command)

{ /* osdp_submit_command */

  char control_port [1024];
  char submission_command [2048];

  strcpy(control_port, " ");
  if (strlen(ctx->control_port) > 0)
    strcpy(control_port, ctx->control_port);
  sprintf(submission_command, "/opt/tester/bin/submit-osdp-command \"%s\" %s", command, control_port);
  if (ctx->verbosity > 3)
    fprintf(stderr, "DEBUG: control port %s\n  command %s\n", control_port, command);
  system(submission_command);

} /* osdp_submit_command */


int pkoc_read_settings
  (PKOC_CONTEXT *ctx)

{
  unsigned char oui [] = {0x0A, 0x00, 0x17};

  memcpy(ctx->oui, oui, sizeof(oui));
  return(ST_OK);
}

