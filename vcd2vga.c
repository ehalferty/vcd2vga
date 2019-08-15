#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lodepng.h"

#define FILE_NAME "dump.vcd"
#define PIXEL_CLOCK_NAME "clk_40mhz"
#define HSYNC_NAME "HS"
#define VSYNC_NAME "VS"
#define RED_NAME "R"
#define GREEN_NAME "G"
#define BLUE_NAME "B"
#define HORIZ_DOTS 800
#define VERT_DOTS 600
#define POLARITY_POSITIVE 1
#define POLARITY_NEGATIVE 0
#define HSYNC_POLARITY POLARITY_POSITIVE
#define VSYNC_POLARITY POLARITY_POSITIVE
#define BACK_PORCH_CYCLES 88
#define VERTICAL_BACK_PORCH_CYCLES 23

char pixel_clock_identifier = 0;
char hsync_identifier = 0;
char vsync_identifier = 0;
char red_identifier = 0;
char green_identifier = 0;
char blue_identifier = 0;
unsigned int all_identifiers_found = 0;

int main(int argc, char **argv) {
  size_t len = 0;
  int frame_index = 0, scanline_index = 0, pixel_index = 0;
  char *line = NULL, frame_filename_tmp[256];
  char red = 0, green = 0, blue = 0, tmp = 0, identifier = 0;
  int tmp_index = 0, image_index = 0;
  unsigned char image[HORIZ_DOTS * VERT_DOTS * 4];
  FILE *f = fopen(FILE_NAME, "r");
  while ((getline(&line, &len, f)) != -1) {
    if (all_identifiers_found != (1 | 2 | 4 | 8 | 16 | 32)) {
      if (strncmp(line, "$var", strlen("$var")) == 0) {
        identifier = line[5] == 'r' ? line[11] : line[12];
        if (strncmp(line + (line[5] == 'r' ? 13 : 14), PIXEL_CLOCK_NAME, strlen(PIXEL_CLOCK_NAME)) == 0) {
          pixel_clock_identifier = identifier;
          all_identifiers_found = all_identifiers_found | 1;
        } else if (strncmp(line + (line[5] == 'r' ? 13 : 14), HSYNC_NAME, strlen(HSYNC_NAME)) == 0) {
          hsync_identifier = identifier;
          all_identifiers_found = all_identifiers_found | 2;
        } else if (strncmp(line + (line[5] == 'r' ? 13 : 14), VSYNC_NAME, strlen(VSYNC_NAME)) == 0) {
          vsync_identifier = identifier;
          all_identifiers_found = all_identifiers_found | 4;
        } else if (strncmp(line + (line[5] == 'r' ? 13 : 14), RED_NAME, strlen(RED_NAME)) == 0) {
          red_identifier = identifier;
          all_identifiers_found = all_identifiers_found | 8;
        } else if (strncmp(line + (line[5] == 'r' ? 13 : 14), GREEN_NAME, strlen(GREEN_NAME)) == 0) {
          green_identifier = identifier;
          all_identifiers_found = all_identifiers_found | 16;
        } else if (strncmp(line + (line[5] == 'r' ? 13 : 14), BLUE_NAME, strlen(BLUE_NAME)) == 0) {
          blue_identifier = identifier;
          all_identifiers_found = all_identifiers_found | 32;
        }
      }
    } else {
      if (line[0] == (VSYNC_POLARITY == POLARITY_POSITIVE ? '0' : '1') && line[1] == vsync_identifier) {
        sprintf(frame_filename_tmp, "frames/frame%d.png", frame_index);
        lodepng_encode32_file(frame_filename_tmp, image, HORIZ_DOTS, VERT_DOTS);
        for (tmp_index = 0; tmp_index < HORIZ_DOTS * VERT_DOTS * 4; tmp_index++) {
          image[tmp_index] = 0;
        }
        frame_index++;
        scanline_index = 0;
        pixel_index = 0;
        image_index = 0;
      } else if (line[0] == (HSYNC_POLARITY == POLARITY_POSITIVE ? '0' : '1') && line[1] == hsync_identifier) {
        scanline_index++;
        pixel_index = 0;
      } else if (line[0] == '1' && line[1] == pixel_clock_identifier) {
        if (scanline_index >= VERTICAL_BACK_PORCH_CYCLES &&
            scanline_index < VERTICAL_BACK_PORCH_CYCLES + VERT_DOTS &&
            pixel_index >= BACK_PORCH_CYCLES &&
            pixel_index < BACK_PORCH_CYCLES + HORIZ_DOTS) {
          image[image_index] = red * 16;
          image[image_index + 1] = green * 16;
          image[image_index + 2] = blue * 16;
          image[image_index + 3] = 255;
          image_index += 4;
        }
        pixel_index++;
      } else if (line[0] == 'b') {
        tmp = 0;
        for (tmp_index = 0; line[tmp_index] != ' '; tmp_index++) {
          tmp = (tmp << 1) | (line[tmp_index] == '1' ? 1 : 0);
        }
        if (line[tmp_index + 1] == red_identifier) {
          red = tmp;
        } else if (line[tmp_index + 1] == green_identifier) {
          green = tmp;
        } else if (line[tmp_index + 1] == blue_identifier) {
          blue = tmp;
        }
      }
    }
  }
  free(line);
  fclose(f);
  exit(EXIT_SUCCESS);
}
