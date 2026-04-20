/*
  ob-pkoc - PKOC-specific routines

  (C)Copyright 2023-2025 Smithee Solutions LLC

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


#include <PCSC/wintypes.h>
#include <PCSC/pcsclite.h>
#include <PCSC/winscard.h>


//#include <eac-encode.h>
#include <ob-crypto.h>

int test (void)
{
  int status;
  status = ob_crypto_digest_init(&crypto_context, &digest_object);
  return(status);
}

#include <openbadger-common.h>
#include <ob-7816.h>
#include <openpkoc.h>


