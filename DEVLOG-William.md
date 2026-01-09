# Dev Log:

This document must be updated daily every time you finish a work session.

## William

### 1/6/26

Set up the "skeleton" of the program. Basically just set up the server socket creation and handshake. Also added rudimentary handling of different requests (just GET requests for now), but that will be rewritten and expanded later. Took around ~2 hours, mostly due to writing small errors that broke everything later on.

### 1/7/26

Added the functionality of downloading files from the server. Along with that came an update to the testing page, which now has links to various files and a 404. Additionally, forking was added, so the server can handle more than one request at a time. Took ~1 hour, and still on schedule.

### 1/8/26

Started adding the function to create files on the server. Implemented a test button, and the server does get the data. However, the functionality remains broken, as getting the content length and delimiter is proving harder than expected. Sets me a day behind schedule, but this will be rectified over the weekend.
