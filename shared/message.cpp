#include "message.h"

using nlohmann::json;
using namespace nibashared;

message_register::message_register(std::string &&id, std::string &&password) :
    id(std::move(id)), password(std::move(password)) {}

bool message_register::validate(const nibashared::sessionstate &session) {
    // TODO: throw error with error code?
    if (session.state != nibashared::gamestate::prelogin)
        return false;

    if (id.size() < 4 || id.size() > 16 || password.size() < 6 || password.size() > 24) {
        return false;
    }
    return true;
}

json message_register::create_response() { return {{"success", success}}; }

// this is not used
json message_register::create_request() {
    return {{"type", type}, {"id", id}, {"password", password}};
}
void message_register::merge_response(const json &j) { j.at("success").get_to(success); }

void message_register::from_request(const json &j) {
    j.at("id").get_to(id);
    j.at("password").get_to(password);
}

message_login::message_login(std::string &&id, std::string &&password) :
    id(std::move(id)), password(std::move(password)) {}

bool message_login::validate(const nibashared::sessionstate &session) {
    if (session.state != nibashared::gamestate::prelogin)
        return false;

    if (id.size() < 4 || id.size() > 16 || password.size() < 6 || password.size() > 24) {
        return false;
    }
    return true;
}

json message_login::create_response() {
    json ret;
    ret["success"] = success;
    // the default serializer couldn't handle optional
    // we could write one or we'll do this for now
    if (player) {
        ret["player"] = *player;
    }
    ret["magics"] = magics;
    ret["equips"] = equips;
    return ret;
}

// this is not used
json message_login::create_request() {
    return {{"type", type}, {"id", id}, {"password", password}};
}

void message_login::merge_response(const json &j) {
    j.at("success").get_to(success);
    auto char_iter = j.find("player");
    if (char_iter != j.end()) {
        player = char_iter.value().get<nibashared::player>();
    }
    j.at("magics").get_to(magics);
    j.at("equips").get_to(equips);
}

void message_login::from_request(const json &j) {
    j.at("id").get_to(id);
    j.at("password").get_to(password);
}

bool nibashared::message_getdata::validate(const nibashared::sessionstate &session) {
    // TODO change this
    if (session.state != nibashared::gamestate::ingame)
        return false;

    return true;
}

nlohmann::json nibashared::message_getdata::create_response() {
    if (!success) {
        return {{"success", false}};
    }
    return {{"success", true}, {"characters", characters}, {"magics", magics}, {"equips", equips}};
}

nlohmann::json nibashared::message_getdata::create_request() { return {{"type", type}}; }

void nibashared::message_getdata::merge_response(const nlohmann::json &j) {
    j.at("success").get_to(success);
    if (success) {
        j.at("characters").get_to(characters);
        j.at("magics").get_to(magics);
        j.at("equips").get_to(equips);
    }
}

void nibashared::message_getdata::from_request(const nlohmann::json &j) { (void)j; }

nibashared::message_fight::message_fight(int enemyid) : enemyid(enemyid) {}

bool nibashared::message_fight::validate(const nibashared::sessionstate &session) {
    // TODO change this
    if (session.state != nibashared::gamestate::ingame)
        return false;

    // TODO: store staticdata in session?
    // TODO: after the change, staticdata should be initialized and useable at this point
    if (enemyid < 0 || enemyid > 8)
        return false;

    return true;
}

nlohmann::json nibashared::message_fight::create_response() { return {{"generated", generated}}; }

nlohmann::json nibashared::message_fight::create_request() {
    return {{"type", type}, {"enemyid", enemyid}};
}

void nibashared::message_fight::merge_response(const nlohmann::json &j) {
    j.at("generated").get_to(generated);
}

void nibashared::message_fight::from_request(const nlohmann::json &j) {
    j.at("enemyid").get_to(enemyid);
}

nibashared::message_createchar::message_createchar(nibashared::player &&player) :
    player(std::move(player)) {}

bool nibashared::message_createchar::validate(const nibashared::sessionstate &session) {
    if (session.state != nibashared::gamestate::createchar)
        return false;
    if (!(player.gender == 'm' || player.gender == 'f'))
        return false;
    if (player.name.empty())
        return false;
    if (player.attrs.strength < 0 || player.attrs.dexterity < 0 || player.attrs.spirit < 0 ||
        player.attrs.physique < 0)
        return false;
    if (player.attrs.strength + player.attrs.dexterity + player.attrs.spirit +
            player.attrs.physique >
        5)
        return false;
    return true;
}

nlohmann::json nibashared::message_createchar::create_response() {
    if (success) {
        return {{"success", true}, {"player", player}, {"magics", magics}, {"equips", equips}};
    }
    return {{"success", false}};
}

nlohmann::json nibashared::message_createchar::create_request() {
    return {{"type", type}, {"player", player}};
}

void nibashared::message_createchar::merge_response(const nlohmann::json &j) {
    j.at("success").get_to(success);
    if (success) {
        j.at("player").get_to(player);
        j.at("magics").get_to(magics);
        j.at("equips").get_to(equips);
    }
}

void nibashared::message_createchar::from_request(const nlohmann::json &j) {
    j.at("player").get_to(player);
}
