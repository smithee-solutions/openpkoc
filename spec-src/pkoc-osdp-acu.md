---
title: OSDP ACU PKOC Card Processing Version 1.11
author: Rodney Thayer rodney@smithee.solutions
date: August 31, 2023
include-before:
- '`\newpage{}`{=latex}'
---

\newpage{}

Introduction
============

This document describes alternatives to process PKOC cards for access control.
It is assumed that these would be considered as an alternative to a reader
that handles the complete PKOC operation and just returns a cardholder number 
to the ACU.
This document describes processing PKOC cards with the work "on-loaded" 
(not off-loaded) to the ACU. 

These all assume the standard OSDP manufacturer specific command
format.  This specification further adopts the multi-part message format 
proposed by Integrated Engineering for PIV.

Use of a PKOC card will require multiple card operations and so the
osdp_KEEPACTIVE command is recommended to ensure the reader maintains
the link to the card during the entire card processing operation.
Since these messages are likely larger than the minimum size OSDP message
it is recommended the ACU send an osdp_ACURXSIZE command with a size of at least 1024
bytes.

\newpage{}

Message Flow
============

Reader-Generated Challenge
--------------------------

In this case the reader creates the PKOC Authentication Request,
the ACU is responsible only for the crypto.  The ACU receives the request and
response TLV values and validates the values.

- ACU initializes secure channel connection with PD.  Initialization
includes osdp_ACURXSIZE of at least 1024 bytes and osdp_KEEPACTIVE.
- ACU and PD exchange conventional osdp_POLL/osdp_ACK steady-state
traffic.
- cardholder presents card
- PD identifiers card is a PKOC card and initiates PKOC card dialog
including an Authentication Request as defined in [1]
- card provides Authentication Response [1]
- PD sends osdp_MFGREP("osdp_FULL_AUTH_RESPONSE") in response to osdp_POLL
- ACU processes the Authentication Response, including necessary EC crypto operations.
- ACU extracts cardholder number from osdp_AUTH_RESPONSE and proceeds with access control
processing.

ACU-Generated Challenge
-----------------------

In this case the reader notifies the ACU that the card is present and the
ACU creates the PKOC Authentication Request,

- ACU initializes secure channel connection with PD.  Initialization
includes osdp_ACURXSIZE of at least 1024 bytes and osdp_KEEPACTIVE.
- ACU and PD exchange conventional osdp_POLL/osdp_ACK steady-state
traffic.
- cardholder presents card
- PD identifiers card is a PKOC card and initiates PKOC card dialog.
- PD sends osdp_PKOC_CARD_PRESENT
- ACU creates transaction id
- ACU sends osdp_PKOC_AUTH_REQUEST
- card provides Authentication Response [1]
- PD sends osdp_MFGREP("osdp_FULL_AUTH_RESPONSE") in response to osdp_POLL
- ACU processes the Authentication Response, including necessary EC crypto operations.
- ACU extracts cardholder number from osdp_AUTH_RESPONSE and proceeds with access control
processing.

Message Flow
------------

Note this uses osdp_RAW as the card-present indication.

```text

EAC ACU     PD    CARD
--- ---    ---    ----
 |   |      |      |
 |   |      |      |
             <-----
             card is presented
 |   |      |      |
 |   |      |      |
      <-----
      osdp_RAW, new format, pkoc card version string
 |   |      |      |
 |   |      |      |
      ----->
Auth Request
 |   |      |      |
 |   |      |      |
             ----->
       Auth Request
 |   |      |      |
 |   |      |      |
      ...
      Poll/Ack Traffic
      ...
 |   |      |      |
 |   |      |      |
             <-----
             Auth Response
 |   |      |      |
 |   |      |      |
      <-----
      Auth Response
 |   |      |      |
 |   |      |      |
  <-----
  Card data
```

Providing the Transaction ID to the PD
--------------------------------------

The PD needs the transaction id to perform the Authentication Request.  This
value can be provided by the PD, or provided by the ACU in response to an osdp_PKOC_CARD_PRESENT response, or provided in advance of a card reader
using the osdp_PKOC_NEXT_TRANSACTION command.  The PD may request a pre-defined
transaction id by using the osdp_PKOC_TRANSACTION_REFRESH response.

\newpage{}

PKOC Manufacturer-specific OSDP commands
========================================

To implement these operations OSDP's manufacturer-specific command
mechanism is used.  Three commands are defined, within the OUI _tbd_.

Commands for use within MFG and MFGREP
--------------------------------------

| Name                   | Value |
| ----                   | ----- |
|                        |       |
| osdp_PKOC_CARD_PRESENT | 0xE0 |
|                        |      |
| osdp_PKOC_AUTH_REQUEST | 0xE1 |
|                        |      |
| osdp_PKOC_AUTH_RESPONSE | 0xE2 |
|                        |      |
| osdp_PKOC_NEXT_TRANSACTION | 0xE3 |
|                        |      |
| osdp_PKOC_TRANSACTION_REFRESH | 0xE4 |
|                               |      |
| osdp_PKOC_READER_ERROR        | 0xE5 |
|                               |      |

\newpage{}

osdp_PKOC_CARD_PRESENT
======================

This RESPONSE is sent by the reader to the ACU so that the ACU may specify
the Authentication Request transaction ID.  It returns the "supported protocol versions" TLV structure as received by the reader in the select response.

Note is expected the osdp_PKOC_AUTH_REQUEST command will be sent soon after the osdp_PKOC_CARD_PRESENT response is received by the ACU, but not necessarily as the next command.
In general the next command after osdp_PKOC_CARD_PRESENt is expected to be a poll.

MFGREP payload
--------------

Contents of the osdp_MFGREP payload:

| Offset | Contents |
| ------ | -------- |
|        |          |
|   0    | Manufacturer OUI (3 octets) |
|        |                             |
|   3    | 0xE0 (Mfg Response Code) |
|        |                                                       |
|   4    | Total response payload size (Least Significant Octet) |
|        |                                                       |
|   5    | Total response payload size (Most Significant Octet)  |
|        |                                                       |
|   6    | Offset in response (Least Significant Octet)          |
|        |                                                       |
|   7    | Offset in response (Most Significant Octet)           |
|        |                                                       |
|   8    | Response length (Least Significant Octet)             |
|        |                                                       |
|   9    | Response length (Most Significant Octet)              |
|        |                                                       |
|  10    | Supported Protocol Versions TLV                       |

\newpage{}

osdp_PKOC_AUTH_REQUEST
======================

This REQUEST is sent by the ACU so that the PD may issue an Authentication Request
to the card.  This contains the protocol version, transaction ID, and
"reader" identifier.  If the Transaction ID has been previously provided the
"Transaction ID TLV" field will contain a zero (tag 0x4c, length 1, value 0.)

MFG payload
-----------

Contents of the osdp_MFG payload:

| Offset | Contents |
| ------ | -------- |
|        |          |
|   0    | Manufacturer OUI (3 octets)                          |
|        |                                                      |
|   3    | 0xE1 (Mfg Request Code)                              |
|        |                                                      |
|   4    | Total request payload size (Least Significant Octet) |
|        |                                                       |
|   5    | Total request payload size (Most Significant Octet)  |
|        |                                                       |
|   6    | Offset in request (Least Significant Octet)          |
|        |                                                       |
|   7    | Offset in request (Most Significant Octet)           |
|        |                                                       |
|   8    | Request length (Least Significant Octet)             |
|        |                                                       |
|   9    | Request length (Most Significant Octet)              |
|        |                                                      |
|  10    | Auth Command P1 |
|        |                                                      |
|  11    | Auth Command P2 |
|        |                                                      |
|  12    | Protocol Version TLV |
|        | Transaction ID TLV (length 1 value 0 if predefined)  |
|        | Reader Identifer                                     |

\newpage{}

osdp_PKOC_AUTH_RESPONSE
=======================

This RESPONSE consists of an osdp_MFGREP response.  It is sent in response to
an osdp_POLL command.
This response is sent after the Authentication Response is received from the card
by the reader.

MFGREP payload
--------------

Contents of the osdp_MFGREP payload:

| Offset | Contents |
| ------ | -------- |
|        |          |
|   0    | Manufacturer OUI (3 octets) |
|        |                             |
|   3    | 0xE2 (Mfg Response Code) |
|        |                                                       |
|   4    | Total response payload size (Least Significant Octet) |
|        |                                                       |
|   5    | Total response payload size (Most Significant Octet)  |
|        |                                                       |
|   6    | Offset in response (Least Significant Octet)          |
|        |                                                       |
|   7    | Offset in response (Most Significant Octet)           |
|        |                                                       |
|   8    | Response length (Least Significant Octet)             |
|        |                                                       |
|   9    | Response length (Most Significant Octet)              |
|        |                                                       |
|  10    | Uncompressed Public Key TLV                           |
|        | Digital Signature TLV                                 |

\newpage{}

osdp_PKOC_TRANSACTION_REFRESH
=============================

This RESPONSE consists of an osdp_MFGREP response.  It is sent in response to
an osdp_POLL command.  
This response is sent when the PD wants to pre-load a transaction id for the next
card read.  It is expected this would be generated within a few poll cycles after an osdp_PKOC_AUTH_RESPONSE.
Note there is no payload beyond the OUI and the response code.

MFGREP payload
--------------

Contents of the osdp_MFGREP payload:

| Offset | Contents |
| ------ | -------- |
|        |          |
|   0    | Manufacturer OUI (3 octets) |
|        |                             |
|   3    | 0xE4 (Mfg Response Code) |
|        |                                                       |

\newpage{}

osdp_PKOC_NEXT_TRANSACTION
==========================

This COMMAND consists of an osdp_MFG command.  It is sent to provide the PD
with a transaction ID to be used in the next Authentication Reuqest.
This command may be sent at any time.

MFG payload
-----------

Contents of the osdp_MFG payload:

| Offset | Contents |
| ------ | -------- |
|        |          |
|   0    | Manufacturer OUI (3 octets) |
|        |                             |
|   3    | 0xE3 (Mfg Command Code) |
|        |                                                       |
|   4    | Total command payload size (Least Significant Octet) |
|        |                                                       |
|   5    | Total command payload size (Most Significant Octet)  |
|        |                                                       |
|   6    | Offset in command (Least Significant Octet)          |
|        |                                                       |
|   7    | Offset in command (Most Significant Octet)           |
|        |                                                       |
|   8    | Command length (Least Significant Octet)             |
|        |                                                       |
|   9    | Command length (Most Significant Octet)              |
|        |                                                       |
|  10    | Transaction ID TLV                           |

\newpage{}

osdp_PKOC_READER_ERROR
======================

This RESPONSE consists of an osdp_MFGREP command and associated payload.  It is sent in response to a poll when
there is an error reading the card.

Error values:

| Error Code (msb,lsb) | Meaning |
| -------------------- | ------- |
|            |         |
| 0x0000     | No error |
|            |         |
| 0x0001     | S1/S2 error from card
|            |         |
| 0x0002     | problem accessing card
|            |         |
| 0x0003-0x07FF | Reserved for future use. |
|            |         |
| 0x8000-0xFFFF | Reserved for private use. |
|            |         |


MFGREP payload
--------------

Contents of the osdp_MFGREP payload:

| Offset | Contents |
| ------ | -------- |
|        |          |
|   0    | Manufacturer OUI (3 octets) |
|        |                             |
|   3    | 0xE5 (Mfg Response Code) |
|        |                                                       |
|   4    | Total response payload size (Least Significant Octet) |
|        |                                                       |
|   5    | Total response payload size (Most Significant Octet)  |
|        |                                                       |
|   6    | Offset in response (Least Significant Octet)          |
|        |                                                       |
|   7    | Offset in response (Most Significant Octet)           |
|        |                                                       |
|   8    | Response length (Least Significant Octet)             |
|        |                                                       |
|   9    | Response length (Most Significant Octet)              |
|        |                                                       |
|  10    | Reader Error Code (Least Significant Octet)           |
|        |                                                       |
|  11    | Reader Error Code (Most Significant Octet)            |
|        |                                                       |
|  12    | Reader Error Code (Most Significant Octet)            |
|        |                                                       |
|  13    | ISO 7818 S1 response value                            |
|        |                                                       |
|  14    | ISO 7818 S2 response value                            |


\newpage{}

Appendix
========

Colophon
--------

This document was written in 'markdown', using pandoc.  PDF converter assistance provided by latex.  Linux command line to create the PDF is

  pandoc --toc -o pkoc-osdp-acu.pdf pkoc-osdp-acu.md

Document source is in github.


Security Considerations
-----------------------

It is assumed all of this message traffic happens inside a proper OSDP
secure channel using a unique paired (not the default) key.

OSDP Considerations
-------------------

If you perform PKOC operations on an OSDP "chain" (RS-485 bus with more than one PD)
it is possible that long delays will be introduced between the time the PD
sends an osdp_PKOC_CARD_PRESENT response and when the ACU sends an
osdp_PKOC_AUTH_REQUEST.  It is recommended you use a chain with a minimal number
of PD's (preferrably only one) and/or a higher line speed (38,400) and/or use
a PD that connects over TCP/IP.


References
----------

[1] PKOC 1.0

[2] Integrated Engineering OSDP extensions, document 100-01G-PS-01-INID "Vendor Specific OSDP Extensions v10c".

[3] OSDP, IEC 60839-11-5

[4] ISO 7816-4-2020

