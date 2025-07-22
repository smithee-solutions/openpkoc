// eac-smartcard

#define EQUALS ==

#define LOG stderr

#define SC_APDU_SIZE (256)
#define SC_HIST_MAX            (8)


typedef struct apdu_record
{
  unsigned char basic_apdu [256];
} APDU_RECORD;


#define SC_SUBSYSTEM_PCSC      (0)
#define SC_SUBSYSTEM_SIMULATOR (1)
#define SC_SUBSYSTEM_LEGIC     (2)
#define SC_SUBSYSTEM_MQTT      (3)


typedef struct eac_smartcard_context
{
  int verbosity;
  int subsystem;
  unsigned short int message_sequence;
  void *subsystem_context;
  APDU_RECORD apdu_history [SC_HIST_MAX];
} EAC_SMARTCARD_CONTEXT;

typedef struct sc_pcsc_context
{
  int tbd;
} SC_PCSC_CONTEXT;

#define ACTION_WAIT (0)
#define ACIION_NO_WAIT (1)
typedef struct sc_simulator_context
{
  int stub;
} SC_SIMULATOR_CONTEXT;


#define STSC_NOT_IMPLEMENTED (1)


int simulate_smartcard (EAC_SMARTCARD_CONTEXT *ctx, unsigned char cla, unsigned char ins,
  unsigned char p1, unsigned char p2, unsigned int Lc, unsigned char Le,
  unsigned char *payload, unsigned char *ret_apdu, int *ret_lth, int action);
int sc_init(EAC_SMARTCARD_CONTEXT *ctx);
int sc_sendrcv(EAC_SMARTCARD_CONTEXT *ctx, unsigned char cla, unsigned char ins,
  unsigned char p1, unsigned char p2, unsigned int Lc, unsigned char Le,
  unsigned char *payload, unsigned char *ret_apdu, int *ret_lth, int action);

