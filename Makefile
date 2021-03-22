# --------------------Paths----------------------------#

CORE_PATH = core/

CONFIG_PATH = config/

NETWORK_PATH = network/

REQUEST_PATH = request/

CLIENT_PATH = client/

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
		${addprefix ${CLIENT_PATH}, Client.cpp}\
		${addprefix ${REQUEST_PATH}, Request.cpp}\
		${addprefix ${REQUEST_PATH}, RequestAuth.cpp}\
		${addprefix ${REQUEST_PATH}, RequestCGI.cpp}\
		${addprefix ${REQUEST_PATH}, RequestMethods.cpp}\
		${addprefix ${REQUEST_PATH}, RequestUtils.cpp}\
		${addprefix ${CONFIG_PATH}, Conf.cpp}

NAME = webserv

#--------------------Commands---------------------------#

CLANG = clang++

FLAGS = -Wall -Werror -Wextra

MKDIROBJS = @mkdir -p ${OBJS_PATH}


MKDIRLOGS = @mkdir -p ${LOGS_PATH}
LOGSFILES = @touch ${LOGS_PATH}error.log ${LOGS_PATH}access.log

OBJS = ${addprefix ${CORE_PATH}, ${SRCS:.cpp=.o}}

OBJS_DEBUG = ${addprefix ${CORE_PATH}, ${SRCS:.cppd=.o}}

RM = rm -rf

DEBUG_FLAG = -D DEBUG_MODE

#---------------------Orders---------------------------#

.cpp.o:
			${MKDIROBJS}
			${MKDIRLOGS}
			${LOGSFILES}
			${CLANG} ${FLAGS} -c $< -o ${addprefix ${OBJS_PATH}, ${notdir ${<:.cpp=.o}}}

$(NAME):	${OBJS}
			${CLANG} ${FLAGS} ${addprefix ${OBJS_PATH}, ${notdir ${OBJS}}} -o ${NAME}

all:		$(NAME)

re:			fclean all

clean:
			${RM} ${TMP_PATH}

fclean:
			${RM} ${NAME}