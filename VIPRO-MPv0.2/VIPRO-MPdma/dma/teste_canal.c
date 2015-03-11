#include <stdio.h>
int ID = 1;

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
	aux = 0x80000004;
	aux[0] = 77;
	aux[1] = 88;
	aux[2] = 99;
	escrita[0] = 4;
	escrita[1] = 1;
	escrita[2] = 9;
	printf("\nESCREVENDO E LENDO DMA ANTES P1(a)-> ESCRITA2: %d, %d, %d", escrita2[0],escrita2[1],escrita2[2]);
	printf("\nESCREVENDO E LENDO DMA ANTES P1(a)-> ESCRITA: %d, %d, %d", escrita[0],escrita[1],escrita[2]);
	vet = 0x90000000;
	vet[0] = &escrita2;
	vet[1] = sizeof(int);
	vet[2] = 3;
	vet[3] = 1; // 3ª position of the channel: 0 -> read in addr/ 1 -> write in addr
	vet[4] = ID;
	vet[5] = &escrita;//0x80000008;
	vet[6] = 1; 		// 1 indica que o canal foi setado 2 indica q o dma o esta usando 0 indica não uso
	vet = 0x90001000;	// aciona o dma_access
	vet[0] = ID;		//indica qual é o channel solicitante

	vet = 0x90000000;
	while(vet[6] != 0)
	{
		printf("..");
	}
	aux = 0x80000004;
 	printf("\n\nencerrada leitura 1a\n\n");
	printf("\nESCREVENDO E LENDO DMA DEPOIS P1(a)-> ESCRITA2: %d, %d, %d", escrita2[0],escrita2[1],escrita2[2]);
	printf("\nESCREVENDO E LENDO DMA DEPOIS P1(a)-> ESCRITA: %d, %d, %d", escrita[0],escrita[1],escrita[2]);

	escrita[0] = 44;
	escrita[1] = 11;
	escrita[2] = 66;
	printf("\nESCREVENDO E LENDO DMA ANTES P1(b)-> ESCRITA: %d, %d, %d", escrita[0],escrita[1],escrita[2]);
	printf("\nESCREVENDO E LENDO DMA ANTES P1(b)-> AUX: %d, %d, %d", aux[0],aux[1],aux[2]);
	vet = 0x90000000;
	vet[0] = 0x80000004;//0x80000004;
	vet[1] = sizeof(int);
	vet[2] = 3;
	vet[3] = 0; // 3ª position of the channel: 0 -> read in addr/ 1 -> write in addr
	vet[4] = ID;
	vet[5] = &escrita;//0x80000008;
	vet[6] = 1; 		// 1 indica que o canal foi setado 2 indica q o dma o esta usando 0 indica não uso

	vet = 0x90001000;	// aciona o dma_access
	vet[0] = ID;		//indica qual é o channel solicitante

	vet = 0x90000000;
	while(vet[6] != 0)
	{
		printf("..");
	}
	printf("\n\nencerrada leitura 1b\n\n");
	printf("\nESCREVENDO E LENDO DMA DEPOIS P1(b)-> ESCRITA: %d, %d, %d", escrita[0],escrita[1],escrita[2]);
	printf("\nESCREVENDO E LENDO DMA DEPOIS P1(b)-> AUX: %d, %d, %d", aux[0],aux[1],aux[2]);

	printf("\nTerminating process PROC1 \n");
	return 0;
}
