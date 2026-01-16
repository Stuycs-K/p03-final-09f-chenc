[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/QfAIDTGi)
# TransferWeb

### Group Name: Groupless

William Chen, Pd 9

### Project Description:

This project aims to simplify the process of transferring one file from one computer to another computer by allowing direct computer to computer file transfer via the web browser.

### Instructions:

1. Download this repository and run "make compile" on one computer (this computer will be the *server*).
2. Move **both** the created executable and the Data folder to the directory of your choice. 
2. Run the created executable. Be sure that port ***38203*** is open. Be sure to remember the external IP of this computer.
3. Navigate to the IP of the server on the second computer (this computer will be the *client*) by using the web browser. You should see a webpage appear with the files in the directory on the server, and setup is done!

From here, you have three things you can do:

1. Upload files! Just select Choose Files, select a file from the browser's file picker, and then hit Send File. This will send the file to the server and automatically reload the page.
2. Download files from the server! Choose one of the files from the list, and click the download button. This will make the server send you the file.
3. Delete files from the server! Choose one of the files from the list, and click the delete button. This will make the server delete the file, and automatically reload the page.

Note that you are constrained to one directory with this program, and attempting to navigate out of the directory will likely not work. Additionally, this server was designed for use by one user at a time, so use by multiple people could lead to unintended results.