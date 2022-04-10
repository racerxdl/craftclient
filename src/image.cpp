#include "image.h"

#include <png.h>

Image LoadPNG(std::string filename) {
    auto img = std::make_shared<_Image>();

    auto idx = filename.find_last_of(".");
    if (idx == -1) {
        filename += ".png";
    } else {
        auto ext = filename.substr(idx);
        if (ext.compare(".png") != 0) {
            throw ImageException("cannot load file " + filename + ": not a png file");
        }
    }

    FILE *fp = fopen(filename.c_str(), "rb");
    if (!fp) {
        throw ImageException("cannot load file " + filename);
    }

    auto png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png) {
        throw ImageException("cannot create PNG struct");
    }

    auto  info = png_create_info_struct(png);
    if(!info) abort();

    if(setjmp(png_jmpbuf(png))) abort();

    png_init_io(png, fp);
    png_read_info(png, info);

    img->width = png_get_image_width(png, info);
    img->height = png_get_image_height(png, info);
    img->colorType = png_get_color_type(png, info);
    img->bpp = png_get_bit_depth(png, info);

    // Read any color_type into 8bit depth, RGBA format.
    // See http://www.libpng.org/pub/png/libpng-manual.txt

    if(img->bpp == 16)
        png_set_strip_16(png);

    if(img->colorType == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);

    // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
    if(img->colorType == PNG_COLOR_TYPE_GRAY && img->bpp < 8)
        png_set_expand_gray_1_2_4_to_8(png);

    if(png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);

    // These color_type don't have an alpha channel then fill it with 0xff.
    if(img->colorType == PNG_COLOR_TYPE_RGB || img->colorType == PNG_COLOR_TYPE_GRAY || img->colorType == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

    if(img->colorType == PNG_COLOR_TYPE_GRAY || img->colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    auto ptrList = new png_bytep[img->height];
    auto rowBytes = png_get_rowbytes(png,info);

    for (int i = 0; i < img->height; i++) {
        img->rows.push_back(Row());
        img->rows[i].reserve(rowBytes);
        ptrList[i] = img->rows[i].data();
    }

    png_read_image(png, ptrList);

    fclose(fp);

    png_destroy_read_struct(&png, &info, NULL);

    delete []ptrList;

    return img;
}