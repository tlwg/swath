#!/bin/sh

#
# Test of UTF-8 word delimiter code
# Ref: https://github.com/tlwg/swath/issues/1
#

set -e

INPUT="ผมมีแฟนแล้วนะครับ"
EXPECT="ผม​มี​แฟน​แล้ว​นะ​ครับ"

echo -n "Testing maximal matching scheme... "

RES=$(echo -n "${INPUT}" | ${SWATH} -m max -d ${DICTDIR} -u u,u -b "​")

if test "x${RES}" != "x${EXPECT}"; then
  echo "FAIL!"
  exit 1
fi
echo "OK"


echo -n "Testing longest matching scheme... "

RES=$(echo -n "${INPUT}" | ${SWATH} -m long -d ${DICTDIR} -u u,u -b "​")

if test "x${RES}" != "x${EXPECT}"; then
  echo "FAIL!"
  exit 1
fi
echo "OK"

