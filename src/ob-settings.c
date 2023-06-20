#define OB_SETTINGS_FILE "openbadger-settings.json"
#if 0
json reader ./openbadget-settings.json
if fail open /opt/tester/etc/openbadger=settings
else use defaults

  reader number
  OSDP PD control port path

    root = json_load_file (json_string, 0, &status_json);
    if (!root)

    value = json_object_get (root, field);
    if (!json_is_string (value))
      found_field = 0;
#endif


#include <stdio.h>
#include <string.h>


#include <jansson.h>


#include <ob-crypto.h>
#include <openbadger-common.h>


/*
  ob_read_settings - reads settings values from json file (local or system default)

  settings file name is openbadger-settings.json

  parameters:
    control - path of libosdp-conformance PD control socket.  default is /opt/osdp-conformance/PD/open-osdp-control
    reader - smartcard reader index.  starts at zero.  default is zero.
    verbosity - message detail level.  default is 3, debug is 9, silent is 0
*/
int ob_read_settings
  (OB_CONTEXT *ctx)

{ /* ob_read_settings */

  int i;
  json_t *root;
  char settings_filename [1024];
  int settings_found;
  int status;
  json_error_t status_json;
  json_t *value;


  status = ST_OK;
  settings_found = 0;

  // try to use local settings file

  sprintf(settings_filename, "./%s", OB_SETTINGS_FILE);
  root = json_load_file(OB_SETTINGS_FILE, 0, &status_json);
  if (root)
  {
    settings_found = 1;
  };
  if (!settings_found)
  {
    sprintf(settings_filename, "/opt/tester/etc/%s", OB_SETTINGS_FILE);
    root = json_load_file(settings_filename, 0, &status_json);
    if (root)
    {
      settings_found = 1;
    };
  };
  if (settings_found)
  {
    value = json_object_get (root, "control");
    if (json_is_string (value))
    {
      strcpy(ctx->pd_control, json_string_value(value));
    };
    value = json_object_get (root, "reader");
    if (json_is_string (value))
    {
      sscanf(json_string_value(value), "%d", &i);
      ctx->reader_index = i;
    };
    value = json_object_get (root, "verbosity");
    if (json_is_string (value))
    {
      sscanf(json_string_value(value), "%d", &i);
      ctx->verbosity = i;
    };
    if (ctx->verbosity > 3)
      fprintf(stderr, "reading settings from %s\n", settings_filename);
  };

  return(status);

} /* ob_read_settings */
