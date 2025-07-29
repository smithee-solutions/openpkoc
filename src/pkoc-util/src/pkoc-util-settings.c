#include <stdio.h>
#include <string.h>


#include <jansson.h>


#include <PCSC/wintypes.h>
#include <PCSC/pcsclite.h>
#include <PCSC/winscard.h>


#include <eac-encode.h>
#include <openbadger-common.h>
#include <ob-stub.h>
#include <pkoc-util.h>

/*
  pkoc_util_read_settings - reads settings values from json file (local or system default)

  settings file name is pkoc-util-settings.json

  parameters:
    reader - smartcard reader index.  starts at zero.  default is zero.
    verbosity - message detail level.  default is 3, debug is 9, silent is 0
*/
int pkoc_util_read_settings
  (PKOC_UTIL_CONTEXT *ctx)

{ /* pkoc_util_read_settings */

  extern char *certificate_filename;
  int i;
  int returned_length;
  json_t *root;
  char settings_filename [1024];
  int settings_found;
  int status;
  json_error_t status_json;
  json_t *value;


  status = ST_OK;
  settings_found = 0;

  // try to use local settings file

  sprintf(settings_filename, "./%s", PKOC_UTIL_SETTINGS_FILE);
  root = json_load_file(PKOC_UTIL_SETTINGS_FILE, 0, &status_json);
  if (root)
  {
    settings_found = 1;
  };
  if (!settings_found)
  {
    sprintf(settings_filename, "/opt/tester/etc/%s", PKOC_UTIL_SETTINGS_FILE);
    root = json_load_file(settings_filename, 0, &status_json);
    if (root)
    {
      settings_found = 1;
    };
  };
  if (settings_found)
  {
    value = json_object_get (root, "verbosity");
    if (json_is_string (value))
    {
      sscanf(json_string_value(value), "%d", &i);
      ctx->verbosity = i;
    };
    if (ctx->verbosity > 3)
      fprintf(ctx->log, "reading settings from %s\n", settings_filename);

    value = json_object_get (root, "certificate-file");
    if (json_is_string (value))
    {
      strcpy(ctx->certificate_filename, json_string_value(value));
    };

    value = json_object_get (root, "reader");
    if (json_is_string (value))
    {
      sscanf(json_string_value(value), "%d", &i);
      ctx->reader_index = i;
    };

    value = json_object_get(root, "reader-location-identifier");
    if (json_is_string(value))
    {
      returned_length = sizeof(ctx->reader_location_identifier);
      status = eac_encode_hex_string_to_bytes((char *)json_string_value(value), ctx->reader_location_identifier, &returned_length);
    };

    value = json_object_get(root, "site-key-identifier");
    if (json_is_string(value))
    {
      returned_length = sizeof(ctx->site_key_identifier);
      status = eac_encode_hex_string_to_bytes((char *)json_string_value(value), ctx->site_key_identifier, &returned_length);
    };

    value = json_object_get (root, "subsystem");
    if (json_is_string (value))
    {
      sscanf(json_string_value(value), "%d", &i);
      ctx->smartcard_subsystem = i;
    };

    value = json_object_get (root, "transaction-identifier-length");
    if (json_is_string (value))
    {
      sscanf(json_string_value(value), "%d", &i);
      ctx->transaction_length = i;
    };
  };

  return(status);

} /* pkoc_util_read_settings */
