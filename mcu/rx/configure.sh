#!/bin/bash

SCRIPT_DIR=$(cd `dirname $0` && pwd)

# Remove all, first
rm $SCRIPT_DIR/*.h
rm $SCRIPT_DIR/*.cpp

# Copy dependencies
# SCoop
cp $SCRIPT_DIR/../common/SCoop/SchedulerARMAVR/*.h .
cp $SCRIPT_DIR/../common/SCoop/SchedulerARMAVR/*.cpp .

# Copy common source code
cp $SCRIPT_DIR/../../common/*.h .
cp $SCRIPT_DIR/../../common/*.cpp .
