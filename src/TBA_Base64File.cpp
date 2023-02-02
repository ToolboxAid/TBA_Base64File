/**/

// TBA_Base64File.cpp
#include "TBA_Base64File.h"

constexpr char alphaTable[64] = {
    0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
    0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
    0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76,
    0x77, 0x78, 0x79, 0x7A, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x2B, 0x2F
};

constexpr uint8_t charTable[128] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xFF, 0xFF, 0x3F,
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
    0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

void a3to4(uint8_t* a3, uint8_t* a4){
    a4[0] = (a3[0] & 0xFC) >> 2;
    a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xF0) >> 4);
    a4[2] = ((a3[1] & 0x0F) << 2) + ((a3[2] & 0xC0) >> 6);
    a4[3] = a3[2] & 0x3F;
}

void a4to3(uint8_t* a4, uint8_t* a3){
    a3[0] = (a4[0] << 2) + ((a4[1] & 0x30) >> 4);
    a3[1] = ((a4[1] & 0x0F) << 4) + ((a4[2] & 0x3C) >> 2);
    a3[2] = ((a4[2] & 0x03) << 6) + a4[3];
}

void TBA_Base64File::encode(const uint8_t* input, size_t inputLength, char* output){
    uint8_t i = 0;
    uint8_t a3[3];
    uint8_t a4[4];

    while(inputLength--){
        a3[i++] = *input++;

        if(i == 3){
            a3to4(a3, a4);

            for(i = 0; i < 4; i++){
                *output++ = alphaTable[a4[i]];
            }

            i = 0;
        }
    }

    if(i){
        for(uint8_t j = i; j < 3; j++){
            a3[j] = '\0';
        }

        a3to4(a3, a4);

        for(uint8_t j = 0; j < i + 1; j++){
            *output++ = alphaTable[a4[j]];
        }

        while(i++ < 3){
            *output++ = '=';
        }
    }

    *output = '\0';
}

size_t TBA_Base64File::encodeLength(size_t inputLength){
    return (inputLength + 2 - ((inputLength + 2) % 3)) / 3 * 4 + 1;
}

void TBA_Base64File::decode(const char* input, uint8_t* output){
    size_t inputLength = strlen(input);
    uint8_t i = 0;
    uint8_t a3[3];
    uint8_t a4[4];

    while(inputLength--){
        if(*input == '='){
            break;
        }

        a4[i++] = *input++;

        if(i == 4){
            for(i = 0; i < 4; i++){
                a4[i] = charTable[a4[i]];
            }

            a4to3(a4, a3);

            for(i = 0; i < 3; i++){
                *output++ = a3[i];
            }

            i = 0;
        }
    }

    if(i){
        for(uint8_t j = i; j < 4; j++){
            a4[j] = '\0';
        }

        for(uint8_t j = 0; j < 4; j++){
            a4[j] = charTable[a4[j]];
        }

        a4to3(a4, a3);

        for(uint8_t j = 0; j < i - 1; j++){
            *output++ = a3[j];
        }
    }
}

size_t TBA_Base64File::decodeLength(const char* input){
    size_t inputLength = strlen(input);
    uint8_t eq = 0;

    for(uint32_t i = inputLength - 1; input[i] == '='; i--){
        eq++;
    }

    return 6 * inputLength / 8 - eq;
}

/*
-> i, V, B, O, R, w, 0, K, G, g, o, A, A, A, A, N, S, U, h, E, U, g, A, A, A, B, A, A, A, A, A, Q, C, A, Y, A, A, A, A, f, 8, /, 9, h, A, A, A, C, m, U, l, E, Q, V, R, 4, A, b, V, T, S, 0, 8, T, U, R, S, e, v, S, t, W, 7, v, s, D, 3, M, j, O, x, E, R, j, E, z, S, N, m, r, h, g, Q, x, M, 0, k, Z, p, o, G, l, N, F, A, w, F, C, 2, v, B, q, F, W, 2, 1, l, r, b, 0, S, W, s, f, g, M, y, U, F, s, g, Q, p, g, x, O, W, m, I, f, d, G, Y, 6, n, f, d, M, p, x, B, C, Q, v, w, P, z, o, W, w, I, N, G, A, C, 0, 5, y, c, u, /, i, f, u, f, 7, z, j, n, f, h, a, C, r, D, J, P, J, d, N, P, t, d, j, +, z, W, C, x, G, g, 8, F, w, /, d, L, A, r, q, 6, u, a, w, C, M, o, u, i, o, p, m, n, +, 3, d, 3, d, b, x, 6, P, Z, 8, B, o, N, N, 6, 4, F, P, j, r, j, O, M, N, v, r, k, R, 3, P, D, a, G, 2, u, +, i, S, q, L, F, w, o, U, R, Y, V, X, V, l, Z, G, h, 4, a, G, H, o, M, 3, f, 2, e, 9, c, +, u, u, w, 9, K, 7, P, G, 2, +, d, +, S, 2, 9, c, t, p, 1, z, C, Z, n, L, A, x, s, X, E, r, 6, 3, r, a, c, 1, y, I, e, g, k, M, w, +, a, d, T, m, f, f, O, T, U, A, b, L, f, b, e, 4, P, T, Y, 0, h, 8, 8, h, 0, l, M, 4, 2, k, z, J, A, J, m, a, q, m, +, F, o, p, K, 3, O, t, e, G, 0, T, W, U, /, O, D, N, O, p, t, 7, 2, H, t, V, r, N, 7, /, V, 6, b, W, A, 2, J, 4, P, K, Z, D, K, 2, n, 7, l, M, F, s, t, G, s, I, 7, I, x, j, Q, 9, F, b, q, W, 0, h, Q, 5, r, L, J, U, Q, m, X, p, R, L, N, S, y, p, K, /, S, o, u, w, 3, 1, U, h, s, X, x, e, E, I, Q, o, g, i, B, T, E, E, E, Q, D, l, E, U, /, e, X, w, V, F, 1, q, 7, i, X, b, q, j, r, f, U, a, R, I, R, x, E, j, a, r, O, e, U, n, Q, 1, b, U, m, I, q, C, 0, q, o, e, g, p, M, l, S, i, M, N, J, 3, y, H, F, c, T, J, b, l, M, N, T, p, d, A, K, q, D, s, p, N, v, h, K, F, K, r, E, E, 5, G, s, i, F, 1, P, 5, V, l, w, m, K, 2, m, N, o, x, M, S, Q, y, 0, o, H, B, N, v, 6, 8, q, E, E, p, r, 7, /, s, l, e, r, V, Q, q, 6, X, q, 9, n, o, I, O, D, g, 4, C, e, p, F, g, o, 4, Q, v, b, b, m, s, k, t, A, k, E, 5, o, i, h, R, V, J, D, L, d, F, L, g, r, u, 4, D, x, V, w, k, f, R, 2, B, d, i, O, 7, +, 8, x, v, N, 8, Y, G, d, n, 5, w, N, E, k, u, T, s, /, v, 7, +, S, Z, H, i, Y, g, S, D, P, w, 7, T, X, G, k, T, 4, X, U, 1, C, k, s, v, i, M, 1, G, U, u, X, o, h, f, r, W, a, h, 4, J, e, Y, i, 4, c, 2, S, v, 1, W, r, 5, A, o, H, A, q, S, +, s, V, u, s, D, Y, B, i, 9, n, z, n, Q, T, n, U, b, h, U, M, j, L, w, R, n, f, 8, 9, x, 7, K, X, p, d, 8, j, 6, 5, O, j, z, w, K, O, j, u, T, E, r, U, 9, 5, a, /, 6, H, L, n, v, X, 5, f, M, /, P, e, a, G, 7, u, 9, u, Q, T, q, d, f, s, y, z, r, B, k, p, A, q, o, o, S, q, u, I, o, T, G, z, k, V, i, W, B, j, 0, i, S, 5, C, 8, W, i, +, N, g, a, /, 9, 0, 4, O, D, g, 4, E, M, c, x, x, 2, 6, f, Y, N, n, h, f, R, 1, B, f, S, 9, n, 7, A, C, I, u, i, i, A, A, w, w, D, L, /, X, 1, +, Q, F, r, O, V, y, e, c, p, s, N, t, +, G, /, i, f, O, 1, O, j, z, u, X, /, R, B, /, o, D, H, u, v, B, a, A, L, p, 4, g, 8, A, A, A, A, A, S, U, V, O, R, K, 5, C, Y, I, I, =, 
-> eqPos: 963   byteCount: 964   Encode Len: 722
*/
  int TBA_Base64File::decodeFileLength(fs::FS &fs, const char * encodedPath){
  boolean debugMethod = true;

  if (debugMethod){
    Serial.printf("1 Reading file : %s\n", encodedPath);
  }

  File file = fs.open(encodedPath);
  if(!file){
    if (debugMethod){
      Serial.println("Failed to open file for reading");
    }
    return -1;
  }

  int byteCount = 0;
  int eqPos = 0;
  if (debugMethod){
    Serial.print  ("Counting bytes: ");
  }

  while(file.available()){
    byte chr = file.read();
    if (chr == '='){// 61 is an equal sign (=)
      eqPos = byteCount;
    }
    byteCount++;
//    if (debugMethod){
//      Serial.write(chr);
//      Serial.print(", ");
//    }
  }
  file.close();

  if (debugMethod){
    Serial.print("eqPos: ");
    Serial.print(eqPos);
    Serial.print("   byteCount: ");
    Serial.print(byteCount);
    Serial.print("   Encode Len: ");  
    Serial.println(6 * byteCount / 8 - (byteCount - eqPos));
  }

  return 6 * byteCount / 8 - (byteCount - eqPos);
}

void TBA_Base64File::decodeFile(fs::FS &fs, const char * encodedPath, const char * pngPath){

  boolean debugMethod = true;
  uint8_t i = 0;
  byte a3[3];
  byte a4[4];
  
  int encodedLen =  TBA_Base64File::decodeFileLength(fs, encodedPath);

  if (debugMethod){
    Serial.printf("2 Creating file: %s\n", pngPath);
  }

//return;

//  File encodedFile = SD.open(encodedPath);
//  File pngFile = SD.open(pngPath,  FILE_WRITE);
  File encodedFile = fs.open(encodedPath);
  File pngFile = fs.open(pngPath,  FILE_WRITE);

  while (encodedFile.available()){
      byte chr = encodedFile.read();
      if(chr == '='){
          break;
      }

      a4[i++] = chr;

      if(i == 4){
          for(i = 0; i < 4; i++){
              a4[i] = charTable[a4[i]];
          }

          a4to3(a4, a3);

          for(i = 0; i < 3; i++){
              pngFile.write(a3[i]); 
//              if (debugMethod){
//                Serial.write(a3[i]);
//              }
          }

          i = 0;
      }
  }

  if(i){
      for(uint8_t j = i; j < 4; j++){
          a4[j] = '\0';
      }

      for(uint8_t j = 0; j < 4; j++){
          a4[j] = charTable[a4[j]];
      }

      a4to3(a4, a3);

      for(uint8_t j = 0; j < i - 1; j++){
          pngFile.write(a3[j]);  
//          if (debugMethod){
//            Serial.write(a3[j]);
//          }
      }
  }

  if (debugMethod){
    Serial.println("pngFile: flush & close");
  }
  pngFile.flush();
  pngFile.close();

  if (debugMethod){
    Serial.println("encodedFile: close");
  }
  encodedFile.close();
}
