$cert = New-SelfSignedCertificate `
  -Type CodeSigningCert `
  -NotAfter 2122-12-31 `
  -CertStoreLocation Cert:\CurrentUser\My `
  -Subject "neuromore Developer"