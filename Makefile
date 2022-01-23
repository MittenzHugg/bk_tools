ifeq ($(OS),Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
    CC = x86_64-w64-mingw32-gcc
else
    CC = gcc
endif
CXX = g++-10
CXXFLAGS = -std=c++2a -fconcepts -Wno-narrowing

SRCS = bk_asm.cpp bk_asset.cpp file_helper.cpp bk_sprite.cpp bk_textures.cpp gif.cpp apng.cpp src/bk/bk_dialog.cpp

# bk_dialog2yaml: SRCS = src/bk/bk_dialog.cpp src/bk_convert.cpp src/bk_dialog2yaml.cpp
D2Y_SRCS = src/bk/bk_dialog.cpp src/bk_convert.cpp

D2Y_OBJS = $(D2Y_SRCS:.cpp=.o)

# bk_yaml2dialog: SRCS = src/bk/bk_dialog.cpp src/bk_convert.cpp src/bk_yaml2dialog.cpp

OBJS = $(SRCS:.cpp=.o)

YAML_DIR := yaml-cpp-yaml-cpp-0.6.0

LIB_DIR = lib
LIBS = -ldeflate
INCLUDE_DIR = include

bk_extract bk_splat_yaml: LIBS = -lcrypto -ldeflate -lyaml-cpp

bk_inflate_code: LIBS = -lcrypto -ldeflate

bk_sprite2gif: LIBS = -ldeflate

bk_dialog2yaml: LIBS = -lyaml-cpp -ldeflate

bk_yaml2dialog: LIBS = -lyaml-cpp -ldeflate

TARGETS = bk_extract bk_assets_build bk_deflate_code bk_inflate_code bk_sprite2gif bk_dialog2yaml bk_yaml2dialog
DEPEND = $(LIB_DIR)/libdeflate.a gzip-1.2.4/gzip $(LIB_DIR)/libyaml-cpp.a
SUBMODULES = libdeflate yaml-cpp gzip-1.2.4

default: all
all: $(TARGETS) bk_splat_yaml

$(TARGETS): $(LIB_DIR)/libdeflate.a gzip-1.2.4/gzip $(OBJS)
	$(CXX) -o $@ $@.cpp $(OBJS) $(CXXFLAGS) -L$(LIB_DIR) $(LIBS) -I$(INCLUDE_DIR) -I.

bk_splat_yaml: $(DEPEND) $(OBJS) $(LIB_DIR)/libyaml-cpp.a
	$(CXX) -o $@ $@.cpp $(OBJS) $(CXXFLAGS) -L$(LIB_DIR) $(LIBS) -I$(YAML_DIR)/include -I$(INCLUDE_DIR)

bk_sprite2gif: $(filter-out src/bk_convert.o ,$(OBJS))
	$(CXX) -o $@ $@.cpp $(OBJS) $(CXXFLAGS) -L$(LIB_DIR) $(LIBS) -I$(INCLUDE_DIR) -I.

bk_font_tool: src/bk/bk_font_tool.cpp $(OBJS) src/bk/bk_dialog.o
	$(CXX) -o $@ $^ $(CXXFLAGS) -L$(LIB_DIR) $(LIBS) -I$(INCLUDE_DIR) -I.

bk_dialog2yaml: src/bk_dialog2yaml.cpp $(D2Y_OBJS) $(LIB_DIR)/libyaml-cpp.a
	$(CXX) -o $@ $< $(D2Y_OBJS) $(CXXFLAGS) -L$(LIB_DIR) $(LIBS) -I$(INCLUDE_DIR) -I. -I$(YAML_DIR)/include

bk_yaml2dialog: src/bk_yaml2dialog.cpp $(D2Y_OBJS) $(LIB_DIR)/libyaml-cpp.a
	$(CXX) -o $@ $< $(D2Y_OBJS) $(CXXFLAGS) -L$(LIB_DIR) $(LIBS) -I$(INCLUDE_DIR) -I. -I$(YAML_DIR)/include 

clean:
	rm -f *.o
	rm -f $(TARGETS)

very_clean: $(foreach mod, $(SUBMODULES), $(mod)_clean) clean
	cd $(LIB_DIR) && rm -f *.a

$(LIB_DIR)/libdeflate.a: $(LIB_DIR) libdeflate/libdeflate.a
	cp -f libdeflate/libdeflate.a $(LIB_DIR)

libdeflate/libdeflate.a:
	cd libdeflate && make libdeflate.a CC=$(CC)

gzip-1.2.4/gzip: gzip-1.2.4/Makefile
	cd gzip-1.2.4 && make gzip

gzip-1.2.4/Makefile:
	cd gzip-1.2.4 && ./configure

$(LIB_DIR)/libyaml-cpp.a: $(LIB_DIR) $(YAML_DIR)/build/libyaml-cppa.a
	cp -f $(YAML_DIR)/build/libyaml-cpp.a $(LIB_DIR)

$(YAML_DIR)/build/libyaml-cppa.a: $(YAML_DIR)/build $(YAML_DIR)/build/Makefile
	cd $(YAML_DIR)/build && $(MAKE) CXX=$(CXX); CXXFLAGS=-std=c++2a

$(YAML_DIR)/build/Makefile:
	cd $(YAML_DIR)/build && cmake ..

$(YAML_DIR)/build:
	mkdir -p $@

$(LIB_DIR):
	mkdir -p $(LIB_DIR)

%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c $<  -o $@ -I$(INCLUDE_DIR) -I. -I$(YAML_DIR)/include 

%_clean:
	cd $* && make clean

yaml-cpp_clean:
	cd yaml-cpp && rm -rf build
