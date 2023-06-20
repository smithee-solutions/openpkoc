/*
  openpkoc-pkoc-test-vectors.h - test values from spec

  (C)Copyright 2023 Smithee Solutions LLC

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

#if 0
308187020100301306072a8648ce3d020106082a8648ce3d030107046d306b0201010420c0c93d0ee2c83d077a91448478f438d633f0c9f863799f9574151fa1260d1349a144034200040ec5d87dc39d14a2c5480686da860c82b16be0b6903b525f84848b79fd463e32bbda1f0252c33503c5287035e6eac55d138d0650dcfb5281d59a9cf4124d2831

// public key
3059301306072a8648ce3d020106082a8648ce3d030107034200040ec5d87dc39d14a2c5480686da860c82b16be0b6903b525f84848b79fd463e32bbda1f0252c33503c5287035e6eac55d138d0650dcfb5281d59a9cf4124d2831

5A41040ec5d87dc39d14a2c5480686da860c82b16be0b6903b525f84848b79fd463e32bbd
a1f0252c33503c5287035e6eac55d138d0650dcfb5281d59a9cf4124d28319E40b98613070c78010b04ed306d143f94ee6dc4eca2585b621405731fb3a53cd877a21685de18435da7cbcc38f1d926300a454efee3594cec5effe28c7feac03d7d9000
#endif

unsigned char TEST_AUTHENTICATION_COMMAND [] =
{
 0x80,0x80,0x00,0x01, 0x38,0x5c,0x02,0x01, 0x00,0x4c,0x10,0x6f, 0xcf,0x50,0x12,0xb2,
 0x24,0x04,0x3b,0x09, 0x35,0x0a,0x4f,0xc5, 0xe5,0x6a,0x8f,0x4d, 0x20,0x7a,0x25,0x43,
 0x2a,0x46,0x2d,0x4a, 0x40,0x4e,0x63,0x52, 0x66,0x55,0x6a,0x58, 0x6e,0xdf,0xee,0x80,
 0x22,0x96,0x63,0x11, 0xed,0xa1,0xeb,0x02, 0x42,0xac,0x12,0x00, 0x02,0x00
};

