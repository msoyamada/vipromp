#include <stdio.h>
int ID = 2;
int tamanho = 40;
volatile int int_cont = 0;
unsigned char *pIRQ;

void __it_handle(){
 	int_cont++;
	printf("IT_HANDLE!!! %d\n",ID); 

}

// Copy the IRQ_Routine to the IRQ address = 0x00300000
void init_IT() { 
   unsigned char *p=0x00300000;
 
   asm(" la  $27, _IRQ_Routine" );
   asm(" sw  $27, pIRQ " );

   memcpy(p, pIRQ, 1024); 

}

void swap(int array[], int begin, int end){
	int aux;
	aux = array[begin];
	array[begin] = array[end];
	array[end] = aux;
}

void quicksort(int array[], int begin, int end) {
   if (end - begin > 0) {
      int pivot = array[begin];
      int l = begin + 1;
      int r = end;
      while(l < r) {
         if (array[l] <= pivot) {
            l++;
         } else {
            swap(array, l, r); 
            r--;
         }
      }
      if (array[l] > pivot) {
         l--;
      }
      swap(array, begin, l);
      quicksort(array, begin, l-1);
      quicksort(array, r, end);
   }
}

/* Intercala as seqüências v[p]..v[q-1] e v[q]..v[r] */
void intercala (int p, int q, int r, int v[]) {
	int i, j, k;
	int w[r+1];
	i = p; 
	j = q;
	k = 0;

	while (i < q && j <= r) {
		if (v[i] <= v[j])  
			w[k++] = v[i++];
		else  
			w[k++] = v[j++];
	}

	while (i < q)  
		w[k++] = v[i++];

	while (j <= r)  
		w[k++] = v[j++];

	for (i = p; i <= r; i++)  
		v[i] = w[i-p];
}

void mergesort (int p, int r, int v[]) {
	int q;
	if (p < r) {
		q = (p + r)/2;
		//mergesort (p, q, v);
		//mergesort (q+1, r, v);
		intercala (p, q+1, r, v);
	}
}

int main(){
	int *vet_aux;
	int *aux_aux;
	int vet[tamanho];
	int aux[tamanho];
	int i;
	int x,aux14;
	int parts = 10;

	init_IT();

	vet_aux = 0x90000000+(4*7);
	vet_aux[0] = 0x80000000+tamanho*4;
	vet_aux[1] = sizeof(int);
	vet_aux[2] = tamanho;//parts;
	vet_aux[3] = 0; 		// 3ª position of the channel: 0 -> read in addr/ 1 -> write in addr
	vet_aux[4] = ID;
	vet_aux[5] = &vet[0];
	vet_aux[6] = 1; 		// 1 indica que o canal foi setado 2 indica q o dma o esta usando 0 indica não uso
	vet_aux = 0x90001000;		// aciona o dma_access
	vet_aux[0] = ID;		//indica qual é o channel solicitante
	while(int_cont == 0);

	x = tamanho/parts;

	for(i = 1; i < x; i++){
		vet_aux = 0x90000000+(4*7);
		vet_aux[0] = 0x80000000+(i*parts*4)+tamanho*4;
		vet_aux[1] = sizeof(int);
		vet_aux[2] = parts;
		vet_aux[3] = 0; 		// 3ª position of the channel: 0 -> read in addr/ 1 -> write in addr
		vet_aux[4] = ID;
		vet_aux[5] = (&vet[0])+parts*i;
		vet_aux[6] = 1; 	// 1 indica que o canal foi setado 2 indica q o dma o esta usando 0 indica não uso
		vet_aux = 0x90001000;	// aciona o dma_access
		vet_aux[0] = ID;		//indica qual é o channel solicitante
		fflush(stdout);
		while(int_cont < i);
// 
// 		for(aux14=(i-1)*parts;aux14<i*parts;aux14++){printf(" >%d",vet[aux14]);}
		quicksort(vet, (i-1)*parts,((i-1)*parts) + parts-1); //first vector in the memory
// 		quicksort(vet,0,tamanho-1);
// 		
	}
	while(int_cont < x);
// 	quicksort(vet, ((tamanho/parts)-1)*parts, tamanho-1); //first vector in the memory
// 	
// 	mergesort(0,tamanho,vet);

	quicksort(vet, 0, tamanho-1); //first vector in the memory	

	for(x = 0; x < tamanho; x++){
		printf(" P2-> %d",vet[x]);
	}

	printf("\nTerminating process PROC1 \n");
	return 0;
}
