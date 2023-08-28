---
title: OSDP ACU PKOC Card Processing
author: Rodney Thayer
date: August 25, 2023
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

Message Flow
============

- ACU initializes secure channel connection with PD.  Initialization
includes osdp_ACURXSIZE of at least 1024 bytes.
- ACU and PD exchange conventional osdp_POLL/osdp_ACK steady-state
traffic.
- cardholder presents card
- PD identifiers card is a PKOC card and initiates PKOC card dialog
including an Authentication Request as defined in [1]
- card provides Authentication Response [1]
- PD sends osdp_MFGREP("osdp_AUTH_RESPONSE") in response to osdp_POLL
- ACU processes the AUthentication Response, including necessary EC crypto operations.
- ACU extracts cardholder number from osdp_AUTH_RESPONSE and proceeds with access control
processing.

osdp_PKOC_AUTH_RESPONSE
=======================

This consists of an osdp_MFGREP response.  It is sent in response to
an osdp_POLL command.

osdp_MFGREP payload
-------------------

| Offset | Contents |
| ------ | -------- |
|        |          |
|   0    | Manufacturer OUI (3 octets) |
|        |                             |
|   3    | 0x01 (Mfg Response Code) |
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
|  10    | blah                                                  |
|        | Uncompressed Public Key TLV                           |
|        | Digital Signature TLV                                 |

References
==========

[1] PKOC 1.0

[2] Integrated Engineering OSDP extensions, document 100-01G-PS-01-INID "Vendor Specific OSDP Extensions v10b"

