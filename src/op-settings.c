/*
  op-settings - read settings from json file

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
#include <string.h>


#include <jansson.h>


#include <ob-crypto.h>
#include <openbadger-common.h>

#include <openpkoc.h>

/*
  ob_read_settings - reads settings values from json file (local or system default)

  settings file name is openbadger-settings.json

  parameters:
    bits - number of bits to output. Must be 512 or 128 
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

  sprintf(settings_filename, "./%s", OPENPKOC_SETTINGS_FILE);
  root = json_load_file(OPENPKOC_SETTINGS_FILE, 0, &status_json);
  if (root)
  {
    settings_found = 1;
  };
  if (!settings_found)
  {
    sprintf(settings_filename, "/opt/tester/etc/%s", OPENPKOC_SETTINGS_FILE);
    root = json_load_file(settings_filename, 0, &status_json);
    if (root)
    {
      settings_found = 1;
    };
  };
  if (settings_found)
  {
    value = json_object_get (root, "bits");
    if (json_is_string (value))
    {
      sscanf(json_string_value(value), "%d", &i);
      ctx->bits_to_return = i;
    };
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
      fprintf(LOG, "reading settings from %s\n", settings_filename);
  };

  return(status);

} /* ob_read_settings */
