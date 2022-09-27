
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
dist-prep:
	echo [VER] $(VERSION4)
	echo [CPG] CodePage 1252
	chcp 1252
	echo [PUB] $(PUBLISHER)
	echo [PFX] $(SIGN_PFX_FILE)
	$(call rmdir,$(DISTDIR)/$(NAME))
	$(call mkdir,$(DISTDIR)/$(NAME))
	$(call copyfiles,$(DISTDIR)/$(NAME).appxmanifest,$(DISTDIR)/$(NAME)/AppxManifest.xml)
	$(call replace,$(DISTDIR)/$(NAME)/AppxManifest.xml,{PUBLISHER},$(PUBLISHER),$(DISTDIR)/$(NAME)/AppxManifest.xml)
	$(call replace,$(DISTDIR)/$(NAME)/AppxManifest.xml,{PUBLISHERID},$(PUBLISHERID),$(DISTDIR)/$(NAME)/AppxManifest.xml)
	$(call replace,$(DISTDIR)/$(NAME)/AppxManifest.xml,{VERSION},$(VERSION4),$(DISTDIR)/$(NAME)/AppxManifest.xml)
	$(call replace,$(DISTDIR)/$(NAME)/AppxManifest.xml,{DISPLAYNAME},$(APPNAME),$(DISTDIR)/$(NAME)/AppxManifest.xml)
	$(call copyfiles,$(DISTDIR)/$(NAME).layout,$(DISTDIR)/$(NAME)/Layout.xml)
dist-%: dist-prep
	$(call rmdir,$(DISTDIR)/$(NAME)/$*)
	$(call mkdir,$(DISTDIR)/$(NAME)/$*)
	$(call copyfiles,./bin/win-$*/$(NAME)$(EXTBIN),$(DISTDIR)/$(NAME)/$*/$(NAME)$(EXTBIN))
	echo [STR] $(DISTDIR)/$(NAME)/$*/$(NAME)$(EXTBIN)
	$(STRIP) $(STRIPFLAGS) $(DISTDIR)/$(NAME)/$*/$(NAME)$(EXTBIN)
	echo [SIG] $(DISTDIR)/$(NAME)/$*/$(NAME)$(EXTBIN)
ifeq ($(SIGN_PFX_PASS),)
	$(call sign,$(DISTDIR)/$(NAME)/$*/$(NAME)$(EXTBIN),$(SIGN_PFX_FILE))
else
	$(call signp,$(DISTDIR)/$(NAME)/$*/$(NAME)$(EXTBIN),$(SIGN_PFX_FILE),$(SIGN_PFX_PASS))
endif
dist: dist-prep dist-x64 dist-x86 dist-arm64
	echo [BDL] $(NAME).appxbundle
	$(call makepkg,$(DISTDIR)/$(NAME)/Layout.xml,$(DISTDIR))
	echo [SIG] $(DISTDIR)/$(NAME).appxbundle
ifeq ($(SIGN_PFX_PASS),)
	$(call sign,$(DISTDIR)/$(NAME).appxbundle,$(SIGN_PFX_FILE))
	$(call sign,$(DISTDIR)/$(NAME)-x64.appx,$(SIGN_PFX_FILE))
	$(call sign,$(DISTDIR)/$(NAME)-x86.appx,$(SIGN_PFX_FILE))
	$(call sign,$(DISTDIR)/$(NAME)-arm64.appx,$(SIGN_PFX_FILE))
else
	$(call signp,$(DISTDIR)/$(NAME).appxbundle,$(SIGN_PFX_FILE),$(SIGN_PFX_PASS))
	$(call signp,$(DISTDIR)/$(NAME)-x64.appx,$(SIGN_PFX_FILE),$(SIGN_PFX_PASS))
	$(call signp,$(DISTDIR)/$(NAME)-x86.appx,$(SIGN_PFX_FILE),$(SIGN_PFX_PASS))
	$(call signp,$(DISTDIR)/$(NAME)-arm64.appx,$(SIGN_PFX_FILE),$(SIGN_PFX_PASS))
endif

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
dist-prep:
	@echo [VER] $(VERSION3)
	@echo [OSX] $(OSXVER) - ${OSXBUILDV}
	@echo [SDK] $(OSXSDKVER) - ${OSXSDKBUILDV}
	@echo [XCO] $(XCODEVER) - ${XCODEBUILDV}
	@echo [KCH] $(KEYCHAIN)
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
dist-%: dist-prep
	@echo [DST] $(NAME)-$*
dist: dist-prep dist-x64 dist-arm64
	@echo [MKD] $(APPNAME).app/Contents/MacOS
	@mkdir -p $(DISTDIRAPP)/Contents/MacOS
	@echo [LIP] $(NAME)$(EXTBIN)
	@lipo -create -output $(DISTDIRAPP)/Contents/MacOS/$(NAME) \
	  ./bin/osx-x64/$(NAME)$(EXTBIN) \
	  ./bin/osx-arm64/$(NAME)$(EXTBIN)
	@echo [SYM] $(NAME).dSYM
	@-rm -rf $(DISTDIR)/$(NAME).dSYM
	@dsymutil \
	  -out $(DISTDIR)/$(NAME).dSYM \
	  $(DISTDIRAPP)/Contents/MacOS/$(NAME)
	@echo [INF] $(NAME).dSYM
	@dwarfdump --uuid $(DISTDIR)/$(NAME).dSYM
	@echo [VFY] $(NAME).dSYM
	@dwarfdump --verify $(DISTDIR)/$(NAME).dSYM
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
	@cp $(SRCDIR)/Resources/AppIcon-neuromore.icns $(DISTDIRAPP)/Contents/Resources/Icon.icns
	@cp $(DISTDIR)/$(NAME).Info.plist $(DISTDIRAPP)/Contents/Info.plist
	@cp $(DISTDIR)/$(NAME).provisionprofile $(DISTDIRAPP)/Contents/embedded.provisionprofile
	@sed -i'.orig' -e 's/{VERSION}/${VERSION3}/g' $(DISTDIRAPP)/Contents/Info.plist
	@sed -i'.orig' -e 's/{DISPLAYNAME}/${APPNAME}/g' $(DISTDIRAPP)/Contents/Info.plist
	@sed -i'.orig' -e 's/{OSXSDKVER}/${OSXSDKVER}/g' $(DISTDIRAPP)/Contents/Info.plist
	@sed -i'.orig' -e 's/{OSXSDKBUILDV}/${OSXSDKBUILDV}/g' $(DISTDIRAPP)/Contents/Info.plist
	@sed -i'.orig' -e 's/{OSXBUILDV}/${OSXBUILDV}/g' $(DISTDIRAPP)/Contents/Info.plist
	@sed -i'.orig' -e 's/{XCODEBUILDV}/${XCODEBUILDV}/g' $(DISTDIRAPP)/Contents/Info.plist
	@rm $(DISTDIRAPP)/Contents/Info.plist.orig
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
	@echo [PKG] $(NAME).pkg
	@productbuild \
	  --version $(VERSION3) \
	  --symbolication $(DISTDIR)/$(NAME).symbols \
	  --product $(DISTDIR)/$(NAME).Requirements.plist \
	  --component $(DISTDIRAPP) /Applications \
	  $(DISTDIR)/$(NAME).pkg
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
	@echo [PKG] $(NAME).pkg
	@pkgbuild \
	  --version $(VERSION3) \
	  --root $(DISTDIR)/$(NAME) \
	  --install-location /Applications \
	  --component-plist $(DISTDIR)/$(NAME).Component.plist \
	  $(DISTDIR)/$(NAME).pkg
endif
	@echo [FIL] $(NAME).pkg
	@pkgutil --payload-files $(DISTDIR)/$(NAME).pkg
ifneq ($(PRODUCTSIGNCN),)
	@echo [SIG] $(NAME).pkg
	@productsign \
	  --sign "$(PRODUCTSIGNCN)" \
	  --keychain $(KEYCHAIN) \
	  --timestamp \
	  $(DISTDIR)/$(NAME).pkg \
	  $(DISTDIR)/$(NAME)-sig.pkg
	@echo [VFY] $(NAME)-sig.pkg
	@pkgutil --check-signature $(DISTDIR)/$(NAME)-sig.pkg
ifneq ($(APPLE_ID),)
ifeq ($(APPLE_DIST_STORE),true)
	@xcrun altool --validate-app \
	  -f $(DISTDIR)/$(NAME)-sig.pkg \
	  -t macOS \
	  -u $(APPLE_ID) \
	  -p $(APPLE_APPSPEC_PASS)
else
	@echo [NTT] $(NAME)-sig.pkg
	@xcrun notarytool submit $(DISTDIR)/$(NAME)-sig.pkg \
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
VERSIONMAJOR = $(shell sed -n 's/^\#define $(VERSIONMACROMAJOR) //p' $(VERSIONFILE))
VERSIONMINOR = $(shell sed -n 's/^\#define $(VERSIONMACROMINOR) //p' $(VERSIONFILE))
VERSIONPATCH = $(shell sed -n 's/^\#define $(VERSIONMACROPATCH) //p' $(VERSIONFILE))
VERSION3     = $(VERSIONMAJOR).$(VERSIONMINOR).$(VERSIONPATCH)
VERSION4     = $(VERSIONMAJOR).$(VERSIONMINOR).$(VERSIONPATCH).0
dist-prep:
	echo [VER] $(VERSION3)
dist-%: dist-prep
	echo [DST] $(NAME)-$*
	$(eval DISTDEBARCH:=$(shell \
	  case $* in \
	    (x64)   echo amd64;; \
		(x86)   echo i386;; \
		(arm64) echo arm64;; \
		(arm)   echo armhf;; \
	  esac))
	$(eval DEBFILE=$(NAME)-$(VERSION3)-1-ubuntu-$(LSBREL)-$(DISTDEBARCH).deb)
	echo [DEB] $(DEBFILE)
	mkdir -p $(DISTDIR)/$(NAME)-$*/DEBIAN
	mkdir -p $(DISTDIR)/$(NAME)-$*/usr/bin
	mkdir -p $(DISTDIR)/$(NAME)-$*/usr/lib
	mkdir -p $(DISTDIR)/$(NAME)-$*/usr/share/applications
	mkdir -p $(DISTDIR)/$(NAME)-$*/usr/share/pixmaps	
	cp $(DISTDIR)/$(NAME).control $(DISTDIR)/$(NAME)-$*/DEBIAN/control
	sed -i 's/{VERSION}/${VERSION3}/g' $(DISTDIR)/$(NAME)-$*/DEBIAN/control
	sed -i 's/{ARCH}/${DEBARCH}/g' $(DISTDIR)/$(NAME)-$*/DEBIAN/control
	cp $(SRCDIR)/Resources/AppIcon-neuromore-256x256.png $(DISTDIR)/$(NAME)-$*/usr/share/pixmaps/$(NAME).png
	cp $(DISTDIR)/$(NAME).desktop $(DISTDIR)/$(NAME)-$*/usr/share/applications/$(NAME).desktop
	cp ./bin/linux-$*/$(NAME)$(EXTBIN) $(DISTDIR)/$(NAME)-$*/usr/bin/$(NAME)$(EXTBIN)
	@chmod +x $(DISTDIR)/$(NAME)-$*/usr/bin/$(NAME)$(EXTBIN)
	-cp ./../../deps/prebuilt/linux/$*/*.so $(DISTDIR)/$(NAME)-$*/usr/lib/
	dpkg-deb --build $(DISTDIR)/$(NAME)-$* $(DISTDIR)/$(DEBFILE) > /dev/null 2>&1
		
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
