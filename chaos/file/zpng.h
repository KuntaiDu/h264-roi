#ifndef ZPNG_H
#define ZPNG_H

#include "ztypes.h"
#include "zimage.h"
//#include "zbitmap.h"
#include "zstring.h"
#include "zpath.h"

#define PNG_DEBUG 3
#include <png.h>

namespace LibChaos {

class ZPNG {
public:
    ZPNG() : channels(0){

    }
    ZPNG(const ZBitmapRGBA &bmp) : channels(0), bitmap(bmp){

    }

    bool read(ZPath path);
    bool write(ZPath path);

    static ZString libpngVersionInfo();

    ZString &pngText(ZString key){
        return text[key];
    }

    ZBitmapRGBA toBitmap() const {
        return bitmap;
    }

private:
    struct PngReadData {
        png_structp png_ptr = NULL;
        png_infop info_ptr = NULL;

        png_uint_32 width, height;
        int bit_depth, color_type;

        unsigned char *image_data = NULL;
    };

    struct PngWriteData {
        png_struct *png_ptr = NULL;
        png_info *info_ptr = NULL;

        FILE *outfile;

        png_uint_32 width, height;
        int bit_depth, color_type;

        unsigned char *image_data = NULL;
        unsigned char **row_pointers = NULL;

        double gamma;

        bool interlaced;

        bool have_bg;
        unsigned char bg_red;
        unsigned char bg_green;
        unsigned char bg_blue;

        bool have_time;
        time_t modtime;

        jmp_buf jmpbuf;
    };

private:
    static int readpng_init(PngReadData *data, FILE *infile, unsigned long *pWidth, unsigned long *pHeight);
    static int readpng_get_bgcolor(PngReadData *data, unsigned char *red, unsigned char *green, unsigned char *blue);
    static unsigned char *readpng_get_image(PngReadData *data, double display_exponent, int *pChannels, unsigned long *pRowbytes);
    static void readpng_cleanup(PngReadData *data);
    static void readpng_warning_handler(png_struct *png_ptr, png_const_charp msg);
    static void readpng_error_handler(png_struct *png_ptr, png_const_charp msg);

    static int writepng_init(PngWriteData *mainprog_ptr, const AsArZ &text);
    static int writepng_encode_image(PngWriteData *mainprog_ptr);
    static int writepng_encode_row(PngWriteData *mainprog_ptr);
    static int writepng_encode_finish(PngWriteData *mainprog_ptr);
    static void writepng_cleanup(PngWriteData *mainprog_ptr);
    static void writepng_error_handler(png_struct *png_ptr, png_const_charp msg);

private:
    int channels;
    ZBitmapRGBA bitmap;
    AsArZ text;
    ZError error;
};

}

#endif // ZPNG_H
