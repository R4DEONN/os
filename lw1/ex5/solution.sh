#!/usr/bin/env bash
set -e

test -d "out" && rm -r out # TODO: Каким образом test возвращает код ошибки

mkdir out

cd out

whoami > me.txt

cp me.txt metoo.txt

man wc > wchelp.txt

cat wchelp.txt

wc -l wchelp.txt | cut -d ' ' -f 1 > wchelp-lines.txt

tac wchelp.txt > wchelp-reversed.txt

cat wchelp.txt wchelp-reversed.txt me.txt metoo.txt wchelp-lines.txt > all.txt

tar -cf result.tar *.txt

gzip result.tar

cd ..

test -f "result.tar.gz" && rm result.tar.gz

mv out/result.tar.gz .

cd out
rm -r -f *
cd ../
rmdir out