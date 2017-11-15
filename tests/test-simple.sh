#!/bin/sh

set -e

test_wseg()
{
  SCHEME=$1
  INPUT=$2
  EXPECT=$3

  echo -n "Testing ${SCHEME} scheme (${INPUT})... "

  RES=$(echo -n "${INPUT}" | ${SWATH} -m ${SCHEME} -d ${DICTDIR} -u u,u)

  if test "x${RES}" != "x${EXPECT}"; then
    echo "FAIL!"
    return 1
  fi
  echo "OK"

  return 0
}

INPUT="ทดสอบการตัดคำอย่างง่าย"
EXPECT="ทดสอบ|การ|ตัด|คำ|อย่าง|ง่าย"

test_wseg long ${INPUT} ${EXPECT}
test_wseg max ${INPUT} ${EXPECT}

test_wseg long "ไปหามเหสี" "ไป|หาม|เห|สี"
test_wseg max "ไปหามเหสี" "ไป|หา|มเหสี"
