#include <stdio.h>
int ID = 1;
int nProc = 2;
volatile int int_cont = 0;
unsigned char *pIRQ;

void __it_handle(){
 	int_cont++;
	printf("IT_HANDLE!!!1 "); 

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
	int *vet_aux, part, cont, *location;
	
	FILE *foto_in = fopen ("/home/marcelo/Área de Trabalho/VIPRO-MPv0.2/dma/negative_image/imagem20.tif", "rb");
	FILE *foto_out = fopen ("/home/marcelo/Área de Trabalho/VIPRO-MPv0.2/dma/negative_image/imagemP20.tif", "w+");

	init_IT();

	if(foto_in){
		vetor_carac = 0X80000000;
		i = 0;
		while (fscanf(foto_in,"%c",&final) != EOF){
			vetor_carac[i] = final;
			i++;
		}
		location = (int*)malloc(2 * sizeof(int));
		vetor_carac2 = (char*)malloc(i * sizeof(char));
	
		/**carrega egrava cabeçalho da imagem*/
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
		while(int_cont == 0);/*{
			printf("\nwhile 1 Proc1 %d %d\n",cont,int_cont);
		}*/
		

		fprintf (foto_out, "%c%c%c%c",vetor_carac2[0],vetor_carac2[1],vetor_carac2[2],vetor_carac2[3]);

		endian = (vetor_carac2[0] << 8) + vetor_carac2[1];
		fprintf (foto_out, "%c%c%c%c",vetor_carac2[4],vetor_carac2[5],vetor_carac2[6],vetor_carac2[7]);
		if (endian = 18761){
			total_char = ((vetor_carac2[7] << 24) + (vetor_carac2[6] << 16) + (vetor_carac2[5] << 8) + vetor_carac2[4]);
		}else{
			total_char = ((vetor_carac2[4] << 24) + (vetor_carac2[5] << 16) + (vetor_carac2[6] << 8) + vetor_carac2[7]);
		}

		total_char = total_char - 8;
		location[0] = total_char;
		
		vet_aux = 0x90000000;
		vet_aux[0] = &location[0];
		vet_aux[1] = sizeof(int);
		vet_aux[2] = 1;
		vet_aux[3] = 0; 		// 3ª position of the channel: 0 -> read in addr/ 1 -> write in addr
		vet_aux[4] = ID;
		vet_aux[5] = 0x8ffffff8;
		vet_aux[6] = 1; 	// 1 indica que o canal foi setado 2 indica q o dma o esta usando 0 indica não uso
		vet_aux = 0x90001000;		// aciona o dma_access
		vet_aux[0] = ID;		//indica qual é o channel solicitante
		while(int_cont == 1);
		

		endian = ((ID-1)*(total_char/nProc)) + 8;  //primeira posição desta parte da imagem
		total_char = ((total_char/nProc)*ID) + 8;    //  ultima posição desta parte da imagem

		printf("\n\ntotal char %d %d\n\n",total_char,endian);

		/**carrega a imagem em partes e em paralelo efetua as operações necessarias*/
		if((total_char- endian) >= 100){
			part = 100;
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
		while(int_cont == 2);/*{
			printf("\nwhile 2 Proc1\n");
		}*/
		beging = endian;
		endian += part;

		while(endian < total_char){
			cont = int_cont;
			if((total_char - endian) >= 100){
				part = 100;
			}else{
				part = total_char- endian;
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
			while(int_cont == cont);/*{
				printf("\nwhile %d Proc1\n", int_cont);
			}*/
			//printf("\nwhile %d Proc1",int_cont);
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
		vet_aux[2] = total_char-8;
		vet_aux[3] = 1; 		// 3ª position of the channel: 0 -> read in addr/ 1 -> write in addr
		vet_aux[4] = ID;
		vet_aux[5] = (&vetor_carac2[8]);
		vet_aux[6] = 1; // 1 indica que o canal foi setado 2 indica q o dma o esta usando 0 indica não uso
		vet_aux = 0x90001000;	// aciona o dma_access
		vet_aux[0] = ID;		//indica qual é o channel solicitante
		while(int_cont <= cont);

		/**espera pelo termino do outro processador*/
		ending = 1;
		while(ending == 1){
// 			location = i;
			cont = int_cont;
			vet_aux = 0x90000000;
			vet_aux[0] = &location[0];
			vet_aux[1] = sizeof(int);
			vet_aux[2] = 1;
			vet_aux[3] = 1; 		// 3ª position of the channel: 0 -> read in addr/ 1 -> write in addr
			vet_aux[4] = ID;
			vet_aux[5] = 0x8ffffff8;
			vet_aux[6] = 1; // 1 indica que o canal foi setado 2 indica q o dma o esta usando 0 indica não uso
			vet_aux = 0x90001000;		// aciona o dma_access
			vet_aux[0] = ID;		//indica qual é o channel solicitante
			while(int_cont == cont);
			if(location[0] == 0)
				ending = 2;
			printf("\nespera P2\n");
		}

		/**recupera dados de outros Proc */
		ending = total_char;
		cont = int_cont;
		vet_aux = 0x90000000;
		vet_aux[0] = 0x80000000+ending;
		vet_aux[1] = sizeof(char);
		vet_aux[2] = total_char-8;
		vet_aux[3] = 0; 		// 3ª position of the channel: 0 -> read in addr/ 1 -> write in addr
		vet_aux[4] = ID;
		vet_aux[5] = &vetor_carac2[0]+ending;
		vet_aux[6] = 1; 	// 1 indica que o canal foi setado 2 indica q o dma o esta usando 0 indica não uso
		vet_aux = 0x90001000;		// aciona o dma_access
		vet_aux[0] = ID;		//indica qual é o channel solicitante
		while(int_cont == cont);	
		printf("\nrecupera Proc2 %d\n",total_char-8);

		/**le dados restantes da imagem*/
		cont = int_cont;
		ending = (total_char-8)*nProc;
		vet_aux = 0x90000000;
		vet_aux[0] = 0x80000000+ending+8;
		vet_aux[1] = sizeof(char);
		vet_aux[2] = i-ending;
		vet_aux[3] = 0; 		// 3ª position of the channel: 0 -> read in addr/ 1 -> write in addr
		vet_aux[4] = ID;
		vet_aux[5] = (&vetor_carac2[8])+ending;
		vet_aux[6] = 1; // 1 indica que o canal foi setado 2 indica q o dma o esta usando 0 indica não uso
		vet_aux = 0x90001000;	// aciona o dma_access
		vet_aux[0] = ID;		//indica qual é o channel solicitante
		while(int_cont <= cont);
				
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
