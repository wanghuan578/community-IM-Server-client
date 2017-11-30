SUBDIRS = socketLite serveBiz main

define make_subdir
	@for subdir in $(SUBDIRS) ; do \
		(cd $$subdir && make $1) \
		done;
endef

all:
	$(call make_subdir, all)
 
install:
	$(call make_subdir, install)
 
clean:
	$(call make_subdir, clean) 
