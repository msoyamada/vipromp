#include <stdio.h>
int ID = 2;
int nProc = 2;
volatile int int_cont = 0;
unsigned char *pIRQ;

void __it_handle(){
 	int_cont++;
	printf("IT_HANDLE!!!2 "); 

}

// Copy the IRQ_Routine to the IRQ address = 0x00300000
void init_IT() { 
   unsigned char *p=0x00300000;
 
   asm(" la  $27, _IRQ_Routine" );
   asm(" sw  $27, pIRQ " );

   memcpy(p, pIRQ, 1024); 

}

int main(){
	unsigned char c0, c1, c2, c3, final, RGB;
	unsigned long int total_char, endian, i, ending, begin;
	unsigned char *vetor_carac, *vetor_carac2,copy;
	int *vet_aux, part, cont,*location;

	init_IT();

		location = (int*)malloc(2 * sizeof(int));
		vetor_carac = 0X80000000;
		i = 0;
		part = 1;
		while(part == 1){
			printf("\nespera P1\n");
			cont = int_cont;
			vet_aux = 0x90000000+(ID-1)*28;
			vet_aux[0] = 0x8ffffff8;
			vet_aux[1] = sizeof(int);
			vet_aux[2] = 1;
			vet_aux[3] = 0; 		// 3ª position of the channel: 0 -> read in addr/ 1 -> write in addr
			vet_aux[4] = ID;
			vet_aux[5] = &location[0];
			vet_aux[6] = 1; // 1 indica que o canal foi setado 2 indica q o dma o esta usando 0 indica não uso
			vet_aux = 0x90001000;		// aciona o dma_access
			vet_aux[0] = ID;		//indica qual é o channel solicitante
			while(int_cont == cont);/*{
				printf("\nwhile 1 Proc 2 %d %d %d\n",cont,int_cont,location[0]);
			}*/
			if(location[0] != 0)
				part = 2;
			
			
		}
		total_char = location[0];
		
		total_char = ((total_char)/nProc*ID) + 8;
		endian = ((total_char)/nProc)*(ID-1) + 4;
		printf("\n\ntotal char2 %d %d\n\n",total_char,endian);

		/**carrega a imagem em partes e em paralelo efetua as operações necessarias*/
		if((total_char- endian) >= 500){
			part = 500;
		}else{
			part = total_char- endian;
		}
		cont = int_cont;
		vet_aux = 0x90000000+(ID-1)*28;
		vet_aux[0] = 0x80000000+endian;
		vet_aux[1] = sizeof(char);
		vet_aux[2] = part;
		vet_aux[3] = 0; 		// 3ª position of the channel: 0 -> read in addr/ 1 -> write in addr
		vet_aux[4] = ID;
		vet_aux[5] = &vetor_carac2[0];
		vet_aux[6] = 1; 	// 1 indica que o canal foi setado 2 indica q o dma o esta usando 0 indica não uso
		vet_aux = 0x90001000;		// aciona o dma_access
		vet_aux[0] = ID;		//indica qual é o channel solicitante
		while(int_cont == cont);/*{
			printf("\nwhile 3 Proc 2\n");
		}*/
		printf("\nwhile 3 Proc 2\n");

		begin = 0;
		endian += part;
		i = part;

		while(endian < total_char){
			cont = int_cont;
			if((total_char - endian) >= 500){
				part = 500;
			}else{
				part = total_char- endian;
			}
			vet_aux = 0x90000000+(ID-1)*28;
			vet_aux[0] = 0x80000000+(endian);
			vet_aux[1] = sizeof(char);
			vet_aux[2] = part;
			vet_aux[3] = 0; 		// 3ª position of the channel: 0 -> read in addr/ 1 -> write in addr
			vet_aux[4] = ID;
			vet_aux[5] = (&vetor_carac2[0])+i;
			vet_aux[6] = 1; // 1 indica que o canal foi setado 2 indica q o dma o esta usando 0 indica não uso
			vet_aux = 0x90001000;	// aciona o dma_access
			vet_aux[0] = ID;		//indica qual é o channel solicitante
			for(begin; begin < i; begin++){
				vetor_carac2[begin] = 255 - vetor_carac2[begin];//copy;
			}
			while(int_cont == cont);
			begin = i;
			endian += part;
			i += part;
		}
		for(begin; begin < i; begin++){
			vetor_carac2[begin] = 255 - vetor_carac2[begin];
		}

		/**escrevendo dados alterados na memoria compartilhada*/
		endian = ((total_char)/nProc)*(ID-1) + 4;
		cont = int_cont;
		vet_aux = 0x90000000+(ID-1)*28;
		vet_aux[0] = 0x80000000+endian;
		vet_aux[1] = sizeof(char);
		vet_aux[2] = i;
		vet_aux[3] = 1; 		// 3ª position of the channel: 0 -> read in addr/ 1 -> write in addr
		vet_aux[4] = ID;
		vet_aux[5] = (&vetor_carac2[0]);
		vet_aux[6] = 1; // 1 indica que o canal foi setado 2 indica q o dma o esta usando 0 indica não uso
		vet_aux = 0x90001000;	// aciona o dma_access
		vet_aux[0] = ID;		//indica qual é o channel solicitante
		while(int_cont <= cont){
// 			printf("\nwhile 2 Proc 2\n");
		}


		/**encerrando atividade */
		cont = int_cont;
		location[0] = 0;
		vet_aux = 0x90000000+(ID-1)*28;
		vet_aux[0] = &location[0];
		vet_aux[1] = sizeof(int);
		vet_aux[2] = 1;
		vet_aux[3] = 0; 		// 3ª position of the channel: 0 -> read in addr/ 1 -> write in addr
		vet_aux[4] = ID;
		vet_aux[5] = 0x8ffffff8;
		vet_aux[6] = 1; 	// 1 indica que o canal foi setado 2 indica q o dma o esta usando 0 indica não uso
		vet_aux = 0x90001000;		// aciona o dma_access
		vet_aux[0] = ID;		//indica qual é o channel solicitante
		while(int_cont == cont);
	
	printf("\nTerminating process PROC2 \n");
	return 0;
}
