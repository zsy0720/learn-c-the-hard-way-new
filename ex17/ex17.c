//this code is copied from https://github.com/oblique63/LCTHW-Exercises/blob/master/ex17.c
//try to read it again and write exegesis
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

//#define MAX_DATA 512
//#define MAX_ROWS 100

//创建一个结构体，包括四个元素，这个结构体就是用来储存输入的数据
struct Address {
  int id;
  int set;
  char *name;
  char *email;
};

//database用来储存address结构体以及声明参数max_data,max_rows
struct Database {
  int max_data;
  int max_rows;
  struct Address *rows;//这是一个结构体指针，用于储存
};

定义一个结构体用来读取文件
struct Connection {
  FILE *file;
  struct Database *db;
};

// Implemented using a single global database connection.
struct Connection *conn;

//打印address成员
void Address_print(struct Address *addr) {
  printf("%d %s %s\n", addr->id, addr->name, addr->email);
}

void Database_close();//在用之前要先声明避免出现warning

//出错时将错误信息打印出来
void die(const char *message) {
  if (errno) {
    perror(message);//error为1是说明出错
  }
  else {
    printf("ERROR: %s\n", message);
  }

  Database_close();
  exit(1);exit参数为1说明不正常返回
}


void Database_write() {
  rewind(conn->file);//将指针指向文件开头
  int rc = 1;
  
  // Take a chunk of data the size of 1 int, from the address
  // 'conn->db->max_data' points to, and write it to 'conn->file'.
  rc = fwrite(&conn->db->max_data, sizeof(int), 1, conn->file);
  

  // Repeat for 'max_rows'
  rc = fwrite(&conn->db->max_rows, sizeof(int), 1, conn->file);
//size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
  //*ptr:被写入对象数组地址/要写入文件的数组
 //size:写入每个元素大小
  //nmemb：写入元素个数
  //stream：写入文件地址
  //写入成功返回1 

  // Since 'rows' is stored in the struct as a pointer to an array of
  // undetermined length, we can't just dump it straight into a file, 
  // because all that will do is save the _address_ the pointer points
  // to. So instead, we have to save the each entry in it manually,
  // explicitly stating how big each one is along the way...

  // Calculate the size of each of the strings in the Address structs.
  int string_size = sizeof(char) * conn->db->max_data;

  int i = 0;
  for (i = 0; i < conn->db->max_rows; i++) {
      struct Address *addr = &conn->db->rows[i];

      // Same as with 'max_data' and 'max_rows', we're just taking an
      // 'int'-sized chunk of data from the address each member points
      // to, and saving it to the file.
      fwrite(&addr->id, sizeof(int), 1, conn->file);
      fwrite(&addr->set, sizeof(int), 1, conn->file);

      // Here however, since the string members are already pointers
      // to addresses, we don't need to get their addresses; all we
      // need is to know is how big they are (which we figured out
      // before entering the loop).
      fwrite(addr->name, string_size, 1, conn->file);
      rc = fwrite(addr->email, string_size, 1, conn->file);
  }

  if (rc != 1)
    die("Failed to write database.");

  rc = fflush(conn->file);
  if (rc == -1)
    die("Could not flush database.");
}


void Database_load() {
  int rc = 1;
  
  // Notice that the following 'read' operations are performed in the same order
  // that their 'write' analogs were in 'Database_write'...

  // Read in data from 'conn->file' that's the size of 1 int,
  // and store it at the address 'conn->db->max_data' points to.
  rc = fread(&conn->db->max_data, sizeof(int), 1, conn->file);
//size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
// ptr:读内容写入数组的地址
  //size：每个读元素的大小
  //nmemb：读元素的数量
  //stream：要读文件地址
  // Repeat for 'conn->db->max_rows'
  rc = fread(&conn->db->max_rows, sizeof(int), 1, conn->file); 

  if (rc != 1)
    die("Failed to load database.");

  
  // Since we don't know what size the rows array might be,
  // we need to calculate the amount of memory it needs,
  // so that we can allocate it and store stuff in there.
  int string_size = sizeof(char) * conn->db->max_data;
  int address_size = (sizeof(int) * 2) + (string_size * 2);
  int rows_size = address_size * conn->db->max_rows;
  conn->db->rows = malloc(rows_size);
  

  // 'rows' is an array of arbirtrarily sized structs, so we can't
  // just read it in one big chunk. Instead, we have to break it down 
  // and manually populate it by reading and storing each of the
  // fields for each Address.

  int i = 0;
  for (i = 0; i < conn->db->max_rows; i++) {
      struct Address *addr = &conn->db->rows[i];

      // Since the first 2 members of 'Address' aren't pointers,
      // just read in the int data and store it in the same addresses
      // as the members themselves.
      fread(&addr->id, sizeof(int), 1, conn->file);
      fread(&addr->set, sizeof(int), 1, conn->file);


      // For the strings however, since they are pointers (to char arrays),
      // we need to first allocate the memory that they're going to be pointing to,
      // so that we can use it to store the strings from the file.
      addr->name = malloc(string_size);
      fread(addr->name, string_size, 1, conn->file);

      addr->email = malloc(string_size);
      rc = fread(addr->email, string_size, 1, conn->file);
  }

  if (rc != 1)
    die("Failed to load database.");
}

void Database_create() {
  int max_data = conn->db->max_data;
  int max_rows = conn->db->max_rows;
  
  // Calculate, and allocate the amount of memory needed for the rows array.
  int string_size = sizeof(char) * max_data;
  int address_size = (sizeof(int) * 2) + (string_size * 2);
  int rows_size = address_size * max_rows;
  conn->db->rows = malloc(rows_size);
  
  int i = 0;
  for (i = 0; i < max_rows; i++) {
    // make an Address struct prototype to initialize it
    struct Address addr = {.id = i, .set = 0};

    // and don't forget to allocate the memory for the strings!
    addr.name = malloc(sizeof(char) * max_data);
    addr.email = malloc(sizeof(char) * max_data);
    conn->db->rows[i] = addr;
  }
}

void Database_set(int id, const char *name, const char *email) {

  int max_data = conn->db->max_data;
  struct Address *addr = &conn->db->rows[id];
  
  if (addr->set)
    die("Already set, delete it first.");
  
  addr->set = 1;
  
  // Get some memory to copy the strings into!
  addr->name = malloc(sizeof(char) * max_data);
  addr->email = malloc(sizeof(char) * max_data);

  // Bug: strncpy doesn't guarantee a null-terminated string
  char *res = strncpy(addr->name, name, max_data);
 // char *strncpy(char *dest, const char *src, size_t n)
//src:被复制的字符串
  //dest目标地址
  //n：复制的字符个数
  // Fix: append a null-terminator to the end of it
  addr->name[max_data-1] = '\0';

  if (!res)
    die("Name copy failed.");

  res = strncpy(addr->email, email, max_data);
  addr->email[max_data-1] = '\0';
  if (!res)
    die("Email copy failed.");
}


void Database_get(int id) {
  struct Address *addr = &conn->db->rows[id];
  if (addr->set)
    Address_print(addr);
  else
    die("ID is not set.");
}

void Database_find(char *term) {
    int i = 0;
    for (i = 0; i < conn->db->max_rows; i++) {
        struct Address *addr = &conn->db->rows[i];
        
        if (strcmp(addr->name, term) == 0 || strcmp(addr->email, term) == 0) {
            //int strcmp(const char *str1, const char *str2)
          //比较两个字符串大小
            Address_print(addr);
            return;
        }
    }
    
    printf("No records matching '%s'\n", term);
}

void Database_delete(int id) {
  struct Address addr = {.id = id, .set = 0};

  // Yep, need to allocate the memory for these strings here too;
  // just like we did when we first initialized all the Addresses.
  addr.name = malloc(sizeof(char) * conn->db->max_data);
  addr.email = malloc(sizeof(char) * conn->db->max_data);
  conn->db->rows[id] = addr;
}


void Database_list() {
  struct Database *db = conn->db;
  
  printf("MAX_DATA: %d\n", db->max_data);
  printf("MAX_ROWS: %d\n", db->max_rows);

  int i = 0;
  for (i = 0; i < db->max_rows; i++) {
    struct Address *cur = &db->rows[i];
    if (cur->set)
      Address_print(cur);
  }
}


void Database_open(const char *filename, char mode) {
  // Initialize the global connection
  conn = malloc(sizeof(struct Connection));
  if (!conn)
    die("Memory error.");
  
  conn->db = malloc(sizeof(struct Database));

  if (!conn->db)
    die("Memory error");

  if (mode == 'c') {
    conn->file = fopen(filename, "w");
   // FILE *fopen(const char *filename, const char *mode)
    //filename：文件名称
  //mode：打开模式。r：只读，且文件必须存在。w：创建一个用于写入的新文件，若文件已经存在则将内容全部删去。a：追加操作，向文件尾部写数据，若文件不存在则新建文件
                 //r+：用于更新的文件，可以写也可以读，文件必须存在。w+：用于读写的文件。a+：用于追加和读取的文件。
  }
  else {
    conn->file = fopen(filename, "r+");

    if (conn->file)
      Database_load();
  }
  
  if (!conn->file)
    die("Failed to open file.");
}


void Database_close() {
  if (conn) {

    // We malloc'd a whole lot of strings up until now,
    // so that means we gotta 'free' em too.
    int i = 0;
    for (i = 0; i < conn->db->max_rows; i++) {
        free(conn->db->rows[i].name);
        free(conn->db->rows[i].email);
    }

    // Not to mention the actual array they were stored in.
    if (conn->db->rows) free(conn->db->rows);

    if (conn->db)       free(conn->db);
    if (conn->file)     fclose(conn->file);
    free(conn);
  }
}



int main(int argc, char *argv[]) {
  if (argc < 3)
    die("USAGE: ex17 <dbfile> <action> [action params]");

  char *filename = argv[1];
  char action = argv[2][0];
  Database_open(filename, action);

  int id = 0;  
  if (action != 'c' && action != 'f' && argc > 3) {
    id = atoi(argv[3]);

    if (id >= conn->db->max_rows)
      die("There are not that many records.");
  }

  switch(action) {
    // Create 
    case 'c':
      if (argc != 5)
        die("MAX_DATA and MAX_ROWS required.");
      
      conn->db->max_data = atoi(argv[3]);
      conn->db->max_rows = atoi(argv[4]);
      Database_create();
      Database_write();
      break;

    // Get
    case 'g':
      if (argc != 4)
        die("Need an ID to get.");
      Database_get(id);
      break;

    // Set
    case 's':
      if (argc != 6)
        die("Need ID, name, and email to set.");
      Database_set(id, argv[4], argv[5]);
      Database_write();
      break;

    // Find
    case 'f':
      if (argc != 4)
        die("Need a name or email to search for.");
      Database_find(argv[3]);
      break;

    // Delete
    case 'd':
      if (argc != 4)
        die("Need ID to delete.");
      Database_delete(id);
      Database_write();
      break;

    // List
    case 'l':
      Database_list();
      break;

    default:
      die("Invalid action, only:  c=create,  g=get,  s=set,  d=delete,  l=list");
  }

  Database_close();
  return 0;
}
