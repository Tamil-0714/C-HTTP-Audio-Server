## Keep the audio file inside a folder "sounds" <br/> `char *audio_path = "sounds/";` <br/> then compile and run the 'updateDatabase.c' file `gcc -o updateDB updateDatabase.c` and `./updateDB` , this will create a SQLite DB file called `file_data.db` that contains the audio files meta data 

# After that run the main server

`gcc clrfnc.c dbfnc.c fileio.c stdfnc.c main.c -o MainServer -lsqlite3` <br />
`./MainServer`

 
`if (strcmp(req->route, "/files") == 0)` this route <b>/files</b> respond back a JSON contains the audio file id and file name , <br/>

`[{"FA37jNCchRYdSBZA":"file name"}]`

use this route <b>/audio/:id</b> , the id is from the JSON and this route will respond back with base64 encoded string of audio file corresponded to the audio id  


