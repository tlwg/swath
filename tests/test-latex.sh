#!/bin/sh

set -e

${SWATH} -d ${DICTDIR} -f latex -u u,u < ${srcdir}/thai-latex.tex \
  > thai-latex-out.tex

if ! cmp thai-latex-out.tex ${srcdir}/thai-latex-wseg.tex >/dev/null; then
  exit 1
fi

