# File Operations on Host OS

# Delete Files in Folder by Pattern
define deletefiles
	powershell "Remove-Item $(1)/$(2) -Force"
endef

# Copy Files between Folders by Pattern
define copyfiles
	powershell "Copy-Item $(1) -Destination $(2) -Force"
endef

# Copy Files recursively
define copyfilesrecursive
	powershell "Copy-Item $(1) -Destination $(2) -Force -Recurse"
endef

# Recursively remove folder
define rmdir
	powershell "Remove-Item $(1) -Force -Recurse"
endef

# Create folder and all sub folders
define mkdir
	powershell "New-Item $(1) -ItemType Directory -Force"
endef

# Move Folder or File
define move
	powershell Move-Item -Force -Path $(1) -Destination $(2)
endef

# Sleep n seconds
define sleep
	powershell Start-Sleep -Seconds $(1)
endef

# Replace string occurrences in file
define replace
	powershell -Command "(gc $(subst /,\,$(1))) -replace '$(subst /,\,$(2))', '$(subst /,\,$(3))' | Out-File -encoding UTF8 $(subst /,\,$(4))"
endef

# Create APPX unencrypted
define makepkg
	MakeAppx.exe build /o /h SHA256 /f $(subst /,\,$(1)) /op $(subst /,\,$(2))
endef

# Create APPXBUNDLE
define makebundle
	MakeAppx.exe bundle /o /d $(subst /,\,$(1)) /p $(subst /,\,$(2))
endef

# Sign File with pfx without password
define sign
	SignTool.exe sign /a /fd SHA256 /td SHA256 /tr http://timestamp.digicert.com /f $(subst /,\,$(2)) $(subst /,\,$(1))
endef

# Sign File with pfx with password
define signp
	SignTool.exe sign /a /fd SHA256 /td SHA256 /tr http://timestamp.digicert.com /f $(subst /,\,$(2)) /p $(3) $(subst /,\,$(1))
endef

# Create .pri resources
define makepri
	MakePri.exe new /v /o /cf $(subst /,\,$(1)) /pr $(subst /,\,$(2)) /of $(subst /,\,$(3)) /IndexName $(4)
endef

# Create ZIP
define makezip
	powershell Compress-Archive -Force $(1) $(2)
endef
