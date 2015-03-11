#include <stdio.h>
int ID = 2;

/**
  Struct of the channel
  for example the first channel
  vet = 0x90000000;
  vet[0] = address for read or write
  vet[1] = size of the word
  vet[2] = number of words
  vet[3] = 0 -> for read in memory/ 1 -> for write in memory :: memory = address = vet[0]
  vet[4] = ID of the SIM 
  vet[5] = point for write
  vet[6] = 1 indicate end of the set here// 2 indicate dma is working // 0 indicate this channel is not in use
  
  vet = 0x90001000;	start the dma
  vet[0] = 1;
**/

int main(){
	int escrita[10];
	int escrita2[10];
	int *vet;
	int *aux;
	int i;

	escrita2[0] = 4;
	escrita2[1] = 1;
	escrita2[2] = 9;
	printf("\nESCREVENDO E LENDO DMA ANTES P2(b)-> ESCRITA2: %d, %d, %d", escrita2[0],escrita2[1],escrita2[2]);
	printf("\nESCREVENDO E LENDO DMA ANTES P2(b)-> ESCRITA: %d, %d, %d", escrita[0],escrita[1],escrita[2]);
	vet = 0x90000000+(7*4);
	vet[0] = &escrita2;//0x80000004;
	vet[1] = sizeof(int);
	vet[2] = 3;
	vet[3] = 0; // 3ª position of the channel: 0 -> read in addr/ 1 -> write in addr
	vet[4] = ID;
	vet[5] = &escrita;//0x80000008;
	vet[6] = 1; 		// 1 indica que o canal foi setado 2 indica q o dma o esta usando 0 indica não uso

	vet = 0x90001000;	// aciona o dma_access
	vet[0] = ID;

	vet = 0x90000000+(7*4);
	while(vet[6] != 0)
	{
		printf("**");
	}

	printf("\n\nproblemas de leitura 2\n\n");
	printf("\nESCREVENDO E LENDO DMA DEPOIS -> ESCRITA2: %d, %d, %d", escrita2[0],escrita2[1],escrita2[2]);
	printf("\nESCREVENDO E LENDO DMA DEPOIS -> ESCRITA: %d, %d, %d", escrita[0],escrita[1],escrita[2]);

	printf("\nTerminating process PROC2 \n");
	return 0;
}
