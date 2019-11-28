# Build Configurations and Outputs

| Folder  | Platform | Compiler             |
|---------|----------|----------------------|
| vs      | Windows  | Visual Studio / MSVC |
| make    | Multiple | Clang/LLVM           |

# Signing Assembly (Windows)

Run this from VS development shell to sign the x86 build with your code signing certificate:

```
sign CERTFILE.p12 CERTPW vs\bin\x86\Studio.exe
```
