#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>

typedef int32_t I32;
typedef uint8_t U8;
typedef int64_t I64;

const I32 MAXBUF = 5;

// 将4个字节的整数编码为[1~5]字节的数组中
// 每个字节的最高位代表是否结束的标志，
// 次高位代表数据是否是负数，如果是负数，其他位表示的是绝对值
// 1 -> 1x******,
// 2 -> 0*******, 1x******,
// 3 -> 0*******, 0*******, 1x******,
// 4 -> 0*******, 0*******, 0*******, 1x******,
// 5 -> 0*******, 0*******, 0*******, 0*******, 1x******,
// SaveInt to variable U8 array
I32 SaveInt(U8 buf[], I32 i)
{
	I64 tmp = i;	// 如果不用64位整数作零时变量，将无法处理 INT32_MIN 这个数
	I32 sign = 0;	// 符号位, 如果第6位为1代表保存的数字是负数
	U8 *p = buf;

	if ( tmp<0 )
	{
		sign = (1<<6);
		tmp = -tmp;
	}

	while ( tmp>>6 )	// 只要6位装不下， 都用7位来装
	{
		*p = (U8)(tmp & ( (1<<7) - 1) );	// 取这个数字的低7位
		++p;
		tmp >>= 7;
	}
	*p = (U8)(tmp | (1<<7) | sign );	// tmp 只有低6位了，所以不用手动取低6位
	++p;

	return p-buf;	// 返回所用空间大小
}

I32 LoadInt(U8 buf[])
{
	I64 tmp = 0;
	U8* p = buf-1;

	do {
		++p;
		assert( p < buf+MAXBUF);	// 缓冲区溢出检测
	} while ( !(*p >> 7) );			// 结束标志是该字节最高位是1

	I32 sign = (*p)&(1<<6);	// 次高位是符号位
	(*p) &= ( (1<<6) -1);	// 清楚最高位和次高位的数据

	do {
		tmp = (tmp<<7) | *p;	// 组装数据
		--p;
	} while ( p != (buf-1) );
	
	if ( sign )
	{
		tmp = -tmp;
	}
	
	assert(INT32_MIN <= tmp && tmp <= INT32_MAX );

	return (I32) tmp;
}

int main(void)
{
	U8 buf[MAXBUF];

	I32 c =0;
	I64 begin = time(NULL);
	I64 end = 0;

	printf("test_begin, time = %ld !\n", begin );

	for(I32 i=INT32_MIN; ; ++i)
	{
		I32 k1 = i;
		I32 k2 = 0;

		SaveInt(buf, k1);
		k2 = LoadInt(buf);

		assert(k1 == k2);

		++c;

		if ( c == 1*10000 * 10000 )
		{
			c = 0;
			printf("test_inprocess , time = %ld ! i = %d\n", time(NULL), i);
		}

		if ( i == INT32_MAX )
		{
			break;
		}
	}
	
	end = time(NULL);

	printf("test_finish time = %ld ! time offset = %ld \n", end, end-begin );

	return 0;
}
