# DMMRR
Dota Match Making Rank Recorder used to locally keep track of MMR for DoTA 2

Dependencies:
 - Tesseract OCR
 - Leptonica - (and additional required libs such as LibTiff, LibJpeg, etc)
 - clip (it's part of the /external folder in this case) TODO: set as a submodule

Usage:
 - Open DMMRR, you can verify that it is open by checking task manager
 - Ctrl + D + PrintScreen(PrtScn) will take a screen shot when the screen is on the Profile -> Stats page ( where the Solo and Party MMR are shown in)
 - Ctrl + D + Q will close the application

 Todo:
 - DMMRR currently does not have a GUI or any sort of interface
    - Add simple pop up boxes to show that the program has started / taken a screenshot / quit / show last mmr values
 - Currently hit and miss, working on some systems, not working in others, some more testing overall is required.

 - Omar Martinez