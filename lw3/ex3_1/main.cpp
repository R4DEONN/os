#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/wait.h>

using namespace std;

int main() {
	int pipefd[2]; // Array to hold the pipe file descriptors
	pid_t cpid; // Variable to hold the child process ID
	char buffer[100]; // Buffer to hold the message

	// Create the pipe
	if (pipe(pipefd) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	// Create a child process
	cpid = fork();
	if (cpid == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if (cpid == 0) { // Child process
		close(pipefd[1]); // Close unused write end

		// Read from the pipe
		while (read(pipefd[0], buffer, sizeof(buffer)) > 0) {
			cout << "Child received: " << buffer << endl;
		}

		close(pipefd[0]); // Close read end
		_exit(EXIT_SUCCESS); // Exit child process
	} else { // Parent process
		close(pipefd[0]); // Close unused read end

		string input;
		while (true) {
			cout << "Enter a message (type 'exit' to quit): ";
			getline(cin, input);

			if (input == "exit") {
				break; // Exit loop if user types 'exit'
			}

			write(pipefd[1], input.c_str(), input.length() + 1); // Send message to child
		}

		close(pipefd[1]); // Close write end
		wait(nullptr); // Wait for child to finish
		cout << "Parent exiting." << endl;
	}

	return 0;
}