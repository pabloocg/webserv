[![License][license-logo]][license-url]

<br />
<p align="center">
  <h2 align="center">Webserv</h2>

  <p align="center">
    This is when you finally understand why a url starts with HTTP
    <br />
    <a href="https://github.com/pabloocg/webserv/tree/master/core"><strong>Explore the docs »</strong></a>
  </p>
</p>

<!-- TABLE OF CONTENTS -->
<details open="open">
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
  </ol>
</details>

---

## About The Project

This project is here to make you write your own HTTP server. You will follow the real HTTP RFC and you will be able to test it with a real browser. HTTP is one of the most used protocol on internet. Knowing its arcane will be useful, even if you won’t be working on a website.

---

### Madatory Part

| Program name     	| webserv                                                                       	|
|------------------	|-------------------------------------------------------------------------------------	|
| Makefile         	| yes                                                     	|
| External functs. 	| malloc, free, write, open, read, close, mkdir,rmdir, unlink, fork, wait, waitpid, wait3, wait4,signal, kill, exit, getcwd, chdir, stat, lstat,fstat, lseek, opendir, readdir, closedir, execve,dup, dup2, pipe, strerror, errno, gettimeofday,strptime, strftime, usleep, select, socket, accept,listen, send, recv, bind, connect, inet_addr,setsockopt, getsockname, fcntl                              	|
| Libft authorized 	| yes                                                                                 	|
| Description      	| Write a HTTP server in C++	|


---

Compliant with the rfc 7230 to 7235 (http 1.1) but only the following headers are implemented:

- Accept-Charsets, Accept-Language, Allow, Authorization, Content-Language, Content-Length, Content-Location, Content-Type, Date, Host, Last-Modified, Location, Referer, Retry-After, Server, Transfer-Encoding, User-Agent, WWW-Authenticate.

## Getting Started

### Installation

1. Clone the repository
    ```sh
    git clone https://github.com/pabloocg/webserv.git
    ```
2. Enter the repository and run make
    ```sh
    cd webserv && make
    ```

### Configuration

The program receives as argument a configuration file with the specifications to be launched. </br>
If no configuration file is offered, a default configuration file is taken. </br>
The configuration file is very similar to the nginx configuration.
The main parameters to configure and launch a server are:

1. Choose the port and host of each "server"
    ```
    listen       8080;
    server_addr 127.0.0.1;
    ```

2. Setup the server_names or not
    ```
    server_name  localhost;
    ```

3. Setup custom error pages (if not specified, they are taken as the default.)
    ```
    error_page  404              error_pages/404.html;
    error_page  403              error_pages/403.html;
    error_page  401              error_pages/401.html;
    error_page  500 502 503 504  error_pages/50x.html;
    ```

4. Setup routes with one or multiple of the following rules/configuration.
    ```
    location / {
        http_methods GET HEAD POST PUT OPTIONS DELETE;
        root   www/;
        index  index.html index.htm;
        upload on;
        path_upload www/uploads;
        auth_basic off;
        languages en-US es-es;
    }

    location /test/ {
        http_methods GET HEAD;
        root   www/test/;
        index  test.html index.html;
        auth_basic "Admin credentials";
        auth_basic_user_file auth/.htpasswd;
    }
    ```

5. Configure a CGI.
    ```
    location ~ \.php$ {
        http_methods    GET POST;
        cgi_exec        /path-to/bin/php-cgi;
        root            www/;
        index           index.php;
    }
    ```
You can see different examples of configuration files [here](https://github.com/pabloocg/webserv/tree/master/tests).
## Usage

The configuration file provided as argument is optional.
```
./webserv tests/example.conf
```
Now with your favourite web browser you can access the address and port where you have set the server.

## License

Distributed under the GNU GPLv3. See `LICENSE` for more information.

## Contact

Pablo Cuadrado García </br>
[Linkedin][linkedin-url] - pablocuadrado97@gmail.com

[license-logo]: https://img.shields.io/cran/l/devtools?style=for-the-badge
[license-url]: https://github.com/pabloocg/webserv/LICENSE
[linkedin-url]: https://linkedin.com/in/pablo-cuadrado97

