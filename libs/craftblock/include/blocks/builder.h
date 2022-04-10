#pragma once

// Block Struct Generator
// The weird /* */ is for being able to debug the generated struct:
// g++ -E -CC blocks.h |grep -v ^# | grep -v ^'*\/' | sed 's/\/\*//g'

#define BLOCKSTRUCT(name, isTransparent) /*
*/   struct name ## Block : public Block {/*
*/      public: /*
*/          const std::string Name() const override { return #name; } /*
*/          const BlockFaceTextureNames TextureNames() const override { return textureNames; };/*
*/          BlockType Type() const override { return BlockType::name; };/*
*/          UBlock ToUBlock() const override { return std::make_unique<name ## Block>(*this); }/*
*/          bool IsTransparent() const override { return isTransparent; };/*
*/      private: /*
*/          static BlockFaceTextureNames textureNames; /*
*/  };
