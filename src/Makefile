# FIXME: Code is x86 specific?

DEBUG := -g -Wall
#~ DEBUG := -g -Wall -fdump-translation-unit -fdump-tree-all

# Need wxwidgets 2.9
#~ WXCONFIG := wx-config
#~ WXCONFIG := /usr/lib/wx/config/gtk2-unicode-2.9
WXCONFIG := /usr/lib/wx/config/gtk2-unicode-2.9-unofficial

#~ INCLUDES := $(shell $(WXCONFIG) --cppflags --cflags) $(shell ls external_libs -1 | grep -v missed | sed 's|^|-I./external_libs/|')
INCLUDES := -I. $(shell $(WXCONFIG) --cppflags --cflags) -I./external_libs/dng_sdk -I./external_libs/MD5 -I./external_libs/xmp_sdk/include -I./external_libs/xmp_sdk/common $(shell pkg-config --cflags libexif)
DEFINES := -DWX_PRECOMP -DwxUSE_UNICODE -DqWinOS=0 -DqMacOS=0 -DqLinOS=1 -DqDNGLittleEndian=1 -DUNIX_ENV=1 -DDISABLE_CALIBR
CPPFLAGS := -std=c++0x $(DEBUG) $(DEFINES) $(INCLUDES)
LDFLAGS := $(shell $(WXCONFIG) --libs) $(shell pkg-config --libs libexif) -lexpat
BINDIR := bin/lin

SOURCES = $(wildcard *.cpp gui/*.cpp lib/*.cpp)
TRANS_SOURCES = $(wildcard langs/*.po)

all: dng4ps2

# add dependency generation
CPPFLAGS += -MMD
-include $(SOURCES:.cpp=.d)

#~ dng4ps2: langs deps $(addsuffix .o,$(basename $(wildcard *.cpp gui/*.cpp lib/*.cpp ))) $(wildcard *.h gui/*.h lib/*.h) $(wildcard *.hpp gui/*.hpp lib/*.hpp)
dng4ps2: langs deps $(SOURCES:cpp=o)
	@#~ $(CC) -o $@ $(wildcard *.o) $(LDFLAGS)
	@#~ echo $(SOURCES:cpp=o)
	$(CC) -o $@ $(shell find . -type f | grep '\.o$$') $(LDFLAGS)
	ln -f $@ $(BINDIR)

#~ deps: dng_sdk xmp_sdk MD5
deps: xmp_sdk $(addsuffix .o,$(basename $(wildcard external_libs/dng_sdk/*.cpp external_libs/MD5/*.cpp )))

xmp_sdk: $(addsuffix .o,$(basename $(wildcard external_libs/xmp_sdk/*/*.cpp)))

#~ langs: $(patsubst langs/%.po, bin/lin/langs/%/dng4ps-2.mo, $(TRANS_SOURCES))
langs: $(wildcard langs/*.po)
#~ langs:
	@#~ echo $(patsubst langs/%.po, bin/lin/langs/%/dng4ps-2.mo, $(wildcard langs/*.po))
	bash ./build_langs.sh $(BINDIR)

#~ %.mo: 

#~ %.o: %.cpp
	#~ echo $(<:cpp=h): $(wildcard $(<:cpp=hpp))
	#~ $(CC) $(CPPFLAGS) $(DEFINES) $(DEBUG) $(INCLUDES) -M -MM -MF .deps -c $<
	#~ $(CC) $(CPPFLAGS) $(DEFINES) $(DEBUG) $(INCLUDES) -o $@ -c $<
	#~ @#~ $(CC) $(CPPFLAGS) $(DEFINES) $(DEBUG) $(INCLUDES) -E -c $< > $<.E

clean:
	@#~ find . -type f | grep '\.o$$' | xargs rm -f
	rm -f dng4ps2 $(SOURCES:.cpp=.d) $(SOURCES:.cpp=.o)

.PHONY: langs deps xmp_sdk all clean
