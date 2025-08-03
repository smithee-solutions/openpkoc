/*
  pkoc-parameters.h - PKOC parameter definitions

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


#define OB_PKOC_PUBKEY_LENGTH                (65)
#define OB_PKOC_SIG_LENGTH                   (64)
#define OB_PKOC_TRANSACTID_LENGTH            (16)
#define OB_PKOC_SITE_KEY_IDENTIFIER_LENGTH   (16)
#define OB_PKOC_READER_KEY_IDENTIFIER_LENGTH (16)

// tag, 1 byte length, 1 byte value, 2 byte arbval

#define PKOC_TAG_TRANSACTION_IDENTIFIER (0x4C)
#define PKOC_TAG_READER_IDENTIFIER      (0x4D)
#define PKOC_TAG_UNCOMP_PUBLIC_KEY      (0x5A)
#define PKOC_TAG_PROTOCOL_VERSION       (0x5C)
#define PKOC_TAG_DIGITAL_SIGNATURE      (0x9E)
#define PKOC_TAG_ATTESTATION_CERT       (0xF8)

// PKOC OSDP spec parameters

#define PKOC_TAG_ERROR_7816    (0x7F)
#define PKOC_TAG_ERROR_GENERAL (0x7E)


