#pragma once

// https://github.com/Sandertv/gophertunnel/blob/master/minecraft/protocol/packet/start_game.go
#include <string>
#include <vector>

#include "minecraft/aliases.h"
#include "protocol/block.h"
#include "protocol/educationSharedResourceUri.h"
#include "protocol/experimentData.h"
#include "protocol/gameRule.h"
#include "protocol/item.h"
#include "protocol/packet/packet.h"
#include "protocol/player.h"

namespace ProtoRock {
namespace Protocol {
namespace Packet {

enum struct SpawnBiome : int16_t {
    TypeDefault,
    TypeUserDefined
};

// StartGame is sent by the server to send information about the world the player will be spawned in. It
// contains information about the position the player spawns in, and information about the world in general
// such as its game rules.
struct StartGame : public Packet {
    // EntityUniqueID is the unique ID of the player. The unique ID is a value that remains consistent across
    // different sessions of the same world, but most servers simply fill the runtime ID of the entity out for
    // this field.
    int64_t EntityUniqueID;
    // EntityRuntimeID is the runtime ID of the player. The runtime ID is unique for each world session, and
    // entities are generally identified in packets using this runtime ID.
    uint64_t EntityRuntimeID;
    // PlayerGameMode is the game mode the player currently has. It is a value from 0-4, with 0 being
    // survival mode, 1 being creative mode, 2 being adventure mode, 3 being survival spectator and 4 being
    // creative spectator.
    // This field may be set to 5 to make the client fall back to the game mode set in the WorldGameMode
    // field.
    int32_t PlayerGameMode;
    // PlayerPosition is the spawn position of the player in the world. In servers this is often the same as
    // the world's spawn position found below.
    CraftBlock::Vec3 PlayerPosition;
    // Pitch is the vertical rotation of the player. Facing straight forward yields a pitch of 0. Pitch is
    // measured in degrees.
    float Pitch;
    // Yaw is the horizontal rotation of the player. Yaw is also measured in degrees.
    float Yaw;
    // WorldSeed is the seed used to generate the world. Unlike in PC edition, the seed is a 32bit integer
    // here.
    uint64_t WorldSeed;
    // SpawnBiomeType specifies if the biome that the player spawns in is user defined (through behaviour
    // packs) or builtin. See the constants above.
    int16_t SpawnBiomeType;
    // UserDefinedBiomeName is a readable name of the biome that the player spawned in, such as 'plains'. This
    // might be a custom biome name if any custom biomes are present through behaviour packs.
    std::string UserDefinedBiomeName;
    // Dimension is the ID of the dimension that the player spawns in. It is a value from 0-2, with 0 being
    // the overworld, 1 being the nether and 2 being the end.
    int32_t Dimension;
    // Generator is the generator used for the world. It is a value from 0-4, with 0 being old limited worlds,
    // 1 being infinite worlds, 2 being flat worlds, 3 being nether worlds and 4 being end worlds. A value of
    // 0 will actually make the client stop rendering chunks you send beyond the world limit.
    int32_t Generator;
    // WorldGameMode is the game mode that a player gets when it first spawns in the world. It is shown in the
    // settings and is used if the PlayerGameMode is set to 5.
    int32_t WorldGameMode;
    // Difficulty is the difficulty of the world. It is a value from 0-3, with 0 being peaceful, 1 being easy,
    // 2 being normal and 3 being hard.
    int32_t Difficulty;
    // WorldSpawn is the block on which the world spawn of the world. This coordinate has no effect on the
    // place that the client spawns, but it does have an effect on the direction that a compass points.
    Minecraft::BlockPos WorldSpawn;
    // AchievementsDisabled defines if achievements are disabled in the world. The client crashes if this
    // value is set to true while the player's or the world's game mode is creative, and it's recommended to
    // simply always set this to false as a server.
    bool AchievementsDisabled;
    // DayCycleLockTime is the time at which the day cycle was locked if the day cycle is disabled using the
    // respective game rule. The client will maintain this time as long as the day cycle is disabled.
    int32_t DayCycleLockTime;
    // EducationEditionOffer is some Minecraft: Education Edition field that specifies what 'region' the world
    // was from, with 0 being None, 1 being RestOfWorld, and 2 being China.
    // The actual use of this field is unknown.
    int32_t EducationEditionOffer;
    // EducationFeaturesEnabled specifies if the world has education edition features enabled, such as the
    // blocks or entities specific to education edition.
    bool EducationFeaturesEnabled;
    // EducationProductID is a UUID used to identify the education edition server instance. It is generally
    // unique for education edition servers.
    std::string EducationProductID;
    // RainLevel is the level specifying the intensity of the rain falling. When set to 0, no rain falls at
    // all.
    float RainLevel;
    // LightningLevel is the level specifying the intensity of the thunder. This may actually be set
    // independently from the RainLevel, meaning dark clouds can be produced without rain.
    float LightningLevel;
    // ConfirmedPlatformLockedContent ...
    bool ConfirmedPlatformLockedContent;
    // MultiPlayerGame specifies if the world is a multi-player game. This should always be set to true for
    // servers.
    bool MultiPlayerGame;
    // LANBroadcastEnabled specifies if LAN broadcast was intended to be enabled for the world.
    bool LANBroadcastEnabled;
    // XBLBroadcastMode is the mode used to broadcast the joined game across XBOX Live.
    int32_t XBLBroadcastMode;
    // PlatformBroadcastMode is the mode used to broadcast the joined game across the platform.
    int32_t PlatformBroadcastMode;
    // CommandsEnabled specifies if commands are enabled for the player. It is recommended to always set this
    // to true on the server, as setting it to false means the player cannot, under any circumstance, use a
    // command.
    bool CommandsEnabled;
    // TexturePackRequired specifies if the texture pack the world might hold is required, meaning the client
    // was forced to download it before joining.
    bool TexturePackRequired;
    // GameRules defines game rules currently active with their respective values. The value of these game
    // rules may be either 'bool', 'int32' or 'float32'. Some game rules are server side only, and don't
    // necessarily need to be sent to the client.
    std::vector<GameRule> GameRules;
    // Experiments holds a list of experiments that are either enabled or disabled in the world that the
    // player spawns in.
    std::vector<ExperimentData> Experiments;
    // ExperimentsPreviouslyToggled specifies if any experiments were previously toggled in this world. It is
    // probably used for some kind of metrics.
    bool ExperimentsPreviouslyToggled;
    // BonusChestEnabled specifies if the world had the bonus map setting enabled when generating it. It does
    // not have any effect client-side.
    bool BonusChestEnabled;
    // StartWithMapEnabled specifies if the world has the start with map setting enabled, meaning each joining
    // player obtains a map. This should always be set to false, because the client obtains a map all on its
    // own accord if this is set to true.
    bool StartWithMapEnabled;
    // PlayerPermissions is the permission level of the player. It is a value from 0-3, with 0 being visitor,
    // 1 being member, 2 being operator and 3 being custom.
    int32_t PlayerPermissions;
    // ServerChunkTickRadius is the radius around the player in which chunks are ticked. Most servers set this
    // value to a fixed number, as it does not necessarily affect anything client-side.
    int32_t ServerChunkTickRadius;
    // HasLockedBehaviourPack specifies if the behaviour pack of the world is locked, meaning it cannot be
    // disabled from the world. This is typically set for worlds on the marketplace that have a dedicated
    // behaviour pack.
    bool HasLockedBehaviourPack;
    // HasLockedTexturePack specifies if the texture pack of the world is locked, meaning it cannot be
    // disabled from the world. This is typically set for worlds on the marketplace that have a dedicated
    // texture pack.
    bool HasLockedTexturePack;
    // FromLockedWorldTemplate specifies if the world from the server was from a locked world template. For
    // servers this should always be set to false.
    bool FromLockedWorldTemplate;
    // MSAGamerTagsOnly ..
    bool MSAGamerTagsOnly;
    // FromWorldTemplate specifies if the world from the server was from a world template. For servers this
    // should always be set to false.
    bool FromWorldTemplate;
    // WorldTemplateSettingsLocked specifies if the world was a template that locks all settings that change
    // properties above in the settings GUI. It is recommended to set this to true for servers that do not
    // allow things such as setting game rules through the GUI.
    bool WorldTemplateSettingsLocked;
    // OnlySpawnV1Villagers is a hack that Mojang put in place to preserve backwards compatibility with old
    // villagers. The bool is never actually read though, so it has no functionality.
    bool OnlySpawnV1Villagers;
    // BaseGameVersion is the version of the game from which Vanilla features will be used. The exact function
    // of this field isn't clear.
    std::string BaseGameVersion;
    // LimitedWorldWidth and LimitedWorldDepth are the dimensions of the world if the world is a limited
    // world. For unlimited worlds, these may simply be left as 0.
    int32_t LimitedWorldWidth, LimitedWorldDepth;
    // NewNether specifies if the server runs with the new nether introduced in the 1.16 update.
    bool NewNether;
    // EducationSharedResourceURI is an education edition feature that transmits education resource settings to clients.
    EducationSharedResourceURI educationSharedResourceURI;
    // ForceExperimentalGameplay specifies if experimental gameplay should be force enabled. For servers this
    // should always be set to false.
    bool ForceExperimentalGameplay;
    // LevelID is a base64 encoded world ID that is used to identify the world.
    std::string LevelID;
    // WorldName is the name of the world that the player is joining. Note that this field shows up above the
    // player list for the rest of the game session, and cannot be changed. Setting the server name to this
    // field is recommended.
    std::string WorldName;
    // TemplateContentIdentity is a UUID specific to the premium world template that might have been used to
    // generate the world. Servers should always fill out an empty string for this.
    std::string TemplateContentIdentity;
    // Trial specifies if the world was a trial world, meaning features are limited and there is a time limit
    // on the world.
    bool Trial;
    // PlayerMovementSettings ...
    PlayerMovementSettings playerMovementSettings;
    // Time is the total time that has elapsed since the start of the world.
    int64_t Time;
    // EnchantmentSeed is the seed used to seed the random used to produce enchantments in the enchantment
    // table. Note that the exact correct random implementation must be used to produce the correct results
    // both client- and server-side.
    int32_t EnchantmentSeed;
    // Blocks is a list of all custom blocks registered on the server.
    std::vector<BlockEntry> Blocks;
    // Items is a list of all items with their legacy IDs which are available in the game. Failing to send any
    // of the items that are in the game will crash mobile clients.
    std::vector<ItemEntry> Items;
    // MultiPlayerCorrelationID is a unique ID specifying the multi-player session of the player. A random
    // UUID should be filled out for this field.
    std::string MultiPlayerCorrelationID;
    // ServerAuthoritativeInventory specifies if the server authoritative inventory system is enabled. This
    // is a new system introduced in 1.16. Backwards compatibility with the inventory transactions has to
    // some extent been preserved, but will eventually be removed.
    bool ServerAuthoritativeInventory;
    // GameVersion is the version of the game the server is running. The exact function of this field isn't clear.
    std::string GameVersion;
    // ServerBlockStateChecksum is a checksum to ensure block states between the server and client match.
    // This can simply be left empty, and the client will avoid trying to verify it.
    uint64_t ServerBlockStateChecksum;

    bool IsRaw() const override { return false; }
    PacketID Id() const override { return PacketID::StartGame; }
    virtual void Serialize(Writer &w) const override {
        w.WriteVarint<int64_t>(EntityUniqueID);
        w.WriteVarint<uint64_t>(EntityRuntimeID);
        w.WriteVarint<int32_t>(PlayerGameMode);
        w.Write(PlayerPosition);
        w.WriteFloat(Pitch);
        w.WriteFloat(Yaw);
        w.WriteBE(WorldSeed);
        w.WriteBE(SpawnBiomeType);
        w.Write(UserDefinedBiomeName);
        w.WriteVarint<int32_t>(Dimension);
        w.WriteVarint<int32_t>(Generator);
        w.WriteVarint<int32_t>(WorldGameMode);
        w.WriteVarint<int32_t>(Difficulty);
        w.Write(WorldSpawn);
        w.Write(AchievementsDisabled);
        w.WriteVarint<int32_t>(DayCycleLockTime);
        w.WriteVarint<int32_t>(EducationEditionOffer);
        w.Write(EducationFeaturesEnabled);
        w.Write(EducationProductID);
        w.WriteFloat(RainLevel);
        w.WriteFloat(LightningLevel);
        w.Write(ConfirmedPlatformLockedContent);
        w.Write(MultiPlayerGame);
        w.Write(LANBroadcastEnabled);
        w.WriteVarint<int32_t>(XBLBroadcastMode);
        w.WriteVarint<int32_t>(PlatformBroadcastMode);
        w.Write(CommandsEnabled);
        w.Write(TexturePackRequired);
        w.WriteVarint<uint32_t>(GameRules.size());
        for (auto &gr: GameRules) {
            gr.Serialize(w);
        }
        w.WriteVarint<uint32_t>(Experiments.size());
        for (auto &ee: Experiments) {
            ee.Serialize(w);
        }
        w.Write(ExperimentsPreviouslyToggled);
        w.Write(BonusChestEnabled);
        w.Write(StartWithMapEnabled);
        w.WriteVarint<int32_t>(PlayerPermissions);
        w.WriteBE(ServerChunkTickRadius);
        w.Write(HasLockedBehaviourPack);
        w.Write(HasLockedTexturePack);
        w.Write(FromLockedWorldTemplate);
        w.Write(MSAGamerTagsOnly);
        w.Write(FromWorldTemplate);
        w.Write(WorldTemplateSettingsLocked);
        w.Write(OnlySpawnV1Villagers);
        w.Write(BaseGameVersion);
        w.WriteBE(LimitedWorldWidth);
        w.WriteBE(LimitedWorldDepth);
        w.Write(NewNether);
        educationSharedResourceURI.Serialize(w);
        w.Write(ForceExperimentalGameplay);
        if (ForceExperimentalGameplay) {
            w.Write(ForceExperimentalGameplay); // Mojang stuff, who knows
        }
        w.Write(LevelID);
        w.Write(WorldName);
        w.Write(TemplateContentIdentity);
        w.Write(Trial);
        playerMovementSettings.Serialize(w);
        w.WriteBE(Time);
        w.WriteVarint<int32_t>(EnchantmentSeed);
        w.WriteVarint<uint32_t>(Blocks.size());
        for (auto &b: Blocks) {
            b.Serialize(w);
        }
        w.WriteVarint<uint32_t>(Items.size());
        for (auto &i: Items) {
            i.Serialize(w);
        }
        w.Write(MultiPlayerCorrelationID);
        w.Write(ServerAuthoritativeInventory);
        w.Write(GameVersion);
        w.WriteBE(ServerBlockStateChecksum);
    };
    virtual void Deserialize(Reader &r) override {
        // TODO
    }
};

}  // namespace Packet
}  // namespace Protocol
}  // namespace ProtoRock