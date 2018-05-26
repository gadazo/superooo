#! /bin/bash

INPUT_DIR="tests"
MY_OUT_DIR="myOut"
OUT_BIN="dflow_calc"
LOG_FILE="log_file"

#returns the parameters for running the test.
#param: $1 = name of the test file.
function get_params {
	(( num_of_cmds=`cat $1 | wc -l` - 1 ))
	query=()
	for i in `seq 0 $num_of_cmds` ; do
		query+=("p${i}")
		query+=("u${i}")
		query+=("d${i}")
	done 
	echo  ${query[@]}
}

function main {
	echo -e '\e[93mconverting tests to UNIX format... \e[0m'
	dos2unix $INPUT_DIR/* 2> $LOG_FILE
	echo -e '\e[93mconverting source to UNIX format... \e[0m'
	dos2unix *.h *.c *.cpp 2> $LOG_FILE
	echo -e '\e[93mbuilding: \e[0m'
	make clean > $LOG_FILE
	make
	chmod 777 $OUT_BIN

	if [[  -e $MY_OUT_DIR ]]; then
		rm -rf $mkdir $MY_OUT_DIR
	fi
	mkdir $MY_OUT_DIR

	number_of_tests=`ls -l $INPUT_DIR | grep in* | wc -l`
	echo -e '\e[93mnumber of tests to run: ' ${number_of_tests} '\e[0m'

	for i in `seq 1 ${number_of_tests}` ; do
		test_name='example'${i}'.in'
		output_name='example'${i}'.out'
		testFile=$INPUT_DIR/$test_name		
		PARAMs=`get_params "$testFile"`

		./$OUT_BIN "$testFile" ${PARAMs} > $MY_OUT_DIR/'myout_'${i}
		if [ `diff $INPUT_DIR/$output_name $MY_OUT_DIR/'myout_'$i | wc -w` -eq 0 ] ; then 
			echo -e 'running test:' $test_name ' - \e[92mPassed!\e[0m'
		
		else 
			echo -e 'running test:' $test_name ' - \e[91mFailed.\e[0m'
		fi		
	done
	echo -e '\e[93mcleaning directory... Exiting.\e[0m '
	make clean > $LOG_FILE
	rm -rf $MY_OUT_DIR 
	rm -f $LOG_FILE	
}
main "$@"