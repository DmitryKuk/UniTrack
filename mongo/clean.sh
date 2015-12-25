#!/bin/bash


mongo <<MONGO_JS_SCRIPT

use unitrack

db.users.drop()
db.sessions.drop()

MONGO_JS_SCRIPT


./mongo/prepare.sh
