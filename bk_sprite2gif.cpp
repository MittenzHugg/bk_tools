#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "bk_sprite.hpp"
#include "gif.hpp"

int main(int argc, char** argv){
	bool loop = true;
	bool optimize = false;
<<<<<<< HEAD
	bool explode = false;
=======
>>>>>>> origin/master
	float  framerate = 30.0f;

	if(argc < 3){
        std::cout << "Incorrect syntex. bk_assets_build [options] <path/to/sprite.bin> <path/to/output/image.bmp>" <<std::endl;
		std::cout << "Options:" << std::endl;
		std::cout << "-n | --no-loop : non-looping gif" <<std::endl;
		std::cout << "(-f | --framerate) <float> : set framerate in fps. (default 30 fps)" <<std::endl;
<<<<<<< HEAD
		std::cout << "-e : explode parts to frames" << std::endl;
=======
>>>>>>> origin/master
		//std::cout << "-o --optimize : optimize gif size" << std::endl;
        return 1;
    }

	//parse arguments
	for(int i = 1; i < argc - 2; i++){
		std::string argi= argv[i];
		if(argi == "-n" || argi == "--no-loop") //no loop
			loop = false;
		//else if(argi == "-o") //optimize
			//optimize = true;
		else if( argi == "-f" || argi =="--framerate"){ //set framerate
			argi= argv[++i];
			try {
				std::size_t pos;
				framerate = std::stof(argi, &pos);
				if (pos < argi.size()) {
					std::cerr << "Trailing characters after number: " << argi << '\n';
				}
			} catch (std::invalid_argument const &ex) {
				std::cerr << "Invalid number: " << argi << '\n';
			} catch (std::out_of_range const &ex) {
				std::cerr << "Number out of range: " << argi << '\n';
			}
		}
<<<<<<< HEAD
		else if(argi == "-e"){
			explode = true;
		}
=======
>>>>>>> origin/master
			
	}
    
    std::string bin_p = argv[argc-2];
    std::string out_p = argv[argc-1];

    std::ifstream bin_f(bin_p, std::ios::in | std::ios::binary | std::ios::ate);
	if(!bin_f)
		return 1;

	size_t bin_size = bin_f.tellg();
	//TODO: make sure size is appropriate

	uint8_t * _buffer = (uint8_t *)malloc(bin_size);
	if(_buffer == nullptr){
		return 1;
	}

    bin_f.seekg(0);
	bin_f.read((char*)_buffer, bin_size); //copy ROM to RAM
	bin_f.close();

	n64_span buffer = n64_span(_buffer, bin_size);

    bk_sprite sprite(buffer);

	if(sprite._format == CI4 || sprite._format == CI8){ 
		gif giffy = sprite.toGIF(explode);
		giffy.set_infinite_loop(loop);
		giffy.set_delay((uint16_t) (100.0f/framerate));
		giffy.write(out_p);
	} else if (sprite._format == RGBA16 || sprite._format == RGBA32){
		std::cout << "Format not gif compatible. Exporting to APNG" << std::endl;
		apng apngy = sprite.toAPNG(explode);
		if(!loop)
			apngy.set_loop_count(1);
		apngy.set_delay((uint16_t) (100.0f/framerate));
		apngy.write(out_p);
	}else{
		std::cout << "Don't know how to handle format" << std::endl;
	}

	return 0;
}