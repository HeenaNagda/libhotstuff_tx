/**
 * Copyright 2018 VMware
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _HOTSTUFF_CLIENT_H
#define _HOTSTUFF_CLIENT_H

#include "salticidae/msg.h"
#include "hotstuff/type.h"
#include "hotstuff/entity.h"
#include "hotstuff/consensus.h"

#define HOTSTUFF_CMD_REQSIZE 50

namespace hotstuff {

struct MsgReqCmd {
    static const opcode_t opcode = 0x4;
    DataStream serialized;
    command_t cmd;
    MsgReqCmd(const Command &cmd) { serialized << cmd; }
    MsgReqCmd(DataStream &&s): serialized(std::move(s)) {}
};

struct MsgRespCmd {
    static const opcode_t opcode = 0x5;
    DataStream serialized;
#if HOTSTUFF_CMD_RESPSIZE > 0
    uint8_t payload[HOTSTUFF_CMD_RESPSIZE];
#endif
    Finality fin;
    MsgRespCmd(const Finality &fin) {
        serialized << fin;
#if HOTSTUFF_CMD_RESPSIZE > 0
        serialized.put_data(payload, payload + sizeof(payload));
#endif
    }
    MsgRespCmd(DataStream &&s) {
        s >> fin;
    }
};

//#ifdef HOTSTUFF_AUTOCLI
//struct MsgDemandCmd {
//    static const opcode_t opcode = 0x6;
//    DataStream serialized;
//    size_t ncmd;
//    MsgDemandCmd(size_t ncmd) { serialized << ncmd; }
//    MsgDemandCmd(DataStream &&s) { s >> ncmd; }
//};
//#endif

class CommandDummy: public Command {
    uint32_t cid;
    uint32_t n;
    uint256_t hash;
#if HOTSTUFF_CMD_REQSIZE > 0
    size_t payload_size;
    // uint64_t payload[HOTSTUFF_CMD_REQSIZE];
    uint64_t *payload;
#endif

    public:
    CommandDummy() {}
    ~CommandDummy() override {}

    CommandDummy(uint32_t cid, uint32_t n):
        cid(cid), n(n), hash(salticidae::get_hash(*this)) {}

    CommandDummy(uint32_t cid, uint32_t n, std::vector<uint64_t> payload):
        cid(cid), n(n), hash(salticidae::get_hash(*this)) {
            payload_size = payload.size();
            this->payload = new uint64_t[payload_size];
            for(size_t i=0; i<payload_size; i++){
                this->payload[i] = payload[i];
            }
    }

    void serialize(DataStream &s) const override {
        s << cid << n;
#if HOTSTUFF_CMD_REQSIZE > 0
        // s.put_data(payload, payload + sizeof(payload));
        s << payload_size;
        for(size_t i=0; i<payload_size; i++){
            s << this->payload[i];
        }
#endif
    }

    void unserialize(DataStream &s) override {
        s >> cid >> n;
#if HOTSTUFF_CMD_REQSIZE > 0
        // auto base = s.get_data_inplace(HOTSTUFF_CMD_REQSIZE);
        // memmove(payload, base, sizeof(payload));
        s >> payload_size;
        payload = new uint64_t[payload_size];
        for(size_t i=0; i<payload_size; i++){
            s >> payload[i];
        }
#endif
        hash = salticidae::get_hash(*this);
    }

    const uint256_t &get_hash() const override {
        return hash;
    }

    size_t get_payload_size() {
        return payload_size;
    }

    const uint64_t* get_payload(){
        return payload;
    }

    bool verify() const override {
        return true;
    }
};

}

#endif
