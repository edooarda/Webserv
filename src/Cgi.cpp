#include "../include/Cgi.hpp"

Cgi::Cgi()
{
	this->code_status = 200;
	this->del = false;
	this->get = false;
	this->post = false;
}

Cgi::~Cgi()
{

}

int Cgi::get_cgi_in(int pos)
{
	return this->cgi_in[pos];
}

int Cgi::get_cgi_out(int pos)
{
	return this->cgi_out[pos];
}

void Cgi::creating_cgi_env(Client &client) // TODO: REVIEW this function
{
	std::string method = client.get_Request("method");
	this->tmp_env.push_back("REQUEST_METHOD=" + method);
	this->tmp_env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	this->tmp_env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	this->tmp_env.push_back("SCRIPT_NAME=" + this->path);
	this->tmp_env.push_back("PATH_INFO=" + client.get_Request("url_path"));
	this->tmp_env.push_back("QUERY_STRING=" + client.get_Request("query_string"));

	if (method == "POST" && this->post == true)
	{
		std::string content_lenght = client.get_Request("content_length");
		if (!content_lenght.empty())
		{
			this->tmp_env.push_back("CONTENT_LENGTH=" + content_lenght);
		}
		std::string content_type = client.get_Request("content_type");
		if (!content_type.empty()) {
			this->tmp_env.push_back("CONTENT_TYPE=" + content_type);
		}
	}

	for (size_t i = 0; i < this->tmp_env.size(); ++i)
		this->env.push_back(const_cast<char*>(this->tmp_env[i].c_str()));
	this->env.push_back(nullptr);

	// for (size_t i = 0; i < this->env.size() && env[i] != nullptr; ++i) {
	// 	std::cout << "				" << this->env[i] << std::endl;
	// }
}

void Cgi::start_cgi(Location config)
{
	this->config_root = config.getRoot();
	std::vector<std::string> config_allowed_methods = config.get_methods();
	for (size_t i = 0; i < config_allowed_methods.size(); ++i)
	{
		if (config_allowed_methods[i] == "GET")
			this->get = true;
		if (config_allowed_methods[i] == "POST")
			this->post = true;
		if (config_allowed_methods[i] == "DELETE")
			this->del = true;
	}
	this->config_autoindex = config.getAutoindex();
	// if (config_autoindex == true && (client.get_Request("url_path").find(".py") == std::string::npos))
	// {
	// 	client.set_error_code("404");
	// 	return ;
	// }

	if (pipe(cgi_in) == -1 || pipe(cgi_out) == -1)
	{
		perror("Pipe");
		return ;
	}
}


void Cgi::run_cgi(Client& client)
{
	pid_t pid;

	pid = fork();
	if (pid == -1)
	{
		perror("Fork");
		return ;
	}

	if (pid == 0)
	{
		//child
		// dup2(this->cgi_in[READ], STDIN_FILENO);
		// close(this->cgi_in[WRITE]);
		dup2(this->cgi_out[WRITE], STDOUT_FILENO);
		dup2(this->cgi_out[WRITE], STDERR_FILENO); // adding error from script to the pipe, all the errors are going to print on the browser x.x
		close(this->cgi_out[WRITE]);
		close(this->cgi_out[READ]);

		this->path = this->config_root + client.get_Request("url_path");
		const char* cgi_program = path.c_str();
		const char* argv[] = {"/usr/bin/python3", cgi_program, nullptr};

		creating_cgi_env(client);
		execve(argv[0], const_cast<char* const*>(argv), env.data());
		perror("execve fail");
		exit(1);
	}
	else
	{
		//parent
		close(this->cgi_out[WRITE]); // TODO: return pipe for the write function 

		client.set_error_code("200");
		int status;
		int result = waitpid(pid, &status, WNOHANG);
		if (result > 0) {
			if (WIFEXITED(status)) {
				int exit_code = WEXITSTATUS(status);
				if (exit_code == 1) {
					client.set_error_code("500");
					std::cout << "error running cgi! exit code = 1" << std::endl;
				}
			} else if (WIFSIGNALED(status)) {
				int sig = WTERMSIG(status);
				client.set_error_code("500");
				std::cout << "CGI process was killed by signal: " << sig << std::endl;
			}
		}
	}
}

	void Cgi::set_code_status(int code)
	{
		this->code_status = code;
	}

	int Cgi::get_code_status() const
	{
		return (this->code_status);
	}