#pragma once

#include <string>

#include "common/json.h"

namespace ProtoRock {

namespace Protocol {

// https://github.com/Sandertv/gophertunnel/blob/master/minecraft/protocol/login/data.go#L211

// PersonaPiece represents a piece of a persona skin. All pieces are sent separately.
struct PersonaPiece : public Common::JsonSerializable {
    // Default specifies if the piece is one of the default pieces. This is true when the piece is one of
    // those that a Steve or Alex skin have.
    bool Default;  // Serializes to IsDefault
    // PackID is a UUID that identifies the pack that the persona piece belongs to.
    std::string PackId;
    // PieceId is a UUID that identifies the piece itself, which is unique for each separate piece.
    std::string PieceId;
    // PieceType holds the type of the piece. Several types I was able to find immediately are listed below.
    // - persona_skeleton
    // - persona_body
    // - persona_skin
    // - persona_bottom
    // - persona_feet
    // - persona_top
    // - persona_mouth
    // - persona_hair
    // - persona_eyes
    // - persona_facial_hair
    std::string PieceType;
    // ProductID is a UUID that identifies the piece when it comes to purchases. It is empty for pieces that
    // have the 'IsDefault' field set to true.
    std::string ProductId;

    void Serialize(Json::Value &root) const override {
        root["Default"] = Default;
        root["PackId"] = PackId;
        root["PieceId"] = PieceId;
        root["PieceType"] = PieceType;
        root["ProductId"] = ProductId;
    }
    void Deserialize(Json::Value &root) override {
        Default = root.get("Default", false).asBool();
        PackId = root.get("PackId", "").asString();
        PieceId = root.get("PieceId", "").asString();
        PieceType = root.get("PieceType", "").asString();
        ProductId = root.get("ProductId", "").asString();
    }
};

// PersonaPieceTintColor describes the tint Colors of a specific piece of a persona skin.
struct PersonaPieceTintColor: public Common::JsonSerializable {
    // Colors is an array of four Colors written in hex notation (note, that unlike the SkinColor field in
    // the ClientData struct, this is actually ARGB, not just RGB).
    // The Colors refer to different parts of the skin piece. The 'persona_eyes' may have the following
    // Colors: ["#ffa12722","#ff2f1f0f","#ff3aafd9","#0"]
    // The first hex Color represents the tint Color of the iris, the second hex Color represents the
    // eyebrows and the third represents the sclera. The fourth is #0 because there are only 3 parts of the
    // persona_eyes skin piece.
    std::string Colors[4];
    // PieceType is the type of the persona skin piece that this tint Color concerns. The piece type must
    // always be present in the persona pieces list, but not each piece type has a tint Color sent.
    // Pieces that do have a tint Color that I was able to find immediately are listed below.
    // - persona_mouth
    // - persona_eyes
    // - persona_hair
    std::string PieceType;

    void Serialize(Json::Value &root) const override {
        for (int i = 0; i < 4; i++) {
            root["Colors"].append(Colors[i]);
        }
        root["PieceType"] = PieceType;
    }
    void Deserialize(Json::Value &root) override {
        auto c = root["Colors"];
        for (int i = 0; i < c.size(); i++) {
            if (i >= 4) { break; }
            Colors[i] = c[i].asString();
        }
        PieceType = root.get("PieceType", "").asString();
    }
};
}  // namespace Protocol
}  // namespace ProtoRock