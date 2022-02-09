#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define byte sizeof(char)


union BITMAPFILEHEADER {
    char data[14];
    struct {
        char bfType[2];
        unsigned int bfSize;
        char bfReserved1[2];
        char bfReserved2[2];
        unsigned int bfOffBits;
    } inf;
};


union BITMAPINFO {
    char data[40];
    struct {
        unsigned int biSize;
        long biWidth;
        long biHeight;
        char biPlanes;
        char biBitCount;
        unsigned int biCompression;
        unsigned int biSizeImage;
        long biXPelsPerMeter;
        long biYPelsPerMeter;
        unsigned int biClrUsed;
        unsigned int biClrImportant;
    } inf;
};

union pixel {
    char data[3];
    struct {
        unsigned char red;
        unsigned char green;
        unsigned char blue;
    } inf;
} typedef pixel;

pixel **readBMP(FILE *in, int height, int width) {
    pixel **array = NULL;
    array = malloc(height * sizeof(pixel *));
    fseek(in, 54, SEEK_SET);
    int mod = width % 4;
    for (int i = height - 1; i >= 0; i--) {
        array[i] = malloc(width * sizeof(pixel));
        for (size_t j = 0; j < width; j++) {
            fread(array[i][j].data, byte, 3, in);
        }
        for (int j = 0; j < mod; j++) {
            fgetc(in);
        }
    }
    return array;
}


void printBMP(pixel **array, int height, int width) {
    for (int i = 0; i <= height - 1; i++) {
        for (int j = 0; j <= width - 1; j++) {
            if (array[i][j].inf.green == 0 && array[i][j].inf.red == 0 && array[i][j].inf.blue == 0) {
                printf("0 ");
            } else {
                printf("1 ");
            }
        }
        printf("\n");
    }
    printf("   ---- \n");
}

int checkBlack(pixel *pixel) {
    return pixel->inf.green == 0 && pixel->inf.blue == 0 && pixel->inf.red == 0;
}

int countNeighbours(pixel **array, int height, int width, int i, int j) {
    int kol;
    if (i == 0) {
        if (j == 0) {
            kol = checkBlack(&array[i + 1][j]) + checkBlack(&array[i + 1][j + 1]) + checkBlack(&array[i][j + 1]);
        } else if (j == width - 1) {
            kol = checkBlack(&array[i][j - 1]) + checkBlack(&array[i + 1][j - 1]) + checkBlack(&array[i + 1][j]);
        } else {
            kol = checkBlack(&array[i][j - 1]) + checkBlack(&array[i + 1][j - 1]) +
                  checkBlack(&array[i + 1][j]) + checkBlack(&array[i + 1][j + 1]) + checkBlack(&array[i][j + 1]);
        }
    } else if (i == height - 1) {
        if (j == 0) {
            kol = checkBlack(&array[i - 1][j]) + checkBlack(&array[i - 1][j + 1]) + checkBlack(&array[i][j + 1]);
        } else if (j == width - 1) {
            kol = checkBlack(&array[i][j - 1]) + checkBlack(&array[i - 1][j - 1]) + checkBlack(&array[i - 1][j]);
        } else {
            kol = checkBlack(&array[i][j - 1]) + checkBlack(&array[i - 1][j - 1]) +
                  checkBlack(&array[i - 1][j]) + checkBlack(&array[i - 1][j + 1]) + checkBlack(&array[i][j + 1]);
        }
    } else {
        if (j == 0) {
            kol = checkBlack(&array[i - 1][j]) + checkBlack(&array[i - 1][j + 1]) + checkBlack(&array[i][j + 1])
                  + checkBlack(&array[i + 1][j + 1]) + checkBlack(&array[i + 1][j]);
        } else if (j == width - 1) {
            kol = checkBlack(&array[i - 1][j]) + checkBlack(&array[i - 1][j - 1]) + checkBlack(&array[i][j - 1])
                  + checkBlack(&array[i + 1][j - 1]) + checkBlack(&array[i + 1][j]);
        } else {
            kol = checkBlack(&array[i - 1][j - 1]) + checkBlack(&array[i - 1][j]) +
                  checkBlack(&array[i - 1][j + 1]) + checkBlack(&array[i][j + 1]) + checkBlack(&array[i + 1][j + 1]) +
                  checkBlack(&array[i + 1][j]) + checkBlack(&array[i + 1][j - 1]) + checkBlack(&array[i][j - 1]);
        }
    }

//    printf("i = %d j = %d kol = %d", i, j, kol);
    return kol;
}


pixel **nextGeneration(pixel **array, int height, int width) {
    pixel **newArray = malloc(sizeof(pixel *) * height);
    for (int i = height - 1; i >= 0; i--) {
        newArray[i] = malloc(sizeof(pixel) * width);
        for (int j = 0; j < width; j++) {
            int countAlive = countNeighbours(array, height, width, i, j);
//            printf(" alive %d\n", checkBlack(&array[i][j]));
            if (checkBlack(&array[i][j]) && (countAlive == 2 || countAlive == 3)) {
                newArray[i][j].inf.red = 0;
                newArray[i][j].inf.green = 0;
                newArray[i][j].inf.blue = 0;
            } else if (countAlive == 3 && !checkBlack(&array[i][j])) {
                newArray[i][j].inf.red = 0;
                newArray[i][j].inf.green = 0;
                newArray[i][j].inf.blue = 0;
            } else {
                newArray[i][j].inf.red = 255;
                newArray[i][j].inf.green = 255;
                newArray[i][j].inf.blue = 255;
            }
        }
    }
    return newArray;
}

void createNewBMP(pixel **array, int height, int width, FILE *out, union BITMAPFILEHEADER *header,
        union BITMAPINFO *info) {
    fwrite(header->data, byte, 14, out);
    fwrite(info->data, byte, 40, out);
    int mod = width % 4;
    for (int i = height - 1; i >= 0; i--) {
        for (size_t j = 0; j < width; j++) {
            fwrite(array[i][j].data, byte, 3, out);
        }
        for (int j = 0; j < mod; j++) {
            fputc('0', out);
        }
    }
}


int main(int argc, char **argv) {
    FILE *in;
    char *out;
    int max_iter = 0;
    int dump_freq = 0;
//    for (int i = 0; i < argc; i++){
//        printf("%d\n", strcmp(argv[i], "--input"));
//        printf("%s\n", argv[i]);
//    }
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--input")) {
            in = fopen(argv[i + 1], "r");
            if (in == NULL) {
                printf("%s", "InputFile error");
            }
            i++;
        } else if (!strcmp(argv[i], "--output")) {
            if (argv[i + 1][0] == '-') {
                out = "";
            } else {
                out = argv[i + 1];
                i++;
            }
        } else if (!strcmp(argv[i], "--max_iter")) {
            max_iter = atoi(argv[i + 1]);
            i++;
        } else if (!strcmp(argv[i], "--dump_freq")) {
            dump_freq = atoi(argv[i + 1]);
            i++;
        }
    }

    fseek(in, 0, SEEK_SET);
    union BITMAPFILEHEADER header;
    fread(header.data, byte, 14, in);
    union BITMAPINFO info;
    fread(info.data, byte, 40, in);
    int height = info.inf.biHeight;
    int width = info.inf.biWidth;
    pixel **array = readBMP(in, height, width);
//    printBMP(array, height, width);
    pixel **newArray = nextGeneration(array, height, width);
//    printBMP(newArray, height, width);
    char str[20];
    char path[100];
    memset(str, 0, 20);
    memset(path, 0, 100);
    for (int i = 0; i < max_iter; i++) {
        if (i % dump_freq == 0) {
            sprintf(str, "gen%d.bmp", i + 1);
            strcpy(path, out);
            strcat(path, str);
            FILE *out = fopen(path, "wb+");
            createNewBMP(newArray, height, width, out, &header, &info);
            fclose(out);
        }
        newArray = nextGeneration(newArray, height, width);
    }
    fclose(in);
}