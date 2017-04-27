#!/usr/bin

case $1 in
1)
	git status
;;

2)
	git commit -a -m"$2"
;;

3)
	git push
;;

4)
	git pull
;;

5)
	git add $2
;;

6)
	sh ./build.sh
;;

*)
	echo "\033[1;32m\t*--help\033[0m"
	echo "\033[1;32m\t1--status\033[0m"
	echo "\033[1;32m\t2--commit\033[0m"
	echo "\033[1;32m\t3--push\033[0m"
	echo "\033[1;32m\t4--pull\033[0m"
	echo "\033[1;32m\t5--add\033[0m"
	echo "\033[1;32m\t6--make\033[0m"
esac
