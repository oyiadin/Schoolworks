#!/bin/sh

IN_DIR=inputs
OUT_DIR=outputs

rm -rf ${OUT_DIR}

mkdir -p ${OUT_DIR}

python3 main.py --debug --demo-fa > ${OUT_DIR}/log-demo-fa.txt

mkdir -p ${OUT_DIR}/lexical
python3 main.py --debug lexical  \
    --in ${IN_DIR}/lexical_input  \
    --out ${OUT_DIR}/lexical  \
    #> ${OUT_DIR}/log-lexical.txt

mkdir -p ${OUT_DIR}/LL1
python3 main.py --debug syntax  \
    --lang LL1  \
    --lexical-load-from ${OUT_DIR}/lexical  \
    --production-load-from ${IN_DIR}.grammar_input  \
    --out ${OUT_DIR}/LL1  \
    #> ${OUT_DIR}/log-LL1.txt

# mkdir -p ${OUT_DIR}/SLR1
python3 main.py --debug syntax  \
    --lang SLR1  \
    --lexical-load-from ${OUT_DIR}/lexical  \
    --production-load-from ${IN_DIR}.grammar_input  \
    --out ${OUT_DIR}  \
    #> ${OUT_DIR}/log-SLR1.txt

sleep 1

mv NFA.pdf ${OUT_DIR}/demo-NFA.pdf
mv DFA.pdf ${OUT_DIR}/demo-DFA.pdf
mv SLR1-DFA.pdf ${OUT_DIR}/SLR1-DFA.pdf

rm -rf NFA DFA SLR1-DFA
rm -rf __pycache__
rm -rf inputs/__pycache__
