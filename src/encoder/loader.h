#define PKOC_CERT_LOADER_VERSION "0.00"

#define EQUALS ==

#define OB_STRING_MAX (1024)

// 0 is stdout, 2 is ctx->log
#define LOG_STDERR (1)

#define ST_OK                (0)
#define STOB_PCSC_TRANSMIT_1 (1)
#define STOB_SCARD_ESTABLISH (2)
#define STOB_SCARD_ERROR     (3)
#define STOB_SCARD_CONNECT   (4)
#define STOB_FILE_ERROR      (5)


typedef struct openbadger_context
{
  int verbosity;
  FILE *log;
  char reader_name [OB_STRING_MAX];
  int reader_index;
  SCARDCONTEXT hContext;
  DWORD last_pcsc_status;
  SCARDHANDLE pcsc;
  SCARD_IO_REQUEST pioSendPci;
  char cert_filename [OB_STRING_MAX];
} OB_CONTEXT;


void ob_dump_buffer(OB_CONTEXT *ctx, int output_destination, char *label, unsigned char *buffer, int buffer_length);
int ob_init_smartcard(OB_CONTEXT *ctx);

