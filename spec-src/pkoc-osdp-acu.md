---
title: OSDP ACU PKOC Card Processing
author: Rodney Thayer
date: August 29, 2023
include-before:
- '`\newpage{}`{=latex}'
---

\newpage{}

Introduction
============

This document describes processing PKOC cards with the work "on-loaded" 
(not off-loaded) to the ACU.

These all assume the standard OSDP manufacturer specific command
format.  This specification further adopts the multi-part message format 
proposed by Integrated Engineering for PIV.

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
|   3    | 0xE1 (Mfg Response Code) |
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
"reader" identifier.

MFG payload
--------------

Contents of the osdp_MFG payload:

| Offset | Contents |
| ------ | -------- |
|        |          |
|   0    | Manufacturer OUI (3 octets) |
|        |                             |
|   3    | 0xE1 (Mfg Request Code) |
|        |                                                       |
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
|        |                                                       |
|  10    | Protocol Version                           |
|        | Transaction ID TLV                       |
|        | Reader Identifer                       |

\newpage{}

osdp_PKOC_FULL_AUTH_RESPONSE
============================

This RESPONSE consists of an osdp_MFGREP response.  It is sent in response to
an osdp_POLL command.  osdp_PKOC_AUTH_RESPONSE is manufacturer code 0xEC.
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

References
==========

[1] PKOC 1.0

[2] Integrated Engineering OSDP extensions, document 100-01G-PS-01-INID "Vendor Specific OSDP Extensions v10b"

[3] OSDP, IEC 60839-11-5

