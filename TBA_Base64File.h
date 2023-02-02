// TBA_Base64File.h
#ifndef TBA_Base64File_h
#define TBA_Base64File_h

#include <Arduino.h>

#include <FS.h>
#include <SD.h>

class TBA_Base64File {
  public:
    void encode(const uint8_t* input, size_t inputLength, char* output);
    size_t encodeLength(size_t inputLength);
  
    void decode(const char* input, uint8_t* output);
    size_t decodeLength(const char* input);
    
    // file methods
    static void decodeFile(fs::FS &fs, const char * path, const char * encoded);
    static int decodeFileLength(fs::FS &fs, const char * encoded);

  private:
};

#endif  // TBA_Base64File
