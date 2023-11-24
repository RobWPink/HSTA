void receiveEvent(int howMany){
  char line[32] = {'\0'};

  int i = 0;
  while(Wire.available()){
    char temp = (char)Wire.read();
    line[i++]= temp;
    if(temp == '\n'){break;}
  }

  String line_ = String(line);
  if(!timer[0]){timer[0] = millis();}
  if(millis() - timer[0] >= 1050 && timer[0]){
    Serial.print("recording:");
    Serial.println(line_);
    appendFile(SD,logFile,line_);
    timer[0] = millis();
  }

}

int latestLogNum(fs::FS &fs){
  int latest = 0;
  const char* rootdir= "/";
  File root = fs.open(rootdir);
  if(!root){
      Serial.println("Failed to open directory");
      return -1;
  }
  if(!root.isDirectory()){
      Serial.println("Not a directory");
      return -1;
  }

  File file = root.openNextFile();
  while(file){
    if(!file.isDirectory()){
      int curNum = 0;
      const char* name = file.name();
      char tempchar = name[strlen(name)-1];
      if(isDigit(tempchar)){
        curNum = (int)tempchar - '0';
      }
      if(curNum > latest){latest = curNum;}
    }
    file = root.openNextFile();
  }
  return latest;
}

String readFile(fs::FS &fs, String path,int lines){
  String out = "";
  int lineCnt = 0;

  File file = fs.open(path);
  if(!file){
    Serial.println("Failed to open file for reading");
    return "fail";
  }

  while(file.available()){
    char xx = file.read();
    if(xx == '\n'){lineCnt++;}
    out += xx;
    if(lineCnt >= lines){break;}
  }
  
  file.close();
  String out_ = String(out);
  return out_;
}
unsigned long long fileSize(fs::FS &fs, String path){
  File file = fs.open(path);
  if(file){
    unsigned long out = file.size();
    file.close();
    return out;
  }
  else{
    Serial.print("Unable to check file size.");
    return -1;
  }
}
int writeFile(fs::FS &fs, String path, String message){

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    return 0;
  }
  if(!file.print(message)){
    Serial.println("Write failed");
    file.close();
    return 0;
  }

  file.close();
  return 1;
}

int appendFile(fs::FS &fs, String path, String message){
  File file = fs.open(path, FILE_APPEND);
  if(!file){
    Serial.println("Failed to open file for appending");
    return 0;
  }
  if(!file.print(message)){
    Serial.println("Append failed");
    file.close();
    return 0;
  }
  file.close();
  return 1;
}

int renameFile(fs::FS &fs, String path1, String path2){
  if(!fs.rename(path1, path2)) {
    Serial.println("Rename failed");
    return 0;
  }
  return 1;
}
