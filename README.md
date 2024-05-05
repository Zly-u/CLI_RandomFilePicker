# CLI Random File Picker
Just a small random thing I thought would be interesting to do since I have never done WinAPI stuff before.
Made me learn about whack of unicode and that C++ has second type of entry for unicode stuffos.

# Usages

### 1. Drag and Drop Batch script
  Make a `.bat` file out of this lil snippet and then drag and drop desired directories on the `.bat` file.
  To change the amount of files it supposed to pick just change the amount in `AMOUNT_OF_FILES` variable of this script.
  ```batch
  @echo off
  set AMOUNT_OF_FILES=10
  CLI_RandomSourcesPicker.exe %AMOUNT_OF_FILES% %*
  pause
  ```

### 2. Terminal/CMD
  `app.exe <AmountOfFilesToPick> <Path1> [Path2] [Path3] ...`

### 3. Just run it
  It will tell you what to do step by step.
