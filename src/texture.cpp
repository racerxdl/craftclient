#include "texture.h"

#include <cstring>
#include <cassert>
#include <iostream>

uint64_t next_pow2(uint64_t x) {
	return x == 1 ? 1 : 1<<(64-__builtin_clzl(x-1));
}

_TextureAtlas::_TextureAtlas(Vec2 sectionSize, std::vector<std::string> files) {
    this->sectionSize = sectionSize;
    int i = 0;
    for (auto file: files) {
        this->textureIndex[file] = i;

        auto idx = file.find_last_of(".");
        file = file.substr(0, idx);
        this->textureIndex[file] = i;

        i++;
    }
}

Texture LoadTextureFromFile(const std::string &filename) {
    auto texture = std::make_shared<_Texture>();
    auto img = LoadPNG(filename);

    texture->SetImage(img);

    return texture;
}

TextureAtlas BuildTextureAtlasFromFiles(std::vector<std::string> filenames) {
    int len = filenames.size();
    assert(len > 0);

    auto images = std::vector<Image>();
    for(auto filename: filenames) {
        images.push_back(LoadPNG(filename));
    }

    int width = images[0]->width;
    int height = images[0]->height;
    int pixels = width * height * len;
    int side = (int)ceil(sqrt((float)pixels));
    side = next_pow2(side);

    int z = 0;
    for (auto &img: images) {
        std::cout << filenames[z] << std::endl;
        assert(img->width == width);
        assert(img->height == height);
        z++;
    }
    auto atlas = std::make_shared<_Image>();

    // Reserve all space
    atlas->width = side;
    atlas->height = side;
    atlas->bpp = 32;
    atlas->rows.resize(side);
    for (int i = 0; i < side; i++) {
        atlas->operator[](i).resize(side * 4);
    }

    memset(atlas->operator[](2).data(), 0xFF, side *4);

    // Map textures
    int i = 0;
    for (auto &img: images) {
        int offX = (i * width) % atlas->width;
        int offY = ((i * width) / atlas->width) * height;

        for (auto &row: img->rows) {
            auto atlasRow = atlas->rows[offY].data();
            auto imgRow = row.data();
            std::memcpy(&atlasRow[offX*4], imgRow, width*4);
            offY++;
        }

        i++;
    }
    auto texture = std::make_shared<_TextureAtlas>(Vec2{(float)width, (float)height}, filenames);
    texture->SetImage(atlas);

    return texture;
}

Vec4 _TextureAtlas::GetBounds(const std::string &name) {
    Vec4 r;
    auto baseCoord = operator[](name);
    auto baseSize = SectionSize();

    r.x = baseCoord.x;
    r.y = baseCoord.y;
    r.z = baseCoord.x + baseSize.x;
    r.w = baseCoord.y + baseSize.y;

    return r;
}

Vec2 _TextureAtlas::operator[](const std::string &filename) {
    Vec2 r;

    assert(textureIndex.count(filename) > 0);

    int idx = textureIndex[filename];
    int wx = img->width / sectionSize.x;
    int wy = img->height / sectionSize.y;

    float nx = 1.0f / wx;
    float ny = 1.0f / wy;

    r.x = (int)(idx * sectionSize.x) % img->width;
    r.y = (int)(idx * sectionSize.x) / img->width;
    r.x /= sectionSize.x;

    r.x *= nx;
    r.y *= ny;

    return r;
}

void _Texture::Use(const RenderState &rs) {
    if (needsBufferUpdate) {
        needsBufferUpdate = false;
        updateBuffer();
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void _Texture::updateBuffer() {
    if (!initialized) {
        glGenTextures(1, &tex);
        initialized = true;
    }

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    std::vector<unsigned char> data;
    data.reserve(img->width * img->height * 4);
    auto dataPtr = data.data();
    int i = 0;
    for (auto &r: img->rows) {
        auto offset = i * img->width * 4;
        auto rptr = r.data();
        std::memcpy(&dataPtr[offset], rptr, img->width * 4);
        i++;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->width, img->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
}