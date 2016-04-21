
IF(NOT DISABLE_ZPNG)
    # Find zlib and libpng
    FIND_PATH(ZLIB_INCLUDE   NAMES zlib.h)
    FIND_LIBRARY(ZLIB_LIB    NAMES libzlibstatic.a libz.a libz.so)
    FIND_PATH(LIBPNG_INCLUDE NAMES png.h)
    FIND_LIBRARY(LIBPNG_LIB  NAMES libpng.a libpng.so)

    IF(ZLIB_LIB AND ZLIB_INCLUDE AND LIBPNG_LIB AND LIBPNG_INCLUDE)
        SET(LIBCHAOS_WITH_PNG TRUE)
        SET(CORE_LIBS ${CORE_LIBS} "PNG")
        SET(LIBCHAOS_CXX_FLAGS "${LIBCHAOS_CXX_FLAGS} -DLIBCHAOS_HAS_PNG")
    ELSEIF(REQUIRE_ZPNG)
        MESSAGE(SEND_ERROR "PNG support is required (REQUIRE_ZPNG)")
    ELSE()
        MESSAGE(WARNING "Could not find zlib / libpng")
    ENDIF()
ENDIF()

IF(NOT DISABLE_ZJPEG)
    # Find libjpeg
    FIND_PATH(LIBJPEG_INCLUDE NAMES jpeglib.h)
    FIND_LIBRARY(LIBJPEG_LIB  NAMES libjpeg.a libjpeg.so)
    # Apparently libjpeg-turbo is not binary-compatible with libjpeg
    #FIND_LIBRARY(LIBJPEG_LIB  NAMES libturbojpeg.a libjpeg.a libturbojpeg.so libjpeg.so)

    IF(LIBJPEG_LIB AND LIBJPEG_INCLUDE)
        SET(LIBCHAOS_WITH_JPEG TRUE)
        SET(CORE_LIBS ${CORE_LIBS} "JPEG")
        SET(LIBCHAOS_CXX_FLAGS "${LIBCHAOS_CXX_FLAGS} -DLIBCHAOS_HAS_JPEG")
    ELSEIF(REQUIRE_ZJPEG)
        MESSAGE(SEND_ERROR "JPEG support is required (REQUIRE_ZJPEG)")
    ELSE()
        MESSAGE(WARNING "Could not find libjpeg")
    ENDIF()
ENDIF()

IF(NOT DISABLE_ZWEBP)
    # Find libwebp
    FIND_PATH(LIBWEBP_INCLUDE NAMES webp/decode.h webp/encode.h)
    FIND_LIBRARY(LIBWEBP_LIB NAMES libwebp.a libwebp.so)

    IF(LIBWEBP_INCLUDE AND LIBWEBP_LIB)
        SET(LIBCHAOS_WITH_WEBP TRUE)
        SET(CORE_LIBS ${CORE_LIBS} "WebP")
        SET(LIBCHAOS_CXX_FLAGS "${LIBCHAOS_CXX_FLAGS} -DLIBCHAOS_HAS_WEBP")
    ELSEIF(REQUIRE_ZWEBP)
        MESSAGE(SEND_ERROR "WebP support is required (REQUIRE_ZWEBP)")
    ELSE()
        MESSAGE(WARNING "Could not find libwebp")
    ENDIF()
ENDIF()
