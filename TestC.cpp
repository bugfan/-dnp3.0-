#include "stdio.h"
#include "winsock2.h"
#include "Windows.h"
#pragma comment (lib,"ws2_32")

/*********���ݽṹ****************/

#define uchar unsigned char 
#define uint unsigned int
#define ushort unsigned short	//stm32����2�ֽڳ�����������
#define APP_LEN 8192			//������ܻ���������
#define APP_BLKNUM_LEN 128		//������ܻ���ÿ�εĳ���
#define SEARCHOK 0		
#define LEN 300

/*********����ȫ������ ����****************/

SOCKET s;						//�����׽���
sockaddr_in sa;

char RxData[LEN];				//������ܻ�����
char TxData[LEN];				//����	
char RxBuff[APP_LEN];			//����Ӧ�ò㻺����
char RxBuffNum[APP_BLKNUM_LEN];	//���建�����������������ÿ�ε�λ��

uint	RecvLen;				//����checkMsg��Ĵ��������ݳ���
uint	TransLen;				//����Ҫ���͵����ݳ���
uint    PosiLen;				//����λ��
char MLnkFCB;					//����Master��������LnkFCB��value----  &0x20,ֻ��һλ����ʱ����Զ���Ϊbit����
bool TranStartFlg=0;			//����㿪ʼ��־
char TranBlkNum=0;				//���������
char MyTranBlks=0;				//�Լ��Ŀ�ţ����ڴ洢ת�浽RxBuff�Ŀ���

//ushort����������Ҫ���������
ushort dnp_crc_table[256] = {
 0x0000, 0x365e, 0x6cbc, 0x5ae2, 0xd978, 0xef26, 0xb5c4, 0x839a,
 0xff89, 0xc9d7, 0x9335, 0xa56b, 0x26f1, 0x10af, 0x4a4d, 0x7c13,
 0xb26b, 0x8435, 0xded7, 0xe889, 0x6b13, 0x5d4d, 0x07af, 0x31f1,
 0x4de2, 0x7bbc, 0x215e, 0x1700, 0x949a, 0xa2c4, 0xf826, 0xce78,
 0x29af, 0x1ff1, 0x4513, 0x734d, 0xf0d7, 0xc689, 0x9c6b, 0xaa35,
 0xd626, 0xe078, 0xba9a, 0x8cc4, 0x0f5e, 0x3900, 0x63e2, 0x55bc,
 0x9bc4, 0xad9a, 0xf778, 0xc126, 0x42bc, 0x74e2, 0x2e00, 0x185e,
 0x644d, 0x5213, 0x08f1, 0x3eaf, 0xbd35, 0x8b6b, 0xd189, 0xe7d7,
 0x535e, 0x6500, 0x3fe2, 0x09bc, 0x8a26, 0xbc78, 0xe69a, 0xd0c4,
 0xacd7, 0x9a89, 0xc06b, 0xf635, 0x75af, 0x43f1, 0x1913, 0x2f4d,
 0xe135, 0xd76b, 0x8d89, 0xbbd7, 0x384d, 0x0e13, 0x54f1, 0x62af,
 0x1ebc, 0x28e2, 0x7200, 0x445e, 0xc7c4, 0xf19a, 0xab78, 0x9d26,
 0x7af1, 0x4caf, 0x164d, 0x2013, 0xa389, 0x95d7, 0xcf35, 0xf96b,
 0x8578, 0xb326, 0xe9c4, 0xdf9a, 0x5c00, 0x6a5e, 0x30bc, 0x06e2,
 0xc89a, 0xfec4, 0xa426, 0x9278, 0x11e2, 0x27bc, 0x7d5e, 0x4b00,
 0x3713, 0x014d, 0x5baf, 0x6df1, 0xee6b, 0xd835, 0x82d7, 0xb489,
 0xa6bc, 0x90e2, 0xca00, 0xfc5e, 0x7fc4, 0x499a, 0x1378, 0x2526,
 0x5935, 0x6f6b, 0x3589, 0x03d7, 0x804d, 0xb613, 0xecf1, 0xdaaf,
 0x14d7, 0x2289, 0x786b, 0x4e35, 0xcdaf, 0xfbf1, 0xa113, 0x974d,
 0xeb5e, 0xdd00, 0x87e2, 0xb1bc, 0x3226, 0x0478, 0x5e9a, 0x68c4,
 0x8f13, 0xb94d, 0xe3af, 0xd5f1, 0x566b, 0x6035, 0x3ad7, 0x0c89,
 0x709a, 0x46c4, 0x1c26, 0x2a78, 0xa9e2, 0x9fbc, 0xc55e, 0xf300,
 0x3d78, 0x0b26, 0x51c4, 0x679a, 0xe400, 0xd25e, 0x88bc, 0xbee2,
 0xc2f1, 0xf4af, 0xae4d, 0x9813, 0x1b89, 0x2dd7, 0x7735, 0x416b,
 0xf5e2, 0xc3bc, 0x995e, 0xaf00, 0x2c9a, 0x1ac4, 0x4026, 0x7678,
 0x0a6b, 0x3c35, 0x66d7, 0x5089, 0xd313, 0xe54d, 0xbfaf, 0x89f1,
 0x4789, 0x71d7, 0x2b35, 0x1d6b, 0x9ef1, 0xa8af, 0xf24d, 0xc413,
 0xb800, 0x8e5e, 0xd4bc, 0xe2e2, 0x6178, 0x5726, 0x0dc4, 0x3b9a,
 0xdc4d, 0xea13, 0xb0f1, 0x86af, 0x0535, 0x336b, 0x6989, 0x5fd7,
 0x23c4, 0x159a, 0x4f78, 0x7926, 0xfabc, 0xcce2, 0x9600, 0xa05e,
 0x6e26, 0x5878, 0x029a, 0x34c4, 0xb75e, 0x8100, 0xdbe2, 0xedbc,
 0x91af, 0xa7f1, 0xfd13, 0xcb4d, 0x48d7, 0x7e89, 0x246b, 0x1235
};
//*********����ṹ��**********
typedef struct GlbSendData{
	char cfm;
}GlbSendData;
GlbSendData GSD;
/*****���϶������ݱ��������涨�庯������***/

/**********��������:��************/
void P(uint);
void P2(uint);
void myRead(char *,const char *);
uint myWrite(char *,uint);
char buildFrame(char *,char *,uint);

//��ʼ������
void initTCP(){
	WSADATA wsa;
	WSAStartup(MAKEWORD(2,2),&wsa);
	s=socket(PF_INET,SOCK_STREAM, IPPROTO_TCP);	//����ȫ�ֱ���Socket
	
	sa.sin_family=PF_INET;
	sa.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");	//����Ŀ���ַ
	sa.sin_port=htons(20000);						//���ö˿�
	//��ʼ����
	connect(s,(SOCKADDR*)&sa,sizeof(SOCKADDR));		//l���ӷ�����
}
//*************CRCУ�鲿��*********************/

ushort  dnp_crc16(ushort crc, unsigned int text)
{
   unsigned char index = (unsigned char)(text ^ crc);             /*xor with previous to form index */
   crc = ((crc & ~0xff) >> 8) ^ (dnp_crc_table[index]);
   return crc;
}
char CRC_DNP(unsigned char*buf, unsigned char len)
{
   ushort  crc = 0;
   while (len--)
      crc = dnp_crc16(crc, (unsigned int)*buf++);

   crc ^= 0xFFFF;
   //�Ѽ������crc�븳ֵ��ĩβ�����ֽ�
   *buf=crc&0x00ff;	
   *(1+buf)=(crc&0xff00)>>8;
   printf ("crc=%02x %02x\n",crc&0x00ff,(crc&0xff00)>>8);
   return 0;
}
char CRC_DNP2(unsigned char*buf, unsigned char len)
{
   ushort  crc = 0;
   while (len--)
      crc = dnp_crc16(crc, (unsigned int)*buf++);

   crc ^= 0xFFFF;
   //У��crc��֤�����Ƿ���ȷ
   if((*buf==crc&0x00ff)&&(*(++buf)==(crc&0xff00)>>8)){
		return -1;			//У�����
   }   
   return 0;				//����
}
/**************************************************/
//�����·�㺯��
char dealLink(uint count)
{
    uint i,j;
	uchar transLastLen,transBlocks,k;
	uchar linkAdd=0;	//��·��������ӵ��ֽڸ���������ͷ�������֡�У�顢����λ
	
	//�жϱ����ܳ�
	if(count<10){
		return -10;		//�����ܳ���С��10
	}
	
	//�ж�֡ͷ
	for(i=0;i<count;i++)
	{
		if(0x05 == RxData[i] && 0x64 == RxData[i+1])	// ֡ͷ��ȷ
		{
			//�жϿ�����
			if((uchar)RxData[i+3] <= 0x80)				//תΪ�޷��Ž��бȽ�
				return -1;								//��·������ֲ���,���ǴӶˣ�����DIRΪ1������>=0x80

			//�ж�Ŀ���ַ
			/*
			if((RxData[i+4]!=LocalAddr.l)&&(RxData[i+5]!=LocalAddr.h)){
				return -2;													//��ַ����
			}
			*/
			
			//�жϳ����Ƿ�С����С����
			if((uchar)RxData[i+2] < 0x05){			
				return -3;							// ��·�㳤���ֽ���ʾ����
			}else{									// >=5
				if((uchar)RxData[i+2] > 5)			// >5
				{
					transBlocks = ((uchar)RxData[i+2]-6)/16 + 1;			//���㴫���
					transLastLen = ((uchar)RxData[i+2]-6) % 16+1;			//���㴫�����һ�������
				}
				else								// ==5
				{
					transBlocks = 0;
				}
			}
			
			linkAdd =5 + transBlocks*2;				//ͳ�Ʊ�����װ���ݵ��ܳ���(������·���CRCУ��)
			
			//���ĳ��Ⱥ�ʵ�ʳ��Ȳ��ȣ�return 
			
			if((count-5-transBlocks*2)!=(uchar)RxData[i+2]){
				printf("%d=============%d\n",(uchar)(count-5-transBlocks*2),(uchar)RxData[i+2]);
				return -8;
			}
			
			//�ж�CRCУ��-��·ͷ
			if(i + 10 <= count )	//���Ȿ�������ܳ��� ����������  <=10��ȥ�ж�CRC�Բ���
			{
				if(!(0==CRC_DNP2((uchar *)&RxData[0],8)))	return -5;		//cRC��·��У��ʧ��
			}	

			//����-�����
			if((uchar)RxData[i+2]>0x05)								// ���ڴ����
			{

				//��ȡ�������������ȡ�����ĩ���ֽڸ���
				//������������CRCУ��
				for(j=0;j<transBlocks;j++)
				{
					if(transBlocks-1 == j)
					{
						if(CRC_DNP2((uchar *)&RxData[10+j*18],transLastLen))
							return -6;									//crc����У��ʧ�ܷ���-6
					}			
					else
					{
						if(CRC_DNP2((uchar *)&RxData[10+j*18],16))				//У����������ֽ�
							return -7;
					}
				}
				
				//��·��crcУ��ɹ��󣬰ѻ���������������ȥ��װ���ݣ���·�㣩
				for(j=0;j<5;j++)	
						RxData[j] = RxData[j+3];
				
				//�����CRCУ��ɹ�����ȡ����㱨��
				for(j=0;j<transBlocks;j++)
				{
					if(transBlocks-1 == j)
					{
						for(k=0;k<transLastLen;k++)
							RxData[5+ k + j*16] = RxData[10 + k + j*18];
					}
					else
					{
						for(k=0;k<16;k++)
							RxData[5+ k + j*16] = RxData[10 + k + j*18];
					}
				}
				//������ȷ,���泤��
				RecvLen=count-linkAdd;
				return 1;		//�д���㣬����ok 
			}else{
				for(k=0;k<5;k++)
				RxData[k] = RxData[k+3];
				RecvLen=5;		//��Ч���ݳ���5
				return 0;		//û�����
			}
		}else{
			return -9;			//ͷ����
		}
    }
	return 0;
}
//sendAck()	�Ӷ˸����˷���ȷ�Ϻͷ���confirm
void sendAck(uchar type){
	/************************��·�㹦����*********************
	Դ��վ��·�㹦����(PRM=1)
       ������     ֡����               ������         FCVλ
         0     SEND/�ڴ�CONFIRM     ʹԶ����·��λ        0
         1     SEND/�ڴ�CONFIRM     ʹ�û����̸�λ        0
         2     SEND/�ڴ�CONFIRM     ����·�Ĳ��Թ���      1
         3     SEND/�ڴ�CONFIRM     �û�����              1
         4     SEND/���ڴ�����      ��ȷ�ϵ��û�����      0
         9     ��ѯ/�ڴ���Ӧ        ��ѯ��·״̬          0
        5-8     δ��	
		10-15	δ��
	  ****************************************************
	 ����վ��·�㹦����(PRM=0)
	������      ֡����              ������
	  0         �϶�ȷ��           ACK=�϶���ȷ��
	  1         ��ȷ��           NACK=����δ�յ�;��·æ
	  11        ��Ӧ			   ��·��״̬��DFC=0��DFC=1��
	 2-10       δ��           
    12-13       δ�� 
     14							   ��·���񲻹�����
     15							   δ����·����,��δʵ����·����
	*******************************************************/
	char rfc=-1;
	uint len=5;
	switch(type){
		case 2: //SEND/�ڴ�CONFIRM     ����·�Ĳ��Թ���

			 break;
		case 3: //SEND/�ڴ�CONFIRM     �û�����  

			 break;
		case 0: //SEND/�ڴ�CONFIRM     ʹԶ����·��λ
			 rfc=1;
			 break;
		case 1: //SEND/�ڴ�CONFIRM     ʹ�û����̸�λ    
			 rfc=1;
			 break;
		case 4: //SEND/���ڴ�����      ��ȷ�ϵ��û�����

			 break;
		case 9:	//��ѯ/�ڴ���Ӧ        ��ѯ��·״̬   

			 break;
	}
	if(-1!=rfc){
		//��һ֡������RxData
		GSD.cfm=rfc;
		buildFrame(&RxData[3],&TxData[0],len);
	}
}
//checkLnkFC()��������:�����·�㹦����
char checkLnkFC(char lfc){

	if(lfc&0x80!=0x80) return -1;					//���󣬷��򲻶�
	//fcv��1,������0
	printf("������:%02x %c %02x\n������:",(unsigned char)lfc,26,(unsigned char)lfc&0x10);
	char ff=lfc&0x10;								//�����ظ�ֵ���
	/*		---��·�㹦����---
		Դ��վ��·�㹦����(PRM=1)
       ������     ֡����               ������         FCVλ
         0     SEND/�ڴ�CONFIRM     ʹԶ����·��λ        0
         1     SEND/�ڴ�CONFIRM     ʹ�û����̸�λ        0
         2     SEND/�ڴ�CONFIRM     ����·�Ĳ��Թ���      1
         3     SEND/�ڴ�CONFIRM     �û�����              1
         4     SEND/���ڴ�����      ��ȷ�ϵ��û�����      0
         9     ��ѯ/�ڴ���Ӧ        ��ѯ��·״̬          0
        5-8     δ��	
		10-15	δ��
	*/
	if(ff==0x10){									//�ж�FCVλʲô���
		MLnkFCB=lfc&0x20;							//�õ�������,��λ����һ�ε�ȡ��
		switch(lfc&0x0f){
			case 2:
				printf("����·�Ĳ��Թ���\n");
				return 2;
				break;
			case 3:
				printf("�û����� \n");
				return 3;
				break;
		}
	}else{
		switch(lfc&0x0f){
			case 0:
				printf("ʹԶ����·��λ\n");
				return 0;
				break;
			case 1:
				printf("ʹ�û����̸�λ\n");
				return 1;
				break;
			case 4:
				printf("���ڴ�����,��ȷ�ϵ��û�����\n");
				return 4;
				break;
			case 9:
				printf("��ѯ��·״̬\n");				
				return 9;
				break;
		}
	}
	return -2;
}

//checkTxFC����������������
uchar checkTranFC(char tfc){
	return (uchar)tfc&0xc0;					//
} 
//getTxBlkNum(���������)	�õ���������
char getTranBlkNum(char v){
	return v&0x3f;
}
//rstRxBuff()		�������������
void rstTranTem(){
	uint i;
	for(i=0;i<sizeof(RxBuff);++i){
		RxBuff[i]&=0x00;
	}
	for(i=0;i<sizeof(RxBuffNum);++i){
		RxBuffNum[i]&=0x00;
	}
	TranBlkNum=MyTranBlks=PosiLen=RecvLen=0;	//�������
}
//ptAppLayer(Դ������)	��ӡӦ�ò㴦��������
void ptAppLayer(uint sl,char t){
	if(1==t){												//����֡
		printf("$��������OK_1_�����ȴ�ӡһ������\n");
		P(sl);
	}else{													//��������
		printf("$��������OK_2_�����ȴ�ӡһ������\n");
		while(RxBuffNum[sl]!=0){
			P2(RxBuffNum[sl]);
			sl++;
		}
	}
}
//saveRxBuff����������Ӧ�ò�buff
void saveRxBuff(char v){
	uint i;
	if(1==v){
		for(i=0;i<RxBuffNum[MyTranBlks];i++){						
			RxBuff[i]=RxData[i];
		}
	}else{
		uint k;
		k=RxBuffNum[MyTranBlks-1];
		for(i=0;i<RxBuffNum[MyTranBlks];i++){
			RxBuff[k+i+PosiLen]=RxData[i];
		}
		PosiLen+=k;
	}
}

//------����Ӧ�ò�------
void dealAppFun(uchar *appData){
	//printf("appData is %02x\n",appData[0]);
	uchar ackCode = 0xff;			//������Ӧ��
	uchar funCode = 0xff;
	uchar objCode = 0xff;
	uchar objGroup = 0xff;	
	uchar objVaria = 0xff;
	uchar qual = 0xff;				// �޶���
	uint sendLen = 0x05;
	//��ʼ��ֵ
	funCode  = appData[1];			//������
	objGroup = appData[2];			//��  �ֽ�
	objVaria = appData[3];			//�����ֽ�
	qual = appData[4];				//�޶���
	printf("\ndealAppFun������funCode=%02x,objGroup=%02x,objVaria=%02x\n",funCode,objGroup,objVaria);
	if(0 == funCode)				//������·��ظ�ȷ��
	{
		ackCode = 0;
	}else if(1 == funCode){			//������
		/******************************************************************
		   ���	����	���ݶ�������
		  //1	1	����Ʒ�������ĵ������룬��ѹ����ʽ��ң������8��/�ֽ�
			1	2	��Ʒ�������ĵ������룬1�ֽ�/���ң����
			2	1	��Ʒ����������ʱ��ĵ����λ��Ϣ��1�ֽ�/��
			2	2	��Ʒ������������ʱ��ĵ����λ��Ϣ��7�ֽ�/��
		  //12	1	�̵���������ƣ�ң�أ�����11�ֽ�/��
			30	2	16λ��Ʒ����������ʱ��ľ�̬��ǰģ������3�ֽ�/��
			30	4	16λ����Ʒ����������ʱ��ľ�̬��ǰģ������2�ֽ�/��
			32	2	16λ��Ʒ����������ʱ��ı仯��ǰģ������3�ֽ�/��
		  //20	1	32λ��Ʒ����������ʱ��ľ�̬�ۼ���������5�ֽ�/��
		  //20	5	32λ����Ʒ����������ʱ��ľ�̬�ۼ���������4�ֽ�/��
		  //22	1	32λ��Ʒ����������ʱ���ұ仯�˵��ۼ���������5�ֽ�/��
			50	1	��ʾ�����ӵľ���ʱ�����6�ֽ�/��
			60	1	0�����ݣ�����̬���ݣ��������ٻ������ȷ�еĶ���
			60	2	1�����ݣ����ȼ���ߵı仯���ݣ��������ٻ�����
			60	3	2�����ݣ����ȼ�������1�����ݵı仯���ݣ��������ٻ�����
			60	4	3�����ݣ����ȼ�����2�����ݵı仯���ݣ��������ٻ�����
		******************************************************************/
		if(1 == objGroup && 2 == objVaria)	// ��Ʒ�������ĵ������룬1�ֽ�/���ң����
		{
			
		}
		else if(2 == objGroup && 1 == objVaria)	// ��Ʒ����������ʱ��ĵ����λ��Ϣ��1�ֽ�/��
		{
			
		}
		else if(2 == objGroup && 2 == objVaria)	// ��Ʒ������������ʱ��ĵ����λ��Ϣ��7�ֽ�/��
		{
			
		}
		else if(30 == objGroup && 2 == objVaria)	// 16λ��Ʒ����������ʱ��ľ�̬��ǰģ������3�ֽ�/��
		{
			
		}
		else if(30 == objGroup && 4 == objVaria)	// 16λ����Ʒ����������ʱ��ľ�̬��ǰģ������2�ֽ�/��
		{
			
		}
		else if(32 == objGroup && 2 == objVaria)	// 16λ��Ʒ����������ʱ��ı仯��ǰģ������3�ֽ�/��
		{
			
		}
		else if(60 == objGroup && 1 == objVaria)	// 0�����ݣ�����̬���ݣ��������ٻ������ȷ�еĶ���(����)
		{
			if(qual == 6)	//�ޱ�� ���޸�������
			{
					
			}

		}else{
			printf("Ӧ�ò�������������,����������,���ˡ�����\n");
			return ;
		}
		/*	********************		�޶��ʱ�
			�޶�ֵ	��������	�� �� �� ��						���ӵĵ����������
			0x00	��ֹģʽ	2���ֽڣ���һ���ֽ�Ϊ��ʼ��ţ��������ڶ����ֽ�Ϊ��ֹ��ţ�������	��
			0x01	��ֹģʽ	4���ֽڣ�ǰ2���ֽ�Ϊ��ʼ��ţ���������2���ֽ�Ϊ��ֹ��ţ�������	��
			0x02	��ֹģʽ	8���ֽڣ�ǰ4���ֽ�Ϊ��ʼ��ţ���������4���ֽ�Ϊ��ֹ��ţ�������	��
			0x07	����ģʽ	1���ֽڣ�����ֵΪN�����漰���ĵ��Ϊ��0 ~ N-1	��
			0x08	����ģʽ	2���ֽڣ�����ֵΪN�����漰���ĵ��Ϊ��0 ~ N-1	��
			0x09	����ģʽ	4���ֽڣ�����ֵΪN�����漰���ĵ��Ϊ��0 ~ N-1	��0x17	����ģʽ	1���ֽڣ�����ֵΪN	ΪN����ţ�ÿ�����ռ1���ֽ�
			0x18	����ģʽ	2���ֽڣ�����ֵΪN	ΪN����ţ�ÿ�����ռ1���ֽ�
			0x19	����ģʽ	4���ֽڣ�����ֵΪN	ΪN����ţ�ÿ�����ռ1���ֽ�
			0x27	����ģʽ	1���ֽڣ�����ֵΪN	ΪN����ţ�ÿ�����ռ2���ֽ�
			0x28	����ģʽ	2���ֽڣ�����ֵΪN	ΪN����ţ�ÿ�����ռ2���ֽ�
			0x29	����ģʽ	4���ֽڣ�����ֵΪN	ΪN����ţ�ÿ�����ռ2���ֽ�
			0x37	����ģʽ	1���ֽڣ�����ֵΪN	ΪN����ţ�ÿ�����ռ4���ֽ�
			0x38	����ģʽ	2���ֽڣ�����ֵΪN	ΪN����ţ�ÿ�����ռ4���ֽ�
			0x39	����ģʽ	4���ֽڣ�����ֵΪN	ΪN����ţ�ÿ�����ռ4���ֽ�
			0x06	ȫ��ģʽ	�ޡ��������ٻ�����漰�ĵ��Ϊ���շ�֧�ֵ����ٻ����������͵����е�	��
		*******************************/
	}else if(2 == funCode){	// д����
		
	}else if(3==funCode){	//ң��Ԥ��
		
	}else if(4==funCode){	//ң��ִ��
		
	}else if(5==funCode){	//ֱ�����������Ҫ����ǰ��ѡ�����ң��ֱ��ִ�У�
		
	}else{
		printf("Ӧ�ò㹦�����������,���������ˡ�����\n");
		//return ;
	}

	/************��ʼƴ������**************/
	if(ackCode < 0xff)
	{
		/*
		sFcLinkSend.Byte |= ackCode;
		buildSendFrame(Uart1RxBuff.Buff,Uart1TxBuff.Buff,5, dstAddr);
		*/
	}
	buildFrame(&RxBuff[0],&TxData[0],8);
}

//buidFrame(�յ�,����,count:userData����)����to make Confirm
char buildFrame(char *buff,char *send,uint count){
	if(count<5) return -1;		//�ж�
	uint len;
	uchar j,k;
	uchar transNum,lastLen;

	send[0]=0x05;
	send[1]=0x64;
	send[2]=count;
	//��ȫ����
	send[3]=GSD.cfm;
	//Ŀ�ĵ�ַ��Դ��ַ����
	send[4]=buff[3];						
	send[5]=buff[4];						
	send[6]=buff[1];
	send[7]=buff[2];
	//crc������δ����
	//send[8]=0xcc;	
	CRC_DNP((uchar *)&send[0],8);
	
	//�ѿ�������������
	if(count>5){
		transNum =(count-6)/16 + 1;
		lastLen =count-5-(transNum-1)*16;
	}else{
		transNum = 0;
		lastLen = 0;
	}
	
	//У�鴫���CRC
	if(transNum == 0){
		len = 10;
	}else{
		for(j=0;j<transNum;j++){
			if(transNum-1 == j){				//ֻ��һ��ĩ��
				for(k=0;k<lastLen;k++){
					send[10+k+j*18] = buff[5+k+j*16];
				}
				CRC_DNP((uchar*)&send[10+j*18],lastLen);
				len = 10 + j*18 + k + 2;		//�����ܳ��ȡ�һ�μ���
			}
			else{								// >2��
				for(k=0;k<16;k++){
					send[10+k+j*18] = buff[5+k+j*16];
					CRC_DNP((uchar*)&send[10+j*18],16);
				}
			}
		}
	}	
	//
	
	printf("Seconder��Ҫ����:\n");
	myWrite(&send[0],len);
	return 0;
}
//�����������ĺ������
uchar dealDNP(uint len)
{ 
    char FlgCheck;
	char LnkFC;
	FlgCheck = dealLink(len);
	//֡ͷ�ж�ͨ��
	printf("$��·�����OK_����:%d\n",FlgCheck);
	printf("User Data����Ϊ:%d\n",RecvLen);
	printf("User DataΪ:");
	P(RecvLen);
	
	//����������Ǹ���,return -1��ȥ������
	if(FlgCheck<0){
		//��·�����������λ�����
		rstTranTem();
		return -1;
	}
	
	//����������������
	if(0==FlgCheck){
		LnkFC=checkLnkFC(RxData[0]);

		if(LnkFC<0){
			printf("��·������ֽ�������:%d\n",LnkFC);
		}else{
			printf("��·������ֽ����ɹ�OK,�޴����,Confirm������Ϊ:%d\n",LnkFC);
			sendAck(LnkFC);				//�������������ҷ��͸�master
		}
	}
	//���������������
	if(1==FlgCheck){
		printf("_��ʼ�������\n");
		if(!TranStartFlg){
			//��һ��
			if(0x80==checkTranFC(RxData[5])){
				printf("������õ��׿��־\n");
				TranStartFlg=1;							//�Ѿ���ʼ
				TranBlkNum=getTranBlkNum(RxData[5]);	//��¼���������
				RxBuffNum[MyTranBlks]=RecvLen;
				saveRxBuff(1);							//��buff 			
				printf("�����׿�,MyTranBlks���Ϊ:%d,ֵΪ:%d\n",MyTranBlks,RxBuffNum[MyTranBlks]);
				return 0;								//����ȥ�����ȴ����ܱ���
			}
			//��һ��
			if(0xc0==checkTranFC(RxData[5])){
				//��ӡ����
				printf("������һ��\n");
				ptAppLayer(RecvLen,1);					//applayer
				//����Ӧ�ò����, ��ʼƴ������
				//û����saveRxBuff����
				dealAppFun((uchar *)&RxData[7]);		//��7λ��ʼӦ�ò�
				return 1;
			}
			printf("���ǵ�һ��,�����ȴ���һ��\n");
			return -1;
		}else{
			//�м䲿��&β��
			if(TranBlkNum+1==getTranBlkNum(RxData[5])){		//���������,0 ~ 63��Χ				
				TranBlkNum++;								//�Ѹ��Ŀ�����һ
				MyTranBlks++;								//���Լ�����Կ��һ  
				RxBuffNum[MyTranBlks]=RecvLen;				//����ǰ�ÿ�εĴ��뻺�����ĳ��ȴ�����������ÿ�ζ�һ����ĩ����ܻ���һЩ
				
				saveRxBuff(2);								//������Ӧ�ò㻺����������
				if(TranBlkNum>=63)	TranBlkNum=0;			//0--63 
				printf("׷����һ��,MyTranBlks���Ϊ:%d,��Ӧ��ֵΪ:%d\n",MyTranBlks,RxBuffNum[MyTranBlks]);
				
				//���� ��ʱʱ��(�ڵ�Ƭ��������ʱʱ��)
					
				//����������ʱʱ��
			}else{

				printf("��������벻�ԣ��ж�...\n");
				TranStartFlg=0;								//��λ�׿��־
				rstTranTem();
				return -2;									//�������벻��Ӧ���ж�,��������
			}	
			//��⵽ĩ����Ƴ� 
			if(0x40==checkTranFC(RxData[5])){		
				printf("��⵽�����ĩ��\n");
				TranStartFlg=0;								//������־�л���
				ptAppLayer(0,2);							//��ӡ

				//����Ӧ�ò����, ��ʼƴ������
				dealAppFun((uchar *)&RxBuff[6]);
				return 1;
				rstTranTem();								//��λ
			}
		}
		
		//��������Ӧ�ò�����

	} //end FlgCheck==1�ж�

	return 0;
}

/**************������*************/
int main(){
	initTCP();													//��ʼ��TCP����
	int len=0,si=0;											//����������ݳ���
	uint k=0;												//����k����ַ�λ��
	uint total=0;
	while(1){
		//send(s,"begin",5,NULL);							//��һ��start�ź�
		
		char tem;
		if((si=recv(s,(char *)&tem,1,NULL))!=-1){
			//������������0xfe����Է��˳�����
			if((uchar)tem==0xfe){							//ͷһ���ֽڲ������0xfe��ô��
				recv(s,(char *)&tem,1,NULL);
				if((uchar)tem==0xfe){
					printf("�Է�ʧȥ���ӣ�\n");
					break;
				}
			}
			si=tem*256;
			recv(s,(char *)&tem,1,NULL);
			si+=tem;	
			printf("��Ҫ���� %d ���ֽ�\n",si);

			//******����������´�ʹ��*********//
			len=k=total=0;

			for(len=0;len<si;len++){							//һ����һ���ַ��ȽϺ��� 
				if(1!=recv(s,(char *)&tem,1,NULL)) break;

				RxData[k++]=tem;							
				total++;										//�õ��ܽ��ܳ���	
			}
		
			P(total);
			dealDNP(total);									//�����Զ�������(���ĳ���)
		}
		//********��master����***********
		

		//send(s,(char *)&RxData[0],1,NULL);
		//send(s,(char *)&RxData[1],1,NULL);
	}
	closesocket(s);	
	WSACleanup();	
	return 0;
}
//�Զ���һ�����ͷ��������ݳ��ȷ��ֽ�
uint myWrite(char *str,uint len){	
	uint i;
	//��ӡ������һ��
	for(i=0;i<len;i++){
		printf("%02x ",(uchar)*(str+i));
	}
	i=send(s,str,len,NULL);
	printf("\n������%d���ֽڳ���\n",i);
	return i;
}
//���յ������ݴ���Ϊz����
void myRead( char * dst, const char * src)
{
   char * p = dst;
   while(*p) p++;                 
   while(*p++ = *src++);
 //  return( dst );
}
void P(uint total){
	uint k;
	for(k=0;k<total;k++)	printf("%02X ",(unsigned char)RxData[k]);
	printf("\n");
}
void P2(uint total){
	uint k;
	for(k=0;k<=total;k++)	printf("%02X ",(unsigned char)RxBuff[k]);
	printf("\n");
}

