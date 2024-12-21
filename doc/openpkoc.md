---
title: openpkoc
subtitle: A PKOC (NFC) Card Reader add-on for libosdp-conformance
---

operating manual for obtest-pkoc

Usage:

1. configure openbadger-settings.json with appropriate parameters.
2. run obtest-pkoc (no arguments)

Settings values

bits - number of bits to output.
control - path to libosdp-conformance control socket (to send card reads)
reader - smartcard reader index.  Starts at zero.  default is zero.
verbosity - logging level.  3=normal 0=silent 9=debug

use "opensc-tool --list-readers" to separately confirm your smartcard
reader is visible.

Typical oenbadger-settings.json
{
  "bits"      " "128",
  "reader"    : "1",
  "verbosity" :"3"
}


# Building openpkoc #

- build openbadger-dev

```
  make
```

# Punchlist #

- package

