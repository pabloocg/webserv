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
		${addprefix ${NETWORK_PATH}, ServerInit.cpp}\
		${addprefix ${NETWORK_PATH}, ServerManageClients.cpp}\
		${addprefix ${NETWORK_PATH}, ServerRequest.cpp}\
		${addprefix ${NETWORK_PATH}, ServerC.cpp}\
		${addprefix ${NETWORK_PATH}, Pending_read.cpp}\
		${addprefix ${CONFIG_PATH}, Logger.cpp}\
		${addprefix ${UTILS_PATH}, Request.cpp}\
		${addprefix ${UTILS_PATH}, Pending_send.cpp}\
		${addprefix ${CONFIG_PATH}, Conf.cpp}

NAME = webserv

#--------------------Commands---------------------------#

CLANG = clang++

FLAGS = -Wall -Werror -Wextra

MKDIROBJS = @mkdir -p ${OBJS_PATH}


MKDIRLOGS = @mkdir -p ${LOGS_PATH}
LOGSFILES = @touch ${LOGS_PATH}error.log ${LOGS_PATH}access.log

OBJS = ${addprefix ${CORE_PATH}, ${SRCS:.cpp=.o}}

RM = rm -rf

#---------------------Orders---------------------------#

.cpp.o:
			${MKDIROBJS}
			${MKDIRLOGS}
			${LOGSFILES}
			${CLANG} ${FLAGS} -c $< -o ${addprefix ${OBJS_PATH}, ${notdir ${<:.cpp=.o}}}

$(NAME):	${OBJS}
			${CLANG} ${FLAGS} ${addprefix ${OBJS_PATH}, ${notdir ${OBJS}}} -o ${NAME}

run:
			./${NAME}

all:		$(NAME)

re:			fclean all

clean:
			${RM} ${TMP_PATH}

fclean:
			${RM} ${NAME}