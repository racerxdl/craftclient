#include "texture.h"

#include <cstring>

Texture LoadTextureFromFile(const std::string &filename) {
    auto texture = std::make_shared<_Texture>();
    auto img = LoadPNG(filename);

    texture->SetImage(img);

    return texture;
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