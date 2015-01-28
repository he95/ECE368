#ifndef __unhuff_h_
#define __unhuff_h_

#define NIL_PT		0xffff
#define LEFT_CHILD	0
#define RIGHT_CHILD   1
#define LEFT_CHILD_MASK	(1<<LEFT_CHILD)
#define RIGHT_CHILD_MASK	(1<<RIGHT_CHILD)
#define SLICE_ARRAY_SIZE(slice_arr)  (slice_arr.tail-slice_arr.head+1)
#define GET_VALUE(ch1,ch2)		 (((MY_UINT16)ch1<<8) + (MY_UINT16)ch2)
#define FREE_CODE_LOW_LIMIT		128

#define TYPE_CTR    0	//control char, the ASCII<32
#define TYPE_SYM	1	//space, ",","+","(" ":" and so on
#define TYPE_NUM    2	//'0' to '9'
#define TYPE_LET	3	//letter of the alphabet

typedef unsigned long  MY_UINT32;
typedef unsigned short MY_UINT16;
typedef unsigned char  MY_BYTE;

typedef struct{
	MY_UINT32 weight;
	char*  huffman_code;
	MY_BYTE   bytes[2];
	MY_UINT16 parent;
	MY_UINT16 l_child;
	MY_UINT16 r_child;
}HM_ENCODE_NODE;

typedef struct{
	MY_UINT32 weight;
	MY_BYTE   bytes[2];
	MY_BYTE   no;
}DOUBLE_BYTES_CODE;	

typedef struct{
	DOUBLE_BYTES_CODE *data;
	int len;
}SEARCH_ARRAY;	//double bytes to single byte convert table

typedef struct{
	SEARCH_ARRAY arr;	
	int  coding_space;		
	int  encode_mode;		
	MY_BYTE reserve_char;		
}MAP_2BYTES_1BYTE;

typedef struct{
	MY_BYTE flag;
	MY_BYTE left;
	MY_BYTE right;
}HM_DECODE_NODE;

HM_ENCODE_NODE  g_myHTree[256*2];			// the global varible, huffman tree used in encoding
char*			g_myHM_code_buff=NULL;	// the buffer of "01"string, global varible, be alloc during run time and release when encoding is completed
const MY_BYTE		g_maskArray[]={1,2,4,8,16,32,64,128};
MAP_2BYTES_1BYTE g_transStruct;

int     my_get_char_type(MY_BYTE ch);
int     my_get_char_type(MY_BYTE ch)
{
	if (ch< ' ')
		return TYPE_CTR;
	else if (ch>='0' && ch<='9')
		return TYPE_NUM;
	else if ((ch>='a' && ch<='z')|| (ch>='A' && ch<='Z' ))
		return TYPE_LET;
	else
		return TYPE_SYM;
}

void my_init_transStruct();
void my_init_transStruct()
{
	g_transStruct.arr.data  =NULL;	
	g_transStruct.arr.len =0;		
	g_transStruct.encode_mode=1;				
	g_transStruct.reserve_char=0;				
}

void my_free_transStruct();
void my_free_transStruct()
{
	if ( g_transStruct.arr.data!=NULL)
	{
		free(g_transStruct.arr.data); g_transStruct.arr.data=NULL;
	}
	
	g_transStruct.arr.len=0;					//the array length
}

/* The compare function be used in qsort*/
int cmp_by_weight(const void *a ,const void *b);
int cmp_by_weight(const void *a ,const void *b)
{
	HM_ENCODE_NODE** pp1=(HM_ENCODE_NODE **)a;
	HM_ENCODE_NODE** pp2=(HM_ENCODE_NODE **)b;
	HM_ENCODE_NODE* p1=*pp1;
	HM_ENCODE_NODE* p2=*pp2;

	if ( p1->weight < p2->weight)
		return -1;
	else if ( p1->weight > p2->weight)
		return 1;
	else if ( p1<p2)
		return -1;
	else if (p1>p2)
		return 1;
	else 
		return 0;

}

/* The compare function be used in qsort*/
int cmp_by_weight_in_doubleBytesArray(const void *a ,const void *b);
int cmp_by_weight_in_doubleBytesArray(const void *a ,const void *b)
{
	const DOUBLE_BYTES_CODE* p1=a;
	const DOUBLE_BYTES_CODE* p2=b;

	if ( p1->weight < p2->weight)
		return -1;
	else if ( p1->weight > p2->weight)
		return 1;
	else
		return 0;
}

/* The compare function be used in qsort*/
int cmp_by_bytes_in_doubleBytesArray(const void *a ,const void *b);
int cmp_by_bytes_in_doubleBytesArray(const void *a ,const void *b)
{
	const DOUBLE_BYTES_CODE* p1=a;
	const DOUBLE_BYTES_CODE* p2=b;
	MY_UINT16 myvalue1=GET_VALUE(p1->bytes[0],p1->bytes[1]);
	MY_UINT16 myvalue2=GET_VALUE(p2->bytes[0],p2->bytes[1]);
	if (myvalue1 < myvalue2)
		return -1;
	else if ( myvalue1 > myvalue2)
		return 1;
	else 
		return 0;
}

void search_and_insert( SEARCH_ARRAY *mypArray, char ch1,char ch2);
void search_and_insert( SEARCH_ARRAY *mypArray, char ch1,char ch2)
{
	int low,high,mid;
	MY_UINT16  myvalue1,myvalue2,key;
	key=GET_VALUE(ch1,ch2);
	low=0;
	high=mypArray->len-1;
	if ( mypArray->len==0)
	{
		mypArray->data[0].bytes[0]=ch1;
		mypArray->data[0].bytes[1]=ch2;
		mypArray->data[0].weight=1;
		mypArray->len++;
		return;
	}
	myvalue1= GET_VALUE(mypArray->data[low].bytes[0], mypArray->data[low].bytes[1]);
	myvalue2= GET_VALUE(mypArray->data[high].bytes[0],mypArray->data[high].bytes[1]);
	if ( key<myvalue1)		//insert the new element before first
	{
		memcpy( mypArray->data+1,mypArray->data,sizeof(DOUBLE_BYTES_CODE)*(mypArray->len));
		mypArray->data[low].bytes[0]=ch1;
		mypArray->data[low].bytes[1]=ch2;
		mypArray->data[low].weight=1;
		mypArray->len++;
		return ;
	}
	else if (key>myvalue2)	//insert the new element after last
	{
		mypArray->data[high+1].bytes[0]=ch1;
		mypArray->data[high+1].bytes[1]=ch2;
		mypArray->data[high+1].weight=1;
		mypArray->len++;
		return ;
	}

	while (low+1<high)
	{
		mid=(low+high)/2;
		myvalue1= ((MY_UINT16)mypArray->data[mid].bytes[0]<<8) + (MY_UINT16)mypArray->data[mid].bytes[1];
		if ( key>=myvalue1)
			low=mid;
		else 
			high=mid;
	}
	
	myvalue1= GET_VALUE(mypArray->data[low].bytes[0], mypArray->data[low].bytes[1]);
	myvalue2= GET_VALUE(mypArray->data[high].bytes[0],mypArray->data[high].bytes[1]);
	if ( key==myvalue1)
	{
		mypArray->data[low].weight++;
		return ;
	}
	else if (key==myvalue2)
	{
		mypArray->data[high].weight++;
		return ;
	}
	//insert the new element between low and high
	memcpy( mypArray->data+high+1,mypArray->data+high,sizeof(DOUBLE_BYTES_CODE)*(mypArray->len));
	mypArray->data[high].bytes[0]=ch1;
	mypArray->data[high].bytes[1]=ch2;
	mypArray->data[high].weight=1;
	mypArray->len++;
	return ;
}

MY_BYTE getOrder( SEARCH_ARRAY *mypArray, char ch1,char ch2,char reserve_char);
MY_BYTE getOrder( SEARCH_ARRAY *mypArray, char ch1,char ch2,char reserve_char)
{
	int low,high,mid;
	MY_UINT16  value,key;
	
	low=0;
	high=mypArray->len-1;
	key=GET_VALUE(ch1,ch2);
	while (low+1<high)
	{
		mid=(low+high)/2;
		value=GET_VALUE(mypArray->data[mid].bytes[0],mypArray->data[mid].bytes[1]);
		if ( key>value)
			low=mid+1;
		else if (key<value)
			high=mid-1;
		else
			return mypArray->data[mid].no;
	}
	
	if ( key==GET_VALUE(mypArray->data[low].bytes[0], mypArray->data[low].bytes[1]))
		return mypArray->data[low].no;

	else if (key==GET_VALUE(mypArray->data[high].bytes[0], mypArray->data[high].bytes[1]))
		return mypArray->data[high].no;

	else
		return reserve_char; //means did not find ch1:ch2	
}

int packBuffer(char *buff,int len,MAP_2BYTES_1BYTE* mypMap);
int packBuffer(char *buff,int len,MAP_2BYTES_1BYTE* mypMap)
{
	int i,no,new_len;
	int type1,type2;
	char ch1,ch2;

	for (new_len=0,i=0;i<len-1;)
	{
		ch1=buff[i];
		ch2=buff[i+1];
		type1=my_get_char_type(ch1);
		type2=my_get_char_type(ch2);
		no=mypMap->reserve_char;
		if (type1==type2)
			no=getOrder( &(mypMap->arr),ch1,ch2,mypMap->reserve_char);

		if (no !=mypMap->reserve_char && type1 == type2)  //replace 2 char with 1 char
			{	buff[new_len++]=no;		i+=2;	}
		else
			{	buff[new_len++]=ch1;	i++;	}
	}
	if (i<len)
		buff[new_len++]= buff[i];
	return new_len;
}

int map_2bytes_to_single_byte(MY_BYTE *buff,int len,MAP_2BYTES_1BYTE *mypMap);
int map_2bytes_to_single_byte(MY_BYTE *buff,int len,MAP_2BYTES_1BYTE *mypMap)
{
	int i,j,max_len,new_len;
	int type1,type2;
	void *tmpBuff;
	
	if ( mypMap->arr.data !=NULL)
		free(g_transStruct.arr.data);

	max_len=(256-mypMap->coding_space-1)*(256- mypMap->coding_space-1);
	mypMap->arr.data=(DOUBLE_BYTES_CODE*)malloc(sizeof(DOUBLE_BYTES_CODE)*max_len);
	
	for (i=0;i<len-1;)
	{
		type1=my_get_char_type(buff[i]);
		type2=my_get_char_type(buff[i+1]);
		if (type1==type2)
		{
			search_and_insert( &(mypMap->arr),buff[i],buff[i+1]); //make a array with  char pair
			i+=2;
		}
		else 
			i++;
	}

	//sort array order by weight
	qsort( mypMap->arr.data,mypMap->arr.len,sizeof(DOUBLE_BYTES_CODE),cmp_by_weight_in_doubleBytesArray);	
	new_len=mypMap->coding_space;
	if ( new_len>mypMap->arr.len)
		new_len=mypMap->arr.len;
	
	//truncate array g_transStruct.arrWeight
	tmpBuff=malloc(sizeof(DOUBLE_BYTES_CODE)*new_len);
	memcpy(tmpBuff,mypMap->arr.data,sizeof(DOUBLE_BYTES_CODE)*new_len);	
	free(mypMap->arr.data);
	mypMap->arr.data=(DOUBLE_BYTES_CODE*)tmpBuff;
	mypMap->arr.len=new_len;

	//sort array order by bytes 
	qsort( mypMap->arr.data,mypMap->arr.len,sizeof(DOUBLE_BYTES_CODE),cmp_by_bytes_in_doubleBytesArray);
	
	for (i=0;i<256;i++)
		g_myHTree[i].bytes[0]=mypMap->reserve_char; //the all of char is single byte coding

	for (j=0,i=mypMap->reserve_char+1;i<256 && j<mypMap->arr.len;i++)
	{
		if ( g_myHTree[i].weight==0 )
		{
			g_myHTree[i].bytes[0]=mypMap->arr.data[j].bytes[0];
			g_myHTree[i].bytes[1]=mypMap->arr.data[j].bytes[1];
			mypMap->arr.data[j].no=i;			// i <==> arr.data[j].bytes[0] and bytes[1], single char <==> double char
			j++;
		}
	}
	
	new_len= packBuffer((char*)buff,len,&g_transStruct);
	for (i=0;i<256;i++)
		g_myHTree[i].weight=0;
	for (i=0;i<new_len;i++)
		g_myHTree[buff[i]].weight++;
	return new_len;
}

int Write_1ByteOr2Byes( MY_BYTE code, const MY_BYTE *convert_tabl,MY_BYTE reserve_char,char* target);
int Write_1ByteOr2Byes( MY_BYTE code, const MY_BYTE *convert_tabl,MY_BYTE reserve_char,char* target)
{
	MY_BYTE ch1,ch2;
	ch1=convert_tabl[code*2];
	ch2=convert_tabl[code*2+1];
	if ( ch1== reserve_char)
	{
		*target=code; return 1;
	}
	else
	{
		target[0]=ch1; target[1]=ch2;	return 2;
	}
}


#endif  // __unhuff_h_ 
