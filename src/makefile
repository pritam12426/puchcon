# -*-Makefile-*-

default: main.c pathData.txt
	@gcc ./main.c -o ~/Developer/system_script/hhpp
	@printf "\033[1;33mgcc\033[0m \033[1;36m./main.c\033[0m \033[1;33m-o\033[0m \033[1;35m~/Developer/system_script/hhpp\033[0m\n" 

update: lib/
	@echo [updating \"./lib/user_function.h\"]
	@curl https://raw.githubusercontent.com/pritam12426/systme-configuration/main/global/user_c_lib/user_function.h -o ./lib/user_function.h > /dev/null 2>&1
