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

this document describes processing PKOC cards with the work "on-loaded" 
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

consists of:
osdp_MFGREP response to an osdp_POLL

Response is an osdp_MFGREP, the payload is:

- multi-part message header
- Uncompressed Public Key TLV exactly as described in [1]
- Digital Signature TLV exatly as described in [2]

References
==========

[1] PKOC 1.0

