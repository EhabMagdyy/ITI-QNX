ARTIFACT = servo

# Build architecture
PLATFORM ?= aarch64le

# Build profile
BUILD_PROFILE ?= debug

CONFIG_NAME ?= $(PLATFORM)-$(BUILD_PROFILE)
OUTPUT_DIR = build/$(CONFIG_NAME)
TARGET = $(OUTPUT_DIR)/$(ARTIFACT)

# Compiler definitions

CC = qcc -Vgcc_nto$(PLATFORM)

CXX = q++ -Vgcc_nto$(PLATFORM)_cxx

LD = $(CC)

# Include paths
INCLUDES += -I../common/system/gpio
INCLUDES += -I../common/rpi_gpio/public

# Libraries
LIBS += -L../common/rpi_gpio/build/$(CONFIG_NAME)
LIBS += -m -lrpi_gpio

# Compiler flags
CCFLAGS_release += -O2
CCFLAGS_debug += -g -O0 -fno-builtin
CCFLAGS_coverage += -g -O0 -ftest-coverage -fprofile-arcs
LDFLAGS_coverage += -ftest-coverage -fprofile-arcs
CCFLAGS_profile += -g -O0 -finstrument-functions
LIBS_profile += -lprofilingS

# Generic flags
CCFLAGS_all += -Wall -fmessage-length=0
CCFLAGS_all += $(CCFLAGS_$(BUILD_PROFILE))
LDFLAGS_all += $(LDFLAGS_$(BUILD_PROFILE))
LIBS_all += $(LIBS_$(BUILD_PROFILE))
DEPS = -Wp,-MMD,$(@:%.o=%.d),-MT,$@

# Source files
rwildcard = $(wildcard $(addprefix $1/*.,$2)) \
            $(foreach d,$(wildcard $1/*),$(call rwildcard,$d,$2))
SRCS = $(call rwildcard, src, c)
OBJS = $(addprefix $(OUTPUT_DIR)/,$(addsuffix .o,$(basename $(SRCS))))

# Compile
$(OUTPUT_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -c $(DEPS) -o $@ \
	$(INCLUDES) \
	$(CCFLAGS_all) \
	$(CCFLAGS) \
	$<

# Link

$(TARGET): $(OBJS) librpi_gpio.a
	$(LD) -o $(TARGET) \
	$(LDFLAGS_all) \
	$(LDFLAGS) \
	$(OBJS) \
	$(LIBS_all) \
	$(LIBS)

# Build rpi gpio library

librpi_gpio.a:
	$(MAKE) -C ../common/rpi_gpio

# Default
all: $(TARGET)

clean:
	rm -fr $(OUTPUT_DIR)

rebuild: clean all	@mkdir -p $(dir $@)
	$(CC) -c $(DEPS) -o $@ \
	$(INCLUDES) \
	$(CCFLAGS_all) \
	$(CCFLAGS) \
	$<

# Link

$(TARGET): $(OBJS) librpi_gpio.a
	$(LD) -o $(TARGET) \
	$(LDFLAGS_all) \
	$(LDFLAGS) \
	$(OBJS) \
	$(LIBS_all) \
	$(LIBS)

# Dependency files
-include $(OBJS:%.o=%.d)
