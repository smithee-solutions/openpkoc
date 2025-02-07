#include <stdio.h>
#include <stdlib.h>

#include <openbadger-common.h>
#include <openpkoc.h>


void
  op_pkoc_print
    (PKOC_CONTEXT *ctx,
    unsigned char *public_key,
    int public_key_length,
    FILE *outfile)

{

  int print_length;
  int print_offset;


  fprintf(outfile, "=== PKOC Public Key Credential ===\n");
  print_offset = 1;
  print_length = public_key_length - 1; //we know it starts with an 04, ditch that.
  fprintf(outfile, "X Component:\n");
  ob_dump_buffer(ctx->ob_ctx, public_key+print_offset, print_length, 1);
  print_offset = 1+(128/8);
  print_length = 128/8;
  fprintf(outfile, "Low 128:\n");
  ob_dump_buffer(ctx->ob_ctx, public_key+print_offset, print_length, 1);
  print_offset = 1+(128/8)+(64/8);
  print_length = 64/8;
  fprintf(outfile, "Low 64:\n");
  ob_dump_buffer(ctx->ob_ctx, public_key+print_offset, print_length, 1);
}

