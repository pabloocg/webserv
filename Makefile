# --------------------Paths----------------------------#

CORE_PATH = core/

CONFIG_PATH = config/

NETWORK_PATH = network/

UTILS_PATH = utils/

TMP_PATH = tmp/

OBJS_PATH = tmp/objs/

LOGS_PATH = tmp/logs/

#--------------------Files-----------------------------#

SRCS =	main.cpp\
		${addprefix ${NETWORK_PATH}, Server.cpp}\
		${addprefix ${CONFIG_PATH}, Conf.cpp}

NAME = webserv

#--------------------Commands---------------------------#

CLANG = clang++

FLAGS = -Wall -Werror -Wextra

MKDIROBJS = @mkdir -p ${OBJS_PATH}

MKDIRLOGS = @mkdir -p ${LOGS_PATH}

OBJS = ${addprefix ${CORE_PATH}, ${SRCS:.cpp=.o}}

RM = rm -f

#---------------------Orders---------------------------#

.cpp.o:
			${MKDIROBJS}
			${MKDIRLOGS}
			${CLANG} ${FLAGS} -c $< -o ${addprefix ${OBJS_PATH}, ${notdir ${<:.cpp=.o}}}

$(NAME):	${OBJS}
			${CLANG} ${FLAGS} ${addprefix ${OBJS_PATH}, ${notdir ${OBJS}}} -o ${NAME}

run:
			./${NAME}

all:		$(NAME)

re:			fclean all

clean:
			${RM} ${addprefix ${OBJS_PATH}, ${notdir ${OBJS}}}

fclean:
			${RM} ${NAME}