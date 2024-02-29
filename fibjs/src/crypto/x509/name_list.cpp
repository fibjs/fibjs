/*
 * name_list.cpp
 *
 *  Created on: Fec 29, 2024
 *      Author: lion
 */

#include "crypto_util.h"

namespace fibjs {

const char* xfKeyUsages[8] = {
    "digitalSignature",
    "nonRepudiation",
    "keyEncipherment",
    "dataEncipherment",
    "keyAgreement",
    "keyCertSign",
    "cRLSign",
    "encipherOnly"
};

const char* xfCertTypes[8] = {
    "client",
    "server",
    "email",
    "objsign",
    "reserved",
    "sslCA",
    "emailCA",
    "objCA"
};

}
