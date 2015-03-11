#include <stdio.h>
int ID = 1;
int nProc = 1;
volatile int int_cont = 0;
unsigned char *pIRQ;

void __it_handle(){
 	int_cont++;
	printf("IT_HANDLE!!! "); 

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
	unsigned long int total_char, endian, i, ending, beging;
	unsigned char *vetor_carac, *vetor_carac2,copy;
	int *vet_aux, part, cont,c_while=0;

	FILE *foto_in = fopen ("/home/marcelo/Área de Trabalho/VIPRO-MPv0.2/dma/negative_image/imagem.tif", "rb");
	FILE *foto_out = fopen ("/home/marcelo/Área de Trabalho/VIPRO-MPv0.2/dma/negative_image/imagemP1.tif", "w+");

	init_IT();

	if(foto_in){
		vetor_carac = 0X80000000;
		i = 0;
		while (fscanf(foto_in,"%c",&final) != EOF){
			vetor_carac[i] = final;
			i++;
		}

		/**carrega egrava cabeçalho da imagem*/
		vetor_carac2 = (char*)malloc(i * sizeof(char));
		vet_aux = 0x90000000;
		vet_aux[0] = 0x80000000;
		vet_aux[1] = sizeof(char);
		vet_aux[2] = 8;
		vet_aux[3] = 0; 		// 3ª position of the channel: 0 -> read in addr/ 1 -> write in addr
		vet_aux[4] = ID;
		vet_aux[5] = &vetor_carac2[0];
		vet_aux[6] = 1; 	// 1 indica que o canal foi setado 2 indica q o dma o esta usando 0 indica não uso
		vet_aux = 0x90001000;		// aciona o dma_access
		vet_aux[0] = ID;		//indica qual é o channel solicitante
		while(int_cont == 0){
			//printf("\n%d\n",int_cont);
			c_while++;
		}

		fprintf (foto_out, "%c%c%c%c",vetor_carac2[0],vetor_carac2[1],vetor_carac2[2],vetor_carac2[3]);
		endian = (vetor_carac2[0] << 8) + vetor_carac2[1];
		fprintf (foto_out, "%c%c%c%c",vetor_carac2[4],vetor_carac2[5],vetor_carac2[6],vetor_carac2[7]);
		if (endian = 18761){
			total_char = ((vetor_carac2[7] << 24) + (vetor_carac2[6] << 16) + (vetor_carac2[5] << 8) + vetor_carac2[4]);
		}else{
			total_char = ((vetor_carac2[4] << 24) + (vetor_carac2[5] << 16) + (vetor_carac2[6] << 8) + vetor_carac2[7]);
		}

		total_char = total_char - 8;

		//printf("\n\ntotal char %d %d\n\n",total_char,i);

		endian = ((ID-1)*(total_char/nProc))+8;  //primeira posição desta parte da imagem
		total_char = (total_char/nProc)*ID;    //  ultima posição desta parte da imagem

		/**carrega a imagem em partes e em paralelo efetua as operações necessarias*/
		if((total_char- endian) >= 1000){
			part = 1000;
		}else{
			part = total_char- endian;
		}
		vet_aux = 0x90000000;
		vet_aux[0] = 0x80000000+8;
		vet_aux[1] = sizeof(char);
		vet_aux[2] = part;
		vet_aux[3] = 0; 		// 3ª position of the channel: 0 -> read in addr/ 1 -> write in addr
		vet_aux[4] = ID;
		vet_aux[5] = &vetor_carac2[8];
		vet_aux[6] = 1; 	// 1 indica que o canal foi setado 2 indica q o dma o esta usando 0 indica não uso
		vet_aux = 0x90001000;		// aciona o dma_access
		vet_aux[0] = ID;		//indica qual é o channel solicitante
		while(int_cont == 1){
// 			printf("\n%d\n",int_cont);
			c_while++;
		}

		beging = endian;
		endian += part;

		while(endian < total_char){
			cont = int_cont;
			if((total_char - (endian-8)) >= 1000){
				part = 1000;
			}else{
				part = total_char - endian + 8;
			}
			vet_aux = 0x90000000;
			vet_aux[0] = 0x80000000+(endian);
			vet_aux[1] = sizeof(char);
			vet_aux[2] = part;
			vet_aux[3] = 0; 		// 3ª position of the channel: 0 -> read in addr/ 1 -> write in addr
			vet_aux[4] = ID;
			vet_aux[5] = (&vetor_carac2[0])+endian;
			vet_aux[6] = 1; // 1 indica que o canal foi setado 2 indica q o dma o esta usando 0 indica não uso
			vet_aux = 0x90001000;	// aciona o dma_access
			vet_aux[0] = ID;		//indica qual é o channel solicitante
			for(beging; beging < endian; beging++){
				vetor_carac2[beging] = 255 - vetor_carac2[beging];//copy;
			}
			while(int_cont == cont){
				//printf("\n%d\n",int_cont);
				c_while++;
			}
			beging = endian;
			
			endian += part;
		}
		for(beging; beging < endian; beging++){
			vetor_carac2[beging] = 255 - vetor_carac2[beging];
		}

		/**escrevendo dados alterados na memoria compartilhada*/
		cont = int_cont;
		vet_aux = 0x90000000;
		vet_aux[0] = 0x80000000+8;
		vet_aux[1] = sizeof(char);
		vet_aux[2] = total_char;
		vet_aux[3] = 1; 		// 3ª position of the channel: 0 -> read in addr/ 1 -> write in addr
		vet_aux[4] = ID;
		vet_aux[5] = (&vetor_carac2[8]);
		vet_aux[6] = 1; // 1 indica que o canal foi setado 2 indica q o dma o esta usando 0 indica não uso
		vet_aux = 0x90001000;	// aciona o dma_access
		vet_aux[0] = ID;		//indica qual é o channel solicitante
		while(int_cont <= cont){
			//printf("\n%d\n",int_cont);
		}

		/**le dados restantes da imagem*/
		cont = int_cont;
		vet_aux = 0x90000000;
		vet_aux[0] = 0x80000000+(endian);
		vet_aux[1] = sizeof(char);
		vet_aux[2] = i-endian;
		vet_aux[3] = 0; 		// 3ª position of the channel: 0 -> read in addr/ 1 -> write in addr
		vet_aux[4] = ID;
		vet_aux[5] = (&vetor_carac2[8])+endian-8;
		vet_aux[6] = 1; // 1 indica que o canal foi setado 2 indica q o dma o esta usando 0 indica não uso
		vet_aux = 0x90001000;	// aciona o dma_access
		vet_aux[0] = ID;		//indica qual é o channel solicitante
		while(int_cont <= cont){
			//printf("\n%d\n",int_cont);
			c_while++;
		}
		
		/**escrevendo dados no arquivo de saida*/
		ending = 8;
		while (ending <= i){
			putc (vetor_carac2[ending++], foto_out);	
		}
	}
	fclose (foto_in);
	fclose (foto_out);
	printf("\nTerminating process PROC1 \n");
	return 0;
}
