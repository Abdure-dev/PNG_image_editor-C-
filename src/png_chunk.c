#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct dimenstion {
  uint32_t width;
  uint32_t height;
};

typedef struct dimenstion dimenstion;


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

char *read_tag(FILE *f){

    char *tag = malloc(5);
    if(!tag) return NULL;
    (void)fread(tag,1,4,f);
    tag[4] = '\0';
    return tag;

}

dimenstion *pt_dimesnion(FILE *f){
    if(!pt_signature(f)){
        return NULL;
    }
    (void)read_int(f);
    fseek(f,4,SEEK_CUR);
    dimenstion *wh = malloc(sizeof(dimenstion));
    if(!wh) return NULL;
     wh->width = read_int(f);
     wh->height = read_int(f);

    return wh;



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
 
        dimenstion *d = pt_dimesnion(f);
        if(d){
            printf("%u x %u\n", d->width, d->height);
            free(d);
        }
 fseek(f,0,SEEK_SET);
 int ok = pt_signature(f);
 printf("%s: %s\n",argv[1],ok? "valid PNG" : "not a PNG");
 
  if (ok) {
     while(1){

        uint32_t length = read_int(f);
        char *tag = read_tag(f);
        printf("%s %u\n", tag, length);
        int done = strcmp(tag, "IEND")==0;
        free(tag);
        if(done) break;
        if(feof(f)) break;
        fseek(f,length + 4, SEEK_CUR);
     }
 }
 fclose(f);
 return ok?0:1;
}
