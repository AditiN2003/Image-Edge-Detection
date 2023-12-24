#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#define KERNELSIZE 3
/* Do not modify write_pgm() or read_pgm() */
int write_pgm(char *file, void *image, uint32_t x, uint32_t y)
{
  FILE *o;

  if (!(o = fopen(file, "w"))) {
    perror(file);

    return -1;
  }

  fprintf(o, "P5\n%u %u\n255\n", x, y);

  /* Assume input data is correctly formatted. *
   * There's no way to handle it, otherwise.   */

  if (fwrite(image, 1, x * y, o) != (x * y)) {
    perror("fwrite");
    fclose(o);

    return -1;
  }

  fclose(o);

  return 0;
}

/* A better implementation of this function would read the image dimensions *
 * from the input and allocate the storage, setting x and y so that the     *
 * user can determine the size of the file at runtime.  In order to         *
 * minimize complication, I've written this version to require the user to  *
 * know the size of the image in advance.                                   */
int read_pgm(char *file, void *image, uint32_t x, uint32_t y)
{
  FILE *f;
  char s[80];
  unsigned i, j;

  if (!(f = fopen(file, "r"))) {
    perror(file);

    return -1;
  }

  if (!fgets(s, 80, f) || strncmp(s, "P5", 2)) {
    fprintf(stderr, "Expected P6\n");

    return -1;
  }

  /* Eat comments */
  do {
    fgets(s, 80, f);
  } while (s[0] == '#');

  if (sscanf(s, "%u %u", &i, &j) != 2 || i != x || j != y) {
    fprintf(stderr, "Expected x and y dimensions %u %u\n", x, y);
    fclose(f);

    return -1;
  }

  /* Eat comments */
  do {
    fgets(s, 80, f);
  } while (s[0] == '#');

  if (strncmp(s, "255", 3)) {
    fprintf(stderr, "Expected 255\n");
    fclose(f);

    return -1;
  }

  if (fread(image, 1, x * y, f) != x * y) {
    perror("fread");
    fclose(f);

    return -1;
  }

  fclose(f);

  return 0;
}

int filter( uint8_t  orignal[1024][1024], uint8_t  result[1024][1024]){
    int xMatrix[KERNELSIZE][KERNELSIZE]={{-1,0,1},{-2,0,2},{-1,0,1}};
    int yMatrix[KERNELSIZE][KERNELSIZE]={{-1,-2,-1},{0,0, 0},{1,2,1}};

    int r, c, j, i;
    int accumulatorX; 
    int accumulatorY;

    for (r=0;r<1024;r++){
        for(c=0;c<1024;c++){
            accumulatorX=0;
            accumulatorY=0;
            if(r==0||c==0||r==1023||c==1023){
                result[r][c]=0;
            }else{
            for(j=0;j<KERNELSIZE;j++){
                for(i=0;i<KERNELSIZE;i++){
                    accumulatorX = accumulatorX + xMatrix[j][i] * orignal[r+(j-(int)ceil(KERNELSIZE/2))][c+(i-(int)ceil(KERNELSIZE/2))];
                    accumulatorY = accumulatorY + yMatrix[j][i] * orignal[r + (j-(int)ceil(KERNELSIZE/2))][c+(i-(int)ceil(KERNELSIZE/2))];
                }
            }
            }
        
            result[r][c]=sqrt((accumulatorX*accumulatorX) + (accumulatorY*accumulatorY));
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
  uint8_t  image[1024][1024];
  uint8_t  out[1024][1024];

   if (argc < 2) {
        printf("No file name found. Please enter a file name with .pgm \n");
        return 1; 
    }

  read_pgm(argv[1], image, 1024, 1024);

  filter(image, out);

  write_pgm("sobel.pgm", out, 1024, 1024);

  return 0;
}