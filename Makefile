# *********************************************
# Author : Kilian SEZNEC
# File : Makefile
# *********************************************

NAME = slowloris

SRCDIR = .
OBJDIR = .

SRC = slowloris.c

LIB = 
INC = -I -Iinclude/

OBJ = $(SRC:.c=.o)
CC = /usr/bin/gcc
CFLAGS = -O2
CDEBUG= -W -Wall -ansi -pedantic -g -ggdb -E
RM  = rm -f
ECHO  = echo -e

$(NAME) : $(OBJ)
	@$(CC) $(INC) $(CFLAGS) -o $(NAME) $(OBJDIR)/$(OBJ) $(LIB)
	@$(ECHO) '\033[0;32m> Compiled\033[0m'

clean :
	-@$(RM) $(OBJ)
	-@$(RM) *~
	-@$(RM) #*#
	@$(ECHO) '\033[0;35m> Directory cleaned\033[0m'

all : $(NAME)

fclean : clean
	-@$(RM) $(NAME)
	@$(ECHO) '\033[0;35m> Remove executable\033[0m'

re : fclean all

.PHONY : all clean re

debug : $(OBJ)
	@$(CC) $(INC) $(CDEBUG) -o $(NAME) $(OBJDIR)/$(OBJ) $(LIB)
	@$(ECHO) '\033[0;32m> Mode Debug: done\033[0m'

.c.o : 
	$(CC) $(INC) $(CFLAGS) -o $(OBJDIR)/$@ -c $<
