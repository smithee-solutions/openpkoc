#define EQUALS ==

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
} PKOC_CONTEXT;

#define ST_OK (0)

int ob_pkoc_commandline(PKOC_CONTEXT *ctx, int argc, char *argv[]);
int ob_pkoc_help(PKOC_CONTEXT *ctx);

