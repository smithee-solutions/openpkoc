---
title: PKOC Certificate Message Examples
---

# Authentication Command #

The authentication command would add a tag to request a certificate.  In this example it requests 
the certificate at index 0.
(You could ask for several certificates, the exchange would take a while,
probably do this one at a time at cardholder reg time.)

The F9 01 00 at the end is the Request Certificate TLV.

```
PKOC Authentication Command:
 5C020100 4C101717 17171717 17171717 17171717 17174D20 CAFEDEED 03030303
 03030303 03030303 03030303 03030303 03030303 03030303 F90100
```

# Authentication Response #

The authentication response will contain the requested certificate if it exists.
The F8... TLV in the example contains CCEE3377 as a placeholder for
the DER encoding of the certificate

```
PKOC Authentication Response:
 5A041122 33449E04 AABBCCDD F804CCEE 33779000 
```

# Certificate Provisioning #

(after authentication) the NFC credential will accept a load-certificate command.

This is assumed to be an extended APDU (for now.)  The example assumes P1 is the certificate index.

The proposed APDU is:

- CLA 0x00
- INS E2
- P1 0 (the cert index?)
- P2 0
- Lc 0x000218 (extended form, the cert der payload itself is 0x0215 octets, includes overhead)
- Data: Tag 0xF8 Length 0x0215 (encoded in DER) followed by the DER of the certificate.
- Le 0


```
PKOC put-certificate:
 00 E2 00 00 00 02 18 F8 82 02 15 30 82 02 11 30
 82 01 B8 A0 03 02 01 02 02 08 12 5F 17 EE A9 5A
 D2 AE 30 0A 06 08 2A 86 48 CE 3D 04 03 02 30 67
 31 0B 30 09 06 03 55 04 06 13 02 55 53 31 13 30
 11 06 03 55 04 08 13 0A 43 61 6C 69 66 6F 72 6E
 69 61 31 14 30 12 06 03 55 04 07 13 0B 53 61 6E
 74 61 20 43 6C 61 72 61 31 0D 30 0B 06 03 55 04
 0A 13 04 50 53 49 41 31 10 30 0E 06 03 55 04 0B
 13 07 50 4B 4F 43 20 57 47 31 0C 30 0A 06 03 55
 04 03 13 03 6B 6D 73 30 1E 17 0D 32 35 30 36 32
 35 30 30 35 36 30 30 5A 17 0D 32 36 30 36 32 35
 30 30 35 36 30 30 5A 30 6E 31 0B 30 09 06 03 55
 04 06 13 02 55 53 31 13 30 11 06 03 55 04 08 13
 0A 43 61 6C 69 66 6F 72 6E 69 61 31 14 30 12 06
 03 55 04 07 13 0B 53 61 6E 74 61 20 43 6C 61 72
 61 31 0D 30 0B 06 03 55 04 0A 13 04 50 53 49 41
 31 10 30 0E 06 03 55 04 0B 13 07 50 4B 4F 43 20
 57 47 31 13 30 11 06 03 55 04 03 13 0A 64 65 62
 75 67 2D 63 61 72 64 30 59 30 13 06 07 2A 86 48
 CE 3D 02 01 06 08 2A 86 48 CE 3D 03 01 07 03 42
 00 04 59 6E 19 70 80 FD BC 79 EF 21 AE 8B C5 E0
 BF 91 BF 20 55 FE CB FC 69 A0 0A D8 EB 74 6E BB
 46 AA 9C D7 18 01 82 7E 7A 99 8D CA 21 1A B5 F0
 81 B0 B6 91 63 80 D9 0F 59 08 60 AF 2B 23 52 62
 FA CA A3 47 30 45 30 1E 06 09 60 86 48 01 86 F8
 42 01 0D 04 11 16 0F 78 63 61 20 63 65 72 74 69
 66 69 63 61 74 65 30 23 06 03 55 1D 11 04 1C 30
 1A A0 18 06 0A 2B 06 01 04 01 83 D2 25 08 11 A0
 0A 04 08 43 41 46 45 44 45 45 44 30 0A 06 08 2A
 86 48 CE 3D 04 03 02 03 47 00 30 44 02 20 5B D2
 B2 29 3B 61 0F 45 4C 8B E3 9B A0 FE 71 B5 5C DE
 5E 39 28 00 43 67 A3 F5 8D C4 97 BA B0 89 02 20
 73 14 F9 41 ED 49 28 35 20 FA 22 4B A6 E5 91 02
 FA 94 5E A4 23 D6 51 85 DF 0B 43 72 D3 A3 F8 78
```

# Appendix #

# A. Sample Attestation Certificate #

```
Certificate:
    Data:
        Version: 3 (0x2)
        Serial Number: 8461525216278967612 (0x756d605a6651753c)
        Signature Algorithm: ecdsa-with-SHA256
        Issuer: C=US, ST=California, L=Santa Clara, O=PSIA, OU=PKOC WG, CN=kms
        Validity
            Not Before: Jun 25 01:13:00 2025 GMT
            Not After : Aug 21 01:13:00 2025 GMT
        Subject: C=US, ST=California, L=Santa Clara, O=PSIA, OU=PKOC WG, CN=Attestation
        Subject Public Key Info:
            Public Key Algorithm: id-ecPublicKey
                Public-Key: (256 bit)
                pub:
                    04:59:6e:19:70:80:fd:bc:79:ef:21:ae:8b:c5:e0:
                    bf:91:bf:20:55:fe:cb:fc:69:a0:0a:d8:eb:74:6e:
                    bb:46:aa:9c:d7:18:01:82:7e:7a:99:8d:ca:21:1a:
                    b5:f0:81:b0:b6:91:63:80:d9:0f:59:08:60:af:2b:
                    23:52:62:fa:ca
                ASN1 OID: prime256v1
                NIST CURVE: P-256
        X509v3 extensions:
            X509v3 Subject Key Identifier: 
                A9:DA:87:1D:07:07:6F:E8:2E:6F:A3:F7:90:42:5D:26:B2:A5:68:33
            X509v3 Authority Key Identifier: 
                61:98:7F:6E:09:C6:91:CA:83:70:71:1E:19:81:7E:3D:23:F8:82:51
            X509v3 Subject Alternative Name: 
                othername: 1.3.6.1.59685.8.17:<unsupported>
    Signature Algorithm: ecdsa-with-SHA256
    Signature Value:
        30:44:02:20:23:89:d7:72:16:7b:81:68:f6:c2:53:59:66:80:
        88:16:58:3a:5d:19:f8:6c:07:61:d2:32:e1:f4:5b:3a:1a:76:
        02:20:5e:46:be:2f:82:87:60:29:16:f2:2f:c7:a4:2b:85:ab:
        4d:b6:88:d0:c3:02:ba:cf:17:d3:e9:b1:4f:a2:e3:dd
-----BEGIN CERTIFICATE-----
MIIC6DCCAo+gAwIBAgIIdW1gWmZRdTwwCgYIKoZIzj0EAwIwZzELMAkGA1UEBhMC
VVMxEzARBgNVBAgTCkNhbGlmb3JuaWExFDASBgNVBAcTC1NhbnRhIENsYXJhMQ0w
CwYDVQQKEwRQU0lBMRAwDgYDVQQLEwdQS09DIFdHMQwwCgYDVQQDEwNrbXMwHhcN
MjUwNjI1MDExMzAwWhcNMjUwODIxMDExMzAwWjBvMQswCQYDVQQGEwJVUzETMBEG
A1UECBMKQ2FsaWZvcm5pYTEUMBIGA1UEBxMLU2FudGEgQ2xhcmExDTALBgNVBAoT
BFBTSUExEDAOBgNVBAsTB1BLT0MgV0cxFDASBgNVBAMTC0F0dGVzdGF0aW9uMFkw
EwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEWW4ZcID9vHnvIa6LxeC/kb8gVf7L/Gmg
CtjrdG67Rqqc1xgBgn56mY3KIRq18IGwtpFjgNkPWQhgrysjUmL6yqOCARswggEX
MB0GA1UdDgQWBBSp2ocdBwdv6C5vo/eQQl0msqVoMzAfBgNVHSMEGDAWgBRhmH9u
CcaRyoNwcR4ZgX49I/iCUTCB1AYDVR0RBIHMMIHJoIHGBggrBgGD0iUIEaCBuQSB
tjMwNTkzMDEzMDYwNzJhODY0OGNlM2QwMjAxMDYwODJhODY0OGNlM2QwMzAxMDcw
MzQyMDAwNDBlYzVkODdkYzM5ZDE0YTJjNTQ4MDY4NmRhODYwYzgyYjE2YmUwYjY5
MDNiNTI1Zjg0ODQ4Yjc5ZmQ0NjNlMzJiYmRhMWYwMjUyYzMzNTAzYzUyODcwMzVl
NmVhYzU1ZDEzOGQwNjUwZGNmYjUyODFkNTlhOWNmNDEyNGQyODMxMAoGCCqGSM49
BAMCA0cAMEQCICOJ13IWe4Fo9sJTWWaAiBZYOl0Z+GwHYdIy4fRbOhp2AiBeRr4v
godgKRbyL8ekK4WrTbaI0MMCus8X0+mxT6Lj3Q==
-----END CERTIFICATE-----
```

\newpage {}

# punchlist #

- Le at end of example
- fill in transaction identifier components
- proper example response filled in with not-bogus values

