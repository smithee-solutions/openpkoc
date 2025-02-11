/*
  ob-pkoc-commandline.h - command line parameters for openpkoc

   (C) Copyright 2023-2025 Smithee Solutions LLC

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
#include <getopt.h>
#include <string.h>
#include <stdlib.h>

#include <jansson.h>

#include <openbadger-common.h>
#include <ob-pkoc.h>


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

char optstring [1024];
extern PKOC_CONTEXT pkoc_context;
struct option longopts [] = {
      {"bits", required_argument, &pkoc_context.option, OBPKOCOPT_BITS},
      {"card-version", required_argument, &pkoc_context.option, OBPKOCOPT_CARD_VERSION},
      {"control-port", required_argument, &pkoc_context.option, OBPKOCOPT_CONTROL_PORT},
      {"error", required_argument, &pkoc_context.option, OBPKOCOPT_ERROR},
      {"help", 0, &pkoc_context.option, OBPKOCOPT_HELP},
      {"mfgrep", required_argument, &pkoc_context.option, OBPKOCOPT_MFGREP},
      {"next-transaction", required_argument, &pkoc_context.option, OBPKOCOPT_NEXT_TRANSACTION},
      {"OUI", required_argument, &pkoc_context.option, OBPKOCOPT_OUI},
      {"response-raw", 0, &pkoc_context.option, OBPKOCOPT_RESPONSE_RAW},
      {"verbosity", required_argument, &pkoc_context.option, OBPKOCOPT_VERBOSITY},
      {0, 0, 0, 0}};


int ob_pkoc_commandline
  (PKOC_CONTEXT *ctx,
  int argc,
  char *argv[])

{ /* ob_pkoc_commandline */

  int done;
  int found_something;
  int i;
  char mfgrep_details [1024];
  int status;
  int status_opt;


  status = ST_OK;
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
fprintf(LOG, "DEBUG: range check bits\n");
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
        status = ob_pkoc_help(ctx);
        break;

      case OBPKOCOPT_MFGREP:
        found_something = 1;
        strcpy(mfgrep_details, optarg);

      case OBPKOCOPT_NEXT_TRANSACTION:
        found_something = 1;
        strcpy(ctx->next_transaction_details, optarg);
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
        fprintf(LOG, "OPTION NOT IMPLEMENTED %d\n", ctx->option);
        break;
      };
      if (status_opt EQUALS -1)
        done = 1;
    };
  };

  return(status);

} /* ob_pkoc_commandline */


int ob_pkoc_help
  (PKOC_CONTEXT *ctx)

{ /* ob_pkoc_help */

  fprintf(ctx->log, "Usage:\n");
  fprintf(ctx->log, "  pkoc-pd <switches>\n");
  fprintf(ctx->log, "  --bits - number of bits in response\n");
  fprintf(ctx->log, "  --card-version - card version TLV to return to ACU (in hex)\n");
  fprintf(ctx->log, "  --control-port - path of libosdp-conformance control port (default /opt/osdp-conformance/run/PD/osdp-control-port)\n");
  fprintf(ctx->log, "  --error <error TLV>\n");
  fprintf(ctx->log, "  --help - this message\n");
  fprintf(ctx->log, "  --mfgrep <value arg from ACU action call-out>\n");
  fprintf(ctx->log, "  --next-transaction <tlv>\n");
  fprintf(ctx->log, "  --OUI - organizational unit indicator to be used in MFG commands and responses\n");
  fprintf(ctx->log, "  --response-raw - use osdp_RAW response for card present.\n");
  fprintf(ctx->log, "  --verbosity - logging level.  0=none, 3=normal, 9=debug.  Default is 3.\n");

  return(ST_OK);

} /* ob_pkoc_help */

