# cFTP : Simple FTP client

Simple command line File Transfer Protocol (FTP) client for browsing and downloading files from FTP servers.

## About

This application was developed as assignment for our exam of networks at the University of Strasbourg. As we had only two hours to code the whole project we resumed our work on this project several months after in order to get it into a more presentable and complete shape. Enjoy :-)!

## Build

It's very straightforward! Just use the following command:

`make`

## Usage

cFTP is an interactive application. Once you've launched it using `./bin/client` (from the repository's root and **without** any argument), you can start to type desired commands. You can get their complete listing and description by taping `help` in the application's prompt or see the lines below:

- `open IPADDR`: Opens new connection to the FTP server identified by the IPv4 address `IPADDR`.

- `close`: Closes currently opened connection.

- `help`: Shows this help message.

- `exit`: Closes currently opened connection and quits the application.

- `list "PATH"`: Gets listing of the directory under the path `PATH`.

- `delete "PATH"`: Deletes the file under the path `PATH` from the server without any confirmation.

- `dir`: Gets path of the current working directory on the server.

- `goto "PATH"`: Navigates to the path `PATH` on the server.

- `get "PATH"`: Retrieves the file under the path `PATH` from the server onto local computer.

- `passive`: Enters passive FTP mode.

- `active`: Enters active FTP mode.

- `debug on|off`: Shows/hides debugging informations on/from the standard output.

## Authors

[Marek Felsoci](mailto:marek.felsoci@etu.unistra.fr) and Aurélien Rausch. For more information, see the [AUTHORS](AUTHORS) file in this repostiory.

## License

This application is licensed under the terms of the MIT licence. For further information and complete license text see the [LICENSE](LICENSE) file in this repository.