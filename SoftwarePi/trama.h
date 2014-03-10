typedef struct
{
	unsigned char add	:7;
	unsigned char un1	:1;
} add;

typedef union
{
	unsigned char all_add;
	add field_add;
} ADD;

typedef struct
{
	unsigned char vel	:5;
	unsigned char dir	:1;
	unsigned char un2	:2;
} ctr;

typedef union
{
	unsigned char all_ctr;
	ctr field_ctr;
} CTR;

typedef struct
{
	ADD address;
	CTR control;
	unsigned char error;
} TRM;
