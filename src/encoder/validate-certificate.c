/*
  attribution:
    from a google ai-generated response.  Yes it worked out of the box.

  reformatted slightly to support instrumenting it's use.
*/

#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/bio.h>
#include <openssl/err.h>


int verbosity;


// Function to load a certificate from a file
X509* load_certificate(const char* cert_file) {
    BIO *certbio = NULL;
    X509 *cert = NULL;
    certbio = BIO_new(BIO_s_file());
    if (BIO_read_filename(certbio, cert_file) <= 0) {
        fprintf(stderr, "Error opening certificate file: %s\n", cert_file);
        BIO_free_all(certbio);
        return NULL;
    }
    cert = PEM_read_bio_X509(certbio, NULL, 0, NULL);
    if (!cert) {
        fprintf(stderr, "Error reading certificate from file\n");
        BIO_free_all(certbio);
        return NULL;
    }
    BIO_free_all(certbio);
    return cert;
}

// Function to verify a certificate's signature

int verify_certificate_signature
  (X509 *cert,
  X509 *issuer_cert)

{ /* verify_certificate_signature */

  int result;


    EVP_PKEY* issuer_public_key = X509_get_pubkey(issuer_cert);
    if (!issuer_public_key) {
if (verbosity > 3)
{
        fprintf(stderr, "Error getting issuer public key\n");
};
        return 0; // or handle error appropriately
    }

    result = X509_verify(cert, issuer_public_key);
if (verbosity > 3)
{
  fprintf(stderr, "result from X509_verify was %d. %X\n", result, result);
    EVP_PKEY_free(issuer_public_key);
};

    if (result == 1) {
if (verbosity > 3)
{
        printf("Certificate signature verified successfully!\n");
};
        return 1;
    } else if (result == 0) {
if (verbosity > 3)
{
        fprintf(stderr, "Certificate signature verification failed.\n");
};
        return 0;
    } else {
if (verbosity > 3)
{
        fprintf(stderr, "Error during signature verification.\n");
        ERR_print_errors_fp(stderr);
};
        return -1; // or handle error appropriately
    }
}


int main
  (int argc,
  char *argv [])

{
  X509 *cert;
  char cert_file [1024];
  const char* issuer_cert_file = "attestation-signer.pem";
  int status;
  int status_openssl;


verbosity = 9;
  status = 0;
  strcpy(cert_file, "path/to/your/certificate.pem");
  if (argc > 1)
  {
    strcpy(cert_file, argv[1]);
  };
if (verbosity > 3)
{
  fprintf(stderr, "to be validated: %s\n", cert_file);
};

  cert = load_certificate(cert_file);
  if (!cert) {
        status = 2;
//        return 1;
    }

  if (status == 0)
  {
    X509* issuer_cert = load_certificate(issuer_cert_file);
    if (!issuer_cert) {
        X509_free(cert);
        return 1;
    }

//    if (!verify_certificate_signature(cert, issuer_cert)) {
    status_openssl = verify_certificate_signature(cert, issuer_cert);
if (verbosity > 0)
{
  fprintf(stderr, "verify returned %d. %X\n", status_openssl, status_openssl);
};
    if (status_openssl != 1)
      status = -1;
    X509_free(cert);
    X509_free(issuer_cert);
  };
//    return 0;
  if (status != 0)
    fprintf(stderr, "return status was %d.\n", status);
  return(status);
}

