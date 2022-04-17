#pragma once

#include <jwt-cpp/jwt.h>
#include <jwt-cpp/traits/kazuho-picojson/traits.h>

#include <string>
#include <vector>

#include "common/json.h"
#include "protocol/error.h"
#include "protocol/os.h"
#include "protocol/personaPiece.h"
#include "protocol/skinAnimation.h"

namespace ProtoRock {
namespace Protocol {
// https://github.com/Sandertv/gophertunnel/blob/master/minecraft/protocol/login/data.go

// ClientData is a container of client specific data of a Login packet. It holds data such as the skin of a
// player, but also its language code and device information.
struct ClientData : Common::JsonSerializable {
    // ClientRandomID is a random client ID number generated for the client. It usually remains consistent
    // through sessions and through game restarts.
    int64_t ClientRandomId = 0;
    // AnimatedImageData is a list of image data for animations. Each of the elements in this slice holds the
    // image data of a single frame of the animation.
    std::vector<SkinAnimation> AnimatedImageData;
    // PersonaPieces is a list of all persona pieces that the skin is composed of.
    std::vector<PersonaPiece> PersonaPieces;
    // PieceTintColors is a list of specific tint Colors for (some of) the persona pieces found in the list
    // above.
    std::vector<PersonaPieceTintColor> PieceTintColors;
    // CapeData is a base64 encoded string of cape data. This is usually an empty string, as skins typically
    // don't carry capes themselves.
    std::string CapeData;
    // CapeID is an ID which, like the SkinID, identifies a skin. Usually this is either empty for no skin or
    // some ID containing a UUID in it.
    std::string CapeId;
    // DeviceModel is a string indicating the device model used by the player. At the moment, it appears that
    // this name is always '(Standard system devices) System devices'.
    std::string DeviceModel;
    // DeviceID is a UUID specific to the device. A different user will have the same UUID for this.
    std::string DeviceId;
    // GameVersion is the game version of the player that attempted to join, for example '1.11.0'.
    std::string GameVersion;
    // LanguageCode is the language code of the player. It looks like 'en_UK'. It follows the ISO language
    // codes, but hyphens ('-') are replaced with underscores. ('_')
    std::string LanguageCode;
    // PlatformOfflineID is either a UUID or an empty string ...
    std::string PlatformOfflineId;
    // PlatformOnlineID is either a uint64 or an empty string ...
    std::string PlatformOnlineId;
    // PlatformUserID holds a UUID which is only sent if the DeviceOS is of type device.XBOX. Its function
    // is not exactly clear.
    std::string PlatformUserId;
    // SelfSignedId is a UUID that remains consistent through restarts of the game and new game sessions.
    std::string SelfSignedId;
    // ServerAddress is the exact address the player used to join the server with. This may be either an
    // actual address, or a hostname. ServerAddress also has the port in it, in the shape of
    // 'address:port`.
    std::string ServerAddress;
    // TODO: Find out what value SkinAnimationData holds and when it does hold something.
    std::string SkinAnimationData;
    // SkinData is a base64 encoded byte slice of 64*32*4, 64*64*4 or 128*128*4 bytes. It is a RGBA ordered
    // byte representation of the skin Colors.
    std::string SkinData;
    // SkinGeometry is a base64 JSON encoded structure of the geometry data of a skin, containing properties
    // such as bones, uv, pivot etc.
    std::string SkinGeometry;
    // SkinGeometryVersion is the version for SkinGeometry.
    std::string SkinGeometryVersion;  // serializes to SkinGeometryDataEngineVersion
    // SkinID is a unique ID produced for the skin, for example 'c18e65aa-7b21-4637-9b63-8ad63622ef01_Alex'
    // for the default Alex skin.
    std::string SkinId;
    // PlayFabId is the PlayFab ID produced for the player's skin. PlayFab is the company that hosts the
    // Marketplace, skins and other related features from the game. This ID is the ID of the skin used to
    // store the skin inside of PlayFab.
    std::string PlayFabId;
    // SkinResourcePatch is a base64 encoded string which holds JSON data. The content of the JSON data points
    // to the assets that should be used to shape the skin. An example with a head animation can be found
    // below.
    // {
    //   "geometry" : {
    //      "animated_face" : "geometry.animated_face_persona_d1625e47f4c9399f_0_1",
    //      "default" : "geometry.persona_d1625e47f4c9399f_0_1"
    //   }
    // }
    // A skin resource patch must be present at all times. The minimum required data that the field must hold
    // is {"geometry": {"default": "geometry.persona_d1625e47f4c9399f_0_1"}}
    std::string SkinResourcePatch;
    // SkinColor is a hex representation (including #) of the base Color of the skin. An example of the
    // Color sent here is '#b37b62'.
    std::string SkinColor;
    // ArmSize is the size of the arms of the player's model. This is either 'wide' (generally for male skins)
    // or 'slim' (generally for female skins).
    std::string ArmSize;
    // ThirdPartyName is the username of the player. This username should not be used however. The DisplayName
    // sent in the IdentityData should be preferred over this.
    std::string ThirdPartyName;
    // CapeImageHeight and CapeImageWidth are the dimensions of the cape's image.
    int CapeImageHeight = 0, CapeImageWidth = 0;
    // CurrentInputMode is the input mode used by the client. It is 1 for mobile and win10, but is different
    // for console input.
    int CurrentInputMode = 0;
    // DefaultInputMode is the default input mode used by the device.
    int DefaultInputMode = 0;
    // DeviceOS is a numerical ID indicating the OS of the device.
    DeviceOS OS;  // Serializes to DeviceOS
    // GUIScale is the GUI scale of the player. It is by default 0, and is otherwise -1 or -2 for a smaller
    // GUI scale than usual.
    int GuiScale = 0;
    // SkinImageHeight and SkinImageWidth are the dimensions of the skin's image data.
    int SkinImageHeight = 0, SkinImageWidth = 0;
    // UIProfile is the UI profile used. For the 'Pocket' UI, this is 1. For the 'Classic' UI, this is 0.
    int UIProfile = 0;
    // CapeOnClassicSkin specifies if the cape that the player has equipped is part of a classic skin, which
    // usually points to one of the older MineCon capes.
    bool CapeOnClassicSkin = false;
    // PersonaSkin specifies if the skin was a persona skin, meaning that it was created through the in-game
    // skin creator.
    bool PersonaSkin = false;
    // PremiumSkin indicates if the skin the player held was a premium skin, meaning it was obtained through
    // payment.
    bool PremiumSkin = false;
    // ThirdPartyNameOnly specifies if the user only has a third party name. It should always be assumed to be
    // false, because the third party name is not XBOX Live Auth protected, meaning it can be tempered with
    // and the username changed.
    // Although this field is obviously here for a reason, allowing this is too dangerous and should never be
    // done.
    bool ThirdPartyNameOnly = false;

    Error Validate() const;
    void AddClaims(jwt::builder<jwt::traits::kazuho_picojson> &builder) const;
    void FromClaims(jwt::decoded_jwt<jwt::traits::kazuho_picojson> &jwt);
    void Serialize(Json::Value &root) const override;
    void Deserialize(Json::Value &root) override;
    ClientData();
};
}  // namespace Protocol
}  // namespace ProtoRock