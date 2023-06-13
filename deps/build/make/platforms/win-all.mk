# File Operations on Host OS

# Delete Files in Folder by Pattern
define deletefiles
	cmd.exe /C "del /s /q $(subst /,\,$(1))\$(2) & exit 0"
endef

# Copy Files between Folders by Pattern
define copyfiles
	cmd.exe /C "copy /Y $(subst /,\,$(1)) $(subst /,\,$(2)) & exit 0"
endef

# Copy Files recursively
define copyfilesrecursive
	cmd.exe /C "xcopy /Y /E /H $(subst /,\,$(1)) $(subst /,\,$(2)) & exit 0"
endef

# Recursively remove folder
define rmdir	
	cmd.exe /C "if exist $(subst /,\,$(1)) rd /s /q $(subst /,\,$(1))"
endef

# Create folder and all sub folders
define mkdir
	-mkdir $(subst /,\,$(1))
endef

# Replace string occurrences in file
define replace
	powershell -Command "(gc $(subst /,\,$(1))) -replace '$(subst /,\,$(2))', '$(subst /,\,$(3))' | Out-File -encoding UTF8 $(subst /,\,$(4))"
endef

# Create APPX unencrypted
define makepkg
	cmd.exe /C "MakeAppx.exe build /o /h SHA256 /f $(subst /,\,$(1)) /op $(subst /,\,$(2))"
endef

# Create APPXBUNDLE
define makebundle
	cmd.exe /C "MakeAppx.exe bundle /o /d $(subst /,\,$(1)) /p $(subst /,\,$(2)) >nul 2>&1"
endef

# Sign File with pfx without password
define sign
	cmd.exe /C "if exist $(subst /,\,$(1)) SignTool.exe sign /a /fd SHA256 /td SHA256 /tr http://timestamp.digicert.com /f $(subst /,\,$(2)) $(subst /,\,$(1)) >nul 2>&1"
endef

# Sign File with pfx with password
define signp
	cmd.exe /C "if exist $(subst /,\,$(1)) SignTool.exe sign /a /fd SHA256 /td SHA256 /tr http://timestamp.digicert.com /f $(subst /,\,$(2)) /p $(3) $(subst /,\,$(1))"
endef

# Create .pri resources
define makepri
	cmd.exe /C "MakePri.exe new /v /o /cf $(subst /,\,$(1)) /pr $(subst /,\,$(2)) /of $(subst /,\,$(3)) /IndexName $(4)"
endef

# Create ZIP
define makezip
	cmd.exe /C "powershell Compress-Archive -Force $(1) $(2)"
endef

# Move Folder or File
define move
	cmd.exe /C "powershell Move-Item -Force -Path $(1) -Destination $(2)"
endef

# Sleep n seconds
define sleep
	cmd.exe /C "powershell Start-Sleep -Seconds $(1)"
endef
