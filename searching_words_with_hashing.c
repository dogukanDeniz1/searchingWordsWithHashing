#include <stdio.h>
#include <stdlib.h>
#define SIZE 997					//Tablo B�y�kl���.
#define WORD_BUFFER 50				//Al�nan kelime i�in maks boyut.
#define HORNER_NUMBER 11			//Horner metodundaki kat say�.

typedef struct node {
	char documentName[WORD_BUFFER]; //Dok�man Ad�
	struct node* next;				//Sonraki adres
}NODE;

typedef struct hash {
	char word[WORD_BUFFER];			//Kelime
	NODE* documentNameHead;			//Dok�man adlar�n� tutan linked listin ba��
}HASH;

typedef struct hashtable {
	HASH table[SIZE];				//Hash tablosu
	float loadfactor;				//doluYer/SIZE
	int indexCounter;				//doluYer
}HASHTABLE;

void prepareHashTable(HASHTABLE* hashTable);					//HashTablosunda alana tan�mlamalar� ve default de�erler ayarlan�r.
int hash1(long unsigned int key);								//1. hash fonksiyonu  h1(key) = key mod M	(index d�ner).
int hash2(long unsigned int key);								//2. hash fonksiyonu  h2(key) = 1 + (key mod MM)	(index d�ner).
void readFile(FILE* fp, HASHTABLE* hashTable, char filename[]);		//Kelimeleri okumak i�in.
int insertToHash(HASHTABLE* hashTable, long unsigned int key, char filename[WORD_BUFFER], char wrd[WORD_BUFFER]);		//Kelimelerei HashTablosuna almak i�in.	 (Tablonun dolup dolmad��� d�ner.)
long unsigned int horner(char word[WORD_BUFFER]);					//Kelimelerin indexini hesaplarken say�ya d�n���m� i�in.	(kelimenin say� hali d�ner).
void printHashTable(HASHTABLE* hashTable);							//Debug i�in hash tablosu yazd�rma.
void addDocumentName(NODE* head, char filename[WORD_BUFFER]);		//Hash tablosundaki kelimeye dok�man ad� ekleme.
void saveHashTable(HASHTABLE* hashTable);							//Hash tablosunu .txt olarak yazar.
HASHTABLE* readHashFile(FILE* hashFile);							//Dosyadan hash tablosu okuma.	(Hash tablosu d�ner.)
void findWord(HASHTABLE* hashTable, char wrd[WORD_BUFFER]);

int main() {

	int choice;		//Men� se�imleri i�in
	int quit = 0;	//��k�� i�in
	char filename[WORD_BUFFER];		//Dosya ad�
	HASHTABLE* hashTable;			//Hash tablosu
	FILE* fp;
	FILE * hashFile = fopen("17011033.txt", "r+");		//Haz�r hash tablosu var m� kontrol�.
	if (hashFile != NULL) {
    	printf("Hazir HashTable bulundu. Yukleniyor.\n");
        hashTable = readHashFile(hashFile);				//Hash tablosu okunur.
        fclose(hashFile);
    }
    else{
    	printf("Hashtable bulunamadi olusturuluyor.\n");
    	hashTable = (HASHTABLE*)malloc(sizeof(HASHTABLE));	//Hash tablosu olu�turuldu.
		prepareHashTable(hashTable);					
	}
	while(!quit){		
		printf("\nLoadfactor : %f\n1.Dokuman ekleme.\n2.Kelime Arama.\n3.Cikis.\n", hashTable->loadfactor);
		scanf("%d", &choice);
		system("CLS");
		if (choice == 1) {
			printf("Dokumanda kelimeler arasinda tek bosluk olduguna ve ozel karakterlerin bulunmadigina dikkat ediniz.\nDosya adi giriniz (input.txt) : ");
			scanf("%s", filename);
			fp = fopen(filename, "r");					//Dok�man a��ld�.
			if (fp == NULL) {
				printf("Dosya bulunamadi.");
				return 0;
			}
			readFile(fp, hashTable, filename);			//D�k�man okundu ve hashe eklendi.
			fclose(fp);									//D�k�man kapat�ld�.
			saveHashTable(hashTable);					//Hash dosyas�na kaydedildi.
			printf("\nDokuman eklendi.\n");
			//printHashTable(hashTable);				//Debug i�in
		}
		else if (choice == 2) {
			char word[WORD_BUFFER];
			printf("Aranicak kelimeyi giriniz : ");
			scanf("%s", word);
			findWord(hashTable, word);					//Kelime arama
		}
		else {
			quit=1;										//��k��
		}
	}
	return 0;
}

void findWord(HASHTABLE* hashTable, char wrd[WORD_BUFFER]){
	int i = 0;
	int index;										//hash fonksiyonundan d�necek de�eri tutucak.
	long unsigned int key = horner(wrd); 					//Kelimenin say� kar��l��� al�nd�.
	do {
		index = (hash1(key) + (i * hash2(key))) % SIZE;		//Hash fonksiyonlar�yla indexi bulundu.
		i++;
	} while (strcmp(hashTable->table[index].word, "-") && strcmp(hashTable->table[index].word, wrd) && i<SIZE);
	//Yukardaki d�ng� bo� bir yer veya ayn� kelimeyi bulana kadar d�ner.

	if(!strcmp(hashTable->table[index].word, wrd)){			//Ayn� kelime tabloda varsa.
		NODE* current = hashTable->table[index].documentNameHead;
		printf("Bulundu %s iceren dokumanlar : ", wrd);		//D�k�manlar yazd�r�l�r.
		while(current != NULL){
			printf(" %s ", current->documentName);
			current = current->next;
		}
	}
	else{
		printf("Bulunamadi.");
	}
	printf("\n");
}

HASHTABLE* readHashFile(FILE* hashFile){
	int i;
	char buffer[WORD_BUFFER];										//Dosyadan okunan kelimeleri tutan de�i�ken.
	HASHTABLE* hashTable = (HASHTABLE*)malloc(sizeof(HASHTABLE));	//Hash tablosu i�in alan a��l�r.
	prepareHashTable(hashTable);									//Hash tablosu haz�rlan�r.
	fscanf(hashFile,"%f\n",&(hashTable->loadfactor));				//loadfactor okunur.
	fscanf(hashFile,"%d\n",&(hashTable->indexCounter));				//IndexCounter okunur.
	for(i=0;i<SIZE;i++){
		fscanf(hashFile, "%s", &buffer);							//Kelime okunur
		strcpy(hashTable->table[i].word, buffer);					//Tabloya al�n�r.
		fscanf(hashFile, "%s", &buffer);							//Bo�luk ge�ilir.
		while(strcmp(buffer, ";")){									// ';' gelen kadar dosya isimlerini okur.
			addDocumentName(hashTable->table[i].documentNameHead, buffer);	// Dosya isimlerini ilgili kelimenin struct�na ekler. 
			fscanf(hashFile, "%s", &buffer);
		}
	}
	return hashTable;
}
void saveHashTable( HASHTABLE* hashTable){
	int i;
	FILE* hashFile= fopen("17011033.txt", "w+");					//Dosya s�f�rlan�r / a��l�r.
	fprintf(hashFile,"%f\n",hashTable->loadfactor);					//Loadfactor yaz�l�r.
	fprintf(hashFile,"%d\n",hashTable->indexCounter);				//Indexcounter yaz�l�r.
	for(i=0;i<SIZE;i++){
		fprintf(hashFile,"%s ", hashTable->table[i].word);			//Kelime yaz�l�r.
		NODE* current = hashTable->table[i].documentNameHead;		
		while(current!=NULL){										//�lgili d�k�manlar yaz�l�r.
			fprintf(hashFile,"%s ",current->documentName);
			current = current->next;
		}
		fprintf(hashFile,";\n");									//Sat�r sonuna geldi�imizi belirten i�aret konur.
	}
}

void printHashTable(HASHTABLE* hashTable) {							//Debug i�in kullan�ld�. Kontrol etmek isterseniz k
	int i,j;
	for (i = 0; i < SIZE; i++) {
		printf("%d - %s - ", i + 1, hashTable->table[i].word);
		NODE* current = hashTable->table[i].documentNameHead;
		while(current != NULL){
			printf(" %s - ", current->documentName);
			current = current->next;
		}
		printf("\n");
	}
	printf("\nLoadFactor : %f", hashTable->loadfactor);
	printf("\nTabloda dolu alan sayisi : %d\n", hashTable->indexCounter);
}

long unsigned int horner(char word[WORD_BUFFER]) {
	int i = 0;
	long unsigned int total = 0;								//Toplam� tutucak.
	long int multiple = 1;							//Her harfin �arp�lca�� de�i�ken.
	char letter;
	while (word[i] != NULL) {
		if (word[i] >= 65 && word[i] <= 90) {
			letter = word[i] - 'A';					//B�y�k harfse 'A' ��kar�l�r.
		}
		else {
			letter = word[i] - 'a';					//K���k harfse 'a' ��kar�l�r.
		}
													//En sonra olu�ucak say�y� k���ltm�� oldum.
		total = total + (letter * multiple);
		multiple *= HORNER_NUMBER;
		i++;
	}
	return total;
}

void addDocumentName(NODE* head, char filename[WORD_BUFFER]){
	NODE* current = head;													//Dok�man listesinin ba��n� tutar.
	while(current->next != NULL && strcmp(current->documentName, filename)){
		current = current->next;											//Dok�man linked list tinin sonuna kadar ilerler
	}
	if(current->next == NULL && strcmp(current->documentName, filename)) {
		NODE* newNode = (NODE*)malloc(sizeof(NODE));
		strcpy(newNode->documentName, filename);							//D�k�man� ekler.
		newNode->next = NULL;
		current->next = newNode;
	}
	
}

int insertToHash(HASHTABLE* hashTable, long unsigned int key, char filename[WORD_BUFFER], char wrd[WORD_BUFFER]) {
	int i;
	int index;															//Hashten d�nen index.
	if (hashTable->loadfactor < 1) {
		if (hashTable->loadfactor > 0.8) {
			printf("LOADFACTOR 0.8 den b�y�k. UYARI!!!!");				//LoadFactor kontrol�.
		}
		i = 0;
		do {
			index = (hash1(key) + (i * hash2(key))) % SIZE;				//Hash fonksiyonlar�ndan d�nen index.
			i++;
		} while (strcmp(hashTable->table[index].word, "-") && strcmp(hashTable->table[index].word, wrd) && i<SIZE);	//Bo� alan veya kelimenin ayn�s�n� bulunca ��k�cak.

		if (!strcmp(hashTable->table[index].word, "-")) {
			strcpy(hashTable->table[index].word, wrd);			//Bo� yer bulundu.
			(hashTable->indexCounter)++;						//Bo� yeri doldurdu�umuz i�in art�rd�k.
			
		}
		addDocumentName(hashTable->table[index].documentNameHead, filename);	//D�k�man ad� eklenir.
		hashTable->loadfactor = (float) (hashTable->indexCounter) / SIZE;		//Loadfactor g�ncellenir.
	}
	else {
		printf("TABLO DOLU.");
		return 0;
	}
	return 1;
}

void readFile(FILE* fp, HASHTABLE* hashTable, char filename[WORD_BUFFER]) {
	char buffer[WORD_BUFFER];									//Kelimeyi tutucak de�i�ken.
	int flag = 1;												//Loadfactor�n kontrol etti�i flag.
	long unsigned int key;												//Kelimenin say� kar��l���.
	while (flag && fscanf(fp, "%s", buffer) != EOF) {
		key = horner(buffer);									//Kelime say�ya �evrilir.
		flag = insertToHash(hashTable, key, filename, buffer);	//Hash tablosuna eklenir. Flag hash tablosunun dolup dolmad���n� kontrol eder.
	}
	if (!flag) {
		printf("Tablo doldu. Ta�an veriler : \n");
		while (flag && fscanf(fp, "%s", buffer) != EOF) {		//Ta�an veriler yazd�r�l�r.
			printf("%s", buffer);
		}
	}
}

void prepareHashTable(HASHTABLE* hashTable) {
	int i;
	for (i = 0; i < SIZE; i++) {
		strcpy(hashTable->table[i].word,"-");								//Bo� alanlara -1 yaz�l�r.
		hashTable->table[i].documentNameHead = (NODE*)malloc(sizeof(NODE));	//Dok�man linked list i�in head tan�mlan�r.
		hashTable->table[i].documentNameHead->next = NULL;					//Next null yap�l�r.
		strcpy(hashTable->table[i].documentNameHead->documentName,"");		// Dok�man ad�na default de�er verilir.
	}
	hashTable->indexCounter = 0;											
	hashTable->loadfactor = 0;
}

int hash1(long unsigned int key) {
	return (key % SIZE);													//Birinci hash fonksiyonu.
}

int hash2(long unsigned int key) {
	return (1 + (key % (SIZE - 1)));										//�kinci hash fonksiyonu.
}
