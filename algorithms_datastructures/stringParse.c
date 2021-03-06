/*

 C 标准库
 https://www.runoob.com/cprogramming/c-standard-library-time-h.html

 C语言字符串操作总结超精细快收藏
https://baijiahao.baidu.com/s?id=1594930486715512180&wfr=spider&for=pc

https://blog.csdn.net/qq_15821725/article/details/78929344  C/C++文件读写操作——FILE*、fstream

//////c语言的split字符串分割函数strtok的使用////////////

说起来很有意思，自认为对C语言理解得还是比较深刻的。但居然到今天才知道有个strtok函数，试用了一下突然感慨以前做了多少重复劳动。每次需要解析配置文件，每次需要分割字符串，居然都是自己去分割字符串，既累人又容易出错。感概继续学得不够全面啊！这里引用一段strtok用法：

c语言也有类似其他语言的split字符串分割函数，就是strtok

头文件：#include <string.h>

定义函数：char * strtok(char *s, const char *delim);

函数说明：strtok()用来将字符串分割成一个个片段。参数s 指向欲分割的字符串，参数delim 则为分割字符串，当strtok()在参数s 的字符串中发现到参数delim 的分割字符时则会将该字符改为\0 字符。在第一次调用时，strtok()必需给予参数s 字符串，往后的调用则将参数s 设置成NULL。每次调用成功则返回下一个分割后的字符串指针。

返回值：返回下一个分割后的字符串指针，如果已无从分割则返回NULL。


 */
#include<string.h>
#include<stdio.h>

int main(void)
{
    char str[] = "now # is the time for all # good men to come to the # aid of their country";
    char delims[] = "#";
    char *result = NULL;
    result = strtok( str, delims );
    while( result != NULL ) {
        printf( "result is \"%s\"\n", result );
        result = strtok( NULL, delims );
    }
}
/*
The above code will display the following output:

   result is "now "
   result is " is the time for all "
   result is " good men to come to the "
   result is " aid of their country"
这个函数跟编译器中的词法分析很向，在以后的文本处理中，会解决很多问题。看来我有必要系统的学习下C的库函数，而不仅仅是死扎在语法和一些算法技巧上面。这样在平常的工作中才能事半功倍。
使用这个函数，形如下面得配置文件就非常容易解析：
id1 value1 value2 value3
id2 value1 value2 value3
...
使用这个函数，分割字符串就更加方便了，例如下面待分割的字符串：
12|2345|asld|alsfalskd
只要读取待处理的数据，然后调用四次strtok就能够解析出每行的值，以前我一般不是自己解析就是用sscanf，但是strtok更加合适，也更加啊灵活！ */

//将任意进制（2~36）的数转换为10进制，参数k=16就是16进制转10进制
/*将k进制转换为10进制 */
int atoD(char *str, int k)
{
    int num = 0;
    while(*str != '\0')
    {
        char ch = *str;
        if(ch>='0' && ch<='9')
            if((ch-'0')<k)
                num = num*k+ch-'0';
        if(ch>='a' && ch<='z')
            if((ch-'a'+10)<k)
                num = num*k+10+ch-'a';
        if(ch>='A' && ch<='Z')
            if((ch-'A'+10)<k)
                num = num*k+10+ch-'A';
        str++;
    }
    return num;
}

int writeRegFromeFile(int fd)/////获取文件的指针  c语言的split字符串分割函数strtok的使用 实例
{

    /*
    定义函数：char * strtok(char *s, const char *delim);
    函数说明：strtok()用来将字符串分割成一个个片段。参数s 指向欲分割的字符串，参数delim 则为分割字符串，当strtok()在参数s 的字符串中发现到参数delim 的分割字符时则会将该字符改为\0 字符。在第一次调用时，strtok()必需给予参数s 字符串，往后的调用则将参数s 设置成NULL。每次调用成功则返回下一个分割后的字符串指针。
    返回值：返回下一个分割后的字符串指针，如果已无从分割则返回NULL。
     */
    /*ad9631.txt文件内容如下
    	    {0b1,0x000,0x00},
    		{0b1,0x3df,0x01},// Required for proper operation
    		{0b1,0x2a6,0x0e},// Enable Master Bias
    		{0b1,0x2a8,0x0e},// Set Bandgap Trim
    		{0b1,0x292,0x08},// Set DCXO Coarse Tune[5:0].  Coarse and Fine nominal values used with eval system.  Other nominal values may be needed in a customer system
    		{0b1,0x293,0x80},// Set DCXO Fine Tune [12:5]
    		{0b1,0x294,0x00},// Set DCXO Fine Tune [4:0]
    */
#if DEBUG
    printf("line:%d,func:%s\n",__LINE__,__FUNCTION__);
#endif
    FILE *pFile=fopen("ad9631.txt","r"); //获取文件的指针
    if(NULL == pFile)
        perror("File open err!\n");

    //char *pBuf;  //定义文件指针
    //fseek(pFile,0,SEEK_END); //把指针移动到文件的结尾 ，获取文件长度
    //int len=ftell(pFile); //获取文件长度
    //pBuf=new char[len+1]; //定义数组长度
    //rewind(pFile); //把指针移动到文件开头 因为我们一开始把指针移动到结尾，如果不移动回来 会出错
    //fread(pBuf,1,len,pFile); //读文件
    //pBuf[len]=0; //把读到的文件最后一位 写为0 要不然系统会一直寻找到0后才结束
    //MessageBox(pBuf);  //显示读到的数据

    //在c语言环境下实现  C++中的getline函数    即在c语言中按行读取
    char * send_msg = NULL;
    size_t len = 0;
    size_t read;
    int count=0;
#if DEBUG
    printf("line:%d,func:%s\n",__LINE__,__FUNCTION__);
#endif
    while ((read = getline(&send_msg, &len, pFile)) != -1)
    {
        //	printf("%s\n",send_msg);
        //	count++;
        //	printf("共计  %d \n ",count);
        //用逗号做分隔符，读出内容
        //	char s[] = "123-sldkf-123ls-343434-dfjdlkfj-dflcmvn";
        char *delim = ",";
        char *p,*tmp;
        uint32_t tx[3];
        int i = 0;
        uint32_t count;
        uint8_t r_data;

        if(strstr(send_msg,"{") == NULL) {
            continue;//如果不包含左大括号"{"，则认为不是数组，跳出循环读下一行。
        }



        p = strtok(send_msg, delim);
        i=0;
#if DEBUG
        printf("%s", p);
#endif

        tmp = strstr(p,"0b") + 2;//ad9631.txt原始数据格式:{0b1,0x000,0x00}，0b后才是数字
        tx[i] = atoi(tmp);
#if DEBUG
        printf("(0x%x) ", tx[i]);
#endif
        i++;



        while((p = strtok(NULL, delim))) {
#if DEBUG
            printf("%s", p);
#endif
            if(i == 1) {
                tmp = strstr(p,"0x") + 2;//ad9631.txt原始数据格式:{0b1,0x000,0x00}，第二个数0x后才是数字
                tx[i] = atoD(tmp,16);   /*int atoD(char a[],int d1)  将d1进制的num,转换为十进制数s*/
#if DEBUG
                printf("(0x%x) ", tx[i]);
#endif
            }
            if(i == 2) {
                tmp = strstr(p,"}") - 2;//ad9631.txt原始数据格式:{0b1,0x000,0x00}，第三个数}前两个字节才是数字
                tx[i] = atoD(tmp,16);
#if DEBUG
                printf("(0x%x)\n", tx[i]);
#endif
            }
            i++;
            if(i>=3) break;
        }
#if DEBUG
        printf("\n");
#endif

        send_msg = NULL;
        if(read == -1)
            break;
    }


    fclose(pFile); // 关闭文件
    return 0;
}




/* char *fgets( char *buf, int n, FILE *fp );
函数 fgets() 从 fp 所指向的输入流中读取 n - 1 个字符。它会把读取的字符串复制到缓冲区 buf，并在最后追加一个 null 字符来终止字符串。
如果这个函数在读取最后一个字符之前就遇到一个换行符 '\n' 或文件的末尾 EOF，则只会返回读取到的字符，包括换行符。您也可以使用 int fscanf(FILE *fp, const char *format, ...) 函数来从文件中读取字符串，但是在遇到第一个空格字符时，它会停止读取。 */
#include <stdio.h>

int main()
{
    FILE *fp = NULL;
    char buff[255];

    fp = fopen("/tmp/test.txt", "r");
    fscanf(fp, "%s", buff);
    printf("1: %s\n", buff );

    fgets(buff, 255, (FILE*)fp);
    printf("2: %s\n", buff );

    fgets(buff, 255, (FILE*)fp);
    printf("3: %s\n", buff );
    fclose(fp);


}
/* 首先，fscanf() 方法只读取了 This，因为它在后边遇到了一个空格。其次，调用 fgets() 读取剩余的部分，直到行尾。最后，调用 fgets() 完整地读取第二行。
 */



/*  从文件中读取一行 */

/* 从文件中读取一行。
文件 runoob.txt 内容:
$ cat runoob.txt
runoob.com
google.com */
#include <stdio.h>
#include <stdlib.h> //  exit() 函数
int main()
{
    char c[1000];
    FILE *fptr;

    if ((fptr = fopen("runoob.txt", "r")) == NULL)
    {
        printf("Error! opening file");
        // 文件指针返回 NULL 则退出
        exit(1);
    }

    // 读取文本，直到碰到新的一行开始
    fscanf(fptr,"%[^\n]", c);

    printf("读取内容:\n%s", c);
    fclose(fptr);

    return 0;
}
/*  输出结果为：
读取内容:
runoob.com */

//////////////////////////////////////////////////////////////////////////////////////


/*
C语言中 如何读取 以逗号分割的文件中 的数据（包括字符串）?
现在定义一个结构体：
typedef struct Node{
char isbn[14];
char name[60]; char author[20]; char publisher[20]; char date[9]; int num; float price; struct Node * next;}Node;
现在如何构造一个函数 读取 TXT文件中的 数据流：！？

txt文件中内容举例如下：
9787512322233,重视大脑的学习指南:Head First Python,Barry,中国电力出版社,20120301,68,
10,
9787302304319,学术研究:你的成功之道,凌晓峰,清华大学出版社,20121001,25,
15,

9787115248497,
软件框架设计的艺术,
JaroslavTulach,
人民邮电出版社,
20110401,
75,
7,
如何 构造函数 把以上数据读入对应的空间中！？

每次用fgets读取整行，然后扫描一遍，看看逗号数量是不是足够一个完整结构体了。
如果不够，继续fgets，注意要读取到上一次fgets内容的末尾。然后再度检查逗号数量。
如果逗号数量够了（也就是内容已经足够一个结构体了），先去掉换行符。
然后每次先用strchr找到下一个逗号，再用strncpy将当前位置到下一个逗号的内容拷贝到结构体相应成员变量中。
读完当前结构体后，用memmove将未处理的内容前移到缓冲区开头。 */




1）字符串操作
strcpy(p, p1) 复制字符串
strncpy(p, p1, n) 复制指定长度字符串
strcat(p, p1) 附加字符串
strncat(p, p1, n) 附加指定长度字符串
strlen(p) 取字符串长度
strcmp(p, p1) 比较字符串
strcasecmp忽略大小写比较字符串
strncmp(p, p1, n) 比较指定长度字符串
strchr(p, c) 在字符串中查找指定字符
strrchr(p, c) 在字符串中反向查找
strstr(p, p1) 查找字符串
strpbrk(p, p1) 以目标字符串的所有字符作为集合，在当前字符串查找该集合的任一元素
strspn(p, p1) 以目标字符串的所有字符作为集合，在当前字符串查找不属于该集合的任一元素的偏移
strcspn(p, p1) 以目标字符串的所有字符作为集合，在当前字符串查找属于该集合的任一元素的偏移 * 具有指定长度的字符串处理函数在已处理的字符串之后填补零结尾符


2）字符串到数值类型的转换
strtod(p, ppend) 从字符串 p 中转换 double 类型数值，并将后续的字符串指针存储到 ppend 指向的 char* 类型存储。
strtol(p, ppend, base) 从字符串 p 中转换 long 类型整型数值，base 显式设置转换的整型进制，设置为 0 以根据特定格式判断所用进制，0x, 0X 前缀以解释为十六进制格式整型，0 前缀以解释为八进制格式整型
atoi(p) 字符串转换到 int 整型
atof(p) 字符串转换到 double 符点数
atol(p) 字符串转换到 long 整型

3）字符检查
isalpha() 检查是否为字母字符
isupper() 检查是否为大写字母字符
islower() 检查是否为小写字母字符
isdigit() 检查是否为数字
isxdigit() 检查是否为十六进制数字表示的有效字符
isspace() 检查是否为空格类型字符
iscntrl() 检查是否为控制字符
ispunct() 检查是否为标点符号
isalnum() 检查是否为字母和数字
isprint() 检查是否是可打印字符
isgraph() 检查是否是图形字符，等效于 isalnum() | ispunct()



4）函数原型

原型：strcpy(char destination[], const char source[]);
功能：将字符串source拷贝到字符串destination中 例程：
#include <iostream.h>
#include <string.h>
void main(void)
{   　
    char str1[10] = { "TsinghuaOK"};
    　
    char str2[10] = { "Computer"};
    　
    cout <<strcpy(str1,str2)<<endl;
}
运行结果是:
Computer 第二个字符串将覆盖掉第一个字符串的所有内容！ 注意：在定义数组时，字符数组1的字符串长度必须大于或等于字符串2的字符串长度。不能用赋值语句将一个字符串常量或字符数组直接赋给一个字符数组。所有字符串处理函数都包含在头文件 string.h中。


strncpy(char destination[], const char source[], int numchars);
strncpy：将字符串source中前numchars个字符拷贝到字符串destination中。 strncpy函数应用举例 原型：strncpy(char destination[], const char source[], int numchars);
功能：将字符串source中前numchars个字符拷贝到字符串destination中 例程：
#include <iostream.h>
#include <string.h>
void main(void) {
    char str1[10] = { "Tsinghua "};
    　
    char str2[10] = { "Computer"};
    　
    cout <<strncpy(str1,str2,3)<<endl;
}
运行结果：Comnghua 注意：字符串source中前numchars个字符将覆盖掉字符串destination中前numchars个字符！




原型：strcat(char target[], const char source[]);
功能：将字符串source接到字符串target的后面
例程：
#include <iostream.h>
#include <string.h>
void main(void) {
    char str1[] = { "Tsinghua "};
    　
    char str2[] = { "Computer"};
    　
    cout <<strcpy(str1,str2)<<endl;
}
运行结果：Tsinghua Computer
注意：在定义字符数组1的长度时应该考虑字符数组2的长度，因为连接后新字符串的长度为两个字符串长度之和。进行字符串连接后，字符串1的结尾符将自动被去掉，在结尾串末尾保留新字符串后面一个结尾符。


原型：strncat(char target[], const char source[], int numchars);
功能：将字符串source的前numchars个字符接到字符串target的后面
例程：
#include <iostream.h>
#include <string.h>
void main(void) {
    char str1[] = { "Tsinghua "};
    　
    char str2[] = { "Computer"};
    　
    cout <<strncat(str1,str2,3)<<endl;
}
运行结果：Tsinghua Com


原型：int strcmp(const char firststring[], const char secondstring);
功能：比较两个字符串firststring和secondstring 例程：
#include <iostream.h>
#include <string.h>
void main(void) {
    char buf1[] = "aaa";
    　
    char buf2[] = "bbb";
    　
    char buf3[] = "ccc";
    　
    int ptr;
    　
    ptr = strcmp(buf2,buf1);
    　
    if(ptr > 0) 　　
        cout <<"Buffer 2 is greater than buffer 1"<<endl;
    　 else 　　
        cout <<"Buffer 2 is less than buffer 1"<<endl;
    ptr = strcmp(buf2,buf3);
    　
    if(ptr > 0) 　
        　 cout <<"Buffer 2 is greater than buffer 3"<<endl;
    　
    else 　　
        cout <<"Buffer 2 is less than buffer 3"<<endl;
}
运行结果是:
Buffer 2 is less than buffer 1 Buffer 2 is greater than buffer 3


void *memmove(void *dest, const void *src, size_t count);
从src复制count字节的字符到dest. 如果src和dest出现重叠, 函数会自动处理. 返回dest的值.

void *memcpy(void *dest, const void *src, size_t count);
从src复制count字节的字符到dest. 与memmove功能一样, 只是不能处理src和dest出现重叠. 返回dest的值.

void *memchr(const void *buf, int c, size_t count);
在buf前面count字节中查找首次出现字符c的位置. 找到了字符c或者已经搜寻了count个字节, 查找即停止. 操作成功则返回buf中首次出现c的位置指针, 否则返回NULL.



void *_memccpy(void *dest, const void *src, int c, size_t count);
从src复制0个或多个字节的字符到dest. 当字符c被复制或者count个字符被复制时, 复制停止.
如果字符c被复制, 函数返回这个字符后面紧挨一个字符位置的指针. 否则返回NULL.

int memcmp(const void *buf1, const void *buf2, size_t count);
比较buf1和buf2前面count个字节大小. 返回值< 0, 表示buf1小于buf2;
返回值为0, 表示buf1等于buf2;
返回值> 0, 表示buf1大于buf2.

int memicmp(const void *buf1, const void *buf2, size_t count);
比较buf1和buf2前面count个字节. 与memcmp不同的是, 它不区分大小写.


char *strrev(char *string);
将字符串string中的字符顺序颠倒过来. NULL结束符位置不变. 返回调整后的字符串的指针.

char *_strupr(char *string);
将string中所有小写字母替换成相应的大写字母, 其它字符保持不变. 返回调整后的字符串的指针.

char *_strlwr(char *string);
将string中所有大写字母替换成相应的小写字母, 其它字符保持不变. 返回调整后的字符串的指针
.
char *strchr(const char *string, int c);
查找字 串string中首次出现的位置, NULL结束符也包含在查找中. 返回一个指针, 指向字符c在字符串string中首次出现的位置, 如果没有找到, 则返回NULL.

char *strrchr(const char *string, int c);
查找字符c在字符串string中最后一次出现的位置, 也就是对string进行反序搜索, 包含NULL结束符. 返回一个指针, 指向字符c在字符串string中最后一次出现的位置, 如果没有找到, 则返回NULL.

char *strstr(const char *string, const char *strSearch);
在字符串string中查找strSearch子串. 返回子串strSearch在string中首次出现位置的指针. 如果没有找到子串strSearch, 则返回NULL. 如果子串strSearch为空串, 函数返回string值.

char *strdup(const char *strSource);
函数运行中会自己调用malloc函数为复制strSource字符串分配存储空间, 然后再将strSource复制到分配到的空间中. 注意要及时释放这个分配的空间. 返回一个指针, 指向为复制字符串分配的空间;
如果分配空间失败, 则返回NULL值.


char *strcat(char *strDestination, const char *strSource);
将源串strSource添加到目标串strDestination后面, 并在得到的新串后面加上NULL结束符. 源串strSource的字符会覆盖目标串strDestination后面的结束符NULL. 在字符串的复制或添加过程中没有溢出检查, 所以要保证目标串空间足够大. 不能处理源串与目标串重叠的情况. 函数返回strDestination值.

char *strncat(char *strDestination, const char *strSource, size_t count);
将源串strSource开始的count个字符添加到目标串strDest后. 源串strSource的字符会覆盖目标串strDestination后面的结束符NULL. 如果count大于源串长度, 则会用源串的长度值替换count值. 得到的新串后面会自动加上NULL结束符. 与strcat函数一样, 本函数不能处理源串与目标串重叠的情况. 函数返回strDestination值.

char *strcpy(char *strDestination, const char *strSource);
复制源串strSource到目标串strDestination所指定的位置, 包含NULL结束符. 不能处理源串与目标串重叠的情况.函数返回strDestination值.

char *strncpy(char *strDestination, const char *strSource, size_t count);
将源串strSource开始的count个字符复制到目标串strDestination所指定的位置. 如果count值小于或等于strSource串的长度, 不会自动添加NULL结束符目标串中, 而count大于strSource串的长度时, 则将strSource用NULL结束符填充补齐count个字符, 复制到目标串中. 不能处理源串与目标串重叠的情况.函数返回strDestination值.

char *strset(char *string, int c);
将string串的所有字符设置为字符c, 遇到NULL结束符停止. 函数返回内容调整后的string指针.

char *strnset(char *string, int c, size_t count);
将string串开始count个字符设置为字符c, 如果count值大于string串的长度, 将用string的长度替换count值. 函数返回内容调整后的string指针.



size_t strspn(const char *string, const char *strCharSet);
查找任何一个不包含在strCharSet串中的字符 (字符串结束符NULL除外) 在string串中首次出现的位置序号. 返回一个整数值, 指定在string中全部由characters中的字符组成的子串的长度. 如果string以一个不包含在strCharSet中的字符开头, 函数将返回0值.


size_t strcspn(const char *string, const char *strCharSet);
查找strCharSet串中任何一个字符在string串中首次出现的位置序号, 包含字符串结束符NULL. 返回一个整数值, 指定在string中全部由非characters中的字符组成的子串的长度. 如果string以一个包含在strCharSet中的字符开头, 函数将返回0值.

char *strspnp(const char *string, const char *strCharSet);
查找任何一个不包含在strCharSet串中的字符 (字符串结束符NULL除外) 在string串中首次出现的位置指针. 返回一个指针, 指向非strCharSet中的字符在string中首次出现的位置.

char *strpbrk(const char *string, const char *strCharSet);
查找strCharSet串中任何一个字符在string串中首次出现的位置, 不包含字符串结束符NULL. 返回一个指针, 指向strCharSet中任一字符在string中首次出现的位置. 如果两个字符串参数不含相同字符, 则返回NULL值.


int strcmp(const char *string1, const char *string2);
比较字符串string1和string2大小. 返回值< 0, 表示string1小于string2;
返回值为0, 表示string1等于string2;
返回值> 0, 表示string1大于string2.

int stricmp(const char *string1, const char *string2);
比较字符串string1和string2大小，和strcmp不同, 比较的是它们的小写字母版本.返回值与strcmp相同.

int strcmpi(const char *string1, const char *string2);
等价于stricmp函数, 只是提供一个向后兼容的版本.

int strncmp(const char *string1, const char *string2, size_t count);
比较字符串string1和string2大小，只比较前面count个字符. 比较过程中, 任何一个字符串的长度小于count, 则count将被较短的字符串的长度取代. 此时如果两串前面的字符都相等, 则较短的串要小. 返回值< 0, 表示string1的子串小于string2的子串;
返回值为0, 表示string1的子串等于string2的子串;
返回值> 0, 表示string1的子串大于string2的子串.

int strnicmp(const char *string1, const char *string2, size_t count);
比较字符串string1和string2大小，只比较前面count个字符. 与strncmp不同的是, 比较的是它们的小写字母版本. 返回值与strncmp相同.

char *strtok(char *strToken, const char *strDelimit);
在strToken 串中查找下一个标记, strDelimit字符集则指定了在当前查找调用中可能遇到的分界符. 返回一个指针, 指向在strToken中找到的下一个标记. 如果找不到标记, 就返回NULL值. 每次调用都会修改strToken内容, 用NULL字符替换遇到的每个分界符.
