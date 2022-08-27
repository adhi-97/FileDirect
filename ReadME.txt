/*
    Problem Statement : Socket Programming III
    Author : Adarsh G Krishnan
    compiler : gcc
    operating system : Linux (UBUNTU 20.04)

*/

Server :
compile command : gcc server.c -o server
run command : ./server 8080

Client :
compile command : gcc client.c -o client
run command : ./client 8080


#Important Design Decisions Taken.

(1) Internal Data structure userInfo is used to store client status and client_records.txt  is used to send the current 
userInfo but it'll be updated only when we perform the /user request however internal data structure will be updated 
whenever a client exits.

(2) For distinguishing files and keeping track of the client and server files,place the client and server in separate folders 
and execute.But even if we keep both client and server in same folder we can keep track of the process since
I'm purposefully not deleting the temporary files.

(3) Following files/data structures are created at server and client side. (Input File at client : <filename>.txt)

                                Server                              Client                          Output

    (i)    /users           userInfo + client_records.txt                                     -->  Terminal
    (ii)   /files           FILEDATA + fileDetails.txt                                        -->  Terminal
    (iii)  /upload          tempUpload.txt/<filename>.txt       
    (iv)   /download                                              Current_Downloaded_File.txt --> <filename>.txt
    (v)    /invite          Invite                       
    (vi)   /read                                                  current_readFile.txt        --> Terminal
    (vii)  /insert          insertFileServer.txt                  current_insertedFile.txt    --> <filename>.txt + Terminal
    (viii) /delete          deleteFileServer.txt                  current_deletedFile.txt     --> <filename>.txt + Terminal
    (ix)   /exit

(4) Whenever a read collaborator is upgraded to edit collaborator , readCollaborator status not changed since it may help us to find if 
the client was upgraded or not to edit collaborator.

(5) In the FILEDATA structure we are using owner id & collaborator id as Socket_ID instead of CLIENT_ID.

(6) Downgrading of client from edit to read collaborator is not considered.

(7) If the server and client are kept in same folder the source txt file under execution will be deleted if a client exists since we are
asked to delete files of that client at server.So keep a copy of the original file with you.Here file is deleted because both of the server
and client side have same filename.

(8) REQUESTS AVAILABLE :

    1 : /users
    2 : /files
    3 : /upload <filename>
    4 : /download <filename>
    5 : /invite <filename> <client_id> <permission>  : Here client_id format is 1000#
    6 : /read <filename> <start_idx> <end_idx>
    7 : /insert <filename> <idx> <message>
    8 : /delete <filename> <start_idx> <end_idx>
    9 : /exit