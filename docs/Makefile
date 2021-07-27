# Minimal makefile for Sphinx documentation
#

# You can set these variables from the command line.
SPHINXOPTS    = 
SPHINXBUILD   = python -msphinx
SPHINXPROJ    = CPAJ2K
SPHINXAPIDOC  = sphinx-apidoc
SOURCEDIR     = source
BUILDDIR      = build
PKG_NAME = cpaj2k
PY_SRC_PATH     = ../$(PKG_NAME)

# Put it first so that "make" without argument is like "make help".
help:
	@$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

clean:
	rm -rf $(BUILDDIR)
	rm -rf $(SOURCEDIR)/$(PKG_NAME)

.PHONY: help Makefile

html: clean
	-$(SPHINXAPIDOC) -f -o $(SOURCEDIR)/$(PKG_NAME) $(PY_SRC_PATH)
	@$(SPHINXBUILD) -M $@ "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINXOPTS).
%: Makefile
	@$(SPHINXBUILD) -M "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)
	
