Generate with DevCert.ps1 on Windows.
Then repacked with OpenSSL on OSX:

openssl pkcs12 -info -in ./DevCert.pfx -out ./DevCert.crt -nokeys
openssl pkcs12 -info -in ./DevCert.pfx -out ./DevCert.key -nocerts
openssl pkcs12 -export -out DevCert.pfx -in ./DevCert.crt -inkey ./DevCert.key
