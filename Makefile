target: 
	@$(MAKE) -C src install --no-print-directory

uninstall:
	@$(MAKE) -C src uninstall --no-print-directory

clean:
	@$(MAKE) -C src clean --no-print-directory

clobber:
	@$(MAKE) -C src clobber --no-print-directory
