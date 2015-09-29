/*******************************************************************************
**                                  LibChaos                                  **
**                                  zimage.h                                  **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#ifndef ZIMAGE_H
#define ZIMAGE_H

#include "ztypes.h"
#include "zarray.h"
#include "zbinary.h"
#include "zmap.h"
#include "yimagebackend.h"
//#include "zbitmap.h"
//#include "zpath.h"

#include <cstring>

namespace LibChaos {

/*! Image bitmap container.
 *  Supports variable color channels, channel depths, and planes.
 *  Guarantee: buffer will never be allocated to a nonzero size different than size(). buffer may be null when size() is nonzero.
 *  size() returns the size of the image in bytes, but this does NOT guarantee the buffer() is allocated.
 */
class ZImage {
public:
    typedef unsigned char byte;

    enum imagetype {
        unknown = 0,
        //! Red, Green, Blue (3 channels, 8 bits each).
        rgb24,
        //! Red, Green, Blue (3 channels, 16 bits each).
        rgb48,
        //! Red, Green, Blue, Alpha (4 channels, 8 bits each).
        rgba32,
        //! Red, Green, Blue, Alpha (4 channels, 16 bits each).
        rgba64,
        //! Greyscale (1 channel, 8 bits each).
        g8,
        //! Greyscale (1 channel, 16 bits each).
        g16,
        //! Greyscale with Alpha (2 channels, 8 bits each).
        ga16,
        //! Greyscale with Alpha (2 channels, 16 bits each).
        ga32,
    };

    struct ImageType {
        zu8 channels;
        zu8 depth;
        zu8 planes;
    };
    static const ZMap<imagetype, ImageType> types;

public:
    //! Default constructor.
    ZImage() : _width(0), _height(0), _channels(0), _depth(0), _buffer(nullptr){

    }

    //! Load a formatted image (e.g. PNG, JPEG).
    ZImage(const ZBinary &image) : ZImage(){

    }

    //! Create an image of \a type with \a width and \a height.
    ZImage(zu64 width, zu64 height, imagetype type = rgb24) : ZImage(){
        setDimensions(width, height, types[type].channels, types[type].depth);
    }
    //! Create an image with \a width and \a height, with \a channels color channels and \a depth bits per color.
    ZImage(zu64 width, zu64 height, zu8 channels = 3, zu8 depth = 8) : ZImage(){
        setDimensions(width, height, channels, depth);
    }

    /*! Create and load an imnage from \a data with \a width, \a height, \a channels and \a depth.
     *  The expected size of the image at data is \a width * \a height * (\a channels * \a depth) bytes.
     */
    ZImage(const byte *data, zu64 width, zu64 height, zu8 channels = 3, zu8 depth = 8) : ZImage(){
        setDimensions(width, height, channels, depth);
        copyData(data);
    }
    //! Copy constructor.
    ZImage(const ZImage &other) : ZImage(other._buffer, other._width, other._height, other._channels, other._depth){

    }

    ~ZImage(){
        delete[] _buffer;
        _buffer = nullptr;
    }

    void destroy();

    //! Copy other image.
    ZImage &operator=(const ZImage &other);
    //! Check if two images are the same.
    bool operator==(const ZImage &other) const;

    //! Get byte in buffer.
    inline byte &operator[](zu64 i){
        return _buffer[i];
    }
    //! Get byte in buffer. (const overload)
    inline const byte &operator[](zu64 i) const {
        return _buffer[i];
    }

    //! Get pixel in image.
    inline byte *pixelAt(zu64 i) const {
        return &_buffer[i * pixelSize()];
    }
    //! Get pixel in image. (const overload)
    inline byte *pixelAt(zu64 x, zu64 y) const {
        return &_buffer[(y * _width + x) * pixelSize()];
    }

    /*! Sets the logical size of the image, but does not allocate buffer.
     *  If given dimensions are invalid, will destroy buffer.
     *  If given dimensions would change the size of the buffer, will destroy buffer.
     *  Check the actual dimensions after setting dimensions.
     *  No image will be loaded unless all dimenions are valid.
     */
    void setDimensions(zu64 width, zu64 height, zu8 channels, zu8 depth);

    //! Check that current dimensions can represent an image.
    inline bool validDimensions() const {
        return validDimensions(_width, _height, _channels, _depth);
    }
    /*! Check that given dimensions can represent an image.
     *  All dimensions are nonzero and (channels * depth) % 8 == 0.
     *  If false, buffer must be null.
     */
    static inline bool validDimensions(zu64 width, zu64 height, zu8 channels, zu8 depth){
        return width && height && channels && depth && ((zu16)(channels * depth) % (zu16)8) == 0 && size(width, height, channels, depth);
    }

    //! Allocates a new buffer with current dimensions.
    void newData();
    //! Zeroes the buffer, buffer is allocated in necessary.
    void zeroData();
    //! Copies raw data of size() into buffer, buffer is allocated if necessary.
    void copyData(const byte *data);
    /*! Takes ownership of raw data.
     *  \warning Do not free memory passed to takeData().
     */
    void takeData(byte *data);

    /*! Takes Y, U, and V planes separately.
     *  Planes are expected unpadded and packed width * height or width * height / 4.
     */
    void convertYUV420toRGB24(zu64 width, zu64 height, const byte *ydata, const byte *udata, const byte *vdata);

    void transferImage(ZImage &other);

    /*! Reformat the channels in the buffer.
     *  \todo
     *  Each unique char corresponds to a channel component of a pixel before and after the transform, in the order they are packed in buffer.
     *  The same char may not be used twice in \p before.
     *  There must be channels() chars in \p before. \n
     *  Channels corresponding to a char in \p before and \p after are re-packed according to \p after in every pixel. \n
     *  Channels corresponding to a char in \p before but not \p after are lost in every pixel. \n
     *  Channels corresponding to a char in \p after but not \p before are zero in every pixel. \n
     *  \p after may be kept in the zimage as a hint about the image content.
     *  \note Example: reformat({'R','G','B'}, {'R','G','B','A'});
     */
    void reformat(ZArray<char> before, ZArray<char> after);

    //! Set the width of the image.
    void setWidth(zu64 width);
    //! Set the height of the image.
    void setHeight(zu64 height);

    //! Set the number of channels in the image. Resizes buffer.
    //! If channels is increased, expandmask will be copied to each pixel before original channels are copied.
    //! Expandmask must be at least the new pixel size.
    void setChannels(zu8 channels, const unsigned char *expandmask = nullptr);

    //! Set the depth of the channels in the image. Resizes buffer.
    //! For now, can only deal with depths aligned to bytes (multiples of 8).
    //! \warning This is dangerous / doesn't work (maybe).
    void setDepth(zu8 depth);

    void strip16to8();

    //! Check that image is loaded, check this before using raw buffer access.
    inline bool isLoaded() const {
        return validDimensions() && _buffer != nullptr;
    }

    //! Check if image is 24-bit RGB.
    inline bool isRGB24() const {
        return _channels == 3 && _depth == 8;
    }
    //! Check if image is 48-bit RGB.
    inline bool isRGB48() const {
        return _channels == 3 && _depth == 16;
    }
    //! Check if image is 32-bit RGB.
    inline bool isRGBA32() const {
        return _channels == 4 && _depth == 8;
    }
    //! Check if image is 64-bit RGB.
    inline bool isRGBA64() const {
        return _channels == 4 && _depth == 16;
    }

    //! Get image width.
    inline zu64 width() const {
        return _width;
    }
    //! Get image height.
    inline zu64 height() const {
        return _height;
    }
    //! Get image channels.
    inline zu8 channels() const {
        return _channels;
    }
    //! Get image depth.
    inline zu8 depth() const {
        return _depth;
    }

    //! Get number of pixels.
    inline zu64 pixels() const {
        return _width * _height;
    }

    //! Get size of pixel in bytes.
    inline zu16 pixelSize() const {
        return pixelSize(_channels, _depth);
    }
    static inline zu16 pixelSize(zu8 channels, zu8 depth){
        return (zu16)(channels * depth) / 8;
    }

    //! Get size of row in bytes.
    inline zu64 rowSize() const {
        return _width * pixelSize();
    }

    //! Get logical size of image in bytes.
    inline zu64 size() const {
        return size(_width, _height, _channels, _depth);
    }
    //! Get size of image in bytes from dimensions.
    static inline zu64 size(zu64 width, zu64 height, zu8 channels, zu8 depth){
        return width * height * pixelSize(channels, depth);
    }

    //! Get real size of image buffer.
    inline zu64 realSize(){
        return (!_buffer) ? 0 : size();
    }

    //! Get pointer to buffer.
    inline byte *buffer() const {
        return _buffer;
    }

private:
    //! Image format backend.
    YImageBackend *_backend;
    //! Image width / height in pixels
    zu64 _width, _height;
    //! Number of planes
    zu8 _planes;
    //! Number of channels per pixel
    zu8 _channels;
    //! Number of bits per channel
    zu8 _depth;
    //! Image type
    imagetype _type;
    //! Image data
    unsigned char *_buffer;
};

}

#endif // ZIMAGE_H
