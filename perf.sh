#1/bin/bash
#set -x
ROOT=${PWD}
DATA_DIR=${PWD}/data
LOG_DIR=${PWD}/log
TEST_FILE=${PWD}/test.cpp
RESULT_FILE=/projects/student/result.txt

FILES=$(ls $DATA_DIR)
for filename in $FILES
do
    echo $filename
    `sed -i "s!cf\.loadTestData(\"\.\/data\/.*\/test_data.txt\")!cf\.loadTestData(\"\.\/data\/${filename}\/test_data.txt\")!g" ${TEST_FILE} `
    sh ${PWD}/mkrun | tee ${LOG_DIR}/log_${filename}
    diff  ${RESULT_FILE} ${DATA_DIR}/${filename}/result.txt > /dev/null
    if [ $? == 0 ]; then
        echo "******************Pass***********************"
    else
        echo "******************Failed*********************"
        vimdiff ${RESULT_FILE} ${DATA_DIR}/${filename}/result.txt
    fi
done
