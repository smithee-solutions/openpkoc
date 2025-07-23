#define EAC_STRING_MAX (1024)

// 0 is stdout, 2 is ctx->log
#define LOG_STDERR (1)

//#define ST_OK                (0)
#define STEAC_PCSC_TRANSMIT_1 (1)
#define STEAC_SCARD_ESTABLISH (2)
#define STEAC_SCARD_ERROR     (3)
#define STEAC_SCARD_CONNECT   (4)
#define STEAC_FILE_ERROR      (5)


// this goes in eac smartcard context after the next merge iteration

typedef struct eac_smartcard_context_ex
{
  int verbosity;
  FILE *log;
  int reader_index;
  char reader_name [EAC_STRING_MAX];
  SCARDCONTEXT hContext;
  DWORD last_pcsc_status;
  SCARDHANDLE pcsc;
  SCARD_IO_REQUEST pioSendPci;
} EAC_SMARTCARD_CONTEXT_EX;


//void ob_dump_buffer(OB_CONTEXT *ctx, int output_destination, char *label, unsigned char *buffer, int buffer_length);
int ob_init_smartcard_ex(EAC_SMARTCARD_CONTEXT_EX *ctx);
//int pkoc_load_certificate(int argc, char *argv []);

