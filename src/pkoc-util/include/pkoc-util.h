// pkoc-util.h


#define PKOC_UTIL_SETTINGS_FILE "pkoc-util-settings.json"

#define PKOC_TRANSACTION_IDENTIFIER_MIN (16)
#define PKOC_TRANSACTION_IDENTIFIER_MAX (65)
#define PKOC_SITE_KEY_IDENTIFIER        (16)
#define PKOC_READER_LOCATION_IDENTIFIER (16)
typedef struct pkoc_util_context
{
  int verbosity;
  int command;
  FILE *log;
  char certificate_filename [EAC_STRING_MAX];
  char public_key_filename [EAC_STRING_MAX];
  int certificate_index;
  void *sc_ctx;
  int smartcard_subsystem;
  int reader_index;
  int transaction_length;
  unsigned char site_key_identifier [PKOC_SITE_KEY_IDENTIFIER];
  unsigned char reader_location_identifier [PKOC_READER_LOCATION_IDENTIFIER];
} PKOC_UTIL_CONTEXT;

#define STPKOC_UNKNOWN_COMMAND ( 1)

int pkoc_load_certificate(PKOC_UTIL_CONTEXT *ctx, int argc, char *argv []);
int pkoc_request_certificate(PKOC_UTIL_CONTEXT *ctx, int cert_index);
int pkoc_util_read_settings(PKOC_UTIL_CONTEXT *ctx);
int setup_pkoc_authenticate(PKOC_UTIL_CONTEXT *ctx, unsigned char *buffer, int *buffer_length,
  unsigned char *transaction_id, int xtn_id_lth, unsigned char *protocol_version, int ver_lth,
  unsigned char *reader_identifier, int cert_index);

