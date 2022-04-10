#pragma once
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <string>
#include <map>

#include "image.h"
#include "renderstate.h"

class _Texture;
class _TextureAtlas;

typedef std::shared_ptr<_Texture> Texture;
typedef std::shared_ptr<_TextureAtlas> TextureAtlas;

class _Texture {
protected:
    GLuint tex;
    Image img;
    bool initialized = false;
    bool needsBufferUpdate = true;
    void updateBuffer();
public:
    void SetImage(Image img) { this->img = img; needsBufferUpdate = true; }
    void Use(const RenderState &rs);
};

class _TextureAtlas: public _Texture {
private:
    std::map<std::string, int> textureIndex;
    CraftBlock::Vec2 sectionSize;
public:
    _TextureAtlas(CraftBlock::Vec2 sectionSize, std::vector<std::string> files);
    CraftBlock::Vec2 operator[](const std::string &);
    CraftBlock::Vec2 SectionSize() { return CraftBlock::Vec2{sectionSize.x / img->width, sectionSize.y / img->height}; };
    CraftBlock::Vec4 GetBounds(const std::string &);
};

TextureAtlas BuildTextureAtlasFromFiles(std::vector<std::string> files);
Texture LoadTextureFromFile(const std::string &filename);
