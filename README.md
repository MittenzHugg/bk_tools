# bk_tools
tools for extracting working with Banjo-Kazooie

**bk_extract:**  
extracts and decompresses all bins for all versions of Banjo-Kazooie  
*-r </path/to/rom._64 | /path/to/rom/dir/>* : specifies path to rom files  
*-p </path/to/output/dir>* : specifies path to output directory.

**bk_assets_build:**  
creates assets.bin from uncompressed assets  
*</path/to/output/bin/dir> </path/to/uncompressed/assets/dir>* : 

**bk_sprite2gif:**  
creates gif from bk_sprites
*[options] </path/to/sprite/bin> </path/to/gif_file>* : 
* -n | --no-loop* : create nonlooping GIF  
* (-f | --framerate) <float>* : set framerate in fps (default = 30)  
