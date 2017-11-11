#include "system_call.h"

/*
* halt (uint8_t status)
* hanlder for system call "halt"
* input: none
* output: none
* side effect: as description
*/
int32_t halt (uint8_t status)
{
	
}

/*
* execute (const uint8_t* command)
* hanlder for system call "execute"
* input: none
* output: none
* return:
*	-1: If command can not be executed
*	256: If the program dies by an exception
	0 to 255: If the program halts, as given by the halt()
* side effect: as description
*/
int32_t execute (const uint8_t* command){
	
	//null check of cmd
	if(command == NULL){
		return -1;
	}
	//Variable Initialization
	uint8_t file_name[MAX_NAME_LENGTH];
	uint8_t local_arg[MAX_LENGTH_ARG];
	uint8_t buffer[BUF_SIZE];

	uint32_t file_length = 0;
	uint32_t spaces = 0;
	uint32_t i = 0;
	uint32_t end_of_file_name = 0;
	uint32_t local_arg_start_flag = 0;
	//Parsing
	while(command[i] != '\0'){
		if(command[i] != ' ' && spaces == 0){
			file_name[i] = command[i];
		}
		else{
			end_of_file_name++;
			if(command[i] == ' ' && local_arg_start_flag == 0){
				spaces++;
			}
			if(end_of_file_name == 1){
				file_length = i;
				file_name[i] = '\0';
				end_of_file_name++;//Temp fix... Just to be on a safer side
			}
			if(command[i] != ' ' || local_arg_start_flag == 1){
				local_arg_start_flag = 1;
				local_arg[i - (file_length + spaces)] = command[i];
			}
		}
		i++;
	}

	local_arg[i - (file_length + spaces)] = '\0';
	if(spaces == 0){
		file_name[i] = '\0';
	}

	//Executable check
	
	/*read_file check*/
	if(read_file((int32_t*)file_name, buffer, 4) == -1){
		return -1;
	}

	/*string comparison between buf and magic numbers to make sure that file is executable or not*/
	if(strncmp((const int8_t*)buffer, (const int8_t*)magic_number, 4) != 0){
		return -1;
	}



}

/*
* read (int32_t fd, void* buf, int32_t nbytes)
* hanlder for system call "read"
* input: none
* output: none
* side effect: as description
*/
int32_t read (int32_t fd, void* buf, int32_t nbytes)
{
	
}

/*
* write (int32_t fd, const void* buf, int32_t nbytes)
* hanlder for system call "write"
* input: none
* output: none
* side effect: as description
*/
int32_t write (int32_t fd, const void* buf, int32_t nbytes)
{
	
}

/*
* open (const uint8_t* filename)
* hanlder for system call "open"
* input: none
* output: none
* side effect: as description
*/
int32_t open (const uint8_t* filename)
{
	
}

/*
* close (int32_t fd)
* hanlder for system call "close"
* input: none
* output: none
* side effect: as description
*/
int32_t close (int32_t fd)
{
	
}
