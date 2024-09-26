export ${EXTRA_COMPILER_FLAGS}
DEDUP_BUILD_PATH = ../dedup/build
MEASURE_DEDUP_BUILD_PATH = ../measure-dedup/build
MEASURE_LOW_ENTROPY_BUILD_PATH = ../measure-low-entropy/build
SUPPORTING_TOOLS_PATH = ../supporting_tools

RM = rm -f

ifeq ($(DEBUG), 1)
# TODO: figure out how to pass this to the invoked Makefile
endif


.PHONY: default
default: all

.PHONY: all
all:
	cd $(DEDUP_BUILD_PATH) && make $@
	cd $(MEASURE_DEDUP_BUILD_PATH) && make $@
	cd $(MEASURE_LOW_ENTROPY_BUILD_PATH) && make $@
	cp $(SUPPORTING_TOOLS_PATH)/gen_config_files.sh .
	cp $(SUPPORTING_TOOLS_PATH)/wrapper.sh .
	cp $(SUPPORTING_TOOLS_PATH)/dedup_script.sh .
	cp $(SUPPORTING_TOOLS_PATH)/archive_extract.sh .
	cp $(SUPPORTING_TOOLS_PATH)/archive_ctl_path.cfg .



.PHONY: clean
clean:
	cd $(DEDUP_BUILD_PATH) && make $@
	cd $(MEASURE_DEDUP_BUILD_PATH) && make $@
	cd $(MEASURE_LOW_ENTROPY_BUILD_PATH) && make $@
	
	$(RM) *.exe
