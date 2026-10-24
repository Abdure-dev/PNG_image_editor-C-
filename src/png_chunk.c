#include <stdio.h>
#include <stdlib.h>



int pt_signature(FILE *f){

  unsigned char expected[8] = {0x89,0x50,0x4E,0x47,0x0D,0x0A,0x1A,0x0A}; 
  unsigned char buf[8];
  size_t n = fread(buf,1,8,f);
  if(n != 8){
    return 0;
  }
 for(int i  =0; i <8; i++){
  if(buf[i] != expected[i]){
    return 0;

 }
 }
  return 1;

}

uint32_t read_int(FILE *f){
    unsigned char b[4];
    (void)fread(b,1,4,f);
    uint32_t len = ((uint32_t)b[0] << 24)
                    |((uint32_t)b[1] << 16)
                    |((uint32_t)b[2] << 8)
                    |(uint32_t)b[3];
  
   return len;
}

int main(int argc, char *argv[]){
 if(argc !=2){
    fprintf(stderr,"usage: %s <file.png>\n", argv[0]);
    return 1;
 }
 FILE *f = fopen(argv[1], "rb");
 if(!f){
  perror(argv[1]);
      return  1;
 }
 int ok = pt_signature(f);
 printf("%s: %s\n",argv[1],ok? "valid PNG" : "not a PNG");
  if (ok) {
     printf("first chunk length: %u\n", read_int(f));
 }
 fclose(f);
 return ok?0:1;
}
