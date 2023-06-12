
# default publisher common name if not specified
ifeq ($(PUBLISHERCN),)
PUBLISHERCN = neuromore Developer
endif

# default full publisher if not specified
ifeq ($(PUBLISHER),)
PUBLISHER = CN=neuromore Developer
endif

# set this from ENV to enable PKG signing on OSX
#PRODUCTSIGNCN =

# set this from ENV to enable notarization of signed PKG on OSX
#APPLE_ID           = someone@somewhere.com
#APPLE_TEAM_ID      = see https://developer.apple.com/account/#!/membership/
#APPLE_APPSPEC_PASS = app-specific-password-for someone@somwhere.com
#APPLE_DIST_STORE   = true if building packages for macOS store
#APPLE_UPLOAD_STORE = true if package should be uploaded to macOS store

# default key if not specified
ifeq ($(SIGN_PFX_FILE),)
SIGN_PFX_FILE = ../../certs/DevCert.pfx
ifeq ($(SIGN_PFX_PASS),)
SIGN_PFX_PASS = neuromore
endif
endif

##############################################################################################################
# WINDOWS
##############################################################################################################

ifeq ($(TARGET_OS),win)
SCRIPTSFILE=Scripts.ps1
PUBLISHERID = $(shell powershell -command "& {\
  Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass;\
  . $(DISTDIR)/$(SCRIPTSFILE);\
  Get-PublisherHash '$(PUBLISHER)'; }")
VERSIONMAJOR = $(shell powershell -command "& {\
  Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass;\
  . $(DISTDIR)/$(SCRIPTSFILE);\
  Extract-Macro '$(VERSIONFILE)' '$(VERSIONMACROMAJOR)'; }")
VERSIONMINOR = $(shell powershell -command "& {\
  Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass;\
  . $(DISTDIR)/$(SCRIPTSFILE);\
  Extract-Macro '$(VERSIONFILE)' '$(VERSIONMACROMINOR)'; }")
VERSIONPATCH = $(shell powershell -command "& {\
  Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass;\
  . $(DISTDIR)/$(SCRIPTSFILE);\
  Extract-Macro '$(VERSIONFILE)' '$(VERSIONMACROPATCH)'; }")
VERSION3 = $(VERSIONMAJOR).$(VERSIONMINOR).$(VERSIONPATCH)
VERSION4 = $(VERSIONMAJOR).$(VERSIONMINOR).$(VERSIONPATCH).0
ZIPPER   = $(DISTDIR)/7za.exe a -y -aoa -mx6 -bd -bb0
dist-vis: dist-vis-ForestScene \
          dist-vis-InfiniteTunnel \
          dist-vis-CartoonTown \
          dist-vis-TropicalIsland
dist-prep:
	echo [NAM] $(APPNAME)
	echo [ID ] $(APPID)
	echo [VER] $(VERSION4)
	echo [ICO] $(APPICON)
	echo [CPG] CodePage 1252
	chcp 1252
	echo [PUB] $(PUBLISHER)
	echo [PFX] $(SIGN_PFX_FILE)
	echo [MKD] $(DISTDIR)/$(NAME)
	$(call rmdir,$(DISTDIR)/$(NAME))
	$(call deletefiles,$(DISTDIR),$(NAME)*.zip)
	$(call deletefiles,$(DISTDIR),$(NAME)*.msi)
	$(call deletefiles,$(DISTDIR),$(NAME)*.wixpdb)
	$(call deletefiles,$(DISTDIR),$(NAME)*.appx)
	$(call deletefiles,$(DISTDIR),$(NAME)*.appxbundle)
	$(call deletefiles,$(DISTDIR),$(NAME)*.appxupload)
	$(call mkdir,$(DISTDIR)/$(NAME))
	$(call mkdir,$(DISTDIR)/$(NAME)/resources)
	$(call mkdir,$(DISTDIR)/$(NAME)/upload)
	$(call mkdir,$(DISTDIR)/$(NAME)/x64)
	$(call mkdir,$(DISTDIR)/$(NAME)/x86)
	$(call mkdir,$(DISTDIR)/$(NAME)/arm64)
	$(call sleep,3)
	dir dist\win-10\Studio
	$(call copyfiles,$(DISTDIR)/$(NAME).appxmanifest,$(DISTDIR)/$(NAME)/AppxManifest.xml)
	$(call replace,$(DISTDIR)/$(NAME)/AppxManifest.xml,{PUBLISHER},$(PUBLISHER),$(DISTDIR)/$(NAME)/AppxManifest.xml)
	$(call replace,$(DISTDIR)/$(NAME)/AppxManifest.xml,{PUBLISHERID},$(PUBLISHERID),$(DISTDIR)/$(NAME)/AppxManifest.xml)
	$(call replace,$(DISTDIR)/$(NAME)/AppxManifest.xml,{VERSION},$(VERSION4),$(DISTDIR)/$(NAME)/AppxManifest.xml)
	$(call replace,$(DISTDIR)/$(NAME)/AppxManifest.xml,{DISPLAYNAME},$(APPNAME),$(DISTDIR)/$(NAME)/AppxManifest.xml)
	$(call replace,$(DISTDIR)/$(NAME)/AppxManifest.xml,{APPID},$(APPID),$(DISTDIR)/$(NAME)/AppxManifest.xml)
	$(call replace,$(DISTDIR)/$(NAME)/AppxManifest.xml,{APPCOMPANY},$(APPCOMPANY),$(DISTDIR)/$(NAME)/AppxManifest.xml)
	$(call replace,$(DISTDIR)/$(NAME)/AppxManifest.xml,{APPSHORTNAME},$(APPSHORTNAME),$(DISTDIR)/$(NAME)/AppxManifest.xml)
	$(call copyfiles,$(DISTDIR)/$(NAME).layout,$(DISTDIR)/$(NAME)/Layout.xml)
	$(call replace,$(DISTDIR)/$(NAME)/Layout.xml,{VERSION},$(VERSION3),$(DISTDIR)/$(NAME)/Layout.xml)
	echo [CPY] Icons
	$(call copyfiles,$(APPICON)-44x44.png,$(DISTDIR)/$(NAME)/resources/app-44x44.png)
	$(call copyfiles,$(APPICON)-50x50.png,$(DISTDIR)/$(NAME)/resources/app-50x50.png)
	$(call copyfiles,$(APPICON)-150x150.png,$(DISTDIR)/$(NAME)/resources/app-150x150.png)
	$(call copyfiles,$(APPICON).ico,$(DISTDIR)/$(NAME)/resources/app.ico)
dist-vis-%: dist-prep
	echo [VIS] $*
	$(call mkdir,$(DISTDIR)/$(NAME)/x64/Visualizations/$*/)
	$(call copyfiles,$(DISTDIR)/../../visualizations/$*/Info.json,$(DISTDIR)/$(NAME)/x64/Visualizations/$*/)
	$(call copyfiles,$(DISTDIR)/../../visualizations/$*/Thumbnail.png,$(DISTDIR)/$(NAME)/x64/Visualizations/$*/)
	$(call copyfilesrecursive,$(DISTDIR)/../../visualizations/$*/win-x64/*,$(DISTDIR)/$(NAME)/x64/Visualizations/$*/)
	$(call sleep,3)
ifeq ($(SIGN_PFX_PASS),)
	$(call sign,$(DISTDIR)/$(NAME)/x64/Visualizations/$*/$*.exe,$(SIGN_PFX_FILE))
else
	$(call signp,$(DISTDIR)/$(NAME)/x64/Visualizations/$*/$*.exe,$(SIGN_PFX_FILE),$(SIGN_PFX_PASS))
endif
	$(call mkdir,$(DISTDIR)/$(NAME)/x86/Visualizations/$*/)
	$(call copyfiles,$(DISTDIR)/../../visualizations/$*/Info.json,$(DISTDIR)/$(NAME)/x86/Visualizations/$*/)
	$(call copyfiles,$(DISTDIR)/../../visualizations/$*/Thumbnail.png,$(DISTDIR)/$(NAME)/x86/Visualizations/$*/)
	$(call copyfilesrecursive,$(DISTDIR)/../../visualizations/$*/win-x86/*,$(DISTDIR)/$(NAME)/x86/Visualizations/$*/)
	$(call sleep,3)
ifeq ($(SIGN_PFX_PASS),)
	$(call sign,$(DISTDIR)/$(NAME)/x86/Visualizations/$*/$*.exe,$(SIGN_PFX_FILE))
else
	$(call signp,$(DISTDIR)/$(NAME)/x86/Visualizations/$*/$*.exe,$(SIGN_PFX_FILE),$(SIGN_PFX_PASS))
endif
	$(call mkdir,$(DISTDIR)/$(NAME)/arm64/Visualizations/$*/)
	$(call copyfiles,$(DISTDIR)/../../visualizations/$*/Info.json,$(DISTDIR)/$(NAME)/arm64/Visualizations/$*/)
	$(call copyfiles,$(DISTDIR)/../../visualizations/$*/Thumbnail.png,$(DISTDIR)/$(NAME)/arm64/Visualizations/$*/)
	$(call copyfilesrecursive,$(DISTDIR)/../../visualizations/$*/win-arm64/*,$(DISTDIR)/$(NAME)/arm64/Visualizations/$*/)
	$(call sleep,3)
ifeq ($(SIGN_PFX_PASS),)
	$(call sign,$(DISTDIR)/$(NAME)/arm64/Visualizations/$*/$*.exe,$(SIGN_PFX_FILE))
else
	$(call signp,$(DISTDIR)/$(NAME)/arm64/Visualizations/$*/$*.exe,$(SIGN_PFX_FILE),$(SIGN_PFX_PASS))
endif
dist-dll-x64: dist-prep
	echo [DLL] Copy X64 DLL
	$(call copyfiles,./bin/win-x64/eego-SDK.dll,$(DISTDIR)/$(NAME)/x64/eego-SDK.dll)
	$(call copyfiles,./bin/win-x64/gforce64.dll,$(DISTDIR)/$(NAME)/x64/gforce64.dll)
	$(call copyfiles,./bin/win-x64/neurosdk-x64.dll,$(DISTDIR)/$(NAME)/x64/neurosdk-x64.dll)
	$(call copyfiles,./bin/win-x64/Unicorn.dll,$(DISTDIR)/$(NAME)/x64/Unicorn.dll)
	$(call copyfiles,./bin/win-x64/GanglionLib.dll,$(DISTDIR)/$(NAME)/x64/GanglionLib.dll)
	$(call copyfiles,./bin/win-x64/gForceSDKWrapper.dll,$(DISTDIR)/$(NAME)/x64/gForceSDKWrapper.dll)
dist-dll-x86: dist-prep
	echo [DLL] Copy X86 DLL
	$(call copyfiles,./bin/win-x86/eego-SDK.dll,$(DISTDIR)/$(NAME)/x86/eego-SDK.dll)
	$(call copyfiles,./bin/win-x86/gforce32.dll,$(DISTDIR)/$(NAME)/x86/gforce32.dll)
	$(call copyfiles,./bin/win-x86/neurosdk-x86.dll,$(DISTDIR)/$(NAME)/x86/neurosdk-x86.dll)
	$(call copyfiles,./bin/win-x86/GanglionLib32.dll,$(DISTDIR)/$(NAME)/x86/GanglionLib32.dll)
	$(call copyfiles,./bin/win-x86/gForceSDKWrapper32.dll,$(DISTDIR)/$(NAME)/x86/gForceSDKWrapper32.dll)
dist-dll-arm64: dist-prep
	echo [DLL] Copy ARM64 DLL
dist-bin-%: dist-prep dist-dll-%
	echo [BIN] $(DISTDIR)/$(NAME)/$*
	$(call mkdir,$(DISTDIR)/$(NAME)/$*)
	$(call copyfiles,./bin/win-$*/$(NAME)$(EXTBIN),$(DISTDIR)/$(NAME)/$*/$(NAME)$(EXTBIN))
	$(call copyfiles,./bin/win-$*/$(NAME)$(EXTPDB),$(DISTDIR)/$(NAME)/$*/$(NAME)$(EXTPDB))
	$(call sleep,3)
	echo [STR] $(DISTDIR)/$(NAME)/$*/$(NAME)$(EXTBIN)
	$(STRIP) $(STRIPFLAGS) $(DISTDIR)/$(NAME)/$*/$(NAME)$(EXTBIN)
	echo [SIG] $(DISTDIR)/$(NAME)/$*/$(NAME)$(EXTBIN)
ifeq ($(SIGN_PFX_PASS),)
	$(call sign,$(DISTDIR)/$(NAME)/$*/$(NAME)$(EXTBIN),$(SIGN_PFX_FILE))
else
	$(call signp,$(DISTDIR)/$(NAME)/$*/$(NAME)$(EXTBIN),$(SIGN_PFX_FILE),$(SIGN_PFX_PASS))
endif
	echo [SYM] $(DISTDIR)/$(NAME)/upload/$(NAME)-$*.appxsym
	$(ZIPPER) $(DISTDIR)/$(NAME)/upload/$(NAME)-$*.appxsym.zip $(DISTDIR)/$(NAME)/$*/$(NAME)$(EXTPDB)	
	$(call move,$(DISTDIR)/$(NAME)/upload/$(NAME)-$*.appxsym.zip,$(DISTDIR)/$(NAME)/upload/$(NAME)-$*.appxsym)
#	echo [ZIP] $(DISTDIR)/$(NAME)-$(VERSION3)-win-10-$*.zip
#	$(ZIPPER) $(DISTDIR)/$(NAME)-$(VERSION3)-win-10-$*.zip $(DISTDIR)/$(NAME)/$*/*
	echo [MSI] $(DISTDIR)/$(NAME)-$(VERSION3)-win-10-$*.msi
	wix extension add WixToolset.Firewall.wixext
	wix build -arch $* \
	  -ext WixToolset.Firewall.wixext \
	  -d APPNAME="$(APPNAME)" \
	  -d APPSHORTNAME="$(APPSHORTNAME)" \
	  -d APPCOMPANY="$(APPCOMPANY)" \
	  -d VERSION=$(VERSION4) \
	  -b $(DISTDIR)/$(NAME) \
	  $(DISTDIR)/$(NAME).wxs \
	  -out $(DISTDIR)/$(NAME)-$(VERSION3)-win-10-$*.msi
dist: dist-prep dist-vis dist-bin-x64 dist-bin-x86 dist-bin-arm64
	echo [BDL] $(DISTDIR)/$(NAME)-$(VERSION3)-win-10.appxbundle
	$(call makepkg,$(DISTDIR)/$(NAME)/Layout.xml,$(DISTDIR))
	echo [SIG] $(DISTDIR)/$(NAME)-$(VERSION3)-win-10.appxbundle
ifeq ($(SIGN_PFX_PASS),)
	$(call sign,$(DISTDIR)/$(NAME).appxbundle,$(SIGN_PFX_FILE))
	$(call sign,$(DISTDIR)/$(NAME)-x64.appx,$(SIGN_PFX_FILE))
	$(call sign,$(DISTDIR)/$(NAME)-x86.appx,$(SIGN_PFX_FILE))
	$(call sign,$(DISTDIR)/$(NAME)-arm64.appx,$(SIGN_PFX_FILE))
	$(call sign,$(DISTDIR)/$(NAME)-$(VERSION3)-win-10-x64.msi,$(SIGN_PFX_FILE))
	$(call sign,$(DISTDIR)/$(NAME)-$(VERSION3)-win-10-x86.msi,$(SIGN_PFX_FILE))
	$(call sign,$(DISTDIR)/$(NAME)-$(VERSION3)-win-10-arm64.msi,$(SIGN_PFX_FILE))
else
	$(call signp,$(DISTDIR)/$(NAME).appxbundle,$(SIGN_PFX_FILE),$(SIGN_PFX_PASS))
	$(call signp,$(DISTDIR)/$(NAME)-x64.appx,$(SIGN_PFX_FILE),$(SIGN_PFX_PASS))
	$(call signp,$(DISTDIR)/$(NAME)-x86.appx,$(SIGN_PFX_FILE),$(SIGN_PFX_PASS))
	$(call signp,$(DISTDIR)/$(NAME)-arm64.appx,$(SIGN_PFX_FILE),$(SIGN_PFX_PASS))
	$(call signp,$(DISTDIR)/$(NAME)-$(VERSION3)-win-10-x64.msi,$(SIGN_PFX_FILE),$(SIGN_PFX_PASS))
	$(call signp,$(DISTDIR)/$(NAME)-$(VERSION3)-win-10-x86.msi,$(SIGN_PFX_FILE),$(SIGN_PFX_PASS))
	$(call signp,$(DISTDIR)/$(NAME)-$(VERSION3)-win-10-arm64.msi,$(SIGN_PFX_FILE),$(SIGN_PFX_PASS))
endif
	echo [APU] $(DISTDIR)/$(NAME)-$(VERSION3)-win-10.appxupload
	$(call copyfiles,$(DISTDIR)/$(NAME).appxbundle,$(DISTDIR)/$(NAME)/upload/$(NAME).appxbundle)
	$(ZIPPER) $(DISTDIR)/$(NAME).appxupload.zip $(DISTDIR)/$(NAME)/upload/*
	$(call move,$(DISTDIR)/$(NAME).appxupload.zip,$(DISTDIR)/$(NAME)-$(VERSION3)-win-10.appxupload)	
	$(call move,$(DISTDIR)/$(NAME).appxbundle,$(DISTDIR)/$(NAME)-$(VERSION3)-win-10.appxbundle)
endif

##############################################################################################################
# OSX
##############################################################################################################

ifeq ($(TARGET_OS),osx)
KEYCHAIN     = sign-$(NAME).keychain-db
VERSIONMAJOR = $(shell sed -n 's/^\#define $(VERSIONMACROMAJOR) //p' $(VERSIONFILE))
VERSIONMINOR = $(shell sed -n 's/^\#define $(VERSIONMACROMINOR) //p' $(VERSIONFILE))
VERSIONPATCH = $(shell sed -n 's/^\#define $(VERSIONMACROPATCH) //p' $(VERSIONFILE))
VERSION3     = $(VERSIONMAJOR).$(VERSIONMINOR).$(VERSIONPATCH)
VERSION4     = $(VERSIONMAJOR).$(VERSIONMINOR).$(VERSIONPATCH).0
DISTDIRAPP   = "$(DISTDIR)/$(NAME)/$(APPNAME).app"
OSXVER       = $(shell sw_vers -productVersion)
OSXBUILDV    = $(shell sw_vers -buildVersion)
OSXSDKVER    = $(shell xcrun --show-sdk-version)
OSXSDKBUILDV = $(shell xcrun --show-sdk-build-version)
XCODEVER     = $(shell xcodebuild -version | grep -E -m1 'Xcode' | sed 's/Xcode //g')
XCODEBUILDV  = $(shell xcodebuild -version | grep -E -m1 'Build version' | sed 's/Build version //g')
PKGSIGNED    = $(NAME)-$(VERSION3)-macOS-10.15-universal.pkg
PKGUNSIGNED  = $(NAME)-$(VERSION3)-macOS-10.15-universal-unsigned.pkg

dist-vis: dist-vis-ForestScene \
          dist-vis-InfiniteTunnel \
          dist-vis-CartoonTown \
          dist-vis-TropicalIsland
dist-prep:
	@echo [NAM] $(APPNAME)
	@echo [ID ] $(APPID)
	@echo [VER] $(VERSION3)
	@echo [ICO] $(APPICON)
	@echo [OSX] $(OSXVER) - ${OSXBUILDV}
	@echo [SDK] $(OSXSDKVER) - ${OSXSDKBUILDV}
	@echo [XCO] $(XCODEVER) - ${XCODEBUILDV}
	@echo [KCH] $(KEYCHAIN)
	@-rm -rf $(DISTDIR)/$(NAME)
	@-rm -rf $(DISTDIR)/$(NAME).dSYM
	@-rm -rf $(DISTDIR)/$(NAME).symbols
	@-rm -rf $(DISTDIR)/*.pkg
	@-security delete-keychain $(KEYCHAIN)
	@security create-keychain -p "$(SIGN_PFX_PASS)" $(KEYCHAIN)
	@security set-keychain-settings -lut 21600 $(KEYCHAIN)
	@security unlock-keychain -p "$(SIGN_PFX_PASS)" $(KEYCHAIN)
	@echo [IMP] $(SIGN_PFX_FILE)
	@security import $(SIGN_PFX_FILE) \
		-P $(SIGN_PFX_PASS) \
		-f pkcs12 \
		-k $(KEYCHAIN) \
		-T /usr/bin/codesign \
		-T /usr/bin/productsign
	@echo [KPL] $(KEYCHAIN)
	@security set-key-partition-list \
		-S apple-tool:,apple:,codesign: \
		-s -k $(SIGN_PFX_PASS) $(KEYCHAIN)
	@echo [LST] $(KEYCHAIN)
	@security list-keychain -d user -s $(KEYCHAIN)
	@echo [INF] $(KEYCHAIN)
	@security show-keychain-info $(KEYCHAIN)
dist-vis-%: dist-prep
	@echo [VIS] $*
	@mkdir -p $(DISTDIRAPP)/Contents/Visualizations/$*/
	@cp $(DISTDIR)/../../visualizations/$*/Info.json $(DISTDIRAPP)/Contents/Visualizations/$*/
	@cp $(DISTDIR)/../../visualizations/$*/Thumbnail.png $(DISTDIRAPP)/Contents/Visualizations/$*/
	@-cp -r $(DISTDIR)/../../visualizations/$*/osx-all/ $(DISTDIRAPP)/Contents/Visualizations/$*/
	@-codesign --verbose \
	  --sign "$(PUBLISHERCN)" \
	  --keychain $(KEYCHAIN) \
	  --timestamp --force --deep \
	  --options runtime \
	  --entitlements $(DISTDIR)/Visualization.Entitlements.plist \
	  $(DISTDIRAPP)/Contents/Visualizations/$*/$*.app
dist-%: dist-prep
	@echo [DST] $(NAME)-$*
dist: dist-prep dist-x64 dist-arm64 dist-vis
	@echo [MKD] $(APPNAME).app/Contents/MacOS
	@mkdir -p $(DISTDIRAPP)/Contents/MacOS
	@echo [LIP] $(NAME)$(EXTBIN)
	@lipo -create -output $(DISTDIRAPP)/Contents/MacOS/$(NAME) \
	  ./bin/osx-x64/$(NAME)$(EXTBIN) \
	  ./bin/osx-arm64/$(NAME)$(EXTBIN)
	@echo [SYM] $(NAME).dSYM
	@dsymutil \
	  -out $(DISTDIR)/$(NAME).dSYM \
	  $(DISTDIRAPP)/Contents/MacOS/$(NAME)
	@echo [INF] $(NAME).dSYM
	@dwarfdump --uuid $(DISTDIR)/$(NAME).dSYM
#	@echo [VFY] $(NAME).dSYM
#	@dwarfdump --verify $(DISTDIR)/$(NAME).dSYM
	@mkdir -p $(DISTDIR)/$(NAME).symbols
	@xcrun symbols -noTextInSOD -noDaemon -arch all \
	  -symbolsPackageDir $(DISTDIR)/$(NAME).symbols \
	  $(DISTDIR)/$(NAME).dSYM
	@echo [STR] $(APPNAME).app/Contents/MacOS/$(NAME)
	@$(STRIP) $(STRIPFLAGS) $(DISTDIRAPP)/Contents/MacOS/$(NAME)
	@chmod +x $(DISTDIRAPP)/Contents/MacOS/$(NAME)
	@echo [MKD] $(APPNAME).app/Contents/Resources
	@mkdir -p $(DISTDIRAPP)/Contents/Resources
	@echo [ICO] $(NAME).icns
	@cp $(APPICON).icns $(DISTDIRAPP)/Contents/Resources/Icon.icns
	@cp $(DISTDIR)/$(NAME).Info.plist $(DISTDIRAPP)/Contents/Info.plist
	@cp $(DISTDIR)/$(NAME).provisionprofile $(DISTDIRAPP)/Contents/embedded.provisionprofile
	@sed -i'.orig' -e 's/{VERSION}/${VERSION3}/g' $(DISTDIRAPP)/Contents/Info.plist
	@sed -i'.orig' -e 's/{DISPLAYNAME}/${APPNAME}/g' $(DISTDIRAPP)/Contents/Info.plist
	@sed -i'.orig' -e 's/{APPSHORTNAME}/${APPSHORTNAME}/g' $(DISTDIRAPP)/Contents/Info.plist
	@sed -i'.orig' -e 's/{APPID}/${APPID}/g' $(DISTDIRAPP)/Contents/Info.plist
	@sed -i'.orig' -e 's/{APPCOMPANY}/${APPCOMPANY}/g' $(DISTDIRAPP)/Contents/Info.plist
	@sed -i'.orig' -e 's/{OSXSDKVER}/${OSXSDKVER}/g' $(DISTDIRAPP)/Contents/Info.plist
	@sed -i'.orig' -e 's/{OSXSDKBUILDV}/${OSXSDKBUILDV}/g' $(DISTDIRAPP)/Contents/Info.plist
	@sed -i'.orig' -e 's/{OSXBUILDV}/${OSXBUILDV}/g' $(DISTDIRAPP)/Contents/Info.plist
	@sed -i'.orig' -e 's/{XCODEBUILDV}/${XCODEBUILDV}/g' $(DISTDIRAPP)/Contents/Info.plist
	@rm $(DISTDIRAPP)/Contents/Info.plist.orig
	@cp $(DISTDIR)/$(NAME).Component.template.plist $(DISTDIR)/$(NAME).Component.plist
	@sed -i'.orig' -e 's/{APPNAME}/${APPNAME}/g' $(DISTDIR)/$(NAME).Component.plist
	@rm $(DISTDIR)/$(NAME).Component.plist.orig
ifeq ($(APPLE_DIST_STORE),true)
	@echo [SIG] $(NAME).app
	@codesign --verbose \
	  --sign "$(PUBLISHERCN)" \
	  --keychain $(KEYCHAIN) \
	  --timestamp \
	  --options runtime \
	  --entitlements $(DISTDIR)/$(NAME).Entitlements.Store.plist \
	  $(DISTDIRAPP)
	@echo [VFY] $(APPNAME).app
	@codesign --verify -vvvd $(DISTDIRAPP)
	@echo [PKG] $(PKGUNSIGNED)
	@productbuild \
	  --version $(VERSION3) \
	  --symbolication $(DISTDIR)/$(NAME).symbols \
	  --product $(DISTDIR)/$(NAME).Requirements.plist \
	  --component $(DISTDIRAPP) /Applications \
	  $(DISTDIR)/$(PKGUNSIGNED)
else
	@echo [SIG] $(APPNAME).app
	@codesign --verbose \
	  --sign "$(PUBLISHERCN)" \
	  --keychain $(KEYCHAIN) \
	  --timestamp \
	  --options runtime \
	  --entitlements $(DISTDIR)/$(NAME).Entitlements.Local.plist \
	  $(DISTDIRAPP)
	@echo [VFY] $(APPNAME).app
	@codesign --verify -vvvd $(DISTDIRAPP)
	@echo [PKG] $(PKGUNSIGNED)
	@pkgbuild \
	  --version $(VERSION3) \
	  --root $(DISTDIR)/$(NAME) \
	  --install-location /Applications \
	  --component-plist $(DISTDIR)/$(NAME).Component.plist \
	  $(DISTDIR)/$(PKGUNSIGNED)
endif
	@echo [FIL] $(PKGUNSIGNED)
	@pkgutil --payload-files $(DISTDIR)/$(PKGUNSIGNED)
ifneq ($(PRODUCTSIGNCN),)
	@echo [SIG] $(PKGSIGNED)
	@productsign \
	  --sign "$(PRODUCTSIGNCN)" \
	  --keychain $(KEYCHAIN) \
	  --timestamp \
	  $(DISTDIR)/$(PKGUNSIGNED) \
	  $(DISTDIR)/$(PKGSIGNED)
	@echo [VFY] $(PKGSIGNED)
	@pkgutil --check-signature $(DISTDIR)/$(PKGSIGNED)
ifneq ($(APPLE_ID),)
ifeq ($(APPLE_DIST_STORE),true)
	@echo [VAL] $(PKGSIGNED)
	@xcrun altool --validate-app \
	  -f $(DISTDIR)/$(PKGSIGNED) \
	  -t macOS \
	  -u $(APPLE_ID) \
	  -p $(APPLE_APPSPEC_PASS)
ifeq ($(APPLE_UPLOAD_STORE),true)
	@echo [UPL] $(PKGSIGNED)
	@xcrun altool --upload-app \
	  -f $(DISTDIR)/$(PKGSIGNED) \
	  -t macOS \
	  -u $(APPLE_ID) \
	  -p $(APPLE_APPSPEC_PASS)
endif
else
	@echo [VAL] $(PKGSIGNED)
	@xcrun notarytool submit $(DISTDIR)/$(PKGSIGNED) \
	  --apple-id=$(APPLE_ID) \
	  --team-id=$(APPLE_TEAM_ID) \
	  --password=$(APPLE_APPSPEC_PASS) \
	  --wait | tee $(DISTDIR)/$(NAME).notary.log
	@cat $(DISTDIR)/$(NAME).notary.log | \
	  grep -E -o -m1 'id: .{36}' | \
	  sed 's/id: //g' > $(DISTDIR)/$(NAME).notary.id
	@bash -c "\
	  export NOTARYID=\$$(cat $(DISTDIR)/$(NAME).notary.id);\
	  xcrun notarytool log \$$NOTARYID \
	    --apple-id=$(APPLE_ID) \
	    --team-id=$(APPLE_TEAM_ID) \
	    --password=$(APPLE_APPSPEC_PASS)"
endif
endif
endif
endif

##############################################################################################################
# LINUX
##############################################################################################################

ifeq ($(TARGET_OS),linux)
VERSIONMAJOR   = $(shell sed -n 's/^\#define $(VERSIONMACROMAJOR) //p' $(VERSIONFILE))
VERSIONMINOR   = $(shell sed -n 's/^\#define $(VERSIONMACROMINOR) //p' $(VERSIONFILE))
VERSIONPATCH   = $(shell sed -n 's/^\#define $(VERSIONMACROPATCH) //p' $(VERSIONFILE))
VERSION3       = $(VERSIONMAJOR).$(VERSIONMINOR).$(VERSIONPATCH)
VERSION4       = $(VERSIONMAJOR).$(VERSIONMINOR).$(VERSIONPATCH).0
VISUALIZATIONS = ForestScene InfiniteTunnel CartoonTown TropicalIsland
dist-prep:
	echo [NAM] $(APPNAME)
	echo [SNM] $(APPSHORTNAME)
	echo [VER] $(VERSION3)
	echo [ICO] $(APPICON)
dist-%: dist-prep
	echo [DST] $(NAME)-$*
	$(eval DISTDEBARCH:=$(shell \
	  case $* in \
	    (x64)   echo amd64;; \
		(x86)   echo i386;; \
		(arm64) echo arm64;; \
		(arm)   echo armhf;; \
	  esac))
	$(eval DEBFILE=$(NAME)-$(VERSION3)-ubuntu-$(LSBREL)-$(DISTDEBARCH).deb)
	echo [PRE] $(DEBFILE)
	mkdir -p $(DISTDIR)/$(NAME)-$*/DEBIAN
	mkdir -p $(DISTDIR)/$(NAME)-$*/usr/bin
	mkdir -p $(DISTDIR)/$(NAME)-$*/usr/lib
	mkdir -p $(DISTDIR)/$(NAME)-$*/usr/share/applications
	mkdir -p $(DISTDIR)/$(NAME)-$*/usr/share/pixmaps	
	cp $(DISTDIR)/$(NAME).control $(DISTDIR)/$(NAME)-$*/DEBIAN/control
	sed -i 's/{VERSION}/${VERSION3}/g' $(DISTDIR)/$(NAME)-$*/DEBIAN/control
	sed -i 's/{ARCH}/${DEBARCH}/g' $(DISTDIR)/$(NAME)-$*/DEBIAN/control	
	sed -i 's/{APPSHORTNAME}/${APPSHORTNAME}/g' $(DISTDIR)/$(NAME)-$*/DEBIAN/control
	cp $(APPICON)-256x256.png $(DISTDIR)/$(NAME)-$*/usr/share/pixmaps/$(APPSHORTNAME).png
	cp $(DISTDIR)/$(NAME).desktop $(DISTDIR)/$(NAME)-$*/usr/share/applications/$(APPSHORTNAME).desktop
	sed -i 's/{DISPLAYNAME}/${APPNAME}/g' $(DISTDIR)/$(NAME)-$*/usr/share/applications/$(APPSHORTNAME).desktop
	sed -i 's/{APPSHORTNAME}/${APPSHORTNAME}/g' $(DISTDIR)/$(NAME)-$*/usr/share/applications/$(APPSHORTNAME).desktop
	cp ./bin/linux-$*/$(NAME)$(EXTBIN) $(DISTDIR)/$(NAME)-$*/usr/bin/$(APPSHORTNAME)$(EXTBIN)
	@chmod +x $(DISTDIR)/$(NAME)-$*/usr/bin/$(APPSHORTNAME)$(EXTBIN)
	-cp ./../../deps/prebuilt/linux/$*/*.so $(DISTDIR)/$(NAME)-$*/usr/lib/
	for vis in $(VISUALIZATIONS) ; do \
	  echo [VIS] $$vis ; \
	  mkdir -p $(DISTDIR)/$(NAME)-$*/usr/share/$(APPSHORTNAME)/visualizations/$$vis ; \
	  cp $(DISTDIR)/../../visualizations/$$vis/Info.json $(DISTDIR)/$(NAME)-$*/usr/share/$(APPSHORTNAME)/visualizations/$$vis/Info.json ; \
	  cp $(DISTDIR)/../../visualizations/$$vis/Thumbnail.png $(DISTDIR)/$(NAME)-$*/usr/share/$(APPSHORTNAME)/visualizations/$$vis/Thumbnail.png ; \
	  cp -r $(DISTDIR)/../../visualizations/$$vis/linux-$*/* $(DISTDIR)/$(NAME)-$*/usr/share/$(APPSHORTNAME)/visualizations/$$vis/ 2>/dev/null || true; \
	done
	echo [DEB] $(DEBFILE)
	dpkg-deb --build $(DISTDIR)/$(NAME)-$* $(DISTDIR)/$(DEBFILE)
		
#dist: dist-prep dist-x64 dist-x86 dist-arm64 dist-arm
dist: dist-prep dist-$(TARGET_ARCH)
endif

##############################################################################################################
# ANDROID
##############################################################################################################

ifeq ($(TARGET_OS),android)
dist-%:
	echo [DST] $(NAME)-$*
dist: dist-x64 dist-x86 dist-arm64 dist-arm
endif
