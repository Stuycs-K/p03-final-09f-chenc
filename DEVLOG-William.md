# Dev Log:

This document must be updated daily every time you finish a work session.

## William

### 1/6/26

Set up the "skeleton" of the program. Basically just set up the server socket creation and handshake. Also added rudimentary handling of different requests (just GET requests for now), but that will be rewritten and expanded later. Took around ~2 hours, mostly due to writing small errors that broke everything later on.

### 1/7/26

Added the functionality of downloading files from the server. Along with that came an update to the testing page, which now has links to various files and a 404. Additionally, forking was added, so the server can handle more than one request at a time. Took ~1 hour, and still on schedule.

### 1/8/26

Started adding the function to create files on the server. Implemented a test button, and the server does get the data. However, the functionality remains broken, as getting the content length and delimiter is proving harder than expected. Sets me a day behind schedule, but this will be rectified over the weekend.

### 1/9/26

Mostly just continued the work from yesterday on bring the files onto the server. It now successfully gets the file size, and the bounds. The next step is to actually read from these.

### 1/10/26

Finally completed the file upload process. This included reading the file name, file delimiter, and actually calculating the file size correctly (ironically, the Content-Length tag is the length of the **entire** message body, not just the file). I am quite behind schedule now, but I am confident that I can get a MVP by Monday.

### 1/11/26

Completed the dynamic homepage, though it might need some CSS (will wait until after MVP to do that). The server now reads what in the current directory, differentiates between file and directory, and makes each of these it's own HTML tag. I am now back on schedule, as I have my MVP. 

### 1/12/26

Started work on supporting changing directories. The code now displays directories on the home page and redirects the executable to be in a different path, though something broke along the way and I have yet to find it.

### 1/13/26

Worked mainly on changing directories, though I might transition to working on deleting files instead. This is because it seems like moving from directory to directory is gong to be harder than I expected, as the browser seems to truncate and change requests automatically. For that, I would probably need to make drastic changes to my forking server, which I don't really want to do right now.
