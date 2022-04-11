#include <blocks/blocks.h>
#include <list>

using namespace CraftBlock;

namespace CraftBlock {
    const std::unordered_map<CraftBlock::BlockType, const Block*> BlockByType = {
        {   Air, new AirBlock()                  },
        {   Grass, new GrassBlock()              },
        {   Dirt, new DirtBlock()                },
        {   Stone, new StoneBlock()              },
        {   Sand, new SandBlock()                },
        {   Water, new WaterBlock()              },
        {   Glass, new GlassBlock()              },
        {   Log, new LogBlock()                  },
        {   Leaves, new LeavesBlock()            },
        {   Rose, new RoseBlock()                },
        {   Buttercup, new ButtercupBlock()      },
        {   Coal, new CoalBlock()                },
        {   Copper, new CopperBlock()            },
        {   Lava, new LavaBlock()                },
        {   Clay, new ClayBlock()                },
        {   Gravel, new GravelBlock()            },
        {   Planks, new PlanksBlock()            },
        {   Torch, new TorchBlock()              },
        {   Cobblestone, new CobblestoneBlock()  },
        {   Snow, new SnowBlock()                },
        {   Podzol, new PodzolBlock()            },
        {   Shrub, new ShrubBlock()              },
        {   TallGrass, new TallGrassBlock()      },
        {   PineLog, new PineLogBlock()          },
        {   PineLeaves, new PineLeavesBlock()    },
        {   Brick, new BrickBlock()              },
        {   Cactus, new CactusBlock()            },
    };

    BlockFaceTextureNames TexturesToLoad() {
        std::list<std::string> files;
        for(const auto &kv: BlockByType) {
            auto b = kv.second->TextureNames();
            files.insert(files.end(), b.begin(), b.end());
        }
        files.sort();
        files.unique();

        return BlockFaceTextureNames(files.begin(), files.end());
    }

    BlockFaceTextureNames CraftBlock::AirBlock::textureNames = {};
    BlockFaceTextureNames CraftBlock::GrassBlock::textureNames = {
        "textures/blocks/grass_side_carried",    // Back
        "textures/blocks/grass_side_carried",    // Front
        "textures/blocks/grass_side_carried",    // Right
        "textures/blocks/grass_side_carried",    // Left
        "textures/blocks/grass_carried",         // Top
        "textures/blocks/dirt",                  // Bottom
    };
    BlockFaceTextureNames CraftBlock::DirtBlock::textureNames = {
        "textures/blocks/dirt",         // Back
        "textures/blocks/dirt",         // Front
        "textures/blocks/dirt",         // Right
        "textures/blocks/dirt",         // Left
        "textures/blocks/dirt",         // Top
        "textures/blocks/dirt",         // Bottom
    };
    BlockFaceTextureNames CraftBlock::StoneBlock::textureNames = {
        "textures/blocks/stone",        // Back
        "textures/blocks/stone",        // Front
        "textures/blocks/stone",        // Right
        "textures/blocks/stone",        // Left
        "textures/blocks/stone",        // Top
        "textures/blocks/stone",        // Bottom
    };
    BlockFaceTextureNames CraftBlock::SandBlock::textureNames = {
        "textures/blocks/sand",        // Back
        "textures/blocks/sand",        // Front
        "textures/blocks/sand",        // Right
        "textures/blocks/sand",        // Left
        "textures/blocks/sand",        // Top
        "textures/blocks/sand",        // Bottom
    };
    BlockFaceTextureNames CraftBlock::WaterBlock::textureNames = {
        "textures/blocks/water_placeholder",        // Back
        "textures/blocks/water_placeholder",        // Front
        "textures/blocks/water_placeholder",        // Right
        "textures/blocks/water_placeholder",        // Left
        "textures/blocks/water_placeholder",        // Top
        "textures/blocks/water_placeholder",        // Bottom
    };
    BlockFaceTextureNames CraftBlock::GlassBlock::textureNames = {
        "textures/blocks/glass",        // Back
        "textures/blocks/glass",        // Front
        "textures/blocks/glass",        // Right
        "textures/blocks/glass",        // Left
        "textures/blocks/glass",        // Top
        "textures/blocks/glass",        // Bottom
    };
    BlockFaceTextureNames CraftBlock::LogBlock::textureNames = {
        "textures/blocks/log_oak",        // Back
        "textures/blocks/log_oak",        // Front
        "textures/blocks/log_oak",        // Right
        "textures/blocks/log_oak",        // Left
        "textures/blocks/log_oak_top",    // Top
        "textures/blocks/log_oak_top",    // Bottom
    };
    BlockFaceTextureNames CraftBlock::LeavesBlock::textureNames = {
        "textures/blocks/leaves_acacia",        // Back
        "textures/blocks/leaves_acacia",        // Front
        "textures/blocks/leaves_acacia",        // Right
        "textures/blocks/leaves_acacia",        // Left
        "textures/blocks/leaves_acacia",        // Top
        "textures/blocks/leaves_acacia",        // Bottom
    };
    BlockFaceTextureNames CraftBlock::RoseBlock::textureNames = {
        "textures/blocks/flower_rose",
    };
    BlockFaceTextureNames CraftBlock::ButtercupBlock::textureNames = {};
    BlockFaceTextureNames CraftBlock::CoalBlock::textureNames = {
        "textures/blocks/coal_ore",        // Back
        "textures/blocks/coal_ore",        // Front
        "textures/blocks/coal_ore",        // Right
        "textures/blocks/coal_ore",        // Left
        "textures/blocks/coal_ore",        // Top
        "textures/blocks/coal_ore",        // Bottom
    };
    BlockFaceTextureNames CraftBlock::CopperBlock::textureNames = {};
    BlockFaceTextureNames CraftBlock::LavaBlock::textureNames = {
        "textures/blocks/lava_placeholder",        // Back
        "textures/blocks/lava_placeholder",        // Front
        "textures/blocks/lava_placeholder",        // Right
        "textures/blocks/lava_placeholder",        // Left
        "textures/blocks/lava_placeholder",        // Top
        "textures/blocks/lava_placeholder",        // Bottom
    };
    BlockFaceTextureNames CraftBlock::ClayBlock::textureNames = {
        "textures/blocks/clay",        // Back
        "textures/blocks/clay",        // Front
        "textures/blocks/clay",        // Right
        "textures/blocks/clay",        // Left
        "textures/blocks/clay",        // Top
        "textures/blocks/clay",        // Bottom
    };
    BlockFaceTextureNames CraftBlock::GravelBlock::textureNames = {
        "textures/blocks/gravel",        // Back
        "textures/blocks/gravel",        // Front
        "textures/blocks/gravel",        // Right
        "textures/blocks/gravel",        // Left
        "textures/blocks/gravel",        // Top
        "textures/blocks/gravel",        // Bottom
    };
    BlockFaceTextureNames CraftBlock::PlanksBlock::textureNames = {
        "textures/blocks/planks_oak",        // Back
        "textures/blocks/planks_oak",        // Front
        "textures/blocks/planks_oak",        // Right
        "textures/blocks/planks_oak",        // Left
        "textures/blocks/planks_oak",        // Top
        "textures/blocks/planks_oak",        // Bottom
    };
    BlockFaceTextureNames CraftBlock::TorchBlock::textureNames = {
        "textures/blocks/torch_on",        // Back
        "textures/blocks/torch_on",        // Front
        "textures/blocks/torch_on",        // Right
        "textures/blocks/torch_on",        // Left
        "textures/blocks/torch_on",        // Top
        "textures/blocks/torch_on",        // Bottom

        // "textures/blocks/stone",        // Back
        // "textures/blocks/stone",        // Front
        // "textures/blocks/stone",        // Right
        // "textures/blocks/stone",        // Left
        // "textures/blocks/stone",        // Top
        // "textures/blocks/stone",        // Bottom
    };
    BlockFaceTextureNames CraftBlock::CobblestoneBlock::textureNames = {
        "textures/blocks/cobblestone",        // Back
        "textures/blocks/cobblestone",        // Front
        "textures/blocks/cobblestone",        // Right
        "textures/blocks/cobblestone",        // Left
        "textures/blocks/cobblestone",        // Top
        "textures/blocks/cobblestone",        // Bottom
    };
    BlockFaceTextureNames CraftBlock::SnowBlock::textureNames = {
        "textures/blocks/snow",        // Back
        "textures/blocks/snow",        // Front
        "textures/blocks/snow",        // Right
        "textures/blocks/snow",        // Left
        "textures/blocks/snow",        // Top
        "textures/blocks/snow",        // Bottom
    };
    BlockFaceTextureNames CraftBlock::PodzolBlock::textureNames = {
        "textures/blocks/dirt_podzol_side",       // Back
        "textures/blocks/dirt_podzol_side",       // Front
        "textures/blocks/dirt_podzol_side",       // Right
        "textures/blocks/dirt_podzol_side",       // Left
        "textures/blocks/dirt_podzol_top",        // Top
        "textures/blocks/dirt_podzol_side",       // Bottom
    };
    BlockFaceTextureNames CraftBlock::ShrubBlock::textureNames = {};
    BlockFaceTextureNames CraftBlock::TallGrassBlock::textureNames = {
        "textures/blocks/tallgrass_carried",
    };
    BlockFaceTextureNames CraftBlock::PineLogBlock::textureNames = {};
    BlockFaceTextureNames CraftBlock::PineLeavesBlock::textureNames = {};
    BlockFaceTextureNames CraftBlock::BrickBlock::textureNames = {
        "textures/blocks/brick",        // Back
        "textures/blocks/brick",        // Front
        "textures/blocks/brick",        // Right
        "textures/blocks/brick",        // Left
        "textures/blocks/brick",        // Top
        "textures/blocks/brick",        // Bottom
    };
    BlockFaceTextureNames CraftBlock::CactusBlock::textureNames = {
        "textures/blocks/cactus_side",        // Back
        "textures/blocks/cactus_side",        // Front
        "textures/blocks/cactus_side",        // Right
        "textures/blocks/cactus_side",        // Left
        "textures/blocks/cactus_top",         // Top
        "textures/blocks/cactus_bottom",      // Bottom
    };
}