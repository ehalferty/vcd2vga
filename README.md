# vcd2vga
Converts a VCD file containing VGA data to PNG frames

### Requirements:

* [LodePNG](https://lodev.org/lodepng/) (lodepng.c and lodepng.h)

### TODO:

* Command-line arguments to set: input file, output dir, output file pattern, names of the VGA signals within the VCD file
* Auto-detect the resolution/refresh-rate (should be easy to fuzzy match against a list of pixel clock frequencies and hsync/vsync duty cycles)
* Option to output a video file
* Read the entire VCD file into memory, which should speed up the program (or maybe even better: memory-mapped files?)
