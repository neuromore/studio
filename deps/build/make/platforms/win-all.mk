# File Operations on Host OS

# Delete Files in Folder by Pattern
define deletefiles
	del /s /q $(subst /,\,$(1))\$(2) >nul 2>&1
endef

# Copy Files between Folders by Pattern
define copyfiles
	cmd.exe /C "copy /Y $(subst /,\,$(1)) $(subst /,\,$(2)) >nul 2>&1"
endef

# Recursively remove folder
define rmdir	
	cmd.exe /C "if exist $(subst /,\,$(1)) rd /s /q $(subst /,\,$(1)) >nul 2>&1"
endef

# Create folder and all sub folders
define mkdir
	cmd.exe /C "if not exist $(subst /,\,$(1)) mkdir $(subst /,\,$(1)) >nul 2>&1"
endef

# Replace string occurrences in file
define replace
	powershell -Command "(gc $(subst /,\,$(1))) -replace '$(subst /,\,$(2))', '$(subst /,\,$(3))' | Out-File -encoding UTF8 $(subst /,\,$(4))"
endef

# Create APPX unencrypted
define makepkg
	cmd.exe /C "MakeAppx.exe build /o /h SHA256 /f $(subst /,\,$(1)) /op $(subst /,\,$(2)) >nul 2>&1"
endef

# Create APPXBUNDLE
define makebundle
	cmd.exe /C "MakeAppx.exe bundle /o /d $(subst /,\,$(1)) /p $(subst /,\,$(2)) >nul 2>&1"
endef

# Sign File with pfx without password
define sign
	cmd.exe /C "SignTool.exe sign /a /fd SHA256 /td SHA256 /tr http://timestamp.digicert.com /f $(subst /,\,$(2)) $(subst /,\,$(1)) >nul 2>&1"
endef

# Sign File with pfx with password
define signp
	cmd.exe /C "SignTool.exe sign /a /fd SHA256 /td SHA256 /tr http://timestamp.digicert.com /f $(subst /,\,$(2)) /p $(3) $(subst /,\,$(1)) >nul 2>&1"
endef
