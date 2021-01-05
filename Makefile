ifeq ($(OS),Windows_NT)     # is Windows_NT on XP, 2000, 7, Vista, 10...
    CC = x86_64-w64-mingw32-gcc
else
    CC = gcc
endif
CXX = g++
CXXFLAGS = -std=c++2a

SRCS = bk_asm.cpp bk_asset.cpp file_helper.cpp
OBJS = $(SRCS:.cpp=.o)

LIB_DIR = lib
LIBS = -lcrypto -ldeflate 

default: all
all: bk_extract bk_assets_build

bk_extract: $(LIB_DIR)/libdeflate.a gzip_1_2_4 $(OBJS)
	g++ -o bk_extract bk_extract.cpp $(OBJS) $(CXXFLAGS) -L$(LIB_DIR) $(LIBS)

bk_assets_build: $(LIB_DIR)/libdeflate.a gzip_1_2_4 $(OBJS)
	g++ -o bk_assets_build bk_assets_build.cpp $(OBJS) $(CXXFLAGS) -L$(LIB_DIR) -ldeflate

clean:
	rm -f *.o
	rm -f bk_extract bk_assets_build
	cd $(LIB_DIR) && rm -f *.a

very_clean: libdeflate_clean clean
	rm -f gzip_1_2_4

gzip_1_2_4:
	cd gzip-1.2.4 && make gzip
	cp -f gzip-1.2.4/gzip ./gzip_1_2_4

$(LIB_DIR)/libdeflate.a: $(LIB_DIR)
	cd libdeflate && make libdeflate.a CC=$(CC)
	cp -f libdeflate/libdeflate.a $(LIB_DIR)

$(LIB_DIR):
	mkdir -p $(LIB_DIR)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<  -o $@

libdeflate_clean:
	cd libdeflate && make clean
