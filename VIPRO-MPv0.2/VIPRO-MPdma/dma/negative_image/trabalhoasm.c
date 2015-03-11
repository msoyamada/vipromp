#include <stdio.h>
#include <stdlib.h>

__attribute__((aligned(16))) unsigned char c0, c1, c2, c3, final, RGB, doiscincocinco[16],vetor_carac[16];
int i;
unsigned long int total_char, endian;

int main(){
	FILE *foto_in = fopen ("/home/alunos/Área de Trabalho/PSC - Imagem/imagem7.tiff", "rb");
	FILE *foto_out = fopen ("/home/alunos/Área de Trabalho/PSC - Imagem/negativo7.tiff", "w+");
	if(!foto_in){
		printf ("Deu errado1");
	}else{
		if(!foto_out){
			printf ("Deu errado2");
		}else{
			for (i = 0; i < 16; i++){
				doiscincocinco[i] = 255;
			}
			fscanf (foto_in, "%c%c%c%c", &c0, &c1, &c2, &c3);
			fprintf (foto_out, "%c%c%c%c", c0, c1, c2, c3);
			endian = (c0 << 8) + c1;
			fscanf (foto_in, "%c%c%c%c", &c0, &c1, &c2, &c3);
			fprintf (foto_out, "%c%c%c%c", c0, c1, c2, c3);
			if (endian = 18761){
				total_char = ((c3 << 24) + (c2 << 16) + (c1 << 8) + c0);
			}else{
				total_char = ((c0 << 24) + (c1 << 16) + (c2 << 8) + c3);
			}
			total_char = total_char - 8;
			asm("xor %esi, %esi");
			for (total_char; total_char > 16; total_char = total_char - 16){
				fscanf (foto_in, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", &vetor_carac[0],&vetor_carac[1],&vetor_carac[2],&vetor_carac[3],&vetor_carac[4],&vetor_carac[5],
							&vetor_carac[6],&vetor_carac[7],&vetor_carac[8],&vetor_carac[9],&vetor_carac[10],&vetor_carac[11],&vetor_carac[12],&vetor_carac[13],
							&vetor_carac[14],&vetor_carac[15]);
				asm ("xor %esi, %esi");
				asm ("movaps vetor_carac(%esi), %xmm1");
				asm ("movaps doiscincocinco(%esi), %xmm2");
				asm ("psubb %xmm1, %xmm2");
				asm ("movaps %xmm2, vetor_carac(%esi)");
				fprintf (foto_out, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", vetor_carac[0],vetor_carac[1],vetor_carac[2],vetor_carac[3],vetor_carac[4],vetor_carac[5],
							vetor_carac[6],vetor_carac[7],vetor_carac[8],vetor_carac[9],vetor_carac[10],vetor_carac[11],vetor_carac[12],vetor_carac[13],
							vetor_carac[14],vetor_carac[15]);
			}
			for(total_char; total_char >= 1; total_char--){
				fscanf (foto_in, "%c", &RGB);
				RGB = 255 - RGB;
				fprintf (foto_out, "%c", RGB);
			}
			while (fscanf(foto_in,"%c",&final) != EOF){
				putc (final, foto_out);	
			}
		}
	}
	fclose (foto_in);
	fclose (foto_out);
	return 0;
}
