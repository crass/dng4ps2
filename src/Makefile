ARCH ?= 64

DEBUG += -g -Wall
#~ DEBUG += -D__WXDEBUG__
#~ DEBUG += -fdump-translation-unit -fdump-tree-all

# Need wxwidgets 2.9
WXCONFIG ?= wx-config

#~ INCLUDES := $(shell $(WXCONFIG) --cppflags --cflags) $(shell ls external_libs -1 | grep -v missed | sed 's|^|-I./external_libs/|')
#~ INCLUDES := -I. $(shell $(WXCONFIG) --cppflags --cflags) -I./external_libs/dng_sdk -I./external_libs/MD5 -I./external_libs/xmp_sdk/public/include -I./external_libs/xmp_sdk/include -I./external_libs/xmp_sdk/common $(shell pkg-config --cflags libexif)
INCLUDES := -I. $(shell $(WXCONFIG) --cppflags --cflags) -I./external_libs/dng_sdk -I./external_libs/MD5 $(shell find ./external_libs/xmp_sdk -type d | sed 's|^|-I|') $(shell pkg-config --cflags libexif)

# dng_sdk 4.1 will not compile without qDNGThreadSafe
WX_DEFINES := -DWX_PRECOMP -DwxUSE_UNICODE -DwxUSE_THREADS
DNG_SDK_DEFINES += -DqDNG$(ARCH)Bit -DqWinOS=0 -DqMacOS=0 -DqLinux=1 -DqDNGLittleEndian=1 -DqDNGThreadSafe=1
# Comment out for 32-bit builds
#~ DNG_SDK_DEFINES += -DqDNG64Bit
# Can't find where SXMPDocOps is declared in the xmp_sdk sources
DNG_SDK_DEFINES += -DqDNGXMPDocOps=0
XMP_SDK_DEFINES := -DUNIX_ENV=1
# Needed to have off_t defined as 64-bit
XMP_SDK_DEFINES += -D__USE_FILE_OFFSET64
# Default is the home grown lock, should we assume pthread here (hence *nix)?
#~ XMP_SDK_DEFINES += -DUsePThreadLock
DEFINES := $(WX_DEFINES) $(DNG_SDK_DEFINES) $(XMP_SDK_DEFINES) -DDISABLE_CALIBR

CPPFLAGS := -std=c++0x $(CPPFLAGS) $(DEBUG) $(DEFINES) $(INCLUDES)
LDFLAGS := $(LDFLAGS) $(shell $(WXCONFIG) --libs) $(shell pkg-config --libs libexif) -lexpat -lz -lm
LDFLAGS_DBG = --sysroot=/usr/lib/debug \$(shell $(WXCONFIG) --prefix=/usr/lib/debug/usr --libs) $(shell pkg-config --libs libexif) -lexpat -lz -lm
BINDIR := bin/lin

SOURCES = $(wildcard *.cpp gui/*.cpp lib/*.cpp)
XMP_SOURCES = $(shell find external_libs/xmp_sdk -type f -iname \*.cpp)
DNG_SOURCES = $(wildcard external_libs/dng_sdk/*.cpp)
OTHER_SOURCES = $(wildcard external_libs/MD5/*.cpp )
TRANS_SOURCES = $(wildcard langs/*.po)

all: dng4ps2

# add dependency generation
CPPFLAGS += -MMD
-include $(SOURCES:.cpp=.d)

#~ dng4ps2: langs deps $(addsuffix .o,$(basename $(wildcard *.cpp gui/*.cpp lib/*.cpp ))) $(wildcard *.h gui/*.h lib/*.h) $(wildcard *.hpp gui/*.hpp lib/*.hpp)
dng4ps2: langs deps $(SOURCES:cpp=o)
	@#~ $(CXX) -o $@ $(wildcard *.o) $(LDFLAGS)
	@#~ echo $(SOURCES:cpp=o)
	$(CXX) -o $@ $(SOURCES:.cpp=.o) $(DNG_SOURCES:.cpp=.o) \
	    $(XMP_SOURCES:.cpp=.o) $(OTHER_SOURCES:.cpp=.o) $(LDFLAGS)
	ln -f $@ $(BINDIR)

dng4ps2-dbg: langs deps $(SOURCES:cpp=o)
	@#~ $(CXX) -o $@ $(wildcard *.o) $(LDFLAGS)
	@#~ echo $(SOURCES:cpp=o)
	$(CXX) -o $@ $(SOURCES:.cpp=.o) $(DNG_SOURCES:.cpp=.o) \
	    $(XMP_SOURCES:.cpp=.o) $(OTHER_SOURCES:.cpp=.o) $(LDFLAGS_DBG)
	ln -f $@ $(BINDIR)

#~ deps: dng_sdk xmp_sdk MD5
deps: xmp_sdk $(addsuffix .o,$(basename $(DNG_SOURCES) $(OTHER_SOURCES)))

xmp_sdk: $(addsuffix .o,$(basename $(XMP_SOURCES)))

#~ langs: $(patsubst langs/%.po, bin/lin/langs/%/dng4ps-2.mo, $(TRANS_SOURCES))
langs: $(wildcard langs/*.po)
#~ langs:
	@#~ echo $(patsubst langs/%.po, bin/lin/langs/%/dng4ps-2.mo, $(wildcard langs/*.po))
	bash ./build_langs.sh $(BINDIR)/langs

#~ %.mo: 

#~ %.o: %.cpp
	#~ echo $(<:cpp=h): $(wildcard $(<:cpp=hpp))
	#~ $(CXX) $(CPPFLAGS) $(DEFINES) $(DEBUG) $(INCLUDES) -M -MM -MF .deps -c $<
	#~ $(CXX) $(CPPFLAGS) $(DEFINES) $(DEBUG) $(INCLUDES) -o $@ -c $<
	#~ @#~ $(CXX) $(CPPFLAGS) $(DEFINES) $(DEBUG) $(INCLUDES) -E -c $< > $<.E

install:
	mkdir -p $(DESTDIR)/usr/bin
	cp dng4ps2 $(DESTDIR)/usr/bin/
	for LANG in bin/lin/langs/*; do \
	  mkdir -p $(DESTDIR)/usr/share/locale/$$(basename $$LANG)/LC_MESSAGES; \
	  cp $$LANG/dng4ps-2.mo $(DESTDIR)/usr/share/locale/$$(basename $$LANG)/LC_MESSAGES/dng4ps-2.mo; \
	done

dng4ps2-clean:
	rm -f $(SOURCES:.cpp=.d) $(SOURCES:.cpp=.o)

clean:
	@#~ find . -type f | grep '\.o$$' | xargs rm -f
	rm -f dng4ps2 dng4ps2-dbg $(SOURCES:.cpp=.d) $(SOURCES:.cpp=.o) \
		$(DNG_SOURCES:.cpp=.d) $(DNG_SOURCES:.cpp=.o) \
		$(XMP_SOURCES:.cpp=.d) $(XMP_SOURCES:.cpp=.o) \
		$(OTHER_SOURCES:.cpp=.d) $(OTHER_SOURCES:.cpp=.o)

.PHONY: langs deps xmp_sdk all clean
