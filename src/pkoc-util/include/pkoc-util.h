// pkoc-util.h

typedef struct pkoc_util_context
{
  int verbosity;
  int command;
  FILE *log;
  int certificate_index;
  void *sc_ctx;
} PKOC_UTIL_CONTEXT;

#define PKOC_TAG_TRANSACTION_IDENTIFIER (0x4C)
#define PKOC_TAG_READER_IDENTIFIER      (0x4D)
#define PKOC_TAG_UNCOMP_PUBLIC_KEY      (0x5A)
#define PKOC_TAG_PROTOCOL_VERSION       (0x5C)
#define PKOC_TAG_DIGITAL_SIGNATURE      (0x9E)

#define PKOC_TRANSACTION_IDENTIFIER_MIN (16)
#define PKOC_TRANSACTION_IDENTIFIER_MAX (65)
#define PKOC_READER_IDENTIFIER_LENGTH   (32)

#define STPKOC_UNKNOWN_COMMAND ( 1)

int pkoc_request_certificate
  (PKOC_UTIL_CONTEXT *ctx,
  int cert_index);
int setup_pkoc_authenticate(PKOC_UTIL_CONTEXT *ctx,
  unsigned char *buffer, int *buffer_length,
  unsigned char *transaction_id, int xtn_id_lth,
  unsigned char *protocol_version, int ver_lth,
  unsigned char *reader_identifier,
  int cert_index);

