/* 
 * SPLPv1.c
 * The file is part of practical task for System programming course. 
 * This file contains validation of SPLPv1 protocol. 
 */



/*
  Венедиктов Никита Валерьевич
  13 группа
*/



/*
---------------------------------------------------------------------------------------------------------------------------
# |      STATE      |         DESCRIPTION       |           ALLOWED MESSAGES            | NEW STATE | EXAMPLE
--+-----------------+---------------------------+---------------------------------------+-----------+----------------------
1 | INIT            | initial state             | A->B     CONNECT                      |     2     |
--+-----------------+---------------------------+---------------------------------------+-----------+----------------------
2 | CONNECTING      | client is waiting for con-| A<-B     CONNECT_OK                   |     3     |
  |                 | nection approval from srv |                                       |           |                      
--+-----------------+---------------------------+---------------------------------------+-----------+----------------------
3 | CONNECTED       | Connection is established | A->B     GET_VER                      |     4     |                     
  |                 |                           |        -------------------------------+-----------+----------------------
  |                 |                           |          One of the following:        |     5     |                      
  |                 |                           |          - GET_DATA                   |           |                      
  |                 |                           |          - GET_FILE                   |           |                      
  |                 |                           |          - GET_COMMAND                |           |
  |                 |                           |        -------------------------------+-----------+----------------------
  |                 |                           |          GET_B64                      |     6     |                      
  |                 |                           |        ------------------------------------------------------------------
  |                 |                           |          DISCONNECT                   |     7     |                                 
--+-----------------+---------------------------+---------------------------------------+-----------+----------------------
4 | WAITING_VER     | Client is waiting for     | A<-B     VERSION ver                  |     3     | VERSION 2                     
  |                 | server to provide version |          Where ver is an integer (>0) |           |                      
  |                 | information               |          value. Only a single space   |           |                      
  |                 |                           |          is allowed in the message    |           |                      
--+-----------------+---------------------------+---------------------------------------+-----------+----------------------
5 | WAITING_DATA    | Client is waiting for a   | A<-B     CMD data CMD                 |     3     | GET_DATA a GET_DATA 
  |                 | response from server      |                                       |           |                      
  |                 |                           |          CMD - command sent by the    |           |                      
  |                 |                           |           client in previous message  |           |                      
  |                 |                           |          data - string which contains |           |                      
  |                 |                           |           the following allowed cha-  |           |                      
  |                 |                           |           racters: small latin letter,|           |                     
  |                 |                           |           digits and '.'              |           |                      
--+-----------------+---------------------------+---------------------------------------+-----------+----------------------
6 | WAITING_B64_DATA| Client is waiting for a   | A<-B     B64: data                    |     3     | B64: SGVsbG8=                    
  |                 | response from server.     |          where data is a base64 string|           |                      
  |                 |                           |          only 1 space is allowed      |           |                      
--+-----------------+---------------------------+---------------------------------------+-----------+----------------------
7 | DISCONNECTING   | Client is waiting for     | A<-B     DISCONNECT_OK                |     1     |                      
  |                 | server to close the       |                                       |           |                      
  |                 | connection                |                                       |           |                      
---------------------------------------------------------------------------------------------------------------------------

IN CASE OF INVALID MESSAGE THE STATE SHOULD BE RESET TO 1 (INIT)

*/



#include "splpv1.h"
#include "string.h"
#include <stdlib.h>


 /* FUNCTION:  validate_message
 *
 * PURPOSE:
 *    This function is called for each SPLPv1 message between client
 *    and server
 *
 * PARAMETERS:
 *    msg - pointer to a structure which stores information about
 *    message
 *
 * RETURN VALUE:
 *    MESSAGE_VALID if the message is correct
 *    MESSAGE_INVALID if the message is incorrect or out of protocol
 *    state
 */


/*#include <iostream>   //для формирования массивов

int main()
{
	int data[256];
	int base64[256];

	for (size_t i = 0; i < 256; ++i)
	{
		if (i == 46 || i >= 48 && i <= 57 || i >= 97 && i <= 122)
			data[i] = 1;
		else
			data[i] = 0;
		if (i == 43 || i >= 47 && i <= 57 || i >= 65 && i <= 90 || i >= 97 && i <= 122)
			base64[i] = 1;
		else
			base64[i] = 0;
	}
	for (auto r : data)
	{
		std::cout << r << ", ";
	}

	std::cout << std::endl << std::endl;

	for (auto r : base64)
	{
		std::cout << r << ", ";
	}
}*/

int current_state = 1;

const char data[] =   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
const char base64[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int command_index;
const char* cmd_commands[] = { "GET_DATA", "GET_FILE", "GET_COMMAND" };
const int commands_lenghts[] = { 8, 8, 11 };
enum test_status invalid_message()
{
	current_state = 1;
	return MESSAGE_INVALID;
}
	
enum test_status valid_message(int cur_state)
{
	current_state = cur_state;
	return MESSAGE_VALID;
}

enum test_status validate_message(struct Message* msg)
{
	const char* message = msg->text_message;
	if (msg->direction == A_TO_B) {
	
		if (current_state == 1 && !strcmp(message, "CONNECT")) 
		{
			return valid_message(2);
		}
		else if (current_state == 3) 
		{
			if (!strcmp(message, "GET_VER")) 
			{
				return valid_message(4);
			}
			else if (!strcmp(message, "GET_DATA")) 
			{
				command_index = 0;
				return valid_message(5);
			}
			else if (!strcmp(message, "GET_FILE")) 
			{
				command_index = 1;
				return valid_message(5);
			}
			else if (!strcmp(message, "GET_COMMAND"))
			{
				command_index = 2;
				return valid_message(5);
			}
			else if (!strcmp(message, "GET_B64")) 
			{
				return valid_message(6);
			}
			else if (!strcmp(message, "DISCONNECT"))
			{
				return valid_message(7);
			}
		}
	}
	else {
		if (current_state == 4 && !strncmp(message, "VERSION ", 8)) 
		{
			message += 8;
			if (*message > 48 && *message < 58) 
			{
				for (++message; *message != '\0'; ++message)
				{
					if (*message < 48 || *message > 57)
					{
						return invalid_message();
					}
				}
				return valid_message(3);
			}
		}
		else if (current_state == 2 && !strcmp(message, "CONNECT_OK"))
		{
			return valid_message(3);
		}
		else if (current_state == 7 && !strcmp(message, "DISCONNECT_OK"))
		{
			return valid_message(1);
		}
		else if (current_state == 5) 
		{
			//int len = strlen(cmd_commands[command_index]);
			int len = commands_lenghts[command_index];
			if (!strncmp(message, cmd_commands[command_index], len))
			{
				message += len;
				if (*message != ' ')
				{
					return invalid_message();
				}
				++message;
				while(data[*message])
				{
					++message;
				}
				/*if (*message != ' ')
				{
					return invalid_message();
				}
			    ++message;*/
				if (*message == ' ' && !strcmp(++message, cmd_commands[command_index])) 
				{
					return valid_message(3);
				}
				
			}
		}
		else if (current_state == 6 && !strncmp(message, "B64: ", 5)) {
			message += 5;
			const char* start = message;
			while (base64[*message])
			{
				++message;
			}
			int lasts = 0;
			while (lasts < 2 && message[lasts] == '=')
			{
				++lasts;
			}
			if (!message[lasts] && (message - start + lasts) % 4 == 0)
			{
				return valid_message(3);
			}
			/*int len = strlen(message);
			if (len == 0 || len % 4 != 0)
			{
				return invalid_message();
			}
			if (message[len - 1] == 61)
			{
				--len;
				if (message[len - 1] == 61)
					--len;
			}
			while (base64[*message])
			{
				++message;
				--len;
			}
			if (len == 0)
			{
				return valid_message(3);
			}*/
		}
	}
	return invalid_message();
}