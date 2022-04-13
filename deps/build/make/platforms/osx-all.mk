# File Operations on Host OS

# Delete Files in Folder by Pattern
define deletefiles
	find $(1) -type f -name '$(2)' -delete
endef

# Copy Files between Folders by Pattern
define copyfiles
	cp $(1) $(2) 2>/dev/null || :
endef
