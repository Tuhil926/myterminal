1) Setup:
	to compile this applications, you need the readline library installed
	on your computer.
	To install readline, run the command:
		sudo apt-get install libreadline8 libreadline-dev

2) How to compile the myterminal application:
	open a terminal in this directory(Assignment 2) and run the command:
		make
	then, run the command:
		./myterminal.out

	or, you can compile and run at the same time using:
		make run

3) The commands implemented be myterminal:
	I have implemented the following commands in my_terminal:
	  myls
	  myps
	  mycp
	  mymv
	  mygrep
	I have also made a man file for each of these commands,
	and implemented the "man" command to print these man files

	to see more info about these commands, run the application and enter:
		man <command name>

	Commands other than these are run using the system() function

	you can exit this terminal by entering the command:
		exit