# Note: The default build target 'all' does not use SIMD acceleration

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

# To enable acceleration, pass the appropriate flags as EXTRA_COMPILER_FLAGS.
# For SSE-128, pass '-msse -msse2 -msse3 -msse4.1' as EXTRA_COMPILER_FLAGS
# For AVX-256, pass '-mavx -mavx2' as EXTRA_COMPILER_FLAGS
# For BMI2, pass '-mbmi -mbmi2' as EXTRA_COMPILER_FLAGS (used only in VSEQ)
# For AVX-512, pass '-mavx512f -mavx512vl -mavx512bw' as EXTRA_COMPILER_FLAGS


.PHONY: sse128
sse128:
	$(MAKE) EXTRA_COMPILER_FLAGS="-msse -msse2 -msse3 -msse4.1" all
	
.PHONY: avx256
avx256:
	$(MAKE) EXTRA_COMPILER_FLAGS="-mavx -mavx2" all

.PHONY: avx512
avx512:
	$(MAKE) EXTRA_COMPILER_FLAGS="-mavx512f -mavx512vl -mavx512bw" all

.PHONY: arm_neon128
arm_neon128:
	$(MAKE) EXTRA_COMPILER_FLAGS="-mfpu=neon" all 

.PHONY: ibm_altivec128
ibm_altivec128:
	$(MAKE) EXTRA_COMPILER_FLAGS="-maltivec=le -mpowerpc64le" all

.PHONY: simd_all
simd_all:
	$(MAKE) EXTRA_COMPILER_FLAGS="-msse -msse2 -msse3 -msse4.1 -mavx -mavx2 -mbmi -mbmi2" all
	
.PHONY: simd512_all
simd512_all:
	$(MAKE) EXTRA_COMPILER_FLAGS="-msse -msse2 -msse3 -msse4.1 -mavx -mavx2 -mbmi -mbmi2 -mavx512f -mavx512vl -mavx512bw" all
	
.PHONY: clean
clean:
	cd $(DEDUP_BUILD_PATH) && make $@
	cd $(MEASURE_DEDUP_BUILD_PATH) && make $@
	cd $(MEASURE_LOW_ENTROPY_BUILD_PATH) && make $@
	
	$(RM) *.exe
