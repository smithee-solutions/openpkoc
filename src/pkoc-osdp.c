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


#define EQUALS ==

// OSDP protocol parameters

#define OSDP_RAW_FORMAT_PRIVATE (0x80)

// PKOC OSDP spec parameters

#define PKOC_TAG_ERROR_7816    (0x7F)
#define PKOC_TAG_ERROR_GENERAL (0x7E)
#define OSDP_MFGREP_PKOC_CARD_PRESENT (0xE0)
#define OSDP_MFGREP_PKOC_READER_ERROR (0xFE)

// program option settings

#define PKOC_CARD_PRESENT_MFG (0)
#define PKOC_CARD_PRESENT_RAW (1)


typedef struct __attribute__((packed)) pkoc_raw_card_present_payload
{
  unsigned char format_code;
  unsigned char bits_lsb;
  unsigned char bits_msb;
  unsigned char data [1024];
} PKOC_RAW_CARD_PRESENT_PAYLOAD;

typedef struct pkoc_context
{
  int option;
  int verbosity;
  FILE *log;
  FILE *console;
  char control_port [1024];
  int bits;
  char error_tlv_hex [1024];
  int card_present_method;
  int request_response_voice; // 0=request
  unsigned char oui [3];
} PKOC_CONTEXT;


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
#if 0
no-reader-id
no-transaction-id
settings
#endif

#define ST_OK (  0)

void bytes_to_hex(unsigned char *raw, int length, char *byte_string);
int hex_to_binary(PKOC_CONTEXT *ctx, char *hex, unsigned char *binary, int *length);
int osdp_send_response_MFG(PKOC_CONTEXT *ctx, unsigned char mfg_response_code, unsigned char * mfg_response_payload, int lth);
int osdp_send_response_RAW(PKOC_CONTEXT *ctx, PKOC_RAW_CARD_PRESENT_PAYLOAD *raw, int lth);
void osdp_submit_command(PKOC_CONTEXT *ctx, char *command);
int pkoc_help(PKOC_CONTEXT *ctx);
int pkoc_read_settings(PKOC_CONTEXT *ctx);

PKOC_CONTEXT pkoc_context;

struct option longopts [] = {
      {"bits", required_argument, &pkoc_context.option, OBPKOCOPT_BITS},
      {"card-version", required_argument, &pkoc_context.option, OBPKOCOPT_CARD_VERSION},
      {"control-port", required_argument, &pkoc_context.option, OBPKOCOPT_CONTROL_PORT},
      {"error", required_argument, &pkoc_context.option, OBPKOCOPT_ERROR},
      {"help", 0, &pkoc_context.option, OBPKOCOPT_HELP},
      {"mfgrep", required_argument, &pkoc_context.option, OBPKOCOPT_MFGREP},
      {"OUI", required_argument, &pkoc_context.option, OBPKOCOPT_OUI},
      {"response-raw", 0, &pkoc_context.option, OBPKOCOPT_RESPONSE_RAW},
      {"verbosity", required_argument, &pkoc_context.option, OBPKOCOPT_VERBOSITY},
      {0, 0, 0, 0}};

char optstring [1024];

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
  PKOC_CONTEXT *ctx;
  json_t *details_root;
  int done;
  int found_something;
  int i;
  char mfgrep_details [1024];
  PKOC_RAW_CARD_PRESENT_PAYLOAD response_raw;
  int status;
  json_error_t status_json;
  int status_opt;


  status = ST_OK;
  memset(&pkoc_context, 0, sizeof(pkoc_context));
  ctx = &pkoc_context;
ctx->verbosity = 9;
  ctx->log = stderr;
  ctx->console = stdout;
  ctx->card_present_method = PKOC_CARD_PRESENT_MFG;
  status = pkoc_read_settings(ctx);

  if (status EQUALS ST_OK)
  {
    done = 0;
    while (!done)
    {
      ctx->option = OBPKOCOPT_NOOP;
      status_opt = getopt_long (argc, argv, optstring, longopts, NULL);
      if (status_opt EQUALS -1)
        if (!found_something)
          ctx->option = OBPKOCOPT_HELP;  // found nothing and/or end of list so give help
      if (ctx->verbosity > 9)
        fprintf (ctx->log, "option %2d\n", ctx->option);
      switch (ctx->option)
      {
      case OBPKOCOPT_BITS:
        found_something = 1;
        sscanf(optstring, "%d", &i);
        ctx->bits = i;
fprintf(stderr, "DEBUG: range check bits\n");
        break;

      case OBPKOCOPT_CONTROL_PORT:
        found_something = 1;
        strcpy(ctx->control_port, optstring);
        break;

      case OBPKOCOPT_ERROR:

        // command line directs sending an MFGREP.
        ctx->request_response_voice = 1;
        found_something = 1;
        strcpy(ctx->error_tlv_hex, optarg);

        break;

      case OBPKOCOPT_HELP:
        found_something = 1;
        status = pkoc_help(ctx);
        break;

      case OBPKOCOPT_MFGREP:
        found_something = 1;
        strcpy(mfgrep_details, optarg);
        break;

      case OBPKOCOPT_RESPONSE_RAW:
        found_something = 1;
        ctx->card_present_method = PKOC_CARD_PRESENT_RAW;
        break;

      case OBPKOCOPT_VERBOSITY:
        found_something = 1;
        sscanf(optstring, "%d", &i);
        ctx->verbosity = i;
        break;

      case OBPKOCOPT_NOOP:
        break;

      default:
        fprintf(stderr, "OPTION NOT IMPLEMENTED %d\n", ctx->option);
        break;
      };
      if (status_opt EQUALS -1)
        done = 1;
    };
  };

  // process options

  if (status EQUALS ST_OK)
  {

    // if --error then send error resposne

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


int pkoc_help
  (PKOC_CONTEXT *ctx)

{ /* pkoc_help */

  fprintf(ctx->log, "Usage:\n");
  fprintf(ctx->log, "  pkoc-pd <switches>\n");
  fprintf(ctx->log, "  --bits - number of bits in response\n");
  fprintf(ctx->log, "  --card-version - card version TLV to return to ACU (in hex)\n");
  fprintf(ctx->log, "  --control-port - path of libosdp-conformance control port (default /opt/osdp-conformance/run/PD/osdp-control-port)\n");
  fprintf(ctx->log, "  --error <error TLV>\n");
  fprintf(ctx->log, "  --help - this message\n");
  fprintf(ctx->log, "  --mfgrep <value arg from ACU action call-out>\n");
  fprintf(ctx->log, "  --OUI - organizational unit indicator to be used in MFG commands and responses\n");
  fprintf(ctx->log, "  --response-raw - use osdp_RAW response for card present.\n");
  fprintf(ctx->log, "  --verbosity - logging level.  0=none, 3=normal, 9=debug.  Default is 3.\n");

  return(ST_OK);

} /* pkoc_help */

int pkoc_read_settings
  (PKOC_CONTEXT *ctx)

{
  unsigned char oui [] = {0x0A, 0x00, 0x17};

  memcpy(ctx->oui, oui, sizeof(oui));
  return(ST_OK);
}

