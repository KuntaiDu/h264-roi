/*******************************************************************************
**                                  LibChaos                                  **
**                                   zpng.h                                   **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#ifndef ZPNG_H
#define ZPNG_H

#include "ztypes.h"
#include "zimage.h"
#include "zstring.h"
#include "zpath.h"
#include "zexception.h"
#include "zbinary.h"
#include "yimagebackend.h"

//#define PNG_DEBUG 3
#include <png.h>

namespace LibChaos {

class ZPNG : public YImageBackend {
public:
    struct PNGError {
        enum pngerrors {
            none = 0,
            badfile = 1,
            sigreadfail = 2,
            sigcheckfail = 3,
            readstructfail = 4,
            infostructfail = 5,
            libpngerror = 6,
            imageallocfail = 7,
            rowallocfail = 8,
            badpointer = 9,
            unsupportedchannelcount = 10,
            emptyimage = 11,
            writestructfail = 12,
            badwritefile = 13,
            invaliddimensions = 14,
            invalidcolortype = 15,
            invalidbkgddepth = 16,
            emptyinput = 17,
        };
    };
    struct PNGWrite {
        enum pngoptions {
            none = 0,
            interlace = 1
        };
    };

    struct PngChunk {
        zu32 size;
        ZString name;
        ZBinary data;
        zu32 crc;
        bool crc_ok;
    };

    struct PngInfo {
        PngInfo() : interlaced(false), gamma(0.0){}

        bool interlaced;
        double gamma;

    };

public:
    ZPNG(){

    }
    ZPNG(const ZImage &img) : image(img){

    }

    bool decode(ZBinary &pngdata_in);
    bool encode(ZBinary &pngdata_out, void *options = nullptr);

    bool read(ZPath path);
    bool write(ZPath path, PNGWrite::pngoptions options = PNGWrite::none);

    static ZArray<PngChunk> parsePngChunks(ZBinary pngdata);
    static ZArray<PngChunk> parsePngAncillaryChunks(ZBinary pngdata);

    static ZString libpngVersionInfo();

    ZString &pngText(ZString key){
        return text[key];
    }

    ZImage &getImage(){
        return image;
    }

private:
    ZImage image;
    AsArZ text;
};

}

#endif // ZPNG_H
