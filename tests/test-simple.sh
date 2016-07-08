#!/bin/sh

set -e

test_wseg()
{
  INPUT=$1
  EXPECT=$2

  RES=$(echo -n "${INPUT}" | ${SWATH} -d ${DICTDIR} -u u,u)

  if test "x${RES}" != "x${EXPECT}"; then
    return 1
  fi

  return 0
}

INPUT="ทดสอบการตัดคำอย่างง่าย"
EXPECT="ทดสอบ|การ|ตัด|คำ|อย่าง|ง่าย"

test_wseg ${INPUT} ${EXPECT}
