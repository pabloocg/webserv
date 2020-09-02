NAME = webserv

SRCS = webserv.cpp \
	wsv_wrapper/wsv_wrapper.cpp \
	wsv_error/wsv_error_log.cpp \
	wsv_hash/wsv_hash.cpp \
	wsv_tcp/wsv_tcp_socket.cpp \
	wsv_tcp/wsv_tcp_multiplexing.cpp \
	wsv_client/wsv_client.cpp \
	wsv_http_message/wsv_http_message.cpp \
	wsv_http_message/wsv_http_header_field_load.cpp \
	wsv_http_message/wsv_http_header_field_parse.cpp \
	wsv_http_message/wsv_http_request_line_parse.cpp \
	wsv_http_message/wsv_http_request_read.cpp \
	wsv_http_message/wsv_http_request_host.cpp \
	wsv_http_message/wsv_http_uri_parse.cpp \
	wsv_http_message/wsv_http_response_write.cpp \
	wsv_http_message/wsv_http_response_status_line.cpp \
	wsv_http_message/wsv_http_response_headers.cpp \
	wsv_http_message/wsv_http_response.cpp \
	wsv_http_message/wsv_http_response_error.cpp \
	wsv_http_message/wsv_http_payload_read.cpp \
	wsv_http_message/wsv_http_payload_parse.cpp \
	wsv_http_method/wsv_http_method_handler.cpp \
	wsv_http_method/wsv_http_method_get.cpp \
	wsv_http_method/wsv_http_method_head.cpp \
	wsv_http_method/wsv_http_method_put.cpp \
	wsv_http_method/wsv_http_method_options.cpp \
	wsv_http_method/wsv_http_method_trace.cpp \
	wsv_http_method/wsv_http_method_delete.cpp \
	wsv_http_method/wsv_http_method_cgi.cpp \
	wsv_http_method/wsv_http_method_cgi_no_body.cpp \
	wsv_http_method/wsv_http_method_target_ressource.cpp \
	wsv_http_header/wsv_http_header.cpp \
	wsv_mime_type/wsv_mime_type.cpp \
	wsv_internet_protocol/wsv_ip_text_form.cpp \
	wsv_internet_protocol/wsv_ip_binary_form.cpp \
	wsv_internet_protocol/wsv_port.cpp \
	wsv_time/wsv_time.cpp \
	wsv_string/wsv_str_cmp_n.cpp \
	wsv_string/wsv_str_cpy_n.cpp \
	wsv_string/wsv_string.cpp \
	wsv_linked_list/wsv_linked_list.cpp \
	wsv_location/wsv_location.cpp \
	wsv_configuration/wsv_conf_block_limit_except.cpp \
	wsv_configuration/wsv_conf_block_server.cpp \
	wsv_configuration/wsv_conf_process.cpp \
	wsv_configuration/wsv_configuration.cpp \
	wsv_configuration/wsv_conf_block_location.cpp \
	wsv_configuration/wsv_conf_parse.cpp \
	wsv_configuration/wsv_conf_read.cpp \
	wsv_configuration/wsv_conf_directive_error_page.cpp \
	wsv_configuration/wsv_conf_directive_listen.cpp \
	wsv_configuration/wsv_conf_directive_size.cpp \
	wsv_configuration/wsv_conf_directive_string.cpp \
	wsv_cgi/wsv_cgi.cpp
SRCS_DIR = srcs/

OBJS = $(notdir $(SRCS:.cpp=.o))
OBJS_DIR = objs/
OBJS_PATH = $(addprefix $(OBJS_DIR), $(OBJS))

FLAGS = -g3 -Wall -Wextra -Werror -Wconversion -Wno-sign-conversion

INCS = . \
	wsv_wrapper \
	wsv_error \
	wsv_hash \
	wsv_tcp \
	wsv_client \
	wsv_http_message \
	wsv_http_method \
	wsv_http_header \
	wsv_mime_type \
	wsv_internet_protocol \
	wsv_time \
	wsv_string \
	wsv_linked_list \
	wsv_location \
	wsv_configuration \
	wsv_cgi
INCS_PATH = $(addprefix $(SRCS_DIR), $(INCS))
INCLUDES = $(addprefix -I, $(INCS_PATH))

LIBS =

DEP_DIR = .depend/
DEP_FILES := $(addprefix $(DEP_DIR), $(notdir $(SRCS:.cpp=.d)))

CPP = clang++

#####################
# binary/executable #
.PHONY : all
all : $(NAME)

$(NAME) : $(OBJS_PATH)
	@$(CPP) $(FLAGS) $(OBJS_PATH) $(LIBS) -o $@
	@/bin/echo "===> binary $@ created <==="

###########
# objects #
$(OBJS_DIR)%.o : $(SRCS_DIR)%.cpp $(DEP_DIR)%.d | $(OBJS_DIR) $(DEP_DIR)
	@$(CPP) $(FLAGS) $(INCLUDES) -MMD -MT $@ -MF $(DEP_DIR)$*.d -c -o $@ $<
	@/bin/echo "-> $< compiled"

$(OBJS_DIR)%.o : $(SRCS_DIR)*/%.cpp $(DEP_DIR)%.d | $(OBJS_DIR) $(DEP_DIR)
	@$(CPP) $(FLAGS) $(INCLUDES) -MMD -MT $@ -MF $(DEP_DIR)$*.d -c -o $@ $<
	@/bin/echo "-> $< compiled"

$(OBJS_DIR) : ; @/bin/mkdir $@
	@/bin/echo "> directory $@ created"

################
# dependencies #
$(DEP_DIR) : ; @/bin/mkdir $@
	@/bin/echo "> directory $@ created"

$(DEP_FILES) :

include $(DEP_FILES)

#############
# utilities #
.PHONY : clean
clean :
	@/bin/rm -f $(OBJS_PATH)
	@/bin/echo "---> object files removed"

.PHONY : fclean
fclean : clean
	@/bin/rm -f $(NAME)
	@/bin/echo "---> binary $(NAME) removed"

.PHONY : dclean
dclean :
	@/bin/rm -f $(DEP_FILES)
	@/bin/echo "---> dependency files removed"

.PHONY : clear
clear : dclean fclean
	@/bin/rm -rf $(DEP_DIR) $(OBJS_DIR)
	@/bin/echo "---> dependencies, objects directories removed"
	@/bin/rm -rf *.log
	@/bin/echo "---> log file(s) removed"

.PHONY : re
re : dclean fclean all
