#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct dimenstion {
  uint32_t width;
  uint32_t height;
};

typedef struct dimenstion dimenstion;

struct chunklist {
    uint32_t start_byte;
    uint32_t length;
    char *chunk_type;
    struct chunklist *next;



};

struct key_value{

  char *key;
  char *value;


};
typedef struct key_value key_value;

typedef struct chunklist chunklist;



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

chunklist *pt_map(FILE *f){
  if(!pt_signature(f)) return NULL;
  chunklist *head = NULL;
  chunklist *tail = NULL;
  while(1){
  long start_byte = ftell(f);
  uint32_t length = read_int(f);
  char *tag = read_tag(f);
  chunklist *node = malloc(sizeof(chunklist));
  if(!node) return NULL;
  node->start_byte = start_byte;
  node->length = length;
  node->chunk_type = tag;
  node->next = NULL;

  if(head == NULL) head = node;
  else tail->next = node;
  tail = node;
  if(strcmp(node->chunk_type, "IEND")==0) break;
  if(feof(f)) break;
  fseek(f,length + 4, SEEK_CUR);
  }
  return head;
}


key_value *extract_text(FILE *f, uint32_t n){
    if(!pt_signature(f)){
      return NULL;
        }
    uint32_t count = 0;
    while(1){
      uint32_t length = read_int(f);
      char *tag = read_tag(f);

      if(strcmp(tag, "tEXt")==0){
          if (count == n){
                char *buf = malloc(length + 1);
                if(!buf) return NULL;
                fread(buf,1,length,f);
                buf[length] = '\0';
                size_t  klen = 0;
                while(buf[klen] != '\0'){
                  klen++;
                }
                size_t vlen = length - klen - 1;
                char *key = malloc(klen + 1);
                if(!key) return NULL;
                memcpy(key, buf, klen);
                key[klen] = '\0';

                char *value = malloc(vlen + 1);
                if(!value) return NULL;
                memcpy(value, buf + klen + 1, vlen);
                value[vlen] = '\0';
                key_value *kv = malloc(sizeof(key_value));
                if(!kv) return NULL;
                kv->key = key;
                 kv->value = value;

  
                free(buf);

                return kv;

          }
        count++;


      }

   int done = strcmp (tag, "IEND") == 0;
   free(tag);
   if(done) break;
   if(feof(f)) break;
   fseek(f,length + 4, SEEK_CUR);

    }
   return NULL;

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
 chunklist *list = pt_map(f);
 for(chunklist *p = list; p !=NULL; p=p->next){

  printf("%u %s %u\n", p->start_byte, p->chunk_type, p->length);

 }
 fseek(f,0,SEEK_SET);

 key_value *kv = extract_text(f,0);
 if(kv){
  printf("%s = %s\n", kv->key, kv->value);
  free(kv->key);
  free(kv->value);
  free(kv);



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






