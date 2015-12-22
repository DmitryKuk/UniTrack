#!/bin/bash


mongo <<MONGO_JS_SCRIPT

use unitrack

db.users.createIndex({ "email": 1 }, { unique: true })
db.users.createIndex({ "ref": 1 }, { unique: true })

db.sessions.createIndex({ "session_id": 1 }, { unique: true })

MONGO_JS_SCRIPT
