# File Operations on Host OS

# Delete Files in Folder by Pattern
define deletefiles
	del /s /q $(subst /,\,$(1))\$(2) >nul 2>&1
endef

# Copy Files between Folders by Pattern
define copyfiles
	copy $(subst /,\,$(1)) $(subst /,\,$(2)) >nul 2>&1
endef
