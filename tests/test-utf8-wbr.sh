#!/bin/sh

#
# Test of UTF-8 word delimiter code
# Ref: https://github.com/tlwg/swath/issues/1
#

set -e

INPUT="ผมมีแฟนแล้วนะครับ"
EXPECT="ผม​มี​แฟน​แล้ว​นะ​ครับ"

RES=$(echo -n "${INPUT}" | ${SWATH} -d ${DICTDIR} -u u,u -b "​")

if test "x${RES}" != "x${EXPECT}"; then
  exit 1
fi
