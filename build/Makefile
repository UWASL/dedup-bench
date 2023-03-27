DEDUP_BUILD_PATH = ../dedup/build
MEASURE_DEDUP_BUILD_PATH = ../measure-dedup/build

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

.PHONY: clean
clean:
	cd $(DEDUP_BUILD_PATH) && make $@
	cd $(MEASURE_DEDUP_BUILD_PATH) && make $@
	$(RM) *.exe
